// speed.c
// 22-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "brutus.h"

// actual speedtest routine

int run_speed(caesar_t *aead,
                unsigned long long mlen, unsigned long long adlen,
                clock_t limit)
{
    int ret, i;
    unsigned long long clen, t, bytes;
    clock_t stim, etim;
    uint8_t key[80], nsec[64], osec[64], npub[64];
    // that 524304 ABYTES factor comes from Trivia. Don't ask.
    uint8_t pt[0x10000], ad[0x10000], ct[0x10000 + 524304];
    double encspeed, decspeed;

    if (mlen > sizeof(pt) || adlen > sizeof(ad)) {
        fprintf(stderr, "run_speed(): invalid parameters\n");
        return -1;
    }

    limit *= CLOCKS_PER_SEC;

    // random fill contents
    rand_fill(pt, mlen);
    rand_fill(ad, adlen);
    rand_fill(key, aead->keybytes);
    rand_fill(nsec, aead->nsecbytes);
    rand_fill(npub, aead->npubbytes);

    encspeed = 0.0;
    decspeed = 0.0;

    stim = clock();
    bytes = 0;
    do {
        for (i = 0; i < 100; i++) {
            clen = 0;
            ret = aead->encrypt(ct, &clen, pt, mlen, ad, adlen,
                nsec, npub, key);
            if (ret != 0 || clen <= 0) {
                fprintf(stderr, "!ERROR\t%s encrypt(%llu)=%d\n",
                    aead->name, mlen, ret);
                return -1;
            }
        }
        bytes += 100 * (mlen + adlen);
        etim = clock() - stim;
    } while (etim < limit);

    encspeed = ((double) CLOCKS_PER_SEC) * ((double) bytes) / ((double) etim);
    printf("[%s] %.2f kB/s  encrypt(mlen=%llu adlen=%llu)\n",
        aead->name, encspeed / 1000.0, mlen, adlen);

    stim = clock();
    bytes = 0;
    do {
        for (i = 0; i < 100; i++) {
            ret = aead->decrypt(pt, &t, osec, ct, clen, ad,
                adlen, npub, key);
            if (ret != 0) {
                fprintf(stderr, "!ERROR\t%s decrypt(%llu)=%d\n",
                    aead->name, clen, ret);
                return -2;
            }
        }
        bytes += 100 * (mlen + adlen);
        etim = clock() - stim;
    } while (etim < limit);

    decspeed = ((double) CLOCKS_PER_SEC) * ((double) bytes) / ((double) etim);
    printf("[%s] %.2f kB/s  decrypt(mlen=%llu adlen=%llu)\n",
        aead->name, decspeed / 1000.0, mlen, adlen);
    fflush(stdout);

    return 0;
}

// comprehensible speed test

int test_speed(caesar_t *aead, int limit)
{
    uint64_t len;

    if (brutus_verbose) {
        printf("[%s] Speed Test (limit=%d)  "
            "key=%d  nsec=%d  npub=%d  a=%d\n",
            aead->name, limit, aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }

    for (len = 0x10; len <= 0x10000; len <<= 2) {
        run_speed(aead, len, 0, limit);
        run_speed(aead, 0, len, limit);
        run_speed(aead, len, len, limit);
    }

    return 0;
}

// faster speed test

int test_throughput(caesar_t *aead, int limit)
{
    if (brutus_verbose) {
        printf("[%s] Throughput (limit=%ds)  "
            "key=%d  nsec=%d  npub=%d a=%d\n",
            aead->name, limit, aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }

    run_speed(aead, 0x10000, 0, limit);

    return 0;
}


