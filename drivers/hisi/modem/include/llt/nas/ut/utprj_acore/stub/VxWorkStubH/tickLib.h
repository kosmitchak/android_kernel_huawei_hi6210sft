/* tickLib.h - header file for tickLib.c */

/*
 * Copyright (c) 1984-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
02e,05aug05,jln  added tickAnnounceHookAdd()
02d,31jan05,kk   add tick64[GS]et() APIs (SPR# 105938)
02c,22may02,jgn  removed vxTicks and vxAbsTicks declarations - SPR #70255
02b,22sep92,rrr  added support for c++
02a,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCtickLibh
#define __INCtickLibh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

extern void 	tickAnnounce (void);
extern void 	tickSet (ULONG ticks);
extern ULONG 	tickGet (void);
extern void 	tick64Set (UINT64 ticks);
extern UINT64	tick64Get (void);
extern STATUS   tickAnnounceHookAdd (FUNCPTR pFunc);

#ifdef __cplusplus
}
#endif

#endif /* __INCtickLibh */
