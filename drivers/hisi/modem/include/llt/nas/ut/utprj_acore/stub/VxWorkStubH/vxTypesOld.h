/* vxTypesOld.h - old VxWorks type definition header */

/*
 * Copyright (c) 1990-1997, 2000-2001, 2003-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. 
*/

/*
modification history
--------------------
02x,24aug05,mcm  Adding protection for STATUS.
02w,23aug05,mcm  Protecting definitions of the funcptr types to prevent
                 redefinition errors.
02v,18aug05,mcm  Adding a macro to detect the definition of fd_set etc.
02u,01oct04,tam  defined PHYS_ADDR only for kernel side
02t,30sep04,yvp  Added LOW32 and HIGH32 definitions.
02s,29nov03,yp   moving ADDR to pgPoolLibP.h
02r,14oct03,sru  Added 'signed' to INT8..INT32 typedefs; also, allow 
		 VIRT_ADDR and PHYS_ADDR types to be defined in an
		 architecture-specific manner.
02q,11aug03,yp	 adding VIRT_ADDR and PHYS_ADDR
02p,18aug03,dbt  Added PAL support for VxSim.
02o,07may03,pes  PAL conditional compilation cleanup. Phase 2. Moving
		 arch-specific definitions out to arch files.
02n,17apr03,dbt  Added SIMLINUX support. Removed SIMHPPA and SIMSPARCSUNOS.
02m,05dec01,mem  Added INT64 and UINT64.
02l,27nov01,sbs  changed FD_SETSIZE from 256 to 2048 (SPR #9377)
02k,22oct01,dee  Merge from T2.1.0 ColdFire
02j,18apr01,mem  Updated Arm/Thumb detection.
02i,25feb00,frf  Add SH support for T2
02h,15aug97,cym  added simnt support.
02g,26mar97,cdp  added Thumb (ARM7TDMI_T) support.
02f,28nov96,cdp  added ARM support.
02e,28sep95,ms	 removed "static __inline__" (SPR #4500)
02d,12jul95,ism  added simsolaris support
02c,19mar95,dvs  removed tron references.
02b,01sep94,ism  fixed comment as per SPR# 1512.
02a,02dec93,pme  added Am29K family support.
01z,12jun93,rrr  vxsim.
01y,26may94,yao  added PPC support.
01x,09jun93,hdn  added support for I80X86
01w,12feb93,srh  added C++ versions of FUNCPTR, et al.
01v,13nov92,dnw  added definition of VOID (SPR #1781)
01u,02oct92,srh  replaced conditional around volatile, const, and signed so
		 they won't be elided when __STDC__ is defined.
		 added __cplusplus to __STDC__ condition.
01t,22sep92,rrr  added support for c++
01s,08sep92,smb  made some additions for the MIPS.
01r,07sep92,smb  added __STDC__ and modes to maintain compatibility with 5.0
01q,07jul92,rrr  moved STACK_GROW and ENDIAN to vxArch.h
01p,03jul92,smb  changed name from vxTypes.h.
01o,26may92,rrr  the tree shuffle
01n,25nov91,llk  included sys/types.h.
01m,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -removed TINY and UTINY
		  -changed VOID to void
		  -changed ASMLANGUAGE to _ASMLANGUAGE
		  -changed copyright notice
01l,01oct91,jpb  fixed MIPS conditional for undefined CPU_FAMILY.
01k,20sep91,wmd  conditionalized out defines for const, unsigned and volatile
		 for the MIPS architecture.
01j,02aug91,ajm  added support for MIPS_R3k.
01i,15may91,gae  added define for "signed" when not available for pre-ANSI.
01h,29apr91,hdn  added defines and macros for TRON architecture.
01g,28apr91,del  added defines of __volatile__ and __const__ if !_STDC_
		 && _GNUC__
01f,24mar91,del  added INSTR * define for I960.
01e,28jan91,kdl  added DBLFUNCPTR and FLTFUNCPTR.
01d,25oct90,dnw  changed void to void except when linting.
01c,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
01b,10aug90,dnw  added VOIDFUNCPTR
01a,29may90,del  written.
*/

/*
DESCRIPTION
This header file contains a mixture of stuff.
1) the old style typedefs (ie. POSIX now says they must end with _t).
   These will be phased out gradually.
2) a mechanism for getting rid of const warning which are produced by the
   GNU C compiler. Hopefully, this will be removed in the future.
3) macros that are so longer needed for vxWorks source code but maybe needed
   by some customer applications and are therefore provided for backward
   compatability.
4) system III typedefs (used by netinet) which do not fit in anywhere else.

*/

#ifndef __INCvxTypesOldh
#define __INCvxTypesOldh

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/types.h"

/* vxWorks types */

typedef	signed char	INT8;
typedef	signed short	INT16;
typedef	signed int	INT32;
typedef	signed long long INT64;

typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned int	UINT32;
typedef	unsigned long long UINT64;

typedef	unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef	unsigned int	UINT;
typedef unsigned long	ULONG;

typedef	int		BOOL;
typedef int 		ARGINT;
typedef void		VOID;

#ifndef _STATUS_DEFINED
#define _STATUS_DEFINED
typedef	int		STATUS;
#endif /* _STATUS_DEFINED */

#ifndef _FUNCPTR_DEFINED
#define _FUNCPTR_DEFINED
#ifdef __cplusplus
typedef int 		(*FUNCPTR) (...);  /* ptr to function returning int */
#else
typedef int 		(*FUNCPTR) ();	   /* ptr to function returning int */
#endif /* __cplusplus */
#endif /* _FUNCPTR_DEFINED */

#ifndef _VOIDFUNCPTR_DEFINED
#define _VOIDFUNCPTR_DEFINED
#ifdef __cplusplus
typedef void 		(*VOIDFUNCPTR) (...); /* ptr to function returning void */
#else
typedef void 		(*VOIDFUNCPTR) (); /* ptr to function returning void */
#endif /* __cplusplus */
#endif /* _VOIDFUNCPTR_DEFINED */

#ifndef _DBLFUNCPTR_DEFINED
#define _DBLFUNCPTR_DEFINED
#ifdef __cplusplus
typedef double 		(*DBLFUNCPTR) (...);  /* ptr to function returning double*/
#else
typedef double 		(*DBLFUNCPTR) ();  /* ptr to function returning double*/
#endif /* __cplusplus */
#endif /* _DBLFUNCPTR_DEFINED */

#ifndef _FLTFUNCPTR_DEFINED
#define _FLTFUNCPTR_DEFINED
#ifdef __cplusplus
typedef float 		(*FLTFUNCPTR) (...);  /* ptr to function returning float */
#else
typedef float 		(*FLTFUNCPTR) ();  /* ptr to function returning float */
#endif /* __cplusplus */
#endif /* _FLTFUNCPTR_DEFINED */


/* This structure and the following definitions are needed to get rid
   of const warning produced by the GNU C compiler.
 */

#if defined(__STDC__) || defined(__cplusplus)
typedef union
    {
    long pm_int;
    void *pm_v;
    const void *pm_cv;
    char *pm_c;
    unsigned char *pm_uc;

    signed char *pm_sc;
    const char *pm_cc;
    const unsigned char *pm_cuc;
    const signed char *pm_csc;
    short *pm_s;
    ushort_t *pm_us;
    const short *pm_cs;
    const ushort_t *pm_cus;
    int *pm_i;
    uint_t *pm_ui;
    const int *pm_ci;
    const uint_t *pm_cui;
    long *pm_l;
    ulong_t *pm_ul;
    const long *pm_cl;
    const ulong_t *pm_cul;

    int8_t *pm_i8;
    uint8_t *pm_ui8;
    const int8_t *pm_ci8;
    const uint8_t *pm_cui8;
    int16_t *pm_i16;
    uint16_t *pm_ui16;
    const int16_t *pm_ci16;
    const uint16_t *pm_cui16;
    int32_t *pm_i32;
    uint32_t *pm_ui32;
    const int32_t *pm_ci32;
    const uint32_t *pm_cui32;
#if _ARCH_MOVE_SIZE > 4
    int64_t *pm_i64;
    const int64_t *pm_ci64;
#if _ARCH_MOVE_SIZE > 8
    int128_t *pm_i128;
    const int128_t *pm_ci128;
#endif
#endif
    } pointer_mix_t;

#define CHAR_FROM_CONST(x)	(char *)(x)
#define VOID_FROM_CONST(x)	(void *)(x)

#endif /* __STDC__ */

#define STACK_DIR		_ARCH_STACK_DIR
#define ALIGN_MEMORY		_ARCH_ALIGN_MEMORY
#define ALIGN_STACK		_ARCH_ALIGN_STACK
#define ALIGN_REGS		_ARCH_ALIGN_REGS

#ifndef NBBY
#define	NBBY	8		/* number of bits in a byte */
#endif

/* modes - must match O_RDONLY/O_WRONLY/O_RDWR in ioLib.h! */

#define READ            0
#define WRITE           1
#define UPDATE          2

/* Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= maxFiles parameter in iosInit call found in usrConfig.c.
 * If this define is changed, recompile the source, or else select() will
 * not work.(The user can set the number lower, but not higher).
 *
 * Changing the default to 2048 adds 224 bytes to the size of the fd_set
 * structure.
 */

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	2048
#endif	/* FD_SETSIZE */

#ifndef _FD_MASK_DEFINED
#define _FD_MASK_DEFINED
typedef long	fd_mask;
#endif /* _FD_MASK_DEFINED */

#ifndef NFDBITS
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */
#endif /* NFDBITS */

#ifndef howmany
#define	howmany(x, y)	((unsigned int)(((x)+((y)-1)))/(unsigned int)(y))
#endif	/* howmany */

#ifndef _FD_SET_DEFINED
#define _FD_SET_DEFINED
typedef	struct fd_set
    {
    fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
    } fd_set;
#endif

#ifndef _FD_FUNC_DEFINED
#define _FD_FUNC_DEFINED
#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif /* _FD_FUNC_DEFINED */

/* system III  typedefs (used by netinet) */

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;


/* historical definitions - now obsolete */

typedef char		TBOOL;		/* obsolete */

/* new memory management global defines */

#ifndef _TYPE_VIRT_ADDR
#define _TYPE_VIRT_ADDR typedef UINT32 VIRT_ADDR
#endif
_TYPE_VIRT_ADDR;

#ifdef _WRS_KERNEL
#ifndef _TYPE_PHYS_ADDR
#define _TYPE_PHYS_ADDR typedef UINT32 PHYS_ADDR
#endif
_TYPE_PHYS_ADDR;
#endif	/* _WRS_KERNEL */

/* macro's for extracting 32-bit halves of a 64-bit number */

#define LOW32(x)  (long) (((long long)(x)) & (unsigned long) 0xffffffff)
#define HIGH32(x) (long) ((((long long)(x)) >> 32) & (unsigned long)0xffffffff)

/* architecture dependent typedefs */

#ifdef	CPU_FAMILY

#if	CPU_FAMILY==MC680X0
typedef unsigned short INSTR;		/* word-aligned instructions */
#endif	/* CPU_FAMILY==MC680X0 || CPU_FAMILY==SH */

#if	CPU_FAMILY==SPARC
typedef unsigned long INSTR;		/* 32 bit word-aligned instructions */
#endif	/* CPU_FAMILY==SPARC */

#if	CPU_FAMILY==I960
typedef	unsigned long INSTR;		/* 32 bit word-aligned instructions */
#endif	/* CPU_FAMILY==I960 */

#if	CPU_FAMILY==AM29XXX
typedef	unsigned long INSTR;		/* 32 bit word-aligned instructions */
#endif	/* CPU_FAMILY==AM29XXX */

#if	CPU_FAMILY==COLDFIRE 
typedef unsigned short INSTR;		/* word-aligned instructions */
#endif	/* CPU_FAMILY==COLDFIRE */

#endif  /* CPU_FAMILY */

/* ANSI type qualifiers */

#if !defined(__STDC__) && !defined(__cplusplus)

#ifdef  __GNUC__
#define	volatile	__volatile__
#define	const		__const__
#define	signed		__signed__
#else
#if     !(defined(CPU_FAMILY) && CPU_FAMILY==MIPS)
#define	volatile
#define	const
#define	signed
#endif	/* !(defined(CPU_FAMILY) && CPU_FAMILY==MIPS) */
#endif	/* __GNUC__ */

#endif	/* !defined(__STDC__) && !defined(__cplusplus) */

#ifdef __cplusplus
}
#endif

#endif /* __INCvxTypesOldh */
