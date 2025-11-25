/**
 * BLE_Client.ino
 * 
 * Demonstrates Method 2: GATT Client.
 * 
 * Scenario:
 * 1. Device is provisioned via WiBLE (GATT Server).
 * 2. Once provisioned (or concurrently), it scans for a specific "Sensor Device".
 * 3. Connects to the Sensor Device and reads a characteristic.
 * 
 * Note: This uses the native ESP32 BLE API for the Client role, co-existing with WiBLE.
 */

#include <WiBLE.h>
#include <BLEDevice.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// Target Service/Char UUIDs (Example: Heart Rate Monitor)
static BLEUUID serviceUUID("180d");
static BLEUUID charUUID("2a37");

bool doConnect = false;
bool connected = false;
bool doScan = false;
BLEAdvertisedDevice* myDevice;

// Client Callbacks
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
        Serial.println("[Client] Connected to Server");
    }
    void onDisconnect(BLEClient* pclient) {
        connected = false;
        Serial.println("[Client] Disconnected");
    }
};

// Connect to the found device
bool connectToServer() {
    Serial.print("[Client] Connecting to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);
    Serial.println("[Client] Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("[Client] Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("[Client] Failed to find our characteristic UUID: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("[Client] The characteristic value was: ");
        Serial.println(value.c_str());
    }

    connected = true;
    return true;
}

// Scan Callbacks
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("[Client] BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.is(serviceUUID)) {
            Serial.println("[Client] Found our device!"); 
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan = true;
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Client Example...");

    // 1. Initialize WiBLE (Server Role)
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Client_Node";
    provisioner.begin(config);

    // 2. Setup BLE Scanning (Client Role)
    // Note: WiBLE initializes BLEDevice, so we can just get the scan object
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    
    // Start scanning
    pBLEScan->start(5, false);
}

void loop() {
    // Handle WiBLE
    provisioner.loop();

    // Handle Client Logic
    if (doConnect == true) {
        if (connectToServer()) {
            Serial.println("[Client] We are now connected to the BLE Server.");
        } else {
            Serial.println("[Client] We have failed to connect to the server; there is nothin more we will do.");
        }
        doConnect = false;
    }
}
