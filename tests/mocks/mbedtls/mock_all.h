#ifndef MBEDTLS_MOCK_H
#define MBEDTLS_MOCK_H

#include <stddef.h>
#include <stdint.h>

// Mock MPI (Multi-Precision Integer)
typedef struct {
    int X;
    int Z;
} mbedtls_mpi;

// Mock ECP Point
typedef struct {
    int X;
    int Z;
} mbedtls_ecp_point;

// Mock ECP Group
typedef struct {
    int P;
} mbedtls_ecp_group;

// Context structs
typedef struct { 
    mbedtls_ecp_group grp; 
    int d; 
    mbedtls_ecp_point Q; 
    mbedtls_ecp_point Qp; 
    int z; 
} mbedtls_ecdh_context;

typedef struct {} mbedtls_entropy_context;
typedef struct {} mbedtls_ctr_drbg_context;
typedef struct {} mbedtls_aes_context;
typedef struct {} mbedtls_sha256_context;

// Constants
#define MBEDTLS_ECP_DP_CURVE25519 1
#define MBEDTLS_AES_ENCRYPT 1
#define MBEDTLS_AES_DECRYPT 0

// Functions
inline int mbedtls_ecdh_gen_public(mbedtls_ecp_group*, int*, mbedtls_ecp_point*, int(*)(void*, unsigned char*, size_t), void*) { return 0; }
inline int mbedtls_ctr_drbg_random(void*, unsigned char*, size_t) { return 0; }
inline int mbedtls_mpi_write_binary(int*, unsigned char*, size_t) { return 0; }
inline size_t mbedtls_mpi_size(int*) { return 32; }
inline int mbedtls_mpi_read_binary(int*, const unsigned char*, size_t) { return 0; }
inline int mbedtls_mpi_lset(int*, int) { return 0; }
inline int mbedtls_ecdh_compute_shared(mbedtls_ecp_group*, int*, mbedtls_ecp_point*, int*, int(*)(void*, unsigned char*, size_t), void*) { return 0; }

inline int mbedtls_aes_crypt_cbc(mbedtls_aes_context*, int, size_t, unsigned char*, const unsigned char*, unsigned char*) { return 0; }
inline void mbedtls_ecdh_init(mbedtls_ecdh_context*) {}
inline void mbedtls_ctr_drbg_init(mbedtls_ctr_drbg_context*) {}
inline void mbedtls_entropy_init(mbedtls_entropy_context*) {}
inline void mbedtls_aes_init(mbedtls_aes_context*) {}
inline int mbedtls_ctr_drbg_seed(mbedtls_ctr_drbg_context*, int(*)(void*, unsigned char*, size_t), mbedtls_entropy_context*, const unsigned char*, size_t) { return 0; }
inline int mbedtls_entropy_func(void*, unsigned char*, size_t) { return 0; }
inline int mbedtls_ecdh_setup(mbedtls_ecdh_context*, int) { return 0; }
inline void mbedtls_ecdh_free(mbedtls_ecdh_context*) {}
inline void mbedtls_ctr_drbg_free(mbedtls_ctr_drbg_context*) {}
inline void mbedtls_entropy_free(mbedtls_entropy_context*) {}
inline void mbedtls_aes_free(mbedtls_aes_context*) {}
inline void mbedtls_aes_setkey_enc(mbedtls_aes_context*, const unsigned char*, unsigned int) {}
inline void mbedtls_aes_setkey_dec(mbedtls_aes_context*, const unsigned char*, unsigned int) {}

inline void mbedtls_sha256_init(mbedtls_sha256_context*) {}
inline void mbedtls_sha256_starts(mbedtls_sha256_context*, int) {}
inline void mbedtls_sha256_update(mbedtls_sha256_context*, const unsigned char*, size_t) {}
inline void mbedtls_sha256_finish(mbedtls_sha256_context*, unsigned char*) {}
inline void mbedtls_sha256_free(mbedtls_sha256_context*) {}

#endif
