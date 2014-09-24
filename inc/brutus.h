// brutus.h
// 21-Sep-14  Markku-Juhani O. Saarinen

#ifndef BRUTUS_H
#define BRUTUS_H

#include "brutus_aead.h"

// structure for candidates
typedef struct {
    void *dlib;     // dynamic library
    char *name;     // name of cipher
    int keybytes, nsecbytes, npubbytes, abytes, nooverlap;

    int (*encrypt)(unsigned char *c, unsigned long long *clen,
        const unsigned char *m, unsigned long long mlen,
        const unsigned char *ad, unsigned long long adlen,
        const unsigned char *nsec, const unsigned char *npub,
        const unsigned char *k);

    int (*decrypt)(unsigned char *m, unsigned long long *outputmlen,
        unsigned char *nsec, const unsigned char *c, unsigned long long clen,
        const unsigned char *ad, unsigned long long adlen,
        const unsigned char *npub, const unsigned char *k);
} caesar_t;

// global flags
extern int brutus_verbose;

// util.c prototypes
void detseq_seed(uint32_t seed);
uint32_t detseq32();
void detseq_fill(void *p, int len);
void hex_dump(void *p, int len);
double plg2chi2(double chi2);
int harness(int (*test_func)(caesar_t *, int), caesar_t *aead, int val);

// test modules
int test_speed(caesar_t *aead, int limit);
int test_throughput(caesar_t *aead, int limit);
int test_coherence(caesar_t *aead, int limit);
int test_kat(caesar_t *aead, int limit);
int test_xprmnt(caesar_t *aead, int limit);

#endif
