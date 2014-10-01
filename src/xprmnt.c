// xprmt.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Experimental stuff.

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "brutus.h"

#if 0

// correlation tester

int test_xprmnt2(caesar_t *aead, int limit)
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
        printf("[%s] Block size xprmnt (limit=%d sec) %s"
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

    if (aead->abytes > 32 || aead->npubbytes > 32 || 
		aead->keybytes > sizeof(key))
        goto fail;
    if (aead->encrypt(cx, &clen, v, 32 - aead->abytes, v, 0, v, nx, key) != 0)
        goto fail;
    if (clen > 32)
        goto fail;

    inp_bits = 256;
    out_bits = 256;
    printf("\tinp_bits=%d\tout_bits=%d\n", inp_bits, out_bits);
    fflush(stdout);

    // counters
    memset(cnt, 0x00, sizeof(cnt));
    steps = 0;
    stim = clock();
    do {
        detseq_fill(nx, aead->npubbytes);
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

#endif

// Feedback gallery generator

#define XBLOCK 256

int test_xprmnt(caesar_t *aead, int limit)
{
	int i, j;
	unsigned long long mlen, clen, steps;
	uint8_t key[256], npub[64], nsec[64], 
		mx[XBLOCK], my[XBLOCK], cx[XBLOCK], cy[XBLOCK];
    clock_t stim, etim;
	uint32_t cnt[XBLOCK][XBLOCK];
	char fn[256];
	FILE *f;

    if (brutus_verbose) {
        printf("[%s] feedback (limit=%d sec) "
            " key=%d  nsec=%d  npub=%d  a=%d\n",
            aead->name, limit, 
            aead->keybytes, aead->nsecbytes,
            aead->npubbytes, aead->abytes);
    }

    limit *= CLOCKS_PER_SEC;

    if (aead->abytes > sizeof(cx) || aead->keybytes > sizeof(key) ||
		aead->nsecbytes > sizeof(nsec) || aead->npubbytes > sizeof(npub))
        goto fail;

	memset(key, 0x00, sizeof(key));
	memset(npub, 0x00, sizeof(npub));
	memset(nsec, 0x00, sizeof(nsec));
	memset(mx, 0x00, sizeof(mx));
	memset(my, 0x00, sizeof(my));
	memset(cx, 0x00, sizeof(cx));
	memset(cy, 0x00, sizeof(cy));

	memset(cnt, 0x00, sizeof(cnt));

	mlen = sizeof(cx) - aead->abytes;
    if (aead->encrypt(cx, &clen, mx, mlen, mx, 0, nsec, npub, key) != 0)
        goto fail;
    if (clen > sizeof(cx))
        goto fail;

    stim = clock();
	steps = 0;
    do {
		detseq_fill(key, aead->keybytes);
		detseq_fill(npub, aead->npubbytes);
		detseq_fill(nsec, aead->nsecbytes);
		detseq_fill(mx, mlen);

	    if (aead->encrypt(cx, &clen, mx, mlen, mx, 0, nsec, npub, key) != 0)
	        goto fail;

		for (i = 0; i < mlen; i++) {
			memcpy(my, mx, mlen);
			my[i] += (detseq32() % 255) + 1;
		    if (aead->encrypt(cy, &clen, my, mlen, mx, 0, 
				nsec, npub, key) != 0)
		        goto fail;
			for (j = 0; j < clen; j++) {
				if (cx[j] != cy[j])
					cnt[i][j]++;
			}
		}
        steps++;
        etim = clock() - stim;
    } while (etim < limit);

	strncpy(fn, aead->name, sizeof(fn));
	for (i = 0; i < sizeof(fn); i++) {
		if (fn[i] == 0 || fn[i] == '-')
			break;
	}

	strncpy(&fn[i], ".def", sizeof(fn) - i);	
	f = fopen(fn, "w");
	if (f == NULL) {
		perror(fn);
		return 1;
	}
	fn[i] = 0;
	fprintf(f, "[%s]<br>key:%d  nsec:%d  npub:%d  pad:%d\n",
            fn, 8 * aead->keybytes, 8 * aead->nsecbytes,
            8 * aead->npubbytes, 8 * aead->abytes);
	fclose(f);

	strncpy(&fn[i], ".pgm", sizeof(fn) - i);	
	f = fopen(fn, "w");
	if (f == NULL) {
		perror(fn);
		return 1;
	}

	// output the file
	
	fprintf(f, "P2\n# %s\n%d %d\n255\n", 
		fn, (int) clen + 2, (int) mlen + 2);

	for (i = 0; i < clen + 1; i++)
		fprintf(f, "0 ");
	fprintf(f, "0\n");

	for (i = 0; i < mlen; i++) {
		fprintf(f, "0 ");
		for (j = 0; j < clen; j++) {
		
			if ((i & 0xF) == 0 || (j & 0xF) == 0) {		

				if (cnt[i][j] > 0)
					fprintf(f, "55 ");
				else
					fprintf(f, "255 ");
					
			} else {

				if (cnt[i][j] > 0)
					fprintf(f, "0 ");
				else
					fprintf(f, "200 ");

			}
		}
		fprintf(f, "0\n");
	}

	for (i = 0; i < clen + 1; i++)
		fprintf(f, "0 ");
	fprintf(f, "0\n");

	fclose(f);

	return 0;

fail:
    printf("[%s] Incompatible parameters, bailing out.\n",
		aead->name);

    return 1;
}


