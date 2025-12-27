// Smooth navigation link active state
document.addEventListener('DOMContentLoaded', function() {
    const navLinks = document.querySelectorAll('.nav-link');
    const sections = document.querySelectorAll('section[id]');
    
    // Update active link on scroll
    window.addEventListener('scroll', () => {
        let current = '';
        
        sections.forEach(section => {
            const sectionTop = section.offsetTop;
            const sectionHeight = section.clientHeight;
            
            if (pageYOffset >= sectionTop - 200) {
                current = section.getAttribute('id');
            }
        });
        
        navLinks.forEach(link => {
            link.classList.remove('active');
            if (link.getAttribute('href').slice(1) === current) {
                link.classList.add('active');
            }
        });
    });
    
    // Smooth scroll for navigation links
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            const href = this.getAttribute('href');
            if (href.startsWith('#')) {
                e.preventDefault();
                const target = document.querySelector(href);
                if (target) {
                    target.scrollIntoView({ behavior: 'smooth' });
                }
            }
        });
    });
    
    // Copy code to clipboard functionality
    initializeCodeBlocks();

    // Mobile nav toggle
    const navToggle = document.querySelector('.nav-toggle');
    const navMenu = document.querySelector('.nav-menu');
    if (navToggle && navMenu) {
        navToggle.addEventListener('click', () => {
            const isOpen = navMenu.classList.toggle('show');
            navToggle.setAttribute('aria-expanded', isOpen ? 'true' : 'false');
        });
    }
});

function initializeCodeBlocks() {
    const codeBlocks = document.querySelectorAll('.code-block');
    
    codeBlocks.forEach(block => {
        // Create copy button
        const copyButton = document.createElement('button');
        copyButton.className = 'copy-btn';
        copyButton.textContent = '📋 Copy';
        copyButton.style.cssText = `
            position: absolute;
            top: 0.5rem;
            right: 0.5rem;
            padding: 0.5rem 1rem;
            background: var(--primary-color, #6366f1);
            color: white;
            border: none;
            border-radius: 0.25rem;
            cursor: pointer;
            font-size: 0.85rem;
            display: none;
            z-index: 10;
        `;
        
        block.style.position = 'relative';
        block.appendChild(copyButton);
        
        // Show copy button on hover
        block.addEventListener('mouseenter', () => {
            copyButton.style.display = 'block';
        });
        
        block.addEventListener('mouseleave', () => {
            copyButton.style.display = 'none';
        });
        
        // Copy functionality
        copyButton.addEventListener('click', () => {
            const text = block.textContent;
            navigator.clipboard.writeText(text).then(() => {
                const originalText = copyButton.textContent;
                copyButton.textContent = '✓ Copied!';
                setTimeout(() => {
                    copyButton.textContent = originalText;
                }, 2000);
            }).catch(() => {
                copyButton.textContent = '✗ Failed';
                setTimeout(() => {
                    copyButton.textContent = originalText;
                }, 2000);
            });
        });
    });
}

// Add loading animation
window.addEventListener('load', () => {
    document.body.style.opacity = '1';
});

// Lazy load sections when they come into view
if ('IntersectionObserver' in window) {
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };
    
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, observerOptions);
    
    document.querySelectorAll('section').forEach(section => {
        section.style.opacity = '0';
        section.style.transform = 'translateY(20px)';
        section.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(section);
    });
}

// Performance monitoring
if ('PerformanceObserver' in window) {
    try {
        const perfObserver = new PerformanceObserver((list) => {
            for (const entry of list.getEntries()) {
                console.log(`${entry.name}: ${entry.duration}ms`);
            }
        });
        perfObserver.observe({ entryTypes: ['navigation', 'resource'] });
    } catch (e) {
        console.log('Performance monitoring not supported');
    }
}
