# References

This document lists the key resources and research papers that influenced the design and implementation of WiBLE.

## Bluetooth Low Energy (BLE) Security

1.  **Bluetooth Core Specification v5.3**
    *   *Bluetooth SIG*
    *   [Link](https://www.bluetooth.com/specifications/specs/)
    *   Defines the core protocols, including GAP, GATT, and SMP (Security Manager Protocol).

2.  **NIST Guide to Bluetooth Security (SP 800-121 Rev. 2)**
    *   *National Institute of Standards and Technology (NIST)*
    *   [Link](https://csrc.nist.gov/publications/detail/sp/800-121/rev-2/final)
    *   Provides recommendations for securing Bluetooth devices, including pairing methods and encryption requirements.

3.  **"BLE Security: A Comprehensive Guide"**
    *   *Various Authors*
    *   Overview of common BLE attacks (MITM, passive eavesdropping) and mitigation strategies (ECDH, OOB pairing).

## Cryptography

1.  **Curve25519: New Diffie-Hellman Speed Records**
    *   *Daniel J. Bernstein*
    *   [Link](https://cr.yp.to/ecdh.html)
    *   The basis for the ECDH key exchange used in WiBLE's secure mode.

2.  **Advanced Encryption Standard (AES)**
    *   *FIPS 197*
    *   [Link](https://csrc.nist.gov/publications/detail/fips/197/final)
    *   Standard for symmetric encryption used to protect credentials during provisioning.

## Provisioning Protocols

1.  **ESP-IDF Provisioning Protocol**
    *   *Espressif Systems*
    *   [Link](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html)
    *   Inspiration for the state machine and workflow of WiBLE.

2.  **Wi-Fi Easy Connect (DPP)**
    *   *Wi-Fi Alliance*
    *   [Link](https://www.wi-fi.org/discover-wi-fi/wi-fi-easy-connect)
    *   Modern standard for secure device provisioning.
