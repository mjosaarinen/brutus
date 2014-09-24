// main.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <ctype.h>

#include "brutus.h"

// global flags
int brutus_verbose;

const char brutus_usage[] =
    "Usage: brutus [flags] aead1.so aead2.so ..\n"
    "  -h   Quick help\n"
    "  -q   Switch off verbose\n"
    "  -tN  Force exit after N seconds\n"
    "  -rN  Use random seed N\n"
    "  -cN  Coherence test (N sec timeout)\n"
    "  -sN  Encryption/Authentication Speed (N secs each)\n"
    "  -fN  Fast throughput test (N secs for enc/dec)\n";
//  "  -xN  Correlation experiment with param N.\n";

// main

int main(int argc, char **argv)
{
    int t, i, *ipt, ciphers;
    char *str;
    caesar_t *aead, *candidate;
    int flag_coherence, flag_speed, flag_fast, flag_xprmt, flag_timeout;

    // test modes
    brutus_verbose = 1;
    flag_coherence = 0;
    flag_speed = 0;
    flag_fast = 0;
    flag_xprmt = 0;
    flag_timeout = 0;

    // no paramets
    if (argc < 2) {
        fprintf(stderr, "%s", brutus_usage);
        return -1;
    }

    // approximate table sizes
    if ((candidate = calloc(argc, sizeof(caesar_t))) == NULL) {
        perror("calloc()");
        return -1;
    }

    ciphers = 0;
    for (i = 1; i < argc; i++) {
        // flag ?
        if (argv[i][0] == '-') {

            if (argv[i][1] != 0) {
                t = argv[i][2];
                if (t >= '0' && t <= '9')
                    t = atoi(&argv[i][2]);
                else
                    t = -1;
            } else {
                fprintf(stderr, "%s: Unknown expression: %s\n",
                    argv[0], argv[i]);
                return -1;
            }

            // for future
            switch(argv[i][1]) {

                case 'c':       // coherence
                    if (t <= 0)
                        flag_coherence = 5;
                    else
                        flag_coherence = t;
                    break;

                case 'f':       // throughput
                    if (t <= 0)
                        flag_fast = 1;
                    else
                        flag_fast = t;
                    break;

                case 'h':       // help to stdout
                    printf("%s", brutus_usage);
                    return 0;

                case 'r':       // random seed
                    if (t < 0)
                        t = time(NULL) & 0x7FFFFFFF;
                    srandom(t);
                    if (brutus_verbose)
                        printf("\tsrandom(%d)\n", t);
                    break;

                case 's':       // speed
                    if (t <= 0)
                        flag_speed = 3;
                    else
                        flag_speed = t;
                    break;

                case 't':       // timeout
                    if (t > 0)
                        flag_timeout = t;
                    else
                        flag_timeout = 0;
                    break;

                case 'q':       // quiet
                    brutus_verbose = 0;
                    break;

                case 'x':       // experiment
                    if (t > 0)
                        flag_xprmt = t;
                    else
                        flag_xprmt = 10;
                    break;

                default:
                    fprintf(stderr, "%s: Unknown flag: %s\n",
                        argv[0], argv[i]);
                return -1;
            }

        } else {

            aead = &candidate[ciphers];
            memset(aead, 0, sizeof(caesar_t));

            aead->dlib = dlopen(argv[i], RTLD_LAZY | RTLD_LOCAL);
            str = dlerror();
            if (str != NULL)
                fprintf(stderr, "%s: %s\n", argv[i], str);

            if (aead->dlib != NULL) {

                // load parameters from the dynamic library
                if ((str = dlsym(aead->dlib, "brutus_name")) != NULL)
                    aead->name = str;
                else
                    aead->name = "<brutus_name>";

                if ((ipt = dlsym(aead->dlib, "brutus_keybytes")) != NULL)
                    aead->keybytes = *ipt;
                if ((ipt = dlsym(aead->dlib, "brutus_nsecbytes")) != NULL)
                    aead->nsecbytes = *ipt;
                if ((ipt = dlsym(aead->dlib, "brutus_npubbytes")) != NULL)
                    aead->npubbytes = *ipt;
                if ((ipt = dlsym(aead->dlib, "brutus_abytes")) != NULL)
                    aead->abytes = *ipt;
                if ((ipt = dlsym(aead->dlib, "brutus_nooverlap")) != NULL)
                    aead->nooverlap = *ipt;

                // these must be present
                aead->encrypt = dlsym(aead->dlib, "crypto_aead_encrypt");
                aead->decrypt = dlsym(aead->dlib, "crypto_aead_decrypt");
                if (aead->encrypt != NULL && aead->decrypt != NULL) {
                    ciphers++;
                } else {
                    dlclose(aead->dlib);
                }
            }
        }
    }

    // banner
    if (brutus_verbose) {
        printf("%s\n",
            "BRUTUS ("__DATE__") by Markku-Juhani O. Saarinen <mjos@iki.fi>");
    }

    // set the timeout
    if (flag_timeout > 0) {
        if (brutus_verbose) {
            printf("\tGlobal timeout in %d secs.\n", flag_timeout);
        }
        alarm(flag_timeout);
    }

    if (ciphers <= 0) {
        fprintf(stderr, "%s: No ciphers specified.\n", argv[0]);
        return 1;
    }

    // now run tests on all ciphers
    for (i = 0; i < ciphers; i++) {
        if (flag_coherence > 0)
            test_coherence(&candidate[i], flag_coherence);
        if (flag_speed > 0)
            test_speed(&candidate[i], flag_speed);
        if (flag_fast > 0)
            test_throughput(&candidate[i], flag_fast);
    }

    // loop until timeout, if specified
    while (flag_xprmt > 0) {
        for (i = 0; i < ciphers; i++)
            test_xprmnt(&candidate[i], flag_xprmt);
        if (flag_timeout == 0)
            break;
    }

    // free up the dynamic libraries
    for (i = 0; i < ciphers; i++)
        dlclose(candidate[i].dlib);
    free(candidate);

    return 0;
}
