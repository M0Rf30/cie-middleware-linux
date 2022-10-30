/*
        RSAEURO.H - header file for RSAEURO cryptographic toolkit

    Copyright (c) J.S.A.Kapp 1994 - 1996.

        RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

        All functions prototypes are the Same as for RSAREF(tm).
        To aid compatibility the source and the files follow the
        same naming conventions that RSAREF(tm) uses.  This should aid
    direct importing to your applications.

        This library is legal everywhere outside the US.  And should
        NOT be imported to the US and used there.

        This header file contains prototypes, and other definitions used
        in and by RSAEURO.

        Revision history
        0.90 First revision, implements all of RSAREF.H plus some minor
        changes.

        0.91 Second revision, Fixed a couple of problems.  SHS support
        added to header file, digest contexts altered.

        0.92 Third revision, add support at this level for the assembler
        routines and the implementation of some routines using the ANSI C
        equivalent.

        0.93 Fourth revision, Library details section added, MD4 details
        added to header file, digest contexts altered.

    1.03 Fifth revision, RSA key structures altered.

    1.04 Sixth revision, New error types added, fix under windows
    regarding IDOK define. RSAEUROINFO release stuff added.

*/

#ifndef _RSAEURO_H_
#define _RSAEURO_H_

#include <string.h>

#include "global.h"
//#include "md2.h"                               /* allow access to others */
//#include "md4.h"
//#include "md5.h"
//#include "shs.h"
#include "nn.h"
//#include "des.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _RSAEURO_EXPORT_

/* Message-digest algorithms. */

#define DA_MD2 2
#define DA_MD4 4
#define DA_MD5 5
#define DA_SHS 3

/* Encryption algorithms to be ored with digest algorithm in Seal and Open. */

#define EA_DES_CBC 1
#define EA_DES_EDE2_CBC 2
#define EA_DES_EDE3_CBC 3
#define EA_DESX_CBC 4

/* RSA key lengths. */

#define MIN_RSA_MODULUS_BITS 508
/*
         PGP 2.6.2 Now allows 2048-bit keys changing below will allow this.
     It does lengthen key generation slightly if the value is increased.
*/
#define MAX_RSA_MODULUS_BITS 4096
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Maximum lengths of encoded and encrypted content, as a function of
         content length len. Also, inverse functions. */

#define ENCODED_CONTENT_LEN(len) (4 * (len) / 3 + 3)
#define ENCRYPTED_CONTENT_LEN(len) ENCODED_CONTENT_LEN((len) + 8)
#define DECODED_CONTENT_LEN(len) (3 * (len) / 4 + 1)
#define DECRYPTED_CONTENT_LEN(len) (DECODED_CONTENT_LEN(len) - 1)

/* Maximum lengths of signatures, encrypted keys, encrypted
         signatures, and message digests. */

#define MAX_SIGNATURE_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_SIGNATURE_LEN ENCODED_CONTENT_LEN(MAX_SIGNATURE_LEN)
#define MAX_ENCRYPTED_KEY_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_ENCRYPTED_KEY_LEN ENCODED_CONTENT_LEN(MAX_ENCRYPTED_KEY_LEN)
#define MAX_PEM_ENCRYPTED_SIGNATURE_LEN ENCRYPTED_CONTENT_LEN(MAX_SIGNATURE_LEN)
#define MAX_DIGEST_LEN 20

/* Maximum length of Diffie-Hellman parameters. */

#define DH_PRIME_LEN(bits) (((bits) + 7) / 8)

/* Error codes. */

#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d
#define RE_FILE 0x040e

/* Library details. */

#define RSAEURO_VER_MAJ 1
#define RSAEURO_VER_MIN 04
#define RSAEURO_IDENT "RSAEURO Toolkit"
#define RSAEURO_DATE "21/08/94"

/* Internal Error Codes */

/* IDOK and IDERROR changed to ID_OK and ID_ERROR */

#define ID_OK 0
#define ID_ERROR 1

/* Internal defs. */

#define TRUE 1
#define FALSE 0

/* Algorithm IDs */

#define IA_MD2 0x00000001
#define IA_MD4 0x00000002
#define IA_MD5 0x00000004
#define IA_SHS 0x00000008
#define IA_DES_CBC 0x00000010
#define IA_DES_EDE2_CBC 0x00000020
#define IA_DES_EDE3_CBC 0x00000040
#define IA_DESX_CBC 0x00000080
#define IA_RSA 0x00010000
#define IA_DH 0x00020000

#define IA_FLAGS                                                      \
  (IA_MD2 | IA_MD4 | IA_MD5 | IA_SHS | IA_DES_CBC | IA_DES_EDE2_CBC | \
   IA_DES_EDE3_CBC | IA_DESX_CBC | IA_RSA | IA_DH)

/* RSAEuro Info Structure */

typedef struct {
  unsigned short int Version;       /* RSAEuro Version */
  unsigned int flags;               /* Version Flags */
  unsigned char ManufacturerID[32]; /* Toolkit ID */
  unsigned int Algorithms;          /* Algorithms Supported */
} RSAEUROINFO;

/* Random structure. */

typedef struct {
  unsigned int bytesNeeded;     /* seed bytes required */
  unsigned char state[16];      /* state of object */
  unsigned int outputAvailable; /* number byte available */
  unsigned char output[16];     /* output bytes */
} R_RANDOM_STRUCT;

/* RSA public and private key. */

typedef struct {
  unsigned short int bits;                     /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];  /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN]; /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct {
  unsigned short int bits;                    /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN]; /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN]; /* public exponent */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];       /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN];         /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN]; /* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN];      /* CRT coefficient */
} R_RSA_PRIVATE_KEY;

/* RSA prototype key. */

typedef struct {
  unsigned int bits; /* length in bits of modulus */
  int useFermat4;    /* public exponent (1 = F4, 0 = 3) */
} R_RSA_PROTO_KEY;

/* Diffie-Hellman parameters. */

typedef struct {
  unsigned char *prime;      /* prime */
  unsigned int primeLen;     /* length of prime */
  unsigned char *generator;  /* generator */
  unsigned int generatorLen; /* length of generator */
} R_DH_PARAMS;

#ifdef __cplusplus
}
#endif

#endif /* _RSAEURO_H_ */
