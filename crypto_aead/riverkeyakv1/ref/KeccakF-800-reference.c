/*
The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by the designers and Ronny Van Keer,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brg_endian.h"
#include "displayIntermediateValues.h"
#include "KeccakF-800-reference.h"

typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef UINT32 tKeccakLane;

#define nrRounds 22
tKeccakLane KeccakRoundConstants[nrRounds];
#define nrLanes 25
unsigned int KeccakRhoOffsets[nrLanes];

/* ---------------------------------------------------------------- */

void KeccakF800_InitializeRoundConstants();
void KeccakF800_InitializeRhoOffsets();
int LFSR86540(UINT8 *LFSR);

void KeccakF800_Initialize()
{
    if (sizeof(tKeccakLane) != 4) {
        printf("tKeccakLane should be 32-bit wide\n");
        abort();
    }
    KeccakF800_InitializeRoundConstants();
    KeccakF800_InitializeRhoOffsets();
}

void KeccakF800_InitializeRoundConstants()
{
    UINT8 LFSRstate = 0x01;
    unsigned int i, j, bitPosition;

    for(i=0; i<nrRounds; i++) {
        KeccakRoundConstants[i] = 0;
        for(j=0; j<7; j++) {
            bitPosition = (1<<j)-1; //2^j-1
            if (LFSR86540(&LFSRstate) && (bitPosition < (sizeof(tKeccakLane)*8)))
                KeccakRoundConstants[i] ^= (tKeccakLane)(1<<bitPosition);
        }
    }
}

#define index(x, y) (((x)%5)+5*((y)%5))

void KeccakF800_InitializeRhoOffsets()
{
    unsigned int x, y, t, newX, newY;

    KeccakRhoOffsets[index(0, 0)] = 0;
    x = 1;
    y = 0;
    for(t=0; t<24; t++) {
        KeccakRhoOffsets[index(x, y)] = ((t+1)*(t+2)/2) % (sizeof(tKeccakLane) * 8);
        newX = (0*x+1*y) % 5;
        newY = (2*x+3*y) % 5;
        x = newX;
        y = newY;
    }
}

int LFSR86540(UINT8 *LFSR)
{
    int result = ((*LFSR) & 0x01) != 0;
    if (((*LFSR) & 0x80) != 0)
        // Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1
        (*LFSR) = ((*LFSR) << 1) ^ 0x71;
    else
        (*LFSR) <<= 1;
    return result;
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateInitialize(void *state)
{
    memset(state, 0, KeccakF_width/8);
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateXORBytesInLane(void *state, unsigned int lanePosition, const unsigned char *data, unsigned int offset, unsigned int length)
{
    unsigned int i;

    for(i=0; i<length; i++)
        ((unsigned char *)state)[lanePosition*sizeof(tKeccakLane)+offset+i] ^= data[i];
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateXORLanes(void *state, const unsigned char *data, unsigned int laneCount)
{
    unsigned int i;

	laneCount *= sizeof(tKeccakLane);
    for(i=0; i<laneCount; i++)
        ((unsigned char *)state)[i] ^= data[i];
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateOverwriteBytesInLane(void *state, unsigned int lanePosition, const unsigned char *data, unsigned int offset, unsigned int length)
{
    memcpy((unsigned char*)state+lanePosition*sizeof(tKeccakLane)+offset, data, length);
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateOverwriteLanes(void *state, const unsigned char *data, unsigned int laneCount)
{
    memcpy(state, data, laneCount*sizeof(tKeccakLane));
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateOverwriteWithZeroes(void *state, unsigned int byteCount)
{
    memset(state, 0, byteCount);
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateComplementBit(void *state, unsigned int position)
{
    if (position < 800) {
        unsigned int bytePosition = position/8;
        unsigned int bitPosition = position%8;
        
        ((unsigned char *)state)[bytePosition] ^= (UINT8)1 << bitPosition;
    }
}

/* ---------------------------------------------------------------- */

void fromBytesToWords(tKeccakLane *stateAsWords, const unsigned char *state);
void fromWordsToBytes(unsigned char *state, const tKeccakLane *stateAsWords);
void KeccakF800OnWords(tKeccakLane *state);
void KeccakF800Round(tKeccakLane *state, unsigned int indexRound);
void theta(tKeccakLane *A);
void rho(tKeccakLane *A);
void pi(tKeccakLane *A);
void chi(tKeccakLane *A);
void iota(tKeccakLane *A, unsigned int indexRound);

void KeccakF800_StatePermute(void *state)
{
#if (PLATFORM_BYTE_ORDER != IS_LITTLE_ENDIAN)
    tKeccakLane stateAsWords[KeccakF_width/sizeof(tKeccakLane)];
#endif

    displayStateAsBytes(1, "Input of permutation", (const unsigned char *)state);
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    KeccakF800OnWords((tKeccakLane*)state);
#else
    fromBytesToWords(stateAsWords, (const unsigned char *)state);
    KeccakF800OnWords(stateAsWords);
    fromWordsToBytes((unsigned char *)state, stateAsWords);
#endif
    displayStateAsBytes(1, "State after permutation", (const unsigned char *)state);
}

void fromBytesToWords(tKeccakLane *stateAsWords, const unsigned char *state)
{
    unsigned int i, j;

    for(i=0; i<nrLanes; i++) {
        stateAsWords[i] = 0;
        for(j=0; j<sizeof(tKeccakLane); j++)
            stateAsWords[i] |= (tKeccakLane)(state[i*sizeof(tKeccakLane)+j]) << (8*j);
    }
}

void fromWordsToBytes(unsigned char *state, const tKeccakLane *stateAsWords)
{
    unsigned int i, j;

    for(i=0; i<nrLanes; i++)
        for(j=0; j<sizeof(tKeccakLane); j++)
            state[i*sizeof(tKeccakLane)+j] = (stateAsWords[i] >> (8*j)) & 0xFF;
}

void KeccakF800OnWords(tKeccakLane *state)
{
    unsigned int i;

    displayStateAsLanes(3, "Same, with lanes as 32-bit words", state);

    for(i=0; i<nrRounds; i++)
		KeccakF800Round(state, i);
}

void KeccakF800Round(tKeccakLane *state, unsigned int indexRound)
{
    displayRoundNumber(3, indexRound);

    theta(state);
    displayStateAsLanes(3, "After theta", state);

    rho(state);
    displayStateAsLanes(3, "After rho", state);

    pi(state);
    displayStateAsLanes(3, "After pi", state);

    chi(state);
    displayStateAsLanes(3, "After chi", state);

    iota(state, indexRound);
    displayStateAsLanes(3, "After iota", state);
}

#define ROL32(a, offset) ((offset != 0) ? ((((tKeccakLane)a) << offset) ^ (((tKeccakLane)a) >> (sizeof(tKeccakLane)*8-offset))) : a)

void theta(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane C[5], D[5];

    for(x=0; x<5; x++) {
        C[x] = 0; 
        for(y=0; y<5; y++) 
            C[x] ^= A[index(x, y)];
    }
    for(x=0; x<5; x++)
        D[x] = ROL32(C[(x+1)%5], 1) ^ C[(x+4)%5];
    for(x=0; x<5; x++)
        for(y=0; y<5; y++)
            A[index(x, y)] ^= D[x];
}

void rho(tKeccakLane *A)
{
    unsigned int x, y;

    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(x, y)] = ROL32(A[index(x, y)], KeccakRhoOffsets[index(x, y)]);
}

void pi(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane tempA[25];

    for(x=0; x<5; x++) for(y=0; y<5; y++)
        tempA[index(x, y)] = A[index(x, y)];
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(0*x+1*y, 2*x+3*y)] = tempA[index(x, y)];
}

void chi(tKeccakLane *A)
{
    unsigned int x, y;
    tKeccakLane C[5];

    for(y=0; y<5; y++) { 
        for(x=0; x<5; x++)
            C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
        for(x=0; x<5; x++)
            A[index(x, y)] = C[x];
    }
}

void iota(tKeccakLane *A, unsigned int indexRound)
{
    A[index(0, 0)] ^= KeccakRoundConstants[indexRound];
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateExtractBytesInLane(const void *state, unsigned int lanePosition, unsigned char *data, unsigned int offset, unsigned int length)
{
    memcpy(data, (unsigned char*)state+lanePosition*sizeof(tKeccakLane)+offset, length);
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateExtractLanes(const void *state, unsigned char *data, unsigned int laneCount)
{
    memcpy(data, state, laneCount*sizeof(tKeccakLane));
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateExtractAndXORBytesInLane(const void *state, unsigned int lanePosition, unsigned char *data, unsigned int offset, unsigned int length)
{
    unsigned int i;

    for(i=0; i<length; i++)
        data[i] ^= ((unsigned char *)state)[lanePosition*sizeof(tKeccakLane)+offset+i];
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateExtractAndXORLanes(const void *state, unsigned char *data, unsigned int laneCount)
{
    unsigned int i;
    for(i=0; i<laneCount*sizeof(tKeccakLane); i++)
        data[i] ^= ((unsigned char *)state)[i];
}

/* ---------------------------------------------------------------- */

void KeccakF800_StateXORPermuteExtract(void *state, const unsigned char *inData, unsigned int inLaneCount, unsigned char *outData, unsigned int outLaneCount)
{
    KeccakF800_StateXORLanes(state, inData, inLaneCount);
    KeccakF800_StatePermute(state);
    KeccakF800_StateExtractLanes(state, outData, outLaneCount);
}

/* ---------------------------------------------------------------- */

void displayRoundConstants(FILE *f)
{
    unsigned int i;

    for(i=0; i<nrRounds; i++) {
        fprintf(f, "RC[%02i][0][0] = ", i);
        fprintf(f, "%08X", (unsigned int)(KeccakRoundConstants[i]));
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

void displayRhoOffsets(FILE *f)
{
    unsigned int x, y;

    for(y=0; y<5; y++) for(x=0; x<5; x++) {
        fprintf(f, "RhoOffset[%i][%i] = ", x, y);
        fprintf(f, "%2i", KeccakRhoOffsets[index(x, y)]);
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}
