// util.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "brutus.h"

// Deterministic sequences (Fibonacci generator)

uint32_t detseq_a = 0, detseq_b = 1;

void detseq_seed(uint32_t seed) 
{
	detseq_a = 0xDEAD4BAD * seed;		// prime
	detseq_b = 1;
}

uint32_t detseq32() 
{
	uint32_t t;
	
	t = detseq_a + detseq_b;
	detseq_a = detseq_b;
	detseq_b = t;
	
	return t;
}

void detseq_fill(void *p, int len)
{
	int i;
	
	for (i = 0; i < len; i++) 
		((uint8_t *) p)[i] = detseq32() >> 24;
}

// utilities

void hex_dump(void *p, int len)
{
    int i, j;
    uint8_t t;

    for (i = 0; i < len; i += 16) {
        printf("[%08X] ", i);
        for (j = 0; j < 16; j++) {
            if (j == 8)
                putchar(' ');
            if (i + j < len)
                printf(" %02X", ((uint8_t *) p)[i + j]);
            else
                printf("   ");
        }
        printf("  |");

        for (j = 0; j < 16; j++) {
            if (i + j >= len)
                break;
            t = ((uint8_t *) p)[i + j];
            if (t >= 0x20 && t < 0x7F)
                putchar(t);
            else
                putchar('.');
        }
        printf("|\n");
    }
}

// forking test harness (against cipher crashes & memory leaks)

int harness(int (*test_func)(caesar_t *, int), caesar_t *aead, int val)
{
	pid_t p;
	int stat;

	p = fork();
	if (p == 0) {
		exit(test_func(aead, val));
	}
	waitpid(p, &stat, 0);
	fflush(stdout);

	// normal exit ?
	if (WIFEXITED(stat))
		return WEXITSTATUS(stat);

	// something weird happened
	if (WIFSIGNALED(stat))
		fprintf(stderr, "\n[SIGNAL %d]\n", WTERMSIG(stat));
	else
		fprintf(stderr, "\n[UNKNOWN]\n");

	return -13;
}

// P value estimate from Chi2, DF=1
double plg2chi2(double chi2)
{
    //  This table should be good enough for two significant digits.
    //      p(x)=x^(-1/2)*exp(-x/2)/gamma(0.5)*2^(1/2)
    //      bits(x)=log(1-intnum(x=1E-50,x,p(x))/2)/log(2)
    //      for(i=1,128,print1(bits(i). ", "))

    const float lg2p[128] = {
        0.00000, 1.65603, 2.66841, 3.58615, 4.45798, 5.30202, 6.12724, 6.93881,
        7.73997, 8.53293, 9.31925, 10.1000, 10.8762, 11.6485, 12.4173, 13.1832,
        13.9464, 14.7073, 15.4662, 16.2231, 16.9784, 17.7321, 18.4845, 19.2355,
        19.9854, 20.7341, 21.4819, 22.2287, 22.9747, 23.7198, 24.4642, 25.2078,
        25.9508, 26.6932, 27.4349, 28.1761, 28.9168, 29.6569, 30.3966, 31.1358,
        31.8746, 32.6130, 33.3510, 34.0886, 34.8258, 35.5627, 36.2993, 37.0355,
        37.7715, 38.5072, 39.2425, 39.9777, 40.7125, 41.4471, 42.1815, 42.9156,
        43.6495, 44.3832, 45.1167, 45.8500, 46.5831, 47.3160, 48.0487, 48.7813,
        49.5137, 50.2459, 50.9779, 51.7098, 52.4415, 53.1731, 53.9046, 54.6359,
        55.3671, 56.0981, 56.8290, 57.5598, 58.2905, 59.0210, 59.7514, 60.4818,
        61.2120, 61.9421, 62.6721, 63.4019, 64.1317, 64.8614, 65.5910, 66.3205,
        67.0499, 67.7792, 68.5085, 69.2376, 69.9666, 70.6955, 71.4243, 72.1528,
        72.8812, 73.6092, 74.3367, 75.0635, 75.7889, 76.5124, 77.2326, 77.9475,
        78.6538, 79.3464, 80.0176, 80.6560, 81.2467, 81.7727, 82.2181, 82.5735,
        82.8400, 83.0286, 83.1557, 83.2382, 83.2904, 83.3229, 83.3429, 83.3551,
        83.3625, 83.3670, 83.3697, 83.3714, 83.3724, 83.3730, 83.3733, 83.3736
    };

    if (chi2 < 0.0)
        return 0.0;
    if (chi2 >= 128.0)
        return -80.0;

    return -lg2p[(int) floor(chi2)];
}

