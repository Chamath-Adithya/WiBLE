# WiBLE AI Agent Development Guide

## 🤖 Building WiBLE with AI Assistance

This guide helps you leverage AI agents (Claude, ChatGPT, Copilot) to develop, debug, and extend WiBLE efficiently.

---

## Table of Contents

1. [AI-Assisted Development Workflow](#ai-assisted-development-workflow)
2. [Prompting Strategies](#prompting-strategies)
3. [Code Generation Prompts](#code-generation-prompts)
4. [Debugging with AI](#debugging-with-ai)
5. [Architecture Design Prompts](#architecture-design-prompts)
6. [Testing & Validation](#testing--validation)
7. [Documentation Generation](#documentation-generation)
8. [Best Practices](#best-practices)
9. [Common Pitfalls](#common-pitfalls)

---

## AI-Assisted Development Workflow

### Phase 1: Planning & Design (Week 1-2)

**Goal**: Create comprehensive architecture and component designs

#### 1.1 Architecture Design

**Prompt Template**:
```
I'm building a BLE provisioning library for ESP32 with these requirements:
- Secure WiFi credential transfer via BLE
- Support for 1000+ devices in production
- ECDH + AES-256 encryption
- Finite state machine for connection lifecycle
- Auto-reconnect with circuit breaker

Please design a layered architecture with:
1. Component breakdown
2. Interaction diagrams
3. Data flow
4. Security considerations
5. Performance optimization points

Use industry best practices from Nordic Semiconductor, Espressif, and enterprise IoT.
```

**AI Agent**: Claude Sonnet 4 or ChatGPT-4 with extended context

**Expected Output**: 
- 5-layer architecture diagram
- Component responsibility matrix
- State transition diagram
- Security architecture
- API design sketches

#### 1.2 API Design

**Prompt Template**:
```
Design a C++ API for the WiBLE library that is:
- Simple for beginners (5 lines to provision)
- Powerful for advanced users (full control)
- Type-safe with Result<T> pattern
- Event-driven with callbacks
- Header-only compatible

Provide:
1. Main class interface
2. Configuration structs
3. Callback signatures
4. Usage examples
5. Error handling patterns
```

**Expected Output**: Complete header file with documented methods

---

### Phase 2: Component Implementation (Week 3-14)

#### 2.1 State Machine Implementation

**Prompt Template**:
```
Implement a Finite State Machine for BLE provisioning with these states:
IDLE, BLE_ADVERTISING, BLE_CONNECTED, AUTHENTICATING, 
RECEIVING_CREDENTIALS, CONNECTING_WIFI, PROVISIONED, ERROR

Requirements:
- State transitions with guard conditions
- Timeout handling per state
- Event-driven architecture
- State history tracking
- State persistence to NVS

Provide complete C++ implementation with:
1. StateManager class
2. State transition table
3. Event handlers
4. Timeout management
5. Unit tests

Use modern C++17 features.
```

**AI Tools**: 
- Code generation: Claude Code, Cursor, GitHub Copilot
- Testing: Claude for test case generation

#### 2.2 Security Manager Implementation

**Prompt Template**:
```
Implement a SecurityManager class for WiBLE using mbedTLS library with:

Features:
- ECDH P-256 key exchange
- AES-256-CBC encryption
- HMAC-SHA256 message authentication
- Secure random number generation
- Session key management with expiry

Requirements:
- Handle mbedTLS initialization/cleanup properly
- Prevent memory leaks
- Secure memory wiping after use
- Thread-safe operations
- Support for multiple security levels

Provide:
1. SecurityManager.h header
2. Key exchange flow
3. Encryption/decryption methods
4. Session management
5. Example usage
```

**Iteration Approach**:
1. Generate initial implementation
2. Ask AI to review for security vulnerabilities
3. Request optimization suggestions
4. Generate unit tests
5. Ask for edge case handling

#### 2.3 BLE Manager Implementation

**Prompt Template**:
```
Implement a BLEManager class for ESP32 that handles:

Core Features:
- GATT service/characteristic setup
- MTU negotiation (up to 512 bytes)
- Connection parameter optimization
- Operation queue (serialized GATT operations)
- Chunked data transfer for large payloads
- RSSI monitoring

Requirements:
- Use ESP32 BLE library (NimBLE or Bluedroid)
- Handle connection lifecycle events
- Prevent race conditions with operation queue
- Support multiple simultaneous connections
- Clean resource management

Platform: ESP32/ESP32-C6
Framework: Arduino

Provide complete implementation with callback handlers.
```

---

### Phase 3: Integration & Testing (Week 15-19)

#### 3.1 Integration Testing with AI

**Prompt for Test Generation**:
```
Generate comprehensive integration tests for WiBLE provisioning flow:

Test Scenarios:
1. Happy path: BLE connect → authenticate → provision → WiFi connect
2. Authentication failure recovery
3. WiFi connection timeout with retry
4. BLE disconnection during provisioning
5. Invalid credentials handling
6. Multiple concurrent connections
7. Power loss during provisioning
8. Memory leak detection

Use Google Test framework. Include:
- Test fixtures
- Mock BLE/WiFi adapters
- Assertion helpers
- Performance benchmarks
```

#### 3.2 Debugging Assistance

**Debugging Prompt Template**:
```
I'm encountering this issue in WiBLE:

Error: [Paste error message or describe behavior]

Code Context:
[Paste relevant code section - 20-50 lines]

Environment:
- Board: ESP32-DevKitC
- Framework: Arduino
- WiBLE Version: 2.0.0
- Steps to reproduce: [List steps]

Expected Behavior: [Describe]
Actual Behavior: [Describe]

Please:
1. Identify the root cause
2. Explain why it's happening
3. Provide a fix with explanation
4. Suggest preventive measures
5. Add logging for future debugging
```

**Advanced Debugging**:
```
Analyze this memory leak in WiBLE BLEManager:

Heap before provisioning: 250KB
Heap after provisioning: 180KB
Heap after 10 cycles: 50KB

Code: [Paste BLEManager implementation]

Using ESP32 heap tracing, I see:
[Paste heap trace output]

Please:
1. Identify leak sources
2. Explain object lifecycle issues
3. Provide fixed implementation
4. Add RAII wrappers where needed
5. Suggest memory profiling strategy
```

---

### Phase 4: Optimization (Week 20-22)

#### 4.1 Performance Optimization

**Prompt Template**:
```
Optimize WiBLE provisioning time. Current performance:

Current Metrics:
- Total provisioning: 35 seconds
- BLE connection: 8s
- Authentication: 5s
- Credential transfer: 3s
- WiFi connection: 19s

Target: < 18 seconds total

Code: [Paste relevant sections]

Please analyze and suggest optimizations for:
1. BLE connection speed (pre-scanning, fast intervals)
2. Crypto performance (hardware acceleration)
3. WiFi connection (channel caching, fast connect)
4. Data transfer (MTU optimization, batching)
5. Memory allocation (pooling, static allocation)

Provide specific code changes with benchmarks.
```

#### 4.2 Power Optimization

**Prompt Template**:
```
Reduce WiBLE power consumption for battery-powered devices:

Current Consumption:
- BLE advertising: 30mA
- BLE connected: 25mA
- WiFi active: 120mA
- Average: 85mA

Target: < 50mA average

Constraints:
- Must maintain < 18s provisioning time
- WiFi connection required for 10s/hour
- BLE advertising during setup only

Please suggest:
1. Power-efficient BLE connection parameters
2. Dynamic power mode switching
3. Optimal advertising intervals
4. WiFi power save modes
5. Deep sleep integration strategy

Provide implementation with power measurements.
```

---

## Prompting Strategies

### 1. Iterative Refinement

**Initial Prompt** (Broad):
```
Create a WiFi manager class for ESP32 with connection handling
```

**Refinement 1** (Specific):
```
Create a WiFi manager class for ESP32 that:
- Handles WPA2/WPA3 networks
- Implements exponential backoff retry (max 3 attempts)
- Supports static IP and DHCP
- Monitors RSSI and connection quality
- Persists credentials to NVS
- Uses callback-based event system

Provide complete implementation with error handling.
```

**Refinement 2** (Edge Cases):
```
Extend the WiFi manager to handle:
- Hidden networks
- 5GHz band support
- Captive portal detection
- Concurrent WiFi and BLE operation
- Connection quality scoring (0-100)
- Auto-reconnect with circuit breaker

Add unit tests for all edge cases.
```

### 2. Context Loading

**Effective Context Structure**:
```
PROJECT CONTEXT:
- Library: WiBLE v2.0
- Target: ESP32/ESP32-C6
- Framework: Arduino
- Language: C++17
- Dependencies: mbedTLS, ESP32 BLE, ESP32 WiFi

CURRENT STATE:
- Implemented: StateManager, SecurityManager (partially)
- In Progress: BLEManager
- Pending: WiFiManager, Orchestrator

ARCHITECTURE:
[Paste architecture diagram or description]

CODING STANDARDS:
- Google C++ Style Guide
- RAII for resource management
- Result<T> for error handling
- Smart pointers (unique_ptr, shared_ptr)
- Structured logging

TASK:
Implement [specific component] with [specific requirements]
```

### 3. Chain-of-Thought Prompting

**Prompt Structure**:
```
Let's implement the BLE operation queue step by step:

Step 1: Analyze the problem
- Why do we need serialized BLE operations?
- What happens with concurrent GATT operations?
- What are the failure modes?

Step 2: Design the solution
- Queue data structure
- Operation lifecycle
- Error handling strategy
- Thread safety mechanisms

Step 3: Implement
- Queue class with mutex
- Operation enqueue/dequeue
- Execution logic
- Timeout handling

Step 4: Test
- Concurrent operation scenarios
- Timeout edge cases
- Memory leak prevention
- Performance benchmarks

Please work through each step with detailed explanations.
```

---

## Code Generation Prompts

### Component Templates

#### State Manager
```
Generate a StateManager class for WiBLE with:

States: [List all states]
Events: [List all events]
Transitions: [Describe valid transitions]

Features:
- Guard conditions for transitions
- State entry/exit callbacks
- Timeout per state (configurable)
- State history (circular buffer, last 20 states)
- State persistence to NVS
- Thread-safe state access

Implementation requirements:
- Use std::map for transition table
- Semaphore for thread safety
- Observer pattern for state change notifications
- State<->String conversion utilities

Provide:
1. Complete header file with Doxygen comments
2. Implementation skeleton
3. Usage example
4. Unit test structure
```

#### Security Manager
```
Generate a SecurityManager class implementing:

Algorithms:
- ECDH P-256 (secp256r1) key exchange
- AES-256-CBC encryption with PKCS#7 padding
- HMAC-SHA256 for message authentication
- HKDF for key derivation

Key Management:
- Generate ephemeral ECDH keypairs
- Compute shared secret from peer public key
- Derive session keys (encryption + MAC)
- Session timeout and renewal

Security Features:
- Constant-time comparison (timing attack prevention)
- Secure memory wiping
- Random IV generation per message
- Nonce to prevent replay attacks

mbedTLS Integration:
- Proper context initialization/cleanup
- Error handling for all mbedTLS calls
- Memory allocation tracking

Provide complete implementation with security best practices.
```

### Protocol Design

```
Design a binary protocol for WiBLE credential transfer:

Requirements:
- Compact (minimize BLE transfer time)
- Versioned (for future compatibility)
- Secure (encrypted with AES-256)
- Authenticated (HMAC)
- Error detection (checksum)

Message Structure:
- Header: version, message type, length
- Payload: encrypted credentials
- Footer: HMAC, checksum

Message Types:
1. KEY_EXCHANGE (ECDH public key)
2. CREDENTIALS (encrypted SSID + password)
3. STATUS (provisioning progress)
4. ERROR (error codes)
5. ACK (acknowledgment)

Provide:
1. Binary protocol specification
2. C++ serialization/deserialization code
3. Python code for mobile app testing
4. Protocol documentation
```

---

## Debugging with AI

### Memory Issues

**Heap Leak Debug Prompt**:
```
Debug memory leak in WiBLE:

Symptoms:
- Heap decreases by ~5KB per provisioning cycle
- After 20 cycles, device crashes with heap exhaustion
- Leak occurs even on successful provisioning

Code: [Paste BLEManager and SecurityManager]

Heap trace output:
[Paste ESP32 heap trace]

Valgrind output (if applicable):
[Paste valgrind report]

Please:
1. Identify leak sources using static analysis
2. Check for missing delete/free calls
3. Verify RAII usage
4. Look for circular references in shared_ptr
5. Suggest memory profiling strategy
6. Provide fixed implementation
```

### Crash Analysis

**Stack Trace Debug Prompt**:
```
Analyze this crash in WiBLE:

Backtrace:
```
#0  0x400d1234 in SecurityManager::encrypt()
#1  0x400d5678 in BLEManager::handleWrite()
#2  0x400d9abc in BLECharacteristicCallbacks::onWrite()
#3  0x400e1def in nimble_gatts_chr_updated()
```

Context:
- Crash occurs randomly during credential transfer
- Only on ESP32 (works on ESP32-C6)
- Happens when MTU > 256

Core dump:
[Paste decoded core dump if available]

Code sections:
[Paste relevant code]

Please analyze:
1. Root cause (stack corruption? race condition?)
2. Why only on original ESP32?
3. Why only with large MTU?
4. Potential fixes with trade-offs
5. Additional logging to add
```

---

## Architecture Design Prompts

### System Design

```
Design a scalable architecture for WiBLE to support:

Scale Requirements:
- 10,000+ devices per deployment
- 100+ concurrent provisioning sessions
- Multi-tenancy (different organizations)
- Geo-distributed deployments
- 99.9% uptime SLA

New Features Needed:
- Cloud-based provisioning (backup to BLE)
- Remote device management
- Bulk provisioning
- Provisioning analytics
- A/B firmware testing

Constraints:
- Maintain backward compatibility with v2.0
- Keep embedded code < 200KB flash
- Support ESP32, ESP32-C3, ESP32-C6
- Mobile apps (iOS, Android, Web)

Please provide:
1. High-level architecture diagram
2. Component breakdown (embedded + cloud + mobile)
3. API specifications
4. Database schema (if needed)
5. Deployment strategy
6. Migration path from v2.0
```

### Security Architecture

```
Design a comprehensive security architecture for WiBLE v3.0:

Threat Model:
- Man-in-the-middle attacks
- Replay attacks
- Credential theft from device storage
- Rogue access point attacks
- Supply chain attacks (firmware tampering)
- Cloud API compromise

Security Requirements:
- End-to-end encryption (mobile → device → cloud)
- Device authentication (prevent impersonation)
- Firmware signing and verification
- Secure boot
- Key rotation every 30 days
- Audit logging
- Compliance: GDPR, SOC2, ISO27001

Please design:
1. Complete threat model with mitigations
2. Cryptographic protocols (key exchange, encryption, signing)
3. PKI architecture if needed
4. Secure element integration (ATECC608, SE050)
5. Key management system
6. Incident response procedures
```

---

## Testing & Validation

### Test Generation

**Comprehensive Test Suite Prompt**:
```
Generate complete test suite for WiBLE with 80%+ coverage:

Components to Test:
1. StateManager (state transitions, timeouts, persistence)
2. SecurityManager (encryption, key exchange, session mgmt)
3. BLEManager (GATT operations, MTU, chunking)
4. WiFiManager (connection, retry, auto-reconnect)
5. Integration (end-to-end provisioning)

Test Types:
- Unit tests (Google Test)
- Integration tests (with mocks)
- Property-based tests (random inputs)
- Stress tests (1000+ cycles)
- Regression tests (known bugs)

For each component provide:
1. Test fixtures
2. Happy path tests
3. Error condition tests
4. Edge case tests (timeouts, disconnections)
5. Performance benchmarks
6. Memory leak checks

Include:
- Mock objects for BLE/WiFi
- Test helpers and utilities
- CI/CD integration (GitHub Actions)
```

### Fuzzing Strategy

```
Design fuzzing strategy for WiBLE security:

Fuzz Targets:
1. BLE GATT characteristic writes (malformed data)
2. Encrypted credential packets (corrupted ciphertext)
3. Key exchange messages (invalid public keys)
4. Protocol message parsing (buffer overflows)

Fuzzing Tools:
- AFL++
- libFuzzer
- Honggfuzz

Please provide:
1. Fuzz harness code for each target
2. Corpus seeds (valid inputs)
3. Dictionary of interesting values
4. Sanitizer flags (ASAN, UBSAN, MSAN)
5. Fuzzing configuration
6. Crash triage workflow
```

---

## Documentation Generation

### API Documentation

**Prompt for API Docs**:
```
Generate comprehensive API documentation for WiBLE:

Style: Doxygen-compatible C++ comments

For each public method provide:
- Brief description (one line)
- Detailed description (paragraph)
- Parameters (@param with types and descriptions)
- Return value (@return with all possible values)
- Exceptions/error codes (@throws or @note)
- Usage example (@code block)
- See also (@see related methods)
- Thread safety notes
- Performance characteristics (time/space complexity)

Example method to document:
```cpp
Result<bool> connect(const String& ssid, const String& password);
```

Please generate docs following Doxygen best practices.
Generate docs for all methods in WiBLE.h
```

### Tutorial Generation

```
Create a step-by-step tutorial: "Building a Smart Thermostat with WiBLE"

Target Audience: Intermediate Arduino developers

Tutorial Structure:
1. Introduction (what we'll build, prerequisites)
2. Hardware setup (ESP32, DHT22 sensor, relay)
3. WiBLE integration (basic provisioning)
4. Sensor reading and control
5. Cloud integration (MQTT to AWS IoT)
6. Mobile app (Flutter) for control
7. OTA updates via WiBLE
8. Troubleshooting common issues

For each section provide:
- Clear explanations
- Complete code with comments
- Wiring diagrams (Fritzing)
- Screenshots/photos
- Debugging tips

Writing style: Friendly, encouraging, assume basic C++ knowledge
```

---

## Best Practices

### 1. Incremental Development

✅ **Do**:
```
Step 1: Generate basic class structure
Step 2: Review and refine API design
Step 3: Implement core functionality
Step 4: Generate unit tests
Step 5: Review for edge cases
Step 6: Optimize performance
Step 7: Add comprehensive logging
```

❌ **Don't**:
```
Generate entire 5000-line codebase in one prompt
(Results in inconsistencies, missing edge cases, poor integration)
```

### 2. Context Management

✅ **Do**:
- Start each session with project context
- Reference previously generated code explicitly
- Keep related components in same conversation
- Use artifacts/files to maintain state

❌ **Don't**:
- Assume AI remembers previous conversations
- Mix unrelated components in one conversation
- Generate code without architecture context

### 3. Code Review with AI

**Prompt Template**:
```
Review this WiBLE component for:

Code Quality:
- [ ] Follows Google C++ Style Guide
- [ ] Proper error handling (no exceptions)
- [ ] Resource management (RAII, smart pointers)
- [ ] Thread safety where needed
- [ ] Memory efficiency (no unnecessary copies)

Security:
- [ ] Input validation
- [ ] Secure crypto usage
- [ ] No hardcoded secrets
- [ ] Proper key management
- [ ] Memory wiping after use

Performance:
- [ ] No unnecessary allocations
- [ ] Efficient algorithms (O(n) vs O(n²))
- [ ] Minimal BLE/WiFi operations
- [ ] Caching where appropriate

Maintainability:
- [ ] Clear naming conventions
- [ ] Comprehensive comments
- [ ] Modular design
- [ ] Testable structure

Code: [Paste component implementation]

Please provide:
1. Issues found (with severity: critical/major/minor)
2. Specific recommendations
3. Refactored code snippets
4. Performance estimates
```

### 4. Version Control Integration

```
Generate a Git commit message for this WiBLE change:

Changes Made:
- Added circuit breaker to WiFiManager
- Implemented exponential backoff retry
- Fixed memory leak in BLE operation queue
- Updated unit tests for new retry logic

Files Changed:
- src/WiFiManager.cpp (+150, -30)
- src/WiFiManager.h (+25, -5)
- src/BLEManager.cpp (+10, -15)
- test/test_wifi_manager.cpp (+80, -0)

Impact:
- Improves connection reliability by 40%
- Reduces power consumption by 15%
- Fixes issue #123 (provisioning fails after 5 attempts)

Please generate:
1. Commit message (conventional commits format)
2. PR description for GitHub
3. Release notes snippet
4. Migration guide if breaking changes
```

---

## Common Pitfalls

### 1. Over-Reliance on AI

❌ **Problem**: Accepting all generated code without review
✅ **Solution**: 
- Always review for security issues
- Test thoroughly (unit + integration)
- Verify memory management
- Check error handling
- Validate against requirements

### 2. Insufficient Context

❌ **Problem**: "Generate a BLE manager" (too vague)
✅ **Solution**: Provide:
- Target platform (ESP32-C6)
- Framework (Arduino)
- Requirements (MTU negotiation, operation queue)
- Constraints (< 50KB RAM)
- Integration points (StateManager, SecurityManager)

### 3. Ignoring Edge Cases

❌ **Problem**: Only test happy path
✅ **Solution**: Ask AI to generate edge cases:
```
List all edge cases for WiFiManager.connect():
- Invalid credentials
- Network not found
- Weak signal (RSSI < -80)
- DHCP timeout
- Concurrent connection attempts
- Connection during deep sleep
- Memory exhaustion during connection
- [AI will generate more]

For each edge case, provide:
1. Test case
2. Expected behavior
3. Error handling code
```

### 4. Architecture Drift

❌ **Problem**: Components don't integrate well
✅ **Solution**: 
- Maintain architecture document
- Reference in every prompt
- Regular integration reviews
- Generate integration tests early

---

## Advanced AI Workflows

### 1. Multi-Agent Development

**Use Case**: Complex features requiring multiple perspectives

**Setup**:
- Agent 1 (Claude): Architecture design, security review
- Agent 2 (ChatGPT): Code generation, optimization
- Agent 3 (GitHub Copilot): Real-time code completion

**Workflow**:
```
1. Claude: Design architecture for feature X
2. ChatGPT: Generate implementation based on Claude's design
3. Claude: Security review of ChatGPT's code
4. ChatGPT: Fix issues found by Claude
5. Copilot: Complete tests and documentation
6. Claude: Final integration review
```

### 2. Continuous Improvement Loop

```
Weekly AI-Assisted Improvement Cycle:

Monday: Performance profiling
- Prompt: "Analyze WiBLE performance bottlenecks from this profiler output: [data]"

Tuesday: Security audit
- Prompt: "Perform security audit on WiBLE using OWASP IoT Top 10: [code]"

Wednesday: Code quality review
- Prompt: "Review WiBLE for technical debt using these metrics: [metrics]"

Thursday: Test coverage analysis
- Prompt: "Identify untested code paths in WiBLE: [coverage report]"

Friday: Documentation update
- Prompt: "Generate updated docs for changed APIs: [git diff]"
```

### 3. AI-Powered Code Migration

```
Migrate WiBLE from Arduino to ESP-IDF:

Current State:
- Framework: Arduino
- BLE: ESP32 BLE library
- WiFi: WiFi.h
- Storage: Preferences

Target State:
- Framework: ESP-IDF v5.1
- BLE: NimBLE
- WiFi: esp_wifi
- Storage: NVS directly

Please provide:
1. API mapping document (Arduino → ESP-IDF)
2. Migration strategy (incremental vs. big bang)
3. Compatibility layer for smooth transition
4. Migrated code for each component
5. Testing strategy to ensure equivalence
6. Performance comparison
```

---

## Resources & Tools

### AI Tools for WiBLE Development

1. **Claude** (Anthropic)
   - Best for: Architecture design, security review
   - Context: 200K tokens
   - Strengths: Long-form documentation, complex reasoning

2. **ChatGPT-4** (OpenAI)
   - Best for: Code generation, debugging
   - Context: 128K tokens
   - Strengths: Fast iteration, broad knowledge

3. **GitHub Copilot**
   - Best for: Real-time code completion
   - Integration: VSCode, Vim, IDEs
   - Strengths: Context-aware suggestions

4. **Cursor**
   - Best for: AI-native IDE experience
   - Features: Multi-file editing, codebase Q&A
   - Strengths: Workspace understanding

5. **Tabnine**
   - Best for: Team collaboration
   - Features: Team-trained models
   - Strengths: Consistency across team

### Prompt Libraries

- **Awesome ChatGPT Prompts**: github.com/f/awesome-chatgpt-prompts
- **Prompt Engineering Guide**: promptingguide.ai
- **OpenAI Cookbook**: github.com/openai/openai-cookbook

---

## Conclusion

AI agents are powerful tools for accelerating WiBLE development, but they work best when:

1. **Provided with clear context** - Architecture, requirements, constraints
2. **Asked specific questions** - Detailed prompts get better results
3. **Used iteratively** - Refine and improve over multiple interactions
4. **Verified rigorously** - Always review, test, and validate
5. **Combined with human expertise** - AI augments, doesn't replace

**Golden Rule**: AI helps you build faster, but you're still the architect.

---

**Next Steps**:
1. Start with architecture design prompts
2. Generate core components incrementally
3. Review and test each component
4. Integrate and iterate
5. Optimize and document

Happy building! 🚀

---

**Questions or improvements?** Open an issue on GitHub or email: ai-dev@wible.dev