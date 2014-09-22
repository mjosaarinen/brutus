// util.c
// 21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "brutus.h"

// utilities

void rand_fill(void *p, int len)
{
    int i;

    for (i = 0; i < len; i++)
        ((uint8_t *) p)[i] = rand();
}

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

