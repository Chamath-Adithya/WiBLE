# WiBLE v2.0.0 - The Security Update

We are proud to announce the release of WiBLE v2.0.0! This release focuses on **security**, **stability**, and **developer experience**.

## 🔒 Major Features
*   **ECDH Key Exchange**: Securely swap keys using Elliptic Curve Diffie-Hellman.
*   **AES-256 Encryption**: All credentials are encrypted in transit.
*   **New Security Levels**: Choose between `NONE`, `BASIC`, `SECURE`, and `ENTERPRISE`.

## 🚀 Improvements
*   **Faster Provisioning**: Optimized BLE MTU negotiation reduces setup time to < 18s.
*   **Auto-Reconnect**: Smarter logic to handle WiFi dropouts.
*   **Better Logging**: Structured logs for easier debugging.

## 📚 Documentation & Examples
*   **New Example**: `SecureProvisioning` demonstrates the new security features.
*   **New Example**: `CustomDataProvisioning` shows how to send API keys and configs.
*   **Architecture Diagram**: Visual overview of the library internals.
*   **SEO Optimized**: Better discoverability in Arduino Library Manager.

## 🐛 Bug Fixes
*   Fixed race condition in BLE state machine.
*   Resolved memory leak during repeated connection attempts.

## 🤝 Contributors
*   @Chamath-Adithya

---

**Upgrade Guide:**
This is a breaking change from v1.x. Please update your `ProvisioningConfig` to include the new `securityLevel` parameter.
