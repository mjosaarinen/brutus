#include "crypto_aead.h"

#include <string.h>
#include "Ketje.h"

int crypto_aead_encrypt(
    unsigned char *c,unsigned long long *clen,
    const unsigned char *m,unsigned long long mlen,
    const unsigned char *ad,unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k
    )
{
    Ketje_Instance instance;

    Ketje_Initialize(&instance, k, 128, npub, 128);
    Ketje_FeedAssociatedData(&instance, ad, adlen);
    Ketje_WrapPlaintext(&instance, m, c, mlen);
    *clen = mlen;
    Ketje_GetTag(&instance, c+mlen, 16);
    *clen += 16;

    return 0;
}

int crypto_aead_decrypt(
    unsigned char *m,unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c,unsigned long long clen,
    const unsigned char *ad,unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k
    )
{
    Ketje_Instance instance;
    unsigned char tag[16];

    if (clen < 16)
        return -1;

    Ketje_Initialize(&instance, k, 128, npub, 128);
    Ketje_FeedAssociatedData(&instance, ad, adlen);
    *mlen = clen-16;
    Ketje_UnwrapCiphertext(&instance, c, m, *mlen);
    Ketje_GetTag(&instance, tag, 16);
    if (memcmp(tag, c+(*mlen), 16) != 0) {
        memset(m, 0, *mlen);
        return -1;
    }
    else
        return 0;
}
