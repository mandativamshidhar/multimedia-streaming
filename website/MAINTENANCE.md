# Website Maintenance Guide

## Overview

This document outlines how to maintain, update, and optimize your Multimedia Streaming System website.

## File Structure

```
website/
├── index.html              # Main HTML page
├── styles.css              # All CSS styling
├── script.js               # JavaScript functionality
├── server.js               # Local development server
├── manifest.json           # Progressive Web App config
├── README.md               # Website overview
├── DEPLOYMENT_GUIDE.md     # Hosting & deployment
├── QUICKSTART.md           # Quick setup guide
└── MAINTENANCE.md          # This file
```

## Common Tasks

### Adding a New Section

1. **Add HTML** - In `index.html`, add your section:
```html
<section id="section-id" class="section-name">
    <div class="container">
        <h2>Section Title</h2>
        <!-- Content here -->
    </div>
</section>
```

2. **Add CSS** - In `styles.css`, add your styles:
```css
.section-name {
    padding: 4rem 20px;
    background-color: var(--bg-color);
}
```

3. **Update Navigation** - Add link in navbar:
```html
<li><a href="#section-id" class="nav-link">Section Title</a></li>
```

4. **Test locally** - Run `node server.js` and check in browser

### Changing Colors

Edit CSS variables at the top of `styles.css`:

```css
:root {
    --primary-color: #6366f1;      /* Main color */
    --secondary-color: #8b5cf6;    /* Accent color */
    --success-color: #10b981;      /* Success green */
    --danger-color: #ef4444;       /* Error red */
    /* ... more colors ... */
}
```

All elements automatically update when you change these values.

### Updating Content

Simply edit the text in `index.html`:

1. Open `index.html`
2. Find the section you want to update
3. Edit the content
4. Save the file
5. Refresh browser to see changes

### Adding Images

1. Place image files in `website/` folder
2. Reference in HTML:
```html
<img src="image-name.png" alt="Description">
```

3. Add responsive styling if needed:
```css
img {
    max-width: 100%;
    height: auto;
}
```

### Updating Links

- **GitHub links** - Search for GitHub URLs and update
- **Internal links** - Use `#section-id` for smooth scrolling
- **External links** - Add `target="_blank"` to open in new tab

## Performance Optimization

### Check Page Speed

1. Open website in Chrome
2. Press F12 (DevTools)
3. Go to Lighthouse tab
4. Click "Generate report"

### Optimization Tips

- Keep images optimized (use online tools like TinyPNG)
- Minimize CSS/JS (current version already optimized)
- Use semantic HTML (already implemented)
- Enable gzip compression (handled by server)
- Cache static assets (configure in hosting provider)

## SEO Optimization

### Meta Tags

Update in `<head>` section of `index.html`:

```html
<meta name="description" content="Your description here">
<meta name="keywords" content="keyword1, keyword2, keyword3">
<meta name="author" content="Your Name">
```

### Sitemap

For better SEO, create `sitemap.xml`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
    <url>
        <loc>https://yourdomain.com</loc>
        <lastmod>2025-01-01</lastmod>
        <priority>1.0</priority>
    </url>
</urlset>
```

### robots.txt

Create `robots.txt` in website folder:

```
User-agent: *
Allow: /
Sitemap: https://yourdomain.com/sitemap.xml
```

## Analytics

### Add Google Analytics

1. Get your Google Analytics ID
2. Add this before closing `</head>` in `index.html`:

```html
<script async src="https://www.googletagmanager.com/gtag/js?id=GA_ID"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());
  gtag('config', 'GA_ID');
</script>
```

### Add Microsoft Clarity

```html
<script type="text/javascript">
  (function(c,l,a,r,i,t,y){
    c[a]=c[a]||function(){(c[a].q=c[a].q||[]).push(arguments)}
    t=l.createElement(r);t.async=1;t.src="https://www.clarity.ms/tag/"+i;
    y=l.getElementsByTagName(r)[0];y.parentNode.insertBefore(t,y);
  })(window, document, "clarity", "script", "YOUR_ID");
</script>
```

## Security

### HTTPS

- ✅ GitHub Pages - Automatic free SSL
- ✅ Netlify - Automatic free SSL
- ✅ Vercel - Automatic free SSL
- ✅ Firebase - Automatic free SSL

### Content Security Policy

Add to `index.html` if needed:

```html
<meta http-equiv="Content-Security-Policy" 
      content="default-src 'self'; script-src 'self' 'unsafe-inline';">
```

## Testing Checklist

Before deploying changes:

- [ ] Test on desktop browser (Chrome, Firefox, Safari)
- [ ] Test on mobile (iPhone, Android)
- [ ] Test on tablet
- [ ] Verify all links work
- [ ] Check for typos
- [ ] Test copy-to-clipboard feature
- [ ] Verify images load
- [ ] Check navigation works
- [ ] Test on slow internet (DevTools)
- [ ] Check lighthouse score (90+)

## Browser Compatibility

Current support:
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

To support older browsers, update the `styles.css` `@supports` rules.

## Mobile Optimization

Already implemented:
- ✅ Responsive breakpoints (768px, 1024px)
- ✅ Touch-friendly buttons
- ✅ Viewport meta tag
- ✅ Font scaling
- ✅ Performance optimized

## Version Control

### Commit Changes

```bash
git add website/
git commit -m "Update website: description of changes"
git push origin main
```

### Rollback Changes

```bash
git revert <commit-hash>
git push origin main
```

## Backup Strategy

1. Keep regular backups on GitHub
2. Use GitHub releases for major versions
3. Keep local copies of important files
4. Consider off-site backup service

## Troubleshooting

### Images Not Loading

- Check file paths (use relative paths)
- Verify file extension
- Ensure file exists in website folder
- Clear browser cache

### Styles Not Applied

- Clear browser cache (Ctrl+Shift+Delete)
- Check CSS file is linked correctly
- Verify class names match
- Check for CSS conflicts

### Links Not Working

- Verify URLs are correct
- Check anchor tags use `href="#id"`
- Ensure section IDs exist
- Test with direct URL

### Slow Performance

- Optimize images
- Minimize CSS/JS
- Enable caching
- Use CDN (Netlify/Vercel)
- Check lighthouse report

## Regular Maintenance

### Weekly
- [ ] Monitor analytics
- [ ] Check for errors in console
- [ ] Test all links

### Monthly
- [ ] Update dependencies
- [ ] Review performance metrics
- [ ] Check for security updates
- [ ] Update content if needed

### Quarterly
- [ ] Full website audit
- [ ] Performance testing
- [ ] SEO review
- [ ] User feedback analysis

## Useful Tools

- **Lighthouse** - Performance & SEO audit
- **Wave** - Accessibility checker
- **GTmetrix** - Page speed analysis
- **Screaming Frog** - SEO crawler
- **BrowserStack** - Cross-browser testing

## Support Resources

- [MDN Web Docs](https://developer.mozilla.org/)
- [CSS-Tricks](https://css-tricks.com/)
- [Stack Overflow](https://stackoverflow.com/)
- [Web.dev](https://web.dev/)

## Contact & Support

For issues:
1. Check existing documentation
2. Test in different browser
3. Check browser console for errors
4. Open GitHub issue with details
5. Provide screenshots/error messages

---

Keep your website fresh and updated! 🚀
