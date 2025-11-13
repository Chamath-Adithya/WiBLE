
import csv

# Create a detailed list of best practices and design patterns
best_practices = [
    ["Category", "Practice/Pattern", "Description", "Implementation Example"],
    
    ["Architecture", "Layered Architecture", "Separate concerns into distinct layers (UI, Business Logic, Data)", "presentation/ → domain/ → data/ folders"],
    ["Architecture", "Dependency Inversion", "Depend on abstractions, not concrete implementations", "interface BleAdapter { ... } implemented by AndroidBleAdapter"],
    ["Architecture", "Repository Pattern", "Abstract data sources behind repository interfaces", "ProvisioningRepository handles all data operations"],
    ["Architecture", "Clean Architecture", "Independent of frameworks, testable, UI-agnostic", "Core business logic has no Android/iOS dependencies"],
    
    ["State Management", "Finite State Machine", "Model connection states explicitly with FSM", "Idle → Scanning → Connecting → Connected → Provisioning"],
    ["State Management", "Event-Driven Design", "Use events/callbacks for state transitions", "onConnectionEstablished(), onProvisioningComplete()"],
    ["State Management", "State Persistence", "Save state for recovery after crashes", "Save to DataStore/UserDefaults during transitions"],
    
    ["Concurrency", "Coroutines/Async-Await", "Use structured concurrency for async operations", "suspend fun connectDevice(), async { scanDevices() }"],
    ["Concurrency", "Flow/Reactive Streams", "Observable data streams for continuous updates", "StateFlow<ConnectionState>, SharedFlow<BleEvent>"],
    ["Concurrency", "Single-threaded Queue", "Serialize BLE operations to prevent race conditions", "Queue all GATT ops, process one at a time"],
    
    ["Error Handling", "Result/Either Types", "Type-safe error handling without exceptions", "sealed class Result<T> { Success, Error }"],
    ["Error Handling", "Retry with Backoff", "Exponential backoff for transient failures", "Retry 3 times: 1s, 2s, 4s delays"],
    ["Error Handling", "Circuit Breaker", "Fail fast after repeated failures", "Open circuit after 5 consecutive failures"],
    ["Error Handling", "Graceful Degradation", "Provide fallback behavior when features fail", "If BLE fails, offer manual WiFi configuration"],
    
    ["Data Transfer", "MTU Negotiation", "Request maximum MTU early in connection", "requestMtu(512) after connection established"],
    ["Data Transfer", "Batching & Compression", "Combine small messages, compress when possible", "Send 10 readings in one packet instead of 10 packets"],
    ["Data Transfer", "Notifications over Polling", "Use BLE notifications instead of reading repeatedly", "Enable notifications on characteristics"],
    ["Data Transfer", "Connection Parameters", "Optimize interval, latency, timeout for use case", "Fast: 7.5ms interval, Battery: 100ms interval"],
    
    ["Security", "Encryption at Rest", "Encrypt stored credentials and keys", "Use Android Keystore / iOS Keychain"],
    ["Security", "Secure Pairing", "Use LE Secure Connections with proper authentication", "Numeric Comparison or Passkey Entry methods"],
    ["Security", "Certificate Pinning", "Validate BLE device identity with certificates", "Pre-shared device certificates checked on connection"],
    ["Security", "OOB Pairing", "Use NFC/QR for secure out-of-band key exchange", "Scan QR code to get pairing key"],
    
    ["Testing", "Unit Tests", "Test individual components in isolation with mocks", "Test ConnectionManager with mock BleAdapter"],
    ["Testing", "Integration Tests", "Test component interactions", "Test full provisioning flow end-to-end"],
    ["Testing", "Property-Based Tests", "Test with random inputs to find edge cases", "Generate random WiFi credentials"],
    ["Testing", "BLE Device Emulation", "Use virtual BLE devices for testing", "Use Blueooth HCI emulator on desktop"],
    
    ["Observability", "Structured Logging", "Use consistent log format with context", "log.info('connection.established', deviceId, rssi)"],
    ["Observability", "Metrics Collection", "Track key performance indicators", "connection_duration, provisioning_success_rate"],
    ["Observability", "Distributed Tracing", "Trace requests across components", "Trace ID through BLE → App → Cloud"],
    ["Observability", "Health Checks", "Expose health status for monitoring", "isBluetoothEnabled(), isLocationEnabled()"],
    
    ["Performance", "Lazy Initialization", "Initialize expensive resources only when needed", "Delay BLE adapter creation until first scan"],
    ["Performance", "Object Pooling", "Reuse objects instead of creating new ones", "Pool for byte buffers used in data transfer"],
    ["Performance", "Memory Management", "Monitor and limit memory usage", "Limit scan result cache to 100 devices"],
    ["Performance", "Power Optimization", "Minimize BLE operations to save battery", "Batch writes, increase connection interval when idle"],
    
    ["API Design", "Builder Pattern", "Fluent API for configuration", "BleProvisioner.builder().timeout(30).security(HIGH).build()"],
    ["API Design", "Factory Pattern", "Abstract object creation", "BleAdapterFactory.create(platform)"],
    ["API Design", "Strategy Pattern", "Pluggable algorithms", "SecurityStrategy: interface with JustWorks, Passkey, OOB"],
    ["API Design", "Facade Pattern", "Simple interface hiding complexity", "ProvisioningFacade with provision() method"],
    
    ["Configuration", "Externalized Config", "Keep configuration outside code", "Load from JSON/YAML configuration files"],
    ["Configuration", "Environment-based", "Different configs for dev/staging/prod", "Use build variants/schemes for configs"],
    ["Configuration", "Feature Flags", "Toggle features without code changes", "Enable/disable mesh support via config"],
    ["Configuration", "Injectable Dependencies", "Pass dependencies explicitly", "Constructor injection for all dependencies"],
]

with open('ble_best_practices.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerows(best_practices)

print("Created best practices guide")
print(f"Total practices: {len(best_practices) - 1}")
print("\nCategories covered:")
categories = set(row[0] for row in best_practices[1:])
for cat in sorted(categories):
    count = sum(1 for row in best_practices[1:] if row[0] == cat)
    print(f"  - {cat}: {count} practices")
