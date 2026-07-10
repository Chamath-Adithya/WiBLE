# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Timeout logic to `StateManager` to handle provisioning and connection timeouts.
- Implementation for `BLEManager` operation queue to serialize GATT operations.
- Full WiFi proxy method implementation in `WiBLE.cpp` (`getIPAddress`, `scanWiFiNetworks`, `connectWiFi`, etc.).
- Complete manual provisioning and clear provisioning functions.
- Missing community files: `SUPPORT.md`, `ROADMAP.md`, `CHANGELOG.md`, `FAQ.md`.

### Fixed
- Missing `TODO` sections throughout the core C++ files have been resolved.
