// brutus_aead.h
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>
// catch-all header file for supercopped reference implementations

#ifndef BRUTUS_STDINT_H
#define BRUTUS_STDINT_H
#include <stdint.h>
#endif

#ifndef BRUTUS_CRYPTO_AEAD_H
#define BRUTUS_CRYPTO_AEAD_H
int crypto_aead_encrypt(unsigned char *c, unsigned long long *clen,
                        const unsigned char *m, unsigned long long mlen,
                        const unsigned char *ad, unsigned long long adlen,
                        const unsigned char *nsec, const unsigned char *npub,
                        const unsigned char *k);

int crypto_aead_decrypt(unsigned char *m, unsigned long long *outputmlen,
                        unsigned char *nsec,
                        const unsigned char *c, unsigned long long clen,
                        const unsigned char *ad, unsigned long long adlen,
                        const unsigned char *npub, const unsigned char *k);
#endif

// integer types
#ifndef BRUTUS_CRYPTO_UINTS
#define BRUTUS_CRYPTO_UINTS
typedef uint8_t crypto_uint8;
typedef uint16_t crypto_uint16;
typedef uint32_t crypto_uint32;
typedef uint64_t crypto_uint64;
#endif

#ifndef BRUTUS_CRYPTO_VERIFY
#define BRUTUS_CRYPTO_VERIFY
int crypto_verify_16(const unsigned char *x, const unsigned char *y);
int crypto_verify_32(const unsigned char *x, const unsigned char *y);
#endif

#ifndef BRUTUS_CRYPTO_CORE_AES
#define BRUTUS_CRYPTO_CORE_AES
void crypto_core_aes128encrypt(void *out, void *in, void *k, int flag);
void crypto_core_aes256encrypt(void *out, void *in, void *k, int flag);
void crypto_core_aes128decrypt(void *out, void *in, void *k, int flag);
void crypto_core_aes256decrypt(void *out, void *in, void *k, int flag);
#endif

