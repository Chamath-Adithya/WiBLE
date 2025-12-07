// Initialize Animate On Scroll
AOS.init({
    once: true,
    offset: 50,
    duration: 800,
    easing: 'ease-out-cubic',
});

// Fluid Cursor Logic
const cursorDot = document.querySelector('.cursor-dot');
const cursorOutline = document.querySelector('.cursor-outline');

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
    // Lerp (Linear Interpolation) for smooth delay
    const speed = 0.15;
    cursorX += (mouseX - cursorX) * speed;
    cursorY += (mouseY - cursorY) * speed;

    cursorOutline.style.left = cursorX + 'px';
    cursorOutline.style.top = cursorY + 'px';

    requestAnimationFrame(animateCursor);
}

animateCursor();

// Magnetic Hover Effect
const interactiveElements = document.querySelectorAll('a, button, .feature-card, .tilt-card');

interactiveElements.forEach(el => {
    el.addEventListener('mouseenter', () => {
        cursorOutline.style.width = '60px';
        cursorOutline.style.height = '60px';
        cursorOutline.style.backgroundColor = 'rgba(59, 130, 246, 0.1)';
    });

    el.addEventListener('mouseleave', () => {
        cursorOutline.style.width = '40px';
        cursorOutline.style.height = '40px';
        cursorOutline.style.backgroundColor = 'transparent';
    });
});

// 3D Tilt Effect (Existing)
const cards = document.querySelectorAll('.tilt-card');

cards.forEach(card => {
    card.addEventListener('mousemove', handleMouseMove);
    card.addEventListener('mouseleave', handleMouseLeave);
});

function handleMouseMove(e) {
    const card = this;
    const cardRect = card.getBoundingClientRect();

    const cardCenterX = cardRect.left + cardRect.width / 2;
    const cardCenterY = cardRect.top + cardRect.height / 2;

    const mouseX = e.clientX - cardCenterX;
    const mouseY = e.clientY - cardCenterY;

    const rotateX = (mouseY / (cardRect.height / 2)) * -10;
    const rotateY = (mouseX / (cardRect.width / 2)) * 10;

    card.style.transform = `perspective(1000px) rotateX(${rotateX}deg) rotateY(${rotateY}deg)`;
}

function handleMouseLeave() {
    this.style.transform = 'perspective(1000px) rotateX(0) rotateY(0)';
}
