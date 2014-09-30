// coherence.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Coherence test: decryption works correctly, trivial forgeries fail.

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "brutus.h"

int test_coherence(caesar_t *aead, int limit)
{
    int ret, forge, iter, off, bit;
    int forge_ok, forge_try;
    unsigned long long mlen, clen, adlen, t;
    time_t tim;
    const char *fcase[4] = { "key", "npub", "ad", "ct" };

    uint8_t key[80], nsec[64], osec[64], npub[64];

    // that 524304 ABYTES factor comes from Trivia. Don't ask.
    uint8_t pt[0x100], ad[0x100], xt[0x100], ct[0x100 + 524304];

    if (aead->name == NULL || aead->abytes + sizeof(pt) > sizeof(ct) ||
        aead->keybytes > sizeof(key) || aead->nsecbytes > sizeof(nsec) ||
        aead->nsecbytes > sizeof(osec) || aead->npubbytes > sizeof(npub)) {
        fprintf(stderr, "test_coherence(): invalid parameters\n");
        return -1;
    }

    if (brutus_verbose) {
        printf("[%s] Coherence Check (limit=%d sec)  "
            "key=%d  nsec=%d  npub=%d  a=%d\n",
            aead->name, limit, aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }
    limit *= CLOCKS_PER_SEC;

    // basic correctness test
    forge_ok = 0;
    forge_try = 0;

    tim = clock();
    for (iter = 0; iter < 20; iter++) {

        // limit time
        if (clock() - tim > limit) {
            if (brutus_verbose) {
                printf("!INFO\t%s timed out at iter=%d\n",
                    aead->name, iter);
            }
            break;
        }

        for (mlen = 0; mlen < sizeof(pt); mlen++) {

            // clearup
            memset(pt, 0x55, sizeof(pt));
            memset(xt, 0xAA, sizeof(xt));
            memset(ct, 0x33, sizeof(ct));
            memset(ad, 0xCC, sizeof(ad));
            memset(osec, 0xF0, sizeof(osec));

            // randomize cryptovariables
            detseq_fill(key, aead->keybytes);
            detseq_fill(nsec, aead->nsecbytes);
            detseq_fill(npub, aead->npubbytes);

            adlen = detseq32() % (sizeof(ad) + 1);
            detseq_fill(ad, adlen);
            detseq_fill(pt, mlen);

            // encrypt
            clen = 0;
            ret = aead->encrypt(ct, &clen, pt, mlen,
                ad, adlen, nsec, npub, key);
            if (ret != 0) {
                fprintf(stderr,
                    "!ERROR\t%s encrypt(%llu)=%d\n", aead->name, mlen, ret);
                return -1;
            }

            // decrypt
            ret = aead->decrypt(xt, &t, osec, ct, clen, ad, adlen, npub, key);
            if (ret != 0) {
                fprintf(stderr,
                    "!ERROR\t%s decrypt(%llu)=%d\n", aead->name, clen, ret);
                return -2;
            }

            // check that the plaintext matches
            if (t != mlen) {
                fprintf(stderr,
                    "!FAIL\t%s decrypt length=%llu "
                    "mlen=%llu adlen=%llu clen=%llu)\n",
                aead->name, t, mlen, adlen, clen);
                return -3;
            }
            if (memcmp(pt, xt, mlen) != 0) {
                fprintf(stderr,
                    "!FAIL\t%s decrypt mismatch "
                    "(mlen=%llu adlen=%llu clen=%llu\n)",
                    aead->name, mlen, adlen, clen);
                return -4;
            }
            if (aead->nsecbytes > 0 &&
                memcmp(nsec, osec, aead->nsecbytes) != 0) {
                fprintf(stderr,
                    "!FAIL\t%s nsec mismatch "
                    "(mlen=%llu adlen=%llu clen=%llu\n)",
                    aead->name, mlen, adlen, clen);
                return -5;
            }

            // attempt random forgery
            forge = detseq32() % 4;
            bit = 0x01 << (detseq32() % 8);
            switch (forge) {
                case 0:
                    off = detseq32() % aead->keybytes;
                    key[off] ^= bit;
                    break;

                case 1:
                    if (aead->npubbytes == 0)
                        continue;
                    off = detseq32() % aead->npubbytes;
                    npub[off] ^= bit;
                    break;

                case 2:
                    if (adlen == 0)
                        continue;
                    off = detseq32() % adlen;
                    ad[off] ^= bit;
                    break;

                case 3:
                    off = detseq32() % clen;
                    ct[off] ^= bit;
                    break;
            }

            // decrypt
            ret = aead->decrypt(xt, &t, osec, ct, clen, ad, adlen, npub, key);
            if (ret == 0) {
                if (brutus_verbose) {
                    printf("!FORGE\t%s %s[%d] ^= 0x%02X "
                        "(iter=%d mlen=%llu adlen=%llu clen=%llu)\n",
                        aead->name, fcase[forge], off, bit,
                        iter, mlen, adlen, clen);
                }
                forge_ok++;
            }
            forge_try++;
        }
    }
    if (forge_ok > 0) {
        fprintf(stderr, "!FORGE\t%s %d/%d = %g forgeries.\n", aead->name,
            forge_ok, forge_try, ((double) forge_ok) / ((double) forge_try));
        return -6;
    }

    return 0;
}

