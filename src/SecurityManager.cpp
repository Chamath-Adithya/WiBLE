/**
 * SecurityManager.cpp - Security Layer Implementation
 */

#include "SecurityManager.h"
#include "utils/LogManager.h"
#include <Preferences.h> // For secure storage on ESP32

namespace WiBLE {

// ============================================================================
// SECURITY MANAGER IMPLEMENTATION
// ============================================================================

SecurityManager::SecurityManager() 
    : initialized(false), 
      sessionEstablished(false), 
      sessionStartTime(0) {
}

SecurityManager::~SecurityManager() {
    cleanup();
}

bool SecurityManager::initialize(const SecurityConfig& config) {
    this->config = config;
    
    if (!initializeMbedTLS()) {
        LogManager::error("Failed to initialize mbedTLS");
        return false;
    }
    
    initialized = true;
    LogManager::info("SecurityManager initialized");
    return true;
}

void SecurityManager::cleanup() {
    cleanupMbedTLS();
    keyPair.clear();
    sessionKey.clear();
    initialized = false;
    sessionEstablished = false;
}

void SecurityManager::reset() {
    sessionEstablished = false;
    sessionKey.clear();
    // Re-generate keys for new session if perfect forward secrecy is enabled
    if (config.enablePerfectForwardSecrecy) {
        generateKeyPair();
    }
}

// ============================================================================
// KEY EXCHANGE (ECDH)
// ============================================================================

bool SecurityManager::generateKeyPair() {
    if (!initialized) return false;
    
    int ret = mbedtls_ecdh_gen_public(&ecdhContext.grp, &ecdhContext.d, &ecdhContext.Q, 
                                     mbedtls_ctr_drbg_random, &ctrDrbgContext);
    if (ret != 0) {
        LogManager::error("ECDH gen public failed: " + String(ret));
        return false;
    }
    
    // Export public key
    uint8_t buf[128];
    size_t olen = 0;
    ret = mbedtls_mpi_write_binary(&ecdhContext.Q.X, buf, mbedtls_mpi_size(&ecdhContext.grp.P));
    
    if (ret != 0) {
        LogManager::error("Failed to export public key");
        return false;
    }
    
    keyPair.publicKey.assign(buf, buf + mbedtls_mpi_size(&ecdhContext.grp.P));
    keyPair.generatedAt = millis();
    keyPair.isValid = true;
    
    return true;
}

std::vector<uint8_t> SecurityManager::getPublicKey() const {
    return keyPair.publicKey;
}

bool SecurityManager::computeSharedSecret(const std::vector<uint8_t>& peerPublicKey) {
    if (!initialized || peerPublicKey.empty()) return false;
    
    // Import peer public key
    // Note: This is a simplified version. In real mbedTLS usage, we need to handle MPI parsing carefully.
    // For this implementation, we assume raw binary format of X coordinate (Curve25519 style)
    
    int ret = mbedtls_mpi_read_binary(&ecdhContext.Qp.X, peerPublicKey.data(), peerPublicKey.size());
    if (ret != 0) return false;
    
    ret = mbedtls_mpi_lset(&ecdhContext.Qp.Z, 1);
    if (ret != 0) return false;
    
    ret = mbedtls_ecdh_compute_shared(&ecdhContext.grp, &ecdhContext.z, &ecdhContext.Qp, &ecdhContext.d,
                                      mbedtls_ctr_drbg_random, &ctrDrbgContext);
                                      
    if (ret != 0) {
        LogManager::error("ECDH compute shared failed: " + String(ret));
        return false;
    }
    
    // Export shared secret
    uint8_t buf[128];
    ret = mbedtls_mpi_write_binary(&ecdhContext.z, buf, mbedtls_mpi_size(&ecdhContext.grp.P));
    
    sharedSecret.assign(buf, buf + mbedtls_mpi_size(&ecdhContext.grp.P));
    
    return true;
}

bool SecurityManager::deriveSessionKey() {
    if (sharedSecret.empty()) return false;
    
    // Use SHA-256 to derive session key from shared secret
    sessionKey.key = hash(sharedSecret);
    sessionKey.iv = generateIV();
    sessionKey.createdAt = millis();
    sessionKey.expiresAt = millis() + config.sessionTimeoutMs;
    sessionKey.sessionId = generateSessionId();
    
    // Initialize AES contexts
    setAESKey(sessionKey.key);
    
    sessionEstablished = true;
    sessionStartTime = millis();
    
    // Wipe shared secret
    SecurityUtils::secureWipe(sharedSecret);
    
    return true;
}

// ============================================================================
// ENCRYPTION / DECRYPTION
// ============================================================================

EncryptedMessage SecurityManager::encrypt(const std::vector<uint8_t>& plaintext) {
    EncryptedMessage msg;
    if (!sessionEstablished) return msg;
    
    // Generate new IV for each message
    msg.iv = generateIV();
    
    // Pad data (PKCS7)
    std::vector<uint8_t> padded = pkcs7Pad(plaintext, 16);
    msg.ciphertext.resize(padded.size());
    
    // Encrypt (CBC mode)
    // Note: In real implementation, we need to handle IV properly with mbedTLS
    uint8_t iv_copy[16];
    memcpy(iv_copy, msg.iv.data(), 16);
    
    int ret = mbedtls_aes_crypt_cbc(&aesEncryptCtx, MBEDTLS_AES_ENCRYPT, padded.size(), 
                                   iv_copy, padded.data(), msg.ciphertext.data());
                                   
    if (ret != 0) {
        LogManager::error("AES encrypt failed");
        msg.ciphertext.clear();
    }
    
    msg.timestamp = millis();
    return msg;
}

EncryptedMessage SecurityManager::encrypt(const String& plaintext) {
    std::vector<uint8_t> data(plaintext.begin(), plaintext.end());
    return encrypt(data);
}

std::vector<uint8_t> SecurityManager::decrypt(const EncryptedMessage& encrypted) {
    if (!sessionEstablished || !encrypted.isValid()) return {};
    
    std::vector<uint8_t> decrypted(encrypted.ciphertext.size());
    
    uint8_t iv_copy[16];
    memcpy(iv_copy, encrypted.iv.data(), 16);
    
    int ret = mbedtls_aes_crypt_cbc(&aesDecryptCtx, MBEDTLS_AES_DECRYPT, encrypted.ciphertext.size(),
                                   iv_copy, encrypted.ciphertext.data(), decrypted.data());
                                   
    if (ret != 0) {
        LogManager::error("AES decrypt failed");
        return {};
    }
    
    return pkcs7Unpad(decrypted);
}

String SecurityManager::decryptToString(const EncryptedMessage& encrypted) {
    std::vector<uint8_t> data = decrypt(encrypted);
    if (data.empty()) return "";
    return String((char*)data.data(), data.size()); // Assuming mock String constructor handles length
}

// ============================================================================
// HELPER METHODS
// ============================================================================

bool SecurityManager::initializeMbedTLS() {
    mbedtls_ecdh_init(&ecdhContext);
    mbedtls_ctr_drbg_init(&ctrDrbgContext);
    mbedtls_entropy_init(&entropyContext);
    mbedtls_aes_init(&aesEncryptCtx);
    mbedtls_aes_init(&aesDecryptCtx);
    
    int ret = mbedtls_ctr_drbg_seed(&ctrDrbgContext, mbedtls_entropy_func, &entropyContext, 
                                   (const unsigned char*)"WiBLE", 5);
    if (ret != 0) return false;
    
    ret = mbedtls_ecdh_setup(&ecdhContext, MBEDTLS_ECP_DP_CURVE25519);
    if (ret != 0) return false;
    
    return true;
}

void SecurityManager::cleanupMbedTLS() {
    mbedtls_ecdh_free(&ecdhContext);
    mbedtls_ctr_drbg_free(&ctrDrbgContext);
    mbedtls_entropy_free(&entropyContext);
    mbedtls_aes_free(&aesEncryptCtx);
    mbedtls_aes_free(&aesDecryptCtx);
}

bool SecurityManager::setAESKey(const std::vector<uint8_t>& key) {
    if (key.size() != 32) return false; // AES-256
    
    mbedtls_aes_setkey_enc(&aesEncryptCtx, key.data(), 256);
    mbedtls_aes_setkey_dec(&aesDecryptCtx, key.data(), 256);
    return true;
}

std::vector<uint8_t> SecurityManager::hash(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> output(32);
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, data.data(), data.size());
    mbedtls_sha256_finish(&ctx, output.data());
    mbedtls_sha256_free(&ctx);
    return output;
}

std::vector<uint8_t> SecurityManager::generateRandomBytes(size_t length) {
    std::vector<uint8_t> output(length);
    mbedtls_ctr_drbg_random(&ctrDrbgContext, output.data(), length);
    return output;
}

std::vector<uint8_t> SecurityManager::generateIV() {
    return generateRandomBytes(16);
}

String SecurityManager::generateSessionId() {
    return SecurityUtils::base64Encode(generateRandomBytes(16));
}

// ============================================================================
// UTILS
// ============================================================================

void SecurityUtils::secureWipe(std::vector<uint8_t>& data) {
    if (data.empty()) return;
    memset(data.data(), 0, data.size());
    data.clear();
}

String SecurityUtils::base64Encode(const std::vector<uint8_t>& data) {
    // Placeholder for Base64 encoding
    // In real implementation, use mbedtls_base64_encode
    return ""; 
}

std::vector<uint8_t> SecurityManager::pkcs7Pad(const std::vector<uint8_t>& data, size_t blockSize) {
    size_t padding = blockSize - (data.size() % blockSize);
    std::vector<uint8_t> padded = data;
    for (size_t i = 0; i < padding; i++) {
        padded.push_back((uint8_t)padding);
    }
    return padded;
}

std::vector<uint8_t> SecurityManager::pkcs7Unpad(const std::vector<uint8_t>& data) {
    if (data.empty()) return {};
    uint8_t padding = data.back();
    if (padding > data.size() || padding == 0) return {}; // Invalid padding
    
    // Verify padding
    for (size_t i = 0; i < padding; i++) {
        if (data[data.size() - 1 - i] != padding) return {};
    }
    
    return std::vector<uint8_t>(data.begin(), data.end() - padding);
}

// Placeholder implementations for other methods to satisfy linker
EncryptedMessage SecurityManager::encryptCredentials(const String& ssid, const String& password) { return encrypt(ssid + ":" + password); }
bool SecurityManager::decryptCredentials(const EncryptedMessage& encrypted, String& ssid, String& password) { return false; }
bool SecurityManager::storeCredentialsSecurely(const String& ssid, const String& password) { return false; }
bool SecurityManager::retrieveCredentialsSecurely(String& ssid, String& password) { return false; }
void SecurityManager::clearStoredCredentials() {}
bool SecurityManager::isSessionEstablished() const { return sessionEstablished; }
bool SecurityManager::isSessionSecure() const { return sessionEstablished; }
bool SecurityManager::renewSessionKey() { return false; }
void SecurityManager::terminateSession() { reset(); }
uint32_t SecurityManager::getSessionAge() const { return millis() - sessionStartTime; }
bool SecurityManager::isEncryptionEnabled() const { return config.level != SecurityLevel::NONE; }
String SecurityManager::getSecurityInfo() const { return "AES-256"; }
void SecurityManager::dumpKeys() const {}
void SecurityManager::dumpSession() const {}
bool SecurityManager::selfTest() { return true; }

} // namespace WiBLE
