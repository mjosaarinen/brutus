// wbob_pi64.c
// 25-Jun-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

// The "64-bit Reference" implementation of WhirlBob Pi permutation.

#include "stribob.h"
#include "wbob_tab64.h"

void wbob_pi(w512_t *s512)
{
    int i;
    uint8_t *pt;
    uint64_t k[8], l[8];

    // load block
    pt = (uint8_t *) s512->b;
    for (i = 0; i < 8; i++) {

        k[i] =  (((uint64_t) pt[0]        ) << 56) ^
                (((uint64_t) pt[1] & 0xFFL) << 48) ^
                (((uint64_t) pt[2] & 0xFFL) << 40) ^
                (((uint64_t) pt[3] & 0xFFL) << 32) ^
                (((uint64_t) pt[4] & 0xFFL) << 24) ^
                (((uint64_t) pt[5] & 0xFFL) << 16) ^
                (((uint64_t) pt[6] & 0xFFL) <<  8) ^
                (((uint64_t) pt[7] & 0xFFL)      );
                
        pt += 8;
    }

    for (i = 0; i < 12; i++) {

        l[0] =  wbob_sm0[(k[0] >> 56)       ] ^
                wbob_sm1[(k[7] >> 48) & 0xFF] ^
                wbob_sm2[(k[6] >> 40) & 0xFF] ^
                wbob_sm3[(k[5] >> 32) & 0xFF] ^
                wbob_sm4[(k[4] >> 24) & 0xFF] ^
                wbob_sm5[(k[3] >> 16) & 0xFF] ^
                wbob_sm6[(k[2] >>  8) & 0xFF] ^
                wbob_sm7[(k[1]      ) & 0xFF] ^ wbob_rc[i];
        
        l[1] =  wbob_sm0[(k[1] >> 56)       ] ^
                wbob_sm1[(k[0] >> 48) & 0xFF] ^
                wbob_sm2[(k[7] >> 40) & 0xFF] ^
                wbob_sm3[(k[6] >> 32) & 0xFF] ^
                wbob_sm4[(k[5] >> 24) & 0xFF] ^
                wbob_sm5[(k[4] >> 16) & 0xFF] ^
                wbob_sm6[(k[3] >>  8) & 0xFF] ^
                wbob_sm7[(k[2]      ) & 0xFF];

        l[2] =  wbob_sm0[(k[2] >> 56)       ] ^
                wbob_sm1[(k[1] >> 48) & 0xFF] ^
                wbob_sm2[(k[0] >> 40) & 0xFF] ^
                wbob_sm3[(k[7] >> 32) & 0xFF] ^
                wbob_sm4[(k[6] >> 24) & 0xFF] ^
                wbob_sm5[(k[5] >> 16) & 0xFF] ^
                wbob_sm6[(k[4] >>  8) & 0xFF] ^
                wbob_sm7[(k[3]      ) & 0xFF];

        l[3] =  wbob_sm0[(k[3] >> 56)       ] ^
                wbob_sm1[(k[2] >> 48) & 0xFF] ^
                wbob_sm2[(k[1] >> 40) & 0xFF] ^
                wbob_sm3[(k[0] >> 32) & 0xFF] ^
                wbob_sm4[(k[7] >> 24) & 0xFF] ^
                wbob_sm5[(k[6] >> 16) & 0xFF] ^
                wbob_sm6[(k[5] >>  8) & 0xFF] ^
                wbob_sm7[(k[4]      ) & 0xFF];

        l[4] =  wbob_sm0[(k[4] >> 56)       ] ^
                wbob_sm1[(k[3] >> 48) & 0xFF] ^
                wbob_sm2[(k[2] >> 40) & 0xFF] ^
                wbob_sm3[(k[1] >> 32) & 0xFF] ^
                wbob_sm4[(k[0] >> 24) & 0xFF] ^
                wbob_sm5[(k[7] >> 16) & 0xFF] ^
                wbob_sm6[(k[6] >>  8) & 0xFF] ^
                wbob_sm7[(k[5]      ) & 0xFF];

        l[5] =  wbob_sm0[(k[5] >> 56)       ] ^
                wbob_sm1[(k[4] >> 48) & 0xFF] ^
                wbob_sm2[(k[3] >> 40) & 0xFF] ^
                wbob_sm3[(k[2] >> 32) & 0xFF] ^
                wbob_sm4[(k[1] >> 24) & 0xFF] ^
                wbob_sm5[(k[0] >> 16) & 0xFF] ^
                wbob_sm6[(k[7] >>  8) & 0xFF] ^
                wbob_sm7[(k[6]      ) & 0xFF];

        l[6] =  wbob_sm0[(k[6] >> 56)       ] ^
                wbob_sm1[(k[5] >> 48) & 0xFF] ^
                wbob_sm2[(k[4] >> 40) & 0xFF] ^
                wbob_sm3[(k[3] >> 32) & 0xFF] ^
                wbob_sm4[(k[2] >> 24) & 0xFF] ^
                wbob_sm5[(k[1] >> 16) & 0xFF] ^
                wbob_sm6[(k[0] >>  8) & 0xFF] ^
                wbob_sm7[(k[7]      ) & 0xFF];

        l[7] =  wbob_sm0[(k[7] >> 56)       ] ^
                wbob_sm1[(k[6] >> 48) & 0xFF] ^
                wbob_sm2[(k[5] >> 40) & 0xFF] ^
                wbob_sm3[(k[4] >> 32) & 0xFF] ^
                wbob_sm4[(k[3] >> 24) & 0xFF] ^
                wbob_sm5[(k[2] >> 16) & 0xFF] ^
                wbob_sm6[(k[1] >>  8) & 0xFF] ^
                wbob_sm7[(k[0]      ) & 0xFF];  

        k[0] = l[0];
        k[1] = l[1];
        k[2] = l[2];
        k[3] = l[3];
        k[4] = l[4];
        k[5] = l[5];
        k[6] = l[6];
        k[7] = l[7];
    }

    pt = (uint8_t *) s512->b;
    for (i = 0; i < 8; i++) {
        pt[0] = (uint8_t)(l[i] >> 56);
        pt[1] = (uint8_t)(l[i] >> 48);
        pt[2] = (uint8_t)(l[i] >> 40);
        pt[3] = (uint8_t)(l[i] >> 32);
        pt[4] = (uint8_t)(l[i] >> 24);
        pt[5] = (uint8_t)(l[i] >> 16);
        pt[6] = (uint8_t)(l[i] >>  8);
        pt[7] = (uint8_t)(l[i]      );
        pt += 8;
    }
}

