// xprmt.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Nonce -> Ciphertext/MAC Correalation Check
// Included as an example.

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "brutus.h"

int test_xprmnt(caesar_t *aead, int limit)
{
    unsigned long long clen;

    int i, j;
    uint64_t steps;
    uint8_t nx[32], ny[32], cx[32], cy[32], v[32], key[256];
    clock_t stim, etim;
    time_t wtime;

    // stats
    uint32_t cnt[256][256];
    double chi2, pval, c2max;
    int inp_bits, out_bits;

    if (brutus_verbose) {
        wtime = time(NULL);
        printf("[%s] Correlation xprmnt (limit=%d) %s"
            "\tkey=%d\tnsec=%d\tnpub=%d\ta=%d",
            aead->name, limit, ctime(&wtime),
            aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }
    limit *= CLOCKS_PER_SEC;

    // test that ciphertext has admissible size
    memset(key, 0x00, sizeof(key));
    memset(v, 0x00, sizeof(v));
    memset(nx, 0x00, sizeof(nx));

    if (aead->abytes > 32 || aead->npubbytes > 32)
        goto fail;
    if (aead->encrypt(cx, &clen, v, 32 - aead->abytes, v, 0, v, nx, key) != 0)
        goto fail;
    if (clen > 32)
        goto fail;

    inp_bits = aead->npubbytes << 3;
    out_bits = clen << 3;
    printf("\tinp_bits=%d\tout_bits=%d\n", inp_bits, out_bits);
    fflush(stdout);

    // counters
    memset(cnt, 0x00, sizeof(cnt));
    steps = 0;
    stim = clock();
    do {
        rand_fill(nx, aead->npubbytes);
        aead->encrypt(cx, &clen, v, 32 - aead->abytes, v, 0, v, nx, key);

        for (i = 0; i < inp_bits; i++) {
            memcpy(ny, nx, aead->npubbytes);
            ny[i >> 3] ^= 1 << (i & 7);
            aead->encrypt(cy, &clen, v, 32 - aead->abytes, v, 0, v, ny, key);
            for (j = 0; j < clen; j++)
                cy[j] ^= cx[j];
            for (j = 0; j < out_bits; j++)
                cnt[i][j] += (cy[j >> 3] >> (j & 7)) & 1;
        }
        steps++;
        etim = clock() - stim;
    } while (etim < limit);

    // check Chi^2 biases
    c2max = 1.0;
    for (i = 0; i < inp_bits; i++) {
        for (j = 0; j < out_bits; j++) {
            chi2 = 2.0 * ((double) cnt[i][j]) - ((double) steps);
            chi2 = chi2 * chi2 / ((double) steps);
            if (chi2 > c2max) {
                c2max = chi2;
                if (chi2 > 23.9281) {       // P = 1/1000000
                    pval = plg2chi2(chi2);
                    printf("[%s] BIAS: %02X->%02X  Chi2=%.1f  P=2^%.1f "
                        "(%d/%lu)=%g+0.5\n", aead->name, i, j,
                        chi2, pval, cnt[i][j], steps,
                        ((double) cnt[i][j]) / ((double) steps) - 0.5);
                    fflush(stdout);
                }
            }
        }
    }

    wtime = time(NULL);
    printf("[%s] %lu step chi2max=%.1f (P=2^%.1f) %s",
        aead->name, steps, c2max, plg2chi2(c2max), ctime(&wtime));
    fflush(stdout);

    return 0;

fail:
    printf("\n!INFO\tincompatible parameters, bailing out.\n");

    return 1;
}

