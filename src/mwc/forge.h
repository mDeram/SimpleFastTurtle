#ifndef F_FORGE_H
#define F_FORGE_H

/*
 * Some helper for C
 */



#ifndef NULL
#define NULL    ((void *)0)
#endif



typedef unsigned char bool;
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif


/* Can be false on some system */
typedef signed   char  int8;
typedef unsigned char  uint8;
typedef unsigned char  uchar;

typedef signed   short int16;
typedef unsigned short uint16;
typedef unsigned short ushort;

typedef signed   int   int32;
typedef unsigned int   uint32;
typedef unsigned int   uint;

typedef signed   long  int64;
typedef unsigned long  uint64;
typedef unsigned long  ulong;



#define F_STR_HELPER(x) #x
#define F_STR(x) F_STR_HELPER(x)



#endif

