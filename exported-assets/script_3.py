
import csv

# Create technology stack recommendations
tech_stack = [
    ["Layer", "Component", "Android", "iOS", "Cross-Platform", "Rationale"],
    
    ["Language", "Primary Language", "Kotlin", "Swift", "Kotlin Multiplatform", "Modern, type-safe, platform-native performance"],
    ["Language", "Async Model", "Coroutines", "async/await", "KMP Coroutines", "Structured concurrency, easy error handling"],
    
    ["BLE Stack", "Core BLE API", "android.bluetooth", "CoreBluetooth", "Custom abstraction", "Platform native APIs for best compatibility"],
    ["BLE Stack", "Helper Library", "Nordic Android BLE", "Not needed", "Custom wrapper", "Simplifies Android BLE complexity"],
    
    ["DI Framework", "Dependency Injection", "Hilt/Koin", "Swinject/Resolver", "Koin Multiplatform", "Simplifies testing, modular architecture"],
    
    ["State Management", "State Machine", "Tinder StateMachine", "Swift State Machine", "Custom FSM", "Type-safe state transitions"],
    ["State Management", "Reactive Streams", "Flow/LiveData", "Combine", "KMP Flow", "Observable state, lifecycle-aware"],
    
    ["Data Layer", "Local Storage", "DataStore/Room", "UserDefaults/CoreData", "KMP Settings/SQLDelight", "Persist configuration and state"],
    ["Data Layer", "Serialization", "Kotlinx.serialization", "Codable", "Kotlinx.serialization", "Type-safe JSON/protobuf handling"],
    
    ["Testing", "Unit Testing", "JUnit + MockK", "XCTest + Quick/Nimble", "kotlin.test", "Comprehensive test coverage"],
    ["Testing", "UI Testing", "Espresso", "XCUITest", "Platform-specific", "Automated UI validation"],
    ["Testing", "BLE Mocking", "MockBluetooth", "CoreBluetoothMock", "Custom mocks", "Test without physical devices"],
    
    ["Networking", "HTTP Client", "Ktor/Retrofit", "URLSession/Alamofire", "Ktor Client", "For cloud provisioning features"],
    
    ["Logging", "Logging Framework", "Timber", "OSLog", "Napier", "Structured, filterable logs"],
    ["Logging", "Analytics", "Firebase/Datadog", "Firebase/Datadog", "Cross-platform SDK", "Monitor production usage"],
    
    ["Security", "Encryption", "Tink", "CryptoKit", "Custom/Native", "Secure credential storage"],
    ["Security", "Key Storage", "Keystore", "Keychain", "Platform-specific", "Hardware-backed security"],
    
    ["Build System", "Build Tool", "Gradle", "Xcode/SPM", "Gradle", "Modern, flexible build configuration"],
    ["Build System", "CI/CD", "GitHub Actions", "GitHub Actions", "GitHub Actions", "Automated testing and deployment"],
    
    ["Documentation", "API Docs", "Dokka", "SwiftDoc/Jazzy", "Dokka", "Generate docs from code"],
    ["Documentation", "Architecture Docs", "Markdown + Mermaid", "Markdown + Mermaid", "Markdown + Mermaid", "Visual architecture diagrams"],
]

with open('ble_tech_stack.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerows(tech_stack)

print("Created technology stack recommendations")
print(f"Total components: {len(tech_stack) - 1}")
print("\nLayers covered:")
layers = {}
for row in tech_stack[1:]:
    layer = row[0]
    layers[layer] = layers.get(layer, 0) + 1
for layer, count in sorted(layers.items()):
    print(f"  - {layer}: {count} components")
