// main.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>

#include "brutus.h"

// global flags
int brutus_verbose;

// main

int main(int argc, char **argv)
{
    int i, *ipt, ciphers;
    char *str;
    caesar_t *aead, *candidate;
    int flag_coherent, flag_speed, flag_fast;

    // test modes
    flag_coherent = 0;
    flag_speed = 0;
    brutus_verbose = 1;
    flag_fast = 0;

    // dynamic parameters
    if (argc < 2) {
        fprintf(stderr, "Usage: brutus [flags] aead1.so aead2.so ..\n"
                        "   -q  Switch off verbose.\n"
                        "   -c  Coherence test.\n"
                        "   -s  Encryption/Authentication Speed test.\n"
                        "   -f  Fast encryption throughput test.\n");
        return -1;
    }

    // allocate tables
    if ((candidate = calloc(argc, sizeof(caesar_t))) == NULL) {
        perror("calloc()");
        return -1;
    }

    ciphers = 0;
    for (i = 1; i < argc; i++) {
        // flag ?
        if (argv[i][0] == '-') {

            // for future
            switch(argv[i][1]) {

                case 'c':
                    flag_coherent++;
                    break;

                case 'f':
                    flag_fast++;
                    break;

                case 's':
                    flag_speed++;
                    break;

                case 'q':
                    brutus_verbose = 0;
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

    // now run tests on all ciphers

    if (flag_coherent) {
        for (i = 0; i < ciphers; i++) {
            test_coherent(&candidate[i]);
            fflush(stdout);
        }
    }
    if (flag_speed > 0 || flag_fast > 0) {
        for (i = 0; i < ciphers; i++) {
            test_speed(&candidate[i], flag_fast);
            fflush(stdout);
        }
    }

    // free up the dynamic libraries
    for (i = 0; i < ciphers; i++)
        dlclose(candidate[i].dlib);
    free(candidate);

    return 0;
}
