/*
The Ketje authenticated encryption scheme, designed by Guido Bertoni,
Joan Daemen, Michaël Peeters, Gilles Van Assche and Ronny Van Keer.
For more information, feedback or questions, please refer to our website:
http://ketje.noekeon.org/

Implementation by the designers,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef _Ketje_h_
#define _Ketje_h_

#include "KeccakF-interface.h"

#ifdef ALIGN
#undef ALIGN
#endif

#if defined(__GNUC__)
#define ALIGN __attribute__ ((aligned(32)))
#elif defined(_MSC_VER)
#define ALIGN __declspec(align(32))
#else
#define ALIGN
#endif

/** The phase is a data element that expresses what Ketje is doing
 * - virgin: the only operation supported is initialization, loading the key and nonce. This will switch 
 *   the phase to feedingAssociatedData
 * - feedingAssociatedData: Ketje is ready for feeding associated data, has started feeding associated data
 *   or has finished feeding associated data. It allows feeding some more associated data in which case the phase does not
 *   change. One can also start wrapping plaintext, that sets the phase to wrapping. Finally, one can
 *   start unwrapping ciphertext, that sets the phase to unwrapping.
 * - wrapping: Ketje is ready for wrapping some more plaintext or for delivering the tag.
 *   Wrapping more plaintext does not modify the phase, asking for the tag sets the phase to feedingAssociatedData.
 * - unwrapping: Ketje is ready for unwrapping some more ciphertext or for delivering the tag.
 *   Unwrapping more ciphertext does not modify the phase, asking for the tag sets the phase to feedingAssociatedData.
 */
enum Phase {
    Ketje_Phase_Virgin 			= 0,
    Ketje_Phase_FeedingAssociatedData	= 1,
    Ketje_Phase_Wrapping		= 2,
    Ketje_Phase_Unwrapping		= 4
};

/**
  * Structure that contains the Ketje instance for use with the
  * Ketje* functions.
  */
ALIGN typedef struct {
    /** The state processed by the permutation. */
	ALIGN unsigned char state[KeccakF_stateSizeInBytes];

    /** The phase. */
    unsigned int phase;

	/** The amount of associated or plaintext data that has been 
      * XORed into the state after the last call to step or stride */
    unsigned int dataRemainderSize;
} Ketje_Instance;

/**
  * Function that feeds the key and the nonce. Both are bit strings and consist of a sequence of bytes.
  *
  * @param  instance		Pointer to the Ketje instance structure.
  * @param  key             Pointer to the key.
  * @param  keySizeInBits   The size of the key in bits.
  * @param  nonce           Pointer to the nonce.
  * @param  nonceSizeInBits The size of the nonce in bits.
  *
  * @pre    phase == *
  * @pre    8*((keySizeInBits+16)/8) + nonceSizeInBits + 2 <= 8*KeccakF_width
  *
  * @post   phase == feedingAssociatedData
  *
  * @return 0 if successful, 1 otherwise.
  */
int Ketje_Initialize(Ketje_Instance *instance, const unsigned char *key, unsigned int keySizeInBits, const unsigned char *nonce, unsigned int nonceSizeInBits);

/**
  * Function that feeds (partial) associated data that consists of a sequence
  * of bytes. Associated data may be fed in multiple calls to this function.
  * The end of it is indicated by a call to wrap or unwrap.
  *
  * @param  instance		    Pointer to the Ketje instance structure.
  * @param  data                Pointer to the (partial) associated data.
  * @param  dataSizeInBytes     The size of the (partial) associated data in bytes.
  *
  * @pre    phase == feedingAssociatedData
  *
  * @post   phase == feedingAssociatedData
  *
  * @return 0 if successful, 1 otherwise.
  */
int Ketje_FeedAssociatedData(Ketje_Instance *instance, const unsigned char *data, unsigned int dataSizeInBytes);

/**
  * Function that presents a (partial) plaintext body that consists of a
  * sequence of bytes for wrapping. A plaintext body may be wrapped in
  * multiple calls to wrap. The end of the plaintext body is indicated
  * by a call to getTag.
  *
  * @param  instance		    Pointer to the Ketje instance structure.
  * @param  plaintext           The (partial) plaintext body.
  * @param  ciphertext          The buffer where enciphered data will be stored, can be equal to plaintext buffer.
  * @param  dataSizeInBytes     The size of the (partial) plaintext body.
  *
  * @pre    ( phase == feedingAssociatedData ) or ( phase == wrapping )
  *
  * @post   phase == wrapping
  *
  * @return 0 if successful, 1 otherwise.
  */
int Ketje_WrapPlaintext(Ketje_Instance *instance, const unsigned char *plaintext, unsigned char *ciphertext, unsigned int dataSizeInBytes);

/**
  * Function that presents a (partial) ciphertext body that consists of a
  * sequence of bytes for unwrapping. A ciphertext body may be wrapped in
  * multiple calls to unwrap. The end of the ciphertext body is indicated
  * by a call to getTag.
  *
  * @param  instance		    Pointer to the Ketje instance structure.
  * @param  ciphertext          The (partial) ciphertext body.
  * @param  plaintext           The buffer where deciphered data will be stored, can be equal to ciphertext buffer.
  * @param  dataSizeInBytes     The size of the (partial) ciphertext body.
  *
  * @pre    ( phase == feedingAssociatedData ) or ( phase == unwrapping )
  *
  * @post   phase == unwrapping
  *
  * @return 0 if successful, 1 otherwise.
  */
int Ketje_UnwrapCiphertext(Ketje_Instance *instance, const unsigned char *ciphertext, unsigned char *plaintext, unsigned int dataSizeInBytes);

/**
  * Function that gets a tag of a requested size in bytes. The full tag must be retrieved
  * with a single call to getTag.
  *
  * @param  instance		    Pointer to the Ketje instance structure.
  * @param  tag                 The buffer where to store the tag.
  * @param  tagSizeInBytes      The length in bytes of the tag requested.
  *
  * @pre    ( phase == wrapping ) or ( phase == unwrapping )
  *
  * @post   phase == feedingAssociatedData
  *
  * @return 0 if successful, 1 otherwise.
  */
int Ketje_GetTag(Ketje_Instance *instance, unsigned char *tag, unsigned int tagSizeInBytes);

#if (KeccakF_width == 400)
	#define KetjeSr_Initialize			Ketje_Initialize
	#define KetjeSr_FeedAssociatedData	Ketje_FeedAssociatedData
	#define KetjeSr_WrapPlaintext		Ketje_WrapPlaintext
	#define KetjeSr_UnwrapCiphertext	Ketje_UnwrapCiphertext
	#define KetjeSr_GetTag				Ketje_GetTag
#else
	#define KetjeJr_Initialize			Ketje_Initialize
	#define KetjeJr_FeedAssociatedData	Ketje_FeedAssociatedData
	#define KetjeJr_WrapPlaintext		Ketje_WrapPlaintext
	#define KetjeJr_UnwrapCiphertext	Ketje_UnwrapCiphertext
	#define KetjeJr_GetTag				Ketje_GetTag
#endif

#endif
