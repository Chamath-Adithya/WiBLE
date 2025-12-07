# Marketing Assets for WiBLE

Use these pre-written assets to promote your library. Consistency is key!

## 1. Blog Post Draft (Medium / Dev.to)

**Title:** Why I Built the Most Secure ESP32 Provisioning Library (WiBLE)

**Subtitle:** Stop hardcoding credentials. Start using ECDH + AES-256.

**Body:**

Connecting an ESP32 to WiFi shouldn't be hard, and it definitely shouldn't be insecure.

I've seen too many projects that:
1. Hardcode SSID/Password (inflexible)
2. Use a captive portal (clunky user experience)
3. Send credentials in plain text over Bluetooth (insecure!)

That's why I built **WiBLE**.

**What is WiBLE?**
WiBLE is a production-grade library that bridges the gap between Bluetooth Low Energy (BLE) and Wi-Fi. It allows you to configure your ESP32 device using a mobile app, securely and quickly.

**Key Features:**
*   **Security First:** Uses ECDH for key exchange and AES-256 for encryption. Your password is never exposed.
*   **Fast:** Provisioning takes less than 18 seconds.
*   **Reliable:** Built-in state machine and auto-reconnect logic.

**How to use it:**
```cpp
#include <WiBLE.h>

WiBLE provisioner;

void setup() {
  ProvisioningConfig config;
  config.securityLevel = SecurityLevel::SECURE;
  provisioner.begin(config);
  provisioner.startProvisioning();
}

void loop() {
  provisioner.loop();
}
```

Check it out on GitHub: [https://github.com/Chamath-Adithya/WiBLE](https://github.com/Chamath-Adithya/WiBLE)

#ESP32 #IoT #Embedded #OpenSource #Security

---

## 2. Social Media Templates

### Twitter / X
**Option 1 (The Problem/Solution):**
> Tired of insecure ESP32 provisioning? 🛑
>
> I built WiBLE to fix that. It uses ECDH + AES-256 to securely transfer WiFi credentials via BLE.
>
> 🚀 < 18s setup time
> 🔒 Military-grade security
> 📦 Production ready
>
> Check it out: https://github.com/Chamath-Adithya/WiBLE
>
> #ESP32 #IoT #Arduino #OpenSource

**Option 2 (The "Just Launched" vibe):**
> Just released WiBLE v2.0! 🎉
>
> The easiest way to provision ESP32 devices using Bluetooth. Now with full Arduino IDE support and improved security.
>
> ⭐ Stars appreciated!
> https://github.com/Chamath-Adithya/WiBLE

### Reddit (r/ESP32, r/Arduino)
**Title:** I built a secure BLE provisioning library for ESP32 (ECDH + AES-256) because I didn't like the existing options.

**Body:**
Hey everyone,

I've been working on a library called **WiBLE** to make ESP32 provisioning easier and more secure.

**Why?**
Most examples I found either sent passwords in plain text or were too complex to integrate. WiBLE aims to be the "drop-in" solution for secure provisioning.

**Features:**
*   **Security:** ECDH key exchange + AES-256 encryption.
*   **Speed:** Optimized MTU negotiation for fast transfer.
*   **Reliability:** Handles WiFi connection retries and auto-reconnects.

I'd love your feedback!
Repo: https://github.com/Chamath-Adithya/WiBLE

---

## 3. Email Template (for Newsletters)

**Subject:** New Open Source Library: WiBLE (Secure ESP32 Provisioning)

**Body:**
Hi [Name],

I wanted to share a new open-source library I released called WiBLE. It's designed to solve the problem of secure WiFi provisioning on ESP32 using Bluetooth LE.

It features ECDH key exchange and AES-256 encryption, making it a robust choice for production IoT devices.

I thought your readers might find it interesting!

Repo: https://github.com/Chamath-Adithya/WiBLE

Best,
Chamath
