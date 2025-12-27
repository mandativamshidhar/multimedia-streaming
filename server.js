#!/usr/bin/env node

const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

const PORT = process.env.PORT || 8000;
const WEBSITE_DIR = __dirname;
const STATUS_FILE = path.join(WEBSITE_DIR, 'status.json');

// Simple in-memory list of SSE clients
const clients = [];

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

// Helper: read current status (fallback defaults)
function readStatus() {
    try {
        const raw = fs.readFileSync(STATUS_FILE, 'utf8');
        return JSON.parse(raw);
    } catch (e) {
        return { status: 'Unknown', uptime: '-' };
    }
}

// Helper: write status and notify SSE clients
function writeStatus(data) {
    fs.writeFileSync(STATUS_FILE, JSON.stringify(data, null, 2));
    const payload = `data: ${JSON.stringify(data)}\n\n`;
    clients.forEach(res => res.write(payload));
}

// API: simple REST endpoints and SSE
server.on('request', (req, res) => {
    const parsedUrl = url.parse(req.url, true);
    if (parsedUrl.pathname === '/api/status' && req.method === 'GET') {
        const status = readStatus();
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(status));
        return;
    }

    if (parsedUrl.pathname === '/api/status' && req.method === 'POST') {
        let body = '';
        req.on('data', chunk => body += chunk);
        req.on('end', () => {
            try {
                const json = JSON.parse(body);
                // Simple admin token check: header 'x-admin-token' or query ?token=
                const token = req.headers['x-admin-token'] || parsedUrl.query.token || process.env.ADMIN_TOKEN || 'localdev';
                const expected = process.env.ADMIN_TOKEN || 'localdev';
                if (token !== expected) {
                    res.writeHead(401, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify({ error: 'Unauthorized' }));
                    return;
                }

                const newStatus = {
                    status: json.status || 'Unknown',
                    uptime: json.uptime || new Date().toISOString()
                };
                writeStatus(newStatus);
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify(newStatus));
            } catch (e) {
                res.writeHead(400, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ error: 'Invalid JSON' }));
            }
        });
        return;
    }

    // Server-Sent Events endpoint
    if (parsedUrl.pathname === '/events' && req.method === 'GET') {
        res.writeHead(200, {
            'Content-Type': 'text/event-stream',
            'Cache-Control': 'no-cache',
            Connection: 'keep-alive'
        });
        // send current status immediately
        const status = readStatus();
        res.write(`data: ${JSON.stringify(status)}\n\n`);
        clients.push(res);
        req.on('close', () => {
            const idx = clients.indexOf(res);
            if (idx !== -1) clients.splice(idx, 1);
        });
        return;
    }
});

server.listen(PORT, () => {
    console.log(`\n🚀 Website server running at: http://localhost:${PORT}\n`);
    console.log(`   Open your browser and navigate to: http://localhost:${PORT}`);
    console.log(`   Press Ctrl+C to stop the server\n`);
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
