document.addEventListener('DOMContentLoaded', () => {
    // Initialize Animate On Scroll
    AOS.init({
        once: true,
        offset: 50,
        duration: 1000,
        easing: 'ease-out-cubic',
    });

    // Theme System
    const themeToggle = document.getElementById('themeToggle');
    const html = document.documentElement;

    // Icons
    const sunIcon = `<svg viewBox="0 0 24 24"><path d="M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zM2 13h2c.55 0 1-.45 1-1s-.45-1-1-1H2c-.55 0-1 .45-1 1s.45 1 1 1zm18 0h2c.55 0 1-.45 1-1s-.45-1-1-1h-2c-.55 0-1 .45-1 1s.45 1 1 1zM11 2v2c0 .55.45 1 1 1s1-.45 1-1V2c0-.55-.45-1-1-1s-1 .45-1 1zm0 18v2c0 .55.45 1 1 1s1-.45 1-1v-2c0-.55-.45-1-1-1s-1 .45-1 1zM5.99 4.58a.996.996 0 00-1.41 0 .996.996 0 000 1.41l1.06 1.06c.39.39 1.03.39 1.41 0s.39-1.03 0-1.41L5.99 4.58zm12.37 12.37a.996.996 0 00-1.41 0 .996.996 0 000 1.41l1.06 1.06c.39.39 1.03.39 1.41 0a.996.996 0 000-1.41l-1.06-1.06zm1.06-10.96a.996.996 0 000-1.41.996.996 0 00-1.41 0l-1.06 1.06c-.39.39-.39 1.03 0 1.41s1.03.39 1.41 0l1.06-1.06zM7.05 18.36a.996.996 0 000 1.41.996.996 0 001.41 0l1.06-1.06c.39-.39.39-1.03 0-1.41s-1.03-.39-1.41 0l-1.06 1.06z"/></svg>`;
    const moonIcon = `<svg viewBox="0 0 24 24"><path d="M12 3c-4.97 0-9 4.03-9 9s4.03 9 9 9 9-4.03 9-9c0-.46-.04-.92-.1-1.36-.98 1.37-2.58 2.26-4.4 2.26-2.98 0-5.4-2.42-5.4-5.4 0-1.81.89-3.42 2.26-4.4-.44-.06-.9-.1-1.36-.1z"/></svg>`;

    // Check saved preference or default to light
    let savedTheme = 'light';
    try {
        savedTheme = localStorage.getItem('theme') || 'light';
    } catch (e) {
        console.warn('LocalStorage access denied', e);
    }

    html.setAttribute('data-theme', savedTheme);
    updateThemeIcon(savedTheme);

    if (themeToggle) {
        themeToggle.addEventListener('click', () => {
            const currentTheme = html.getAttribute('data-theme');
            const newTheme = currentTheme === 'light' ? 'dark' : 'light';

            html.setAttribute('data-theme', newTheme);
            try {
                localStorage.setItem('theme', newTheme);
            } catch (e) { }
            updateThemeIcon(newTheme);
        });
    }

    function updateThemeIcon(theme) {
        if (!themeToggle) return;
        // If theme is light, show Moon (to switch to dark). If dark, show Sun (to switch to light).
        themeToggle.innerHTML = theme === 'light' ? moonIcon : sunIcon;
    }

    // Fluid Cursor Logic (Reverted to Phase 12 - Lighter, No Magnet)
    const cursorDot = document.querySelector('.cursor-dot');
    const cursorOutline = document.querySelector('.cursor-outline');

    if (cursorDot && cursorOutline) {
        let mouseX = 0;
        let mouseY = 0;
        let cursorX = 0;
        let cursorY = 0;

        document.addEventListener('mousemove', (e) => {
            mouseX = e.clientX;
            mouseY = e.clientY;

            // Instant follow for dot
            cursorDot.style.left = mouseX + 'px';
            cursorDot.style.top = mouseY + 'px';
        });

        function animateCursor() {
            // Lighter Lerp (Phase 12 feel)
            const speed = 0.2;
            cursorX += (mouseX - cursorX) * speed;
            cursorY += (mouseY - cursorY) * speed;

            cursorOutline.style.left = cursorX + 'px';
            cursorOutline.style.top = cursorY + 'px';

            requestAnimationFrame(animateCursor);
        }

        animateCursor();

        // Simple Hover Effect (No Magnetic Pull)
        const interactiveElements = document.querySelectorAll('a, button, .feature-card, .config-input, select, label');

        interactiveElements.forEach(el => {
            el.addEventListener('mouseenter', () => {
                cursorOutline.style.width = '60px';
                cursorOutline.style.height = '60px';
                cursorOutline.style.backgroundColor = 'rgba(255, 255, 255, 0.1)';
            });

            el.addEventListener('mouseleave', () => {
                cursorOutline.style.width = '40px';
                cursorOutline.style.height = '40px';
                cursorOutline.style.backgroundColor = 'transparent';
            });
        });
    }

    // Interactive Configurator with Syntax Highlighting
    const configName = document.getElementById('configName');
    const configSecurity = document.getElementById('configSecurity');
    const configLogging = document.getElementById('configLogging');
    const codeBlock = document.getElementById('generatedCode');

    if (configName && configSecurity && configLogging && codeBlock) {
        function updateCode() {
            const name = configName.value || 'My-ESP32';
            const security = configSecurity.value;
            const logging = configLogging.checked;

            // Build code with syntax highlighting spans
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

        // Run once to set initial state
        updateCode();
    }
});
