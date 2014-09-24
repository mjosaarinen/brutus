// kat.c
// 24-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>
// Generate Known Aswer Tests (KAT)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <ctype.h>

#include "brutus.h"

#ifndef KAT_LIMIT
#define KAT_LIMIT 0x10000
#endif

void kat_vec(void *p, int len, char *label)
{
    int i;
    printf("%4s[%5d]=", label, len);
    for (i = 0; i < len; i++)
        printf("%02X", ((uint8_t *) p)[i]);
    printf("\n");
}

void do_kat(caesar_t *aead, int mlen, int adlen)
{
    int ret;
    unsigned long long clen;

    uint8_t key[80], nsec[64], osec[64], npub[64];
    // that 524304 ABYTES factor comes from Trivia. Don't ask.
    uint8_t pt[KAT_LIMIT], ad[KAT_LIMIT], ct[KAT_LIMIT + 524304];

    if (aead->name == NULL ||
        aead->abytes + sizeof(pt) > sizeof(ct) ||
        aead->keybytes > sizeof(key) || aead->nsecbytes > sizeof(nsec) ||
        aead->nsecbytes > sizeof(osec) || aead->npubbytes > sizeof(npub)) {
        fprintf(stderr, "do_kat(): invalid parameters\n");
    }

	// process the name

    memset(pt, 0x00, sizeof(pt));
    memset(ad, 0x00, sizeof(ad));
    memset(ct, 0x00, sizeof(ct));
    memset(key, 0x00, sizeof(key));
    memset(nsec, 0x00, sizeof(nsec));
    memset(npub, 0x00, sizeof(npub));

    detseq_fill(key, aead->keybytes);
    detseq_fill(pt, mlen);
    detseq_fill(ad, adlen);
    detseq_fill(npub, aead->npubbytes);
    detseq_fill(nsec, aead->nsecbytes);

    kat_vec(key, aead->keybytes, "key");
    kat_vec(nsec, aead->nsecbytes, "nsec");
    kat_vec(npub, aead->npubbytes, "npub");
    kat_vec(pt, mlen, "m");
    kat_vec(ad, adlen, "ad");

    ret = aead->encrypt(ct, &clen, pt, mlen,
        ad, adlen, nsec, npub, key);
    if (ret != 0) {
        printf("[%s] encrypt failed with code %d\n", aead->name, ret);
        return;
    }
    kat_vec(ct, clen, "c");
    printf("\n");
}

int test_kat(caesar_t *aead, int limit)
{
    int len, t;

    if (brutus_verbose) {
        printf("[%s] KAT (limit=%d bytes)  "
            "key=%d  nsec=%d  npub=%d  a=%d\n",
            aead->name, limit, aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }
    if (limit > 0x10000) {
        fprintf(stderr, "Limit %d too high. Increse KAT_LIMIT (%d).\n",
            limit, KAT_LIMIT);
        return -1;
    }

    detseq_seed(-1);
    do_kat(aead, 0, 0);

    for (len = 1; len <= limit; len++) {
        detseq_seed(len);
        do_kat(aead, len, 0);
        do_kat(aead, 0, len);
        if (len > 1) {
            t = detseq32() % (len - 1) + 1;
            do_kat(aead, t, len - t);
        }
    }

    return 0;
}

