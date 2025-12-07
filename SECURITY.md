# Security Policy

## Supported Versions

Use this section to tell people about which versions of your project are
currently being supported with security updates.

| Version | Supported          |
| ------- | ------------------ |
| 2.0.x   | :white_check_mark: |
| 1.x.x   | :x:                |

## Reporting a Vulnerability

We take the security of WiBLE seriously. If you discover a security vulnerability, please follow these steps:

1.  **Do NOT open a public issue.**
2.  Send an email to **achamth1@gmail.com** with a description of the vulnerability.
3.  Include steps to reproduce the issue.
4.  We will acknowledge your email within 48 hours.
5.  We will provide a fix or workaround as soon as possible.

## Security Features

WiBLE is designed with security in mind:
*   **ECDH Key Exchange**: Uses Curve25519 (or P-256 depending on config) for secure key agreement.
*   **AES-256 Encryption**: All sensitive data (SSID, Password) is encrypted.
*   **No Plaintext**: Credentials are never sent in plain text over the air.

Thank you for helping keep WiBLE secure!
