// aead_params.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

// get the encryption parameters

#include "api.h"
#include "openssl/aes.h"

// helper functions for implementations

int crypto_verify_16(const unsigned char *x, const unsigned char *y)
{
    return memcmp(x, y, 16) == 0 ? 0 : -1;
}

int crypto_verify_32(const unsigned char *x, const unsigned char *y)
{
    return memcmp(x, y, 32) == 0 ? 0 : -1;
}

void crypto_core_aes128encrypt(void *out, void *in, void *k, int flag)
{
    AES_KEY ek;
    AES_set_encrypt_key(k, 128, &ek);
    AES_encrypt(in, out, &ek);
}

void crypto_core_aes256encrypt(void *out, void *in, void *k, int flag)
{
    AES_KEY ek;
    AES_set_encrypt_key(k, 256, &ek);
    AES_encrypt(in, out, &ek);
}

void crypto_core_aes128decrypt(void *out, void *in, void *k, int flag)
{
    AES_KEY ek;
    AES_set_decrypt_key(k, 128, &ek);
    AES_decrypt(in, out, &ek);
}

void crypto_core_aes256decrypt(void *out, void *in, void *k, int flag)
{
    AES_KEY ek;

    AES_set_decrypt_key(k, 256, &ek);
    AES_decrypt(in, out, &ek);
}

// make them available as symbols

#ifdef BRUTUS_NAME
const char brutus_name[] = BRUTUS_NAME;
#endif

#ifdef CRYPTO_KEYBYTES
const int brutus_keybytes = (CRYPTO_KEYBYTES);
#endif

#ifdef CRYPTO_NSECBYTES
const int brutus_nsecbytes = (CRYPTO_NSECBYTES);
#endif

#ifdef CRYPTO_NPUBBYTES
const int brutus_npubbytes = (CRYPTO_NPUBBYTES);
#endif

#ifdef CRYPTO_ABYTES
const int brutus_abytes = (CRYPTO_ABYTES);
#endif

#ifdef CRYPTO_NOOVERLAP
const int brutus_nooverlap = (CRYPTO_NOOVERLAP);
#endif

