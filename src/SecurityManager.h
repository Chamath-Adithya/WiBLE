/**
 * SecurityManager.h - Security Layer for WiBLE
 * 
 * Implements ECDH key exchange, AES encryption, authentication,
 * and secure credential handling.
 */

#ifndef WIBLE_SECURITY_MANAGER_H
#define WIBLE_SECURITY_MANAGER_H

#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/sha256.h>
#include <vector>

#include "WiBLE_Defs.h"

namespace WiBLE {

// ============================================================================
// SECURITY ENUMS
// ============================================================================

enum class PairingMethod {
    JUST_WORKS,
    NUMERIC_COMPARISON,
    PASSKEY_ENTRY,
    OUT_OF_BAND
};

enum class EncryptionMode {
    NONE,
    AES_128_ECB,
    AES_128_CBC,
    AES_128_CTR,
    AES_256_CBC,
    AES_256_GCM
};

// ============================================================================
// SECURITY CONFIGURATION
// ============================================================================

struct SecurityConfig {
    SecurityLevel level = SecurityLevel::SECURE;
    PairingMethod pairingMethod = PairingMethod::NUMERIC_COMPARISON;
    EncryptionMode encryptionMode = EncryptionMode::AES_256_CBC;
    
    bool requireAuthentication = true;
    bool enableBonding = true;
    bool enableSecureConnections = true;
    
    uint32_t authTimeoutMs = 30000;
    uint32_t sessionTimeoutMs = 300000; // 5 minutes
    
    String pinCode = "000000";
    bool randomizePinCode = true;
    
    // Advanced
    bool enablePerfectForwardSecrecy = true;
    bool enableCertificatePinning = false;
    uint16_t minKeySize = 128;
    uint16_t maxKeySize = 256;
};

// ============================================================================
// CRYPTOGRAPHIC KEY STRUCTURES
// ============================================================================

struct KeyPair {
    std::vector<uint8_t> publicKey;
    std::vector<uint8_t> privateKey;
    uint32_t generatedAt;
    bool isValid;
    
    KeyPair() : generatedAt(0), isValid(false) {}
    
    void clear() {
        publicKey.clear();
        privateKey.clear();
        isValid = false;
    }
    
    size_t getKeySize() const {
        return publicKey.size() * 8; // bits
    }
};

struct SessionKey {
    std::vector<uint8_t> key;
    std::vector<uint8_t> iv;  // Initialization Vector
    uint32_t createdAt;
    uint32_t expiresAt;
    String sessionId;
    
    SessionKey() : createdAt(0), expiresAt(0) {}
    
    bool isExpired() const {
        return millis() > expiresAt;
    }
    
    bool isValid() const {
        return !key.empty() && !isExpired();
    }
    
    void clear() {
        key.clear();
        iv.clear();
        sessionId = "";
    }
};

struct AuthToken {
    String token;
    String clientId;
    uint32_t issuedAt;
    uint32_t expiresAt;
    bool verified;
    
    AuthToken() : issuedAt(0), expiresAt(0), verified(false) {}
    
    bool isValid() const {
        return verified && millis() < expiresAt;
    }
};

// ============================================================================
// ENCRYPTED MESSAGE
// ============================================================================

struct EncryptedMessage {
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> iv;
    std::vector<uint8_t> authTag;  // For GCM mode
    String messageId;
    uint32_t timestamp;
    
    bool isValid() const {
        return !ciphertext.empty();
    }
    
    // Serialize to bytes for transmission
    std::vector<uint8_t> serialize() const;
    
    // Deserialize from bytes
    static EncryptedMessage deserialize(const std::vector<uint8_t>& data);
};

// ============================================================================
// SECURITY MANAGER CLASS
// ============================================================================

class SecurityManager {
public:
    SecurityManager();
    ~SecurityManager();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * Initialize security subsystem
     */
    bool initialize(const SecurityConfig& config = SecurityConfig());
    
    /**
     * Cleanup and free resources
     */
    void cleanup();
    
    /**
     * Reset security state (new session)
     */
    void reset();
    
    // ========================================================================
    // KEY EXCHANGE (ECDH)
    // ========================================================================
    
    /**
     * Generate ECDH key pair
     */
    bool generateKeyPair();
    
    /**
     * Get our public key for transmission
     */
    std::vector<uint8_t> getPublicKey() const;
    
    /**
     * Compute shared secret from peer's public key
     */
    bool computeSharedSecret(const std::vector<uint8_t>& peerPublicKey);
    
    /**
     * Derive session key from shared secret
     */
    bool deriveSessionKey();
    
    /**
     * Get current session key (for debugging)
     */
    const SessionKey& getSessionKey() const { return sessionKey; }
    
    // ========================================================================
    // ENCRYPTION / DECRYPTION
    // ========================================================================
    
    /**
     * Encrypt plaintext data
     * @param plaintext Input data
     * @return Encrypted message with IV and auth tag
     */
    EncryptedMessage encrypt(const std::vector<uint8_t>& plaintext);
    
    /**
     * Encrypt string
     */
    EncryptedMessage encrypt(const String& plaintext);
    
    /**
     * Decrypt ciphertext
     * @param encrypted Encrypted message
     * @return Decrypted plaintext (empty on failure)
     */
    std::vector<uint8_t> decrypt(const EncryptedMessage& encrypted);
    
    /**
     * Decrypt to string
     */
    String decryptToString(const EncryptedMessage& encrypted);
    
    /**
     * Quick encrypt/decrypt without EncryptedMessage wrapper
     */
    std::vector<uint8_t> encryptBytes(const std::vector<uint8_t>& data, 
                                       const std::vector<uint8_t>& iv);
    std::vector<uint8_t> decryptBytes(const std::vector<uint8_t>& data, 
                                       const std::vector<uint8_t>& iv);
    
    // ========================================================================
    // AUTHENTICATION
    // ========================================================================
    
    /**
     * Generate authentication challenge
     */
    String generateChallenge();
    
    /**
     * Verify challenge response
     */
    bool verifyChallenge(const String& challenge, const String& response);
    
    /**
     * Generate PIN code for pairing
     */
    String generatePinCode();
    
    /**
     * Verify PIN code
     */
    bool verifyPinCode(const String& pin);
    
    /**
     * Create authentication token
     */
    AuthToken createAuthToken(const String& clientId);
    
    /**
     * Verify authentication token
     */
    bool verifyAuthToken(const AuthToken& token);
    
    // ========================================================================
    // HASHING & SIGNATURES
    // ========================================================================
    
    /**
     * Compute SHA-256 hash
     */
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data);
    
    /**
     * Compute SHA-256 hash of string
     */
    String hashString(const String& data);
    
    /**
     * Generate HMAC for message authentication
     */
    std::vector<uint8_t> computeHMAC(const std::vector<uint8_t>& data);
    
    /**
     * Verify HMAC
     */
    bool verifyHMAC(const std::vector<uint8_t>& data, 
                    const std::vector<uint8_t>& hmac);
    
    // ========================================================================
    // RANDOM NUMBER GENERATION
    // ========================================================================
    
    /**
     * Generate cryptographically secure random bytes
     */
    std::vector<uint8_t> generateRandomBytes(size_t length);
    
    /**
     * Generate random IV for encryption
     */
    std::vector<uint8_t> generateIV();
    
    /**
     * Generate random session ID
     */
    String generateSessionId();
    
    // ========================================================================
    // CREDENTIAL PROTECTION
    // ========================================================================
    
    /**
     * Encrypt WiFi credentials for transmission
     */
    EncryptedMessage encryptCredentials(const String& ssid, const String& password);
    
    /**
     * Decrypt received credentials
     */
    bool decryptCredentials(const EncryptedMessage& encrypted, 
                           String& ssid, String& password);
    
    /**
     * Store credentials securely in NVS
     */
    bool storeCredentialsSecurely(const String& ssid, const String& password);
    
    /**
     * Retrieve stored credentials
     */
    bool retrieveCredentialsSecurely(String& ssid, String& password);
    
    /**
     * Clear stored credentials
     */
    void clearStoredCredentials();
    
    // ========================================================================
    // SESSION MANAGEMENT
    // ========================================================================
    
    /**
     * Check if session is established
     */
    bool isSessionEstablished() const;
    
    /**
     * Check if session is secure
     */
    bool isSessionSecure() const;
    
    /**
     * Renew session key
     */
    bool renewSessionKey();
    
    /**
     * Terminate current session
     */
    void terminateSession();
    
    /**
     * Get session age (ms)
     */
    uint32_t getSessionAge() const;
    
    // ========================================================================
    // SECURITY STATUS
    // ========================================================================
    
    SecurityLevel getSecurityLevel() const { return config.level; }
    bool isEncryptionEnabled() const;
    bool isAuthenticationRequired() const { return config.requireAuthentication; }
    String getSecurityInfo() const;
    
    // ========================================================================
    // DEBUGGING
    // ========================================================================
    
    void dumpKeys() const;
    void dumpSession() const;
    bool selfTest();

private:
    // Configuration
    SecurityConfig config;
    
    // ECDH Context
    mbedtls_ecdh_context ecdhContext;
    mbedtls_entropy_context entropyContext;
    mbedtls_ctr_drbg_context ctrDrbgContext;
    
    // Keys
    KeyPair keyPair;
    std::vector<uint8_t> sharedSecret;
    SessionKey sessionKey;
    
    // Authentication
    String currentChallenge;
    String currentPinCode;
    AuthToken currentAuthToken;
    
    // AES Context (reused for performance)
    mbedtls_aes_context aesEncryptCtx;
    mbedtls_aes_context aesDecryptCtx;
    
    // State
    bool initialized;
    bool sessionEstablished;
    uint32_t sessionStartTime;
    
    // Internal methods
    bool initializeMbedTLS();
    void cleanupMbedTLS();
    bool setAESKey(const std::vector<uint8_t>& key);
    std::vector<uint8_t> pkcs7Pad(const std::vector<uint8_t>& data, size_t blockSize);
    std::vector<uint8_t> pkcs7Unpad(const std::vector<uint8_t>& data);
    bool validateKeySize(size_t keySize) const;
    String bytesToHex(const std::vector<uint8_t>& bytes) const;
    std::vector<uint8_t> hexToBytes(const String& hex) const;
};

// ============================================================================
// SECURITY UTILITIES
// ============================================================================

class SecurityUtils {
public:
    /**
     * Constant-time comparison (prevents timing attacks)
     */
    static bool constantTimeCompare(const std::vector<uint8_t>& a, 
                                    const std::vector<uint8_t>& b);
    
    /**
     * Secure memory wipe
     */
    static void secureWipe(std::vector<uint8_t>& data);
    static void secureWipe(uint8_t* data, size_t length);
    
    /**
     * Base64 encoding/decoding
     */
    static String base64Encode(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> base64Decode(const String& encoded);
    
    /**
     * Generate secure random string
     */
    static String generateRandomString(size_t length);
    
    /**
     * Validate credential strength
     */
    static bool isPasswordStrong(const String& password);
    static uint8_t getPasswordStrength(const String& password);
};

} // namespace WiBLE

#endif // WIBLE_SECURITY_MANAGER_H
