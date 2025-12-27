#!/usr/bin/env node

const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

const PORT = 8000;
const WEBSITE_DIR = __dirname;

const MIME_TYPES = {
    '.html': 'text/html; charset=utf-8',
    '.css': 'text/css',
    '.js': 'application/javascript',
    '.json': 'application/json',
    '.md': 'text/markdown',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.svg': 'image/svg+xml',
    '.ico': 'image/x-icon',
    '.woff': 'font/woff',
    '.woff2': 'font/woff2'
};

const server = http.createServer((req, res) => {
    // Parse URL
    const parsedUrl = url.parse(req.url);
    
    // Get file path
    let filePath = path.join(WEBSITE_DIR, parsedUrl.pathname);
    
    // Default to index.html if directory requested
    if (fs.existsSync(filePath) && fs.statSync(filePath).isDirectory()) {
        filePath = path.join(filePath, 'index.html');
    }
    
    // If file doesn't exist and it's not index.html, redirect to index.html
    if (!fs.existsSync(filePath) && parsedUrl.pathname !== '/' && !parsedUrl.pathname.includes('.')) {
        filePath = path.join(WEBSITE_DIR, 'index.html');
    }
    
    // Check if file exists
    if (!fs.existsSync(filePath)) {
        res.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end('<h1>404 - File Not Found</h1>', 'utf-8');
        return;
    }
    
    // Get file extension
    const ext = path.extname(filePath).toLowerCase();
    const contentType = MIME_TYPES[ext] || 'application/octet-stream';
    
    // Read and serve file
    fs.readFile(filePath, (err, data) => {
        if (err) {
            res.writeHead(500, { 'Content-Type': 'text/plain' });
            res.end('Server error: ' + err.message);
            return;
        }
        
        // Set cache headers
        res.writeHead(200, {
            'Content-Type': contentType,
            'Cache-Control': ext === '.html' ? 'no-cache' : 'public, max-age=3600'
        });
        res.end(data);
    });
});

server.listen(PORT, () => {
    console.log(`\n🚀 Website server running at: http://localhost:${PORT}\n`);
    console.log(`   Open your browser and navigate to: http://localhost:${PORT}`);
    console.log(`   Press Ctrl+C to stop the server\n`);
});

// Handle errors
server.on('error', (err) => {
    if (err.code === 'EADDRINUSE') {
        console.error(`\n❌ Port ${PORT} is already in use!`);
        console.error(`   Try using a different port: node server.js 8001\n`);
        process.exit(1);
    }
    throw err;
});
