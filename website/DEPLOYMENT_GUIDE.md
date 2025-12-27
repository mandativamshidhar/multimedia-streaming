# Deploying Your Multimedia Streaming System Website

This guide shows you how to deploy your website to various hosting platforms.

## Quick Start - Local Testing

To test the website locally:

```bash
# If you have Python installed
cd website
python -m http.server 8000

# Or with Node.js
npx http-server website

# Then visit: http://localhost:8000
```

## Hosting Options

### 1. GitHub Pages (Free & Easy)

**Steps:**
1. Your repository is already on GitHub: https://github.com/mandativamshidhar/multimedia-streaming
2. Go to repository Settings → Pages
3. Select `main` branch and `/website` folder as source
4. Your site will be at: `https://mandativamshidhar.github.io/multimedia-streaming`

**Update site:**
```bash
git add website/
git commit -m "Update website"
git push origin main
```

### 2. Netlify (Free Tier)

**Steps:**
1. Go to https://netlify.com
2. Click "New site from Git"
3. Connect your GitHub repository
4. Set build command: (leave empty - static site)
5. Set publish directory: `website`
6. Deploy!

**Benefits:**
- Automatic deployments on push
- Free SSL/HTTPS
- Custom domain support
- Form submissions
- Serverless functions

### 3. Vercel (Free Tier)

**Steps:**
1. Go to https://vercel.com
2. Click "New Project"
3. Import your GitHub repository
4. Set root directory: `website`
5. Deploy!

**Benefits:**
- Automatic deployments
- Global CDN
- Free SSL/HTTPS
- Analytics included
- One-click rollbacks

### 4. Docker Container

**Dockerfile (minimal):**
```dockerfile
FROM nginx:alpine
COPY website/ /usr/share/nginx/html/
EXPOSE 80
```

**Build & Run:**
```bash
docker build -t multimedia-streaming-website .
docker run -p 80:80 multimedia-streaming-website
```

**Visit:** http://localhost

### 5. Traditional Web Hosting

For cPanel/Plesk hosting:
1. Upload `website/` folder contents to `public_html/`
2. Ensure `.htaccess` is configured (if needed):
   ```
   <IfModule mod_rewrite.c>
     RewriteEngine On
     RewriteBase /
     RewriteCond %{REQUEST_FILENAME} !-f
     RewriteCond %{REQUEST_FILENAME} !-d
     RewriteRule ^(.*)$ index.html [L]
   </IfModule>
   ```
3. Access via your domain

### 6. Amazon S3 + CloudFront

**Steps:**
1. Create S3 bucket for static content
2. Upload `website/` contents
3. Enable static website hosting
4. Set up CloudFront distribution
5. Configure Route 53 DNS

**Benefits:**
- Scalable
- Global CDN
- Pay-per-use
- Fast delivery

### 7. Firebase Hosting

**Setup:**
```bash
npm install -g firebase-tools
firebase login
firebase init hosting
# Select website folder
firebase deploy
```

**Benefits:**
- Free SSL/HTTPS
- Global CDN
- One-command deployment
- Analytics

## Custom Domain Setup

### For GitHub Pages:
1. In repository Settings → Pages
2. Add custom domain (e.g., multimedia-streaming.dev)
3. Update DNS records:
   ```
   CNAME: multimedia-streaming.dev → mandativamshidhar.github.io
   ```

### For Netlify/Vercel:
1. Add domain in site settings
2. Follow provider's DNS instructions
3. Usually: Point nameservers or add CNAME record

## SSL/HTTPS

All modern hosting (GitHub Pages, Netlify, Vercel, Firebase) provide **free SSL certificates**. No additional setup required.

## Performance Optimization

The website includes:
- ✓ Responsive design (mobile-friendly)
- ✓ CSS/JS optimization
- ✓ Semantic HTML
- ✓ Fast load times
- ✓ SEO-friendly structure

## Adding More Content

### To add new sections:
1. Edit `website/index.html` - add new `<section>` elements
2. Update `website/styles.css` - add styles
3. Update navigation in navbar if needed
4. Push changes to deploy

### To add more pages:
1. Create new `.html` file in `website/` folder
2. Link from main `index.html`
3. Commit and push

## Analytics

### Add Google Analytics:
```html
<!-- Add before closing </head> in index.html -->
<script async src="https://www.googletagmanager.com/gtag/js?id=GA_ID"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());
  gtag('config', 'YOUR_GA_ID');
</script>
```

## Troubleshooting

**Site not updating:**
- Clear browser cache (Ctrl+Shift+Delete)
- Wait 5-10 minutes for CDN to update
- Check deployment status in hosting provider

**Styles not loading:**
- Ensure CSS path is correct
- Check file permissions
- Clear browser cache

**Images not showing:**
- Use relative paths or absolute URLs
- Check file extensions (case-sensitive on Linux)
- Ensure images are in website folder

## Security Checklist

- ✓ HTTPS/SSL enabled
- ✓ No sensitive data in HTML
- ✓ Regular security updates
- ✓ Monitor for breaking changes
- ✓ Backup your content

## Next Steps

1. Choose a hosting platform above
2. Deploy using provided instructions
3. Monitor website analytics
4. Keep content updated
5. Share your deployed site!

---

**Need help?** Check your hosting provider's documentation or open an issue on GitHub.
