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
    const icon = themeToggle ? themeToggle.querySelector('span') : null;

    // Check saved preference or default to dark
    let savedTheme = 'dark';
    try {
        savedTheme = localStorage.getItem('theme') || 'dark';
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
        if (!icon) return;
        icon.textContent = theme === 'light' ? '🌙' : '☀️';
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

    // Interactive Configurator
    const configName = document.getElementById('configName');
    const configSecurity = document.getElementById('configSecurity');
    const configLogging = document.getElementById('configLogging');
    const codeBlock = document.getElementById('generatedCode');

    if (configName && configSecurity && configLogging && codeBlock) {
        function updateCode() {
            const name = configName.value || 'My-ESP32';
            const security = configSecurity.value;
            const logging = configLogging.checked;

            const code = `#include <WiBLE.h>

WiBLE provisioner;

void setup() {
    ${logging ? 'Serial.begin(115200);' : '// Serial disabled'}
    
    ProvisioningConfig config;
    config.deviceName = "${name}";
    config.securityLevel = SecurityLevel::${security};
    
    provisioner.begin(config);
    provisioner.startProvisioning();
}

void loop() {
    provisioner.loop();
}`;
            codeBlock.textContent = code;
        }

        configName.addEventListener('input', updateCode);
        configSecurity.addEventListener('change', updateCode);
        configLogging.addEventListener('change', updateCode);

        // Run once to set initial state
        updateCode();
    }
});
