document.addEventListener('DOMContentLoaded', () => {
    // Initialize AOS
    AOS.init({
        once: true,
        offset: 50,
        duration: 1000,
        easing: 'ease-out-cubic',
    });

    // --- Particle System ---
    const canvas = document.createElement('canvas');
    canvas.id = 'particle-canvas';
    document.body.prepend(canvas);
    const ctx = canvas.getContext('2d');

    let width, height;
    let particles = [];

    function resize() {
        width = window.innerWidth;
        height = window.innerHeight;
        canvas.width = width;
        canvas.height = height;
    }

    class Particle {
        constructor() {
            this.x = Math.random() * width;
            this.y = Math.random() * height;
            this.vx = (Math.random() - 0.5) * 0.5;
            this.vy = (Math.random() - 0.5) * 0.5;
            this.size = Math.random() * 2 + 1;
            this.color = `rgba(255, 255, 255, ${Math.random() * 0.3})`;
        }

        update() {
            this.x += this.vx;
            this.y += this.vy;

            if (this.x < 0) this.x = width;
            if (this.x > width) this.x = 0;
            if (this.y < 0) this.y = height;
            if (this.y > height) this.y = 0;
        }

        draw() {
            ctx.fillStyle = this.color;
            ctx.beginPath();
            ctx.arc(this.x, this.y, this.size, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    function initParticles() {
        resize();
        for (let i = 0; i < 50; i++) {
            particles.push(new Particle());
        }
        animateParticles();
    }

    function animateParticles() {
        ctx.clearRect(0, 0, width, height);
        particles.forEach(p => {
            p.update();
            p.draw();
        });
        requestAnimationFrame(animateParticles);
    }

    window.addEventListener('resize', resize);
    initParticles();

    // --- Magnetic Cursor ---
    const cursorDot = document.querySelector('.cursor-dot');
    const cursorOutline = document.querySelector('.cursor-outline');

    if (cursorDot && cursorOutline) {
        let mouseX = 0, mouseY = 0;
        let cursorX = 0, cursorY = 0;

        document.addEventListener('mousemove', (e) => {
            mouseX = e.clientX;
            mouseY = e.clientY;

            cursorDot.style.left = `${mouseX}px`;
            cursorDot.style.top = `${mouseY}px`;
        });

        function animateCursor() {
            const speed = 0.15;
            cursorX += (mouseX - cursorX) * speed;
            cursorY += (mouseY - cursorY) * speed;

            cursorOutline.style.left = `${cursorX}px`;
            cursorOutline.style.top = `${cursorY}px`;

            requestAnimationFrame(animateCursor);
        }
        animateCursor();

        // Hover Effects
        const interactiveElements = document.querySelectorAll('a, button, .glass-card, input, select');
        interactiveElements.forEach(el => {
            el.addEventListener('mouseenter', () => {
                cursorOutline.style.transform = 'translate(-50%, -50%) scale(1.5)';
                cursorOutline.style.backgroundColor = 'rgba(255, 255, 255, 0.1)';
                cursorOutline.style.borderColor = 'transparent';
            });
            el.addEventListener('mouseleave', () => {
                cursorOutline.style.transform = 'translate(-50%, -50%) scale(1)';
                cursorOutline.style.backgroundColor = 'transparent';
                cursorOutline.style.borderColor = 'rgba(255, 255, 255, 0.5)';
            });
        });
    }

    // --- Navbar Scroll Effect ---
    const navbar = document.querySelector('.navbar');
    window.addEventListener('scroll', () => {
        if (window.scrollY > 50) {
            navbar.classList.add('scrolled');
        } else {
            navbar.classList.remove('scrolled');
        }
    });

    // --- Mobile Menu ---
    const mobileMenuBtn = document.querySelector('.mobile-menu-btn');
    const mobileNav = document.querySelector('.mobile-nav');

    if (mobileMenuBtn && mobileNav) {
        mobileMenuBtn.addEventListener('click', () => {
            mobileNav.classList.toggle('active');
            document.body.style.overflow = mobileNav.classList.contains('active') ? 'hidden' : '';
        });

        // Close on link click
        mobileNav.querySelectorAll('a').forEach(link => {
            link.addEventListener('click', () => {
                mobileNav.classList.remove('active');
                document.body.style.overflow = '';
            });
        });
    }

    // --- Configurator Logic (Preserved) ---
    const configName = document.getElementById('configName');
    const configSecurity = document.getElementById('configSecurity');
    const configLogging = document.getElementById('configLogging');
    const codeBlock = document.getElementById('generatedCode');

    if (configName && configSecurity && configLogging && codeBlock) {
        function updateCode() {
            const name = configName.value || 'My-ESP32';
            const security = configSecurity.value;
            const logging = configLogging.checked;

            const code = `<span class="code-keyword">#include</span> &lt;<span class="code-type">WiBLE.h</span>&gt;

<span class="code-type">WiBLE</span> provisioner;

<span class="code-keyword">void</span> <span class="code-function">setup</span>() {
    ${logging ? '<span class="code-function">Serial.begin</span>(<span class="code-string">115200</span>);' : '<span class="code-comment">// Serial disabled</span>'}
    
    <span class="code-type">ProvisioningConfig</span> config;
    config.deviceName = <span class="code-string">"${name}"</span>;
    config.securityLevel = <span class="code-type">SecurityLevel</span>::<span class="code-keyword">${security}</span>;
    
    provisioner.<span class="code-function">begin</span>(config);
    provisioner.<span class="code-function">startProvisioning</span>();
}

<span class="code-keyword">void</span> <span class="code-function">loop</span>() {
    provisioner.<span class="code-function">loop</span>();
}`;
            codeBlock.innerHTML = code;
        }

        configName.addEventListener('input', updateCode);
        configSecurity.addEventListener('change', updateCode);
        configLogging.addEventListener('change', updateCode);
        updateCode();
    }

    // Copy Button
    const copyButton = document.getElementById('copyButton');
    if (copyButton && codeBlock) {
        copyButton.addEventListener('click', () => {
            navigator.clipboard.writeText(codeBlock.textContent).then(() => {
                const originalText = copyButton.innerHTML;
                copyButton.innerHTML = '<span>Copied!</span>';
                copyButton.style.color = '#0aff0a';
                setTimeout(() => {
                    copyButton.innerHTML = originalText;
                    copyButton.style.color = '';
                }, 2000);
            });
        });
    }
});
