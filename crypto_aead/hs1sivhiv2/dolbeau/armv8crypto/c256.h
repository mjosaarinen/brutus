/*
c256.h version $Date: 2016/01/31 08:19:22 $
D. J. Bernstein
Romain Dolbeau
Public domain.
*/

#ifndef USE_3INST_ROT
#define VEC4_ROT(a,imm) vsliq_n_u32(vshrq_n_u32(a, 32-imm),a,imm)
#else
#define VEC4_ROT(a,imm) veorq_u32(vshlq_n_u32(a,imm),vshrq_n_u32(a, 32-imm))
#endif

#define VEC4_ROT16(a) vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(a)))

#define VEC4_QUARTERROUND(a,b,c,d)                                \
   x_##a = vaddq_u32(x_##a, x_##b); t_##a = veorq_u32(x_##d, x_##a); x_##d = VEC4_ROT16(t_##a); \
   x_##c = vaddq_u32(x_##c, x_##d); t_##c = veorq_u32(x_##b, x_##c); x_##b = VEC4_ROT(t_##c, 12); \
   x_##a = vaddq_u32(x_##a, x_##b); t_##a = veorq_u32(x_##d, x_##a); x_##d = VEC4_ROT(t_##a,  8); \
   x_##c = vaddq_u32(x_##c, x_##d); t_##c = veorq_u32(x_##b, x_##c); x_##b = VEC4_ROT(t_##c,  7)

  u32 in12, in13;
  uint32x4_t x_0 = vdupq_n_u32(x[0]);
  uint32x4_t x_1 = vdupq_n_u32(x[1]);
  uint32x4_t x_2 = vdupq_n_u32(x[2]);
  uint32x4_t x_3 = vdupq_n_u32(x[3]);
  uint32x4_t x_4 = vdupq_n_u32(x[4]);
  uint32x4_t x_5 = vdupq_n_u32(x[5]);
  uint32x4_t x_6 = vdupq_n_u32(x[6]);
  uint32x4_t x_7 = vdupq_n_u32(x[7]);
  uint32x4_t x_8 = vdupq_n_u32(x[8]);
  uint32x4_t x_9 = vdupq_n_u32(x[9]);
  uint32x4_t x_10 = vdupq_n_u32(x[10]);
  uint32x4_t x_11 = vdupq_n_u32(x[11]);
  uint32x4_t x_12;// = vdupq_n_u32(x[12]); /* useless */
  uint32x4_t x_13;// = vdupq_n_u32(x[13]); /* useless */
  uint32x4_t x_14 = vdupq_n_u32(x[14]);
  uint32x4_t x_15 = vdupq_n_u32(x[15]);
  uint32x4_t orig0 = x_0;
  uint32x4_t orig1 = x_1;
  uint32x4_t orig2 = x_2;
  uint32x4_t orig3 = x_3;
  uint32x4_t orig4 = x_4;
  uint32x4_t orig5 = x_5;
  uint32x4_t orig6 = x_6;
  uint32x4_t orig7 = x_7;
  uint32x4_t orig8 = x_8;
  uint32x4_t orig9 = x_9;
  uint32x4_t orig10 = x_10;
  uint32x4_t orig11 = x_11;
  uint32x4_t orig12;// = x_12; /* useless */
  uint32x4_t orig13;// = x_13; /* useless */
  uint32x4_t orig14 = x_14;
  uint32x4_t orig15 = x_15;
  uint32x4_t t_0;
  uint32x4_t t_1;
  uint32x4_t t_2;
  uint32x4_t t_3;
  uint32x4_t t_4;
  uint32x4_t t_5;
  uint32x4_t t_6;
  uint32x4_t t_7;
  uint32x4_t t_8;
  uint32x4_t t_9;
  uint32x4_t t_10;
  uint32x4_t t_11;
  uint32x4_t t_12;
  uint32x4_t t_13;
  uint32x4_t t_14;
  uint32x4_t t_15;

    x_0 = orig0;
    x_1 = orig1;
    x_2 = orig2;
    x_3 = orig3;
    x_4 = orig4;
    x_5 = orig5;
    x_6 = orig6;
    x_7 = orig7;
    x_8 = orig8;
    x_9 = orig9;
    x_10 = orig10;
    x_11 = orig11;
    //x_12 = orig12; /* useless */
    //x_13 = orig13; /* useless */
    x_14 = orig14;
    x_15 = orig15;



    const uint64x2_t addv12 = vcombine_u64(vcreate_u64(2),vcreate_u64(3));
    const uint64x2_t addv13 = vcombine_u64(vcreate_u64(0),vcreate_u64(1));
    uint64x2_t t12, t13;
    in12 = x[12];
    in13 = x[13];
    u64 in1213 = ((u64)in12) | (((u64)in13) << 32);
    t12 = vdupq_n_u64(in1213);
    t13 = vdupq_n_u64(in1213);

    x_12 = vreinterpretq_u32_u64(vaddq_u64(addv12, t12));
    x_13 = vreinterpretq_u32_u64(vaddq_u64(addv13, t13));

    uint32x4x2_t t;
    t = vuzpq_u32(x_13,x_12);
    x_12 = t.val[0];
    x_13 = t.val[1];

    orig12 = x_12;
    orig13 = x_13;

    in1213 += 4;
    
    x[12] = in1213 & 0xFFFFFFFF;
    x[13] = (in1213>>32)&0xFFFFFFFF;

    for (i = 0 ; i < 20 ; i+=2) {
      VEC4_QUARTERROUND( 0, 4, 8,12);
      VEC4_QUARTERROUND( 1, 5, 9,13);
      VEC4_QUARTERROUND( 2, 6,10,14);
      VEC4_QUARTERROUND( 3, 7,11,15);
      VEC4_QUARTERROUND( 0, 5,10,15);
      VEC4_QUARTERROUND( 1, 6,11,12);
      VEC4_QUARTERROUND( 2, 7, 8,13);
      VEC4_QUARTERROUND( 3, 4, 9,14);
    }

#define ONEQUAD_TRANSPOSE_NOXOR(a,b,c,d)                                \
    {                                                                   \
      uint32x4_t t0l, t0h, t1l, t1h;                                    \
      x_##a = vaddq_u32(x_##a, orig##a);                                \
      x_##b = vaddq_u32(x_##b, orig##b);                                \
      x_##c = vaddq_u32(x_##c, orig##c);                                \
      x_##d = vaddq_u32(x_##d, orig##d);                                \
      t0l = vtrn1q_u32(x_##a,x_##b);                                    \
      t0h = vtrn2q_u32(x_##a,x_##b);                                    \
      t1l = vtrn1q_u32(x_##c,x_##d);                                    \
      t1h = vtrn2q_u32(x_##c,x_##d);                                    \
      x_##a = (uint32x4_t)vtrn1q_u64((uint64x2_t)t0l,(uint64x2_t)t1l);  \
      x_##b = (uint32x4_t)vtrn1q_u64((uint64x2_t)t0h,(uint64x2_t)t1h);  \
      x_##c = (uint32x4_t)vtrn2q_u64((uint64x2_t)t0l,(uint64x2_t)t1l);  \
      x_##d = (uint32x4_t)vtrn2q_u64((uint64x2_t)t0h,(uint64x2_t)t1h);  \
      vst1q_u32((uint32_t*)(out+0),x_##a);                              \
      vst1q_u32((uint32_t*)(out+64),x_##b);                             \
      vst1q_u32((uint32_t*)(out+128),x_##c);                            \
      vst1q_u32((uint32_t*)(out+192),x_##d);                            \
    }

    ONEQUAD_TRANSPOSE_NOXOR(0,1,2,3);
    out+=16;
    ONEQUAD_TRANSPOSE_NOXOR(4,5,6,7);
    out+=16;
    ONEQUAD_TRANSPOSE_NOXOR(8,9,10,11);
    out+=16;
    ONEQUAD_TRANSPOSE_NOXOR(12,13,14,15);
    out-=48;
    
#undef ONEQUAD_TRANSPOSE_NOXOR

    out += 256;
#undef VEC4_ROT
#undef VEC4_QUARTERROUND
