/* vxWorks.h - VxWorks standard definitions header */

/*
 * Copyright (c) 1984-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
modification history
--------------------
04c,30sep05,yvp  Reworked SPR 112908 fix.
04b,26sep05,yvp  Reworked lib configuration inclusion (SPR 112908).
04a,12aug05,yvp  Moved vxLibCfgDefsP.h to h/sys.
03z,25jul05,yvp  Moved LIB_BUILD conditional to vxLibCfgDefsP.h.
03y,21jul05,yvp  Added include of vxLibCfgDefsP.h and prjComps.h
                 Updated copyright. Includes now with angle brackets.
03x,28jun05,pad  Added definition of _WRS_KERNEL macro (SPR 110428).
		 Updated copyright notice.
03w,18mar04,job  Refactored file to use vxWorksCommon.h
03v,18mar04,mcm  Fix for SPR 95066 - moving isascii/toascii to ctype.h
03u,09feb04,dat  New release id macros
03t,02dec03,dat  removing NULL
03s,14may03,pes  PAL conditional compilation cleanup. Phase 2.
03r,25jun03,dcc  removed non-ANSI prototypes.
03q,14mar03,dcc  added include of types/vxWind.h
03p,27nov01,mrs  Fix to conform to ANSI standard for token pasting.
03o,31oct01,tdl  Check for preexsiting __P def before defining it
03n,18oct01,dat  Chg TOOLCHAIN to TOOL_FAMILY
03m,24sep01,gls  Added definition of __P() macro (SPR #28330)
03l,28sep01,rae  added #define _WRS_VXWORKS_5_X
03k,19sep01,dat  Rework of gnu-diab changes
03j,08sep01,dat  Gnu-Diab infrastructure
03i,20aug01,dat  Gnu-Diab, added #include toolMacros.h
03h,17feb99,mrs  Add C++ support for NULL, (SPR #25079).
03g,25nov97,dat  added BUS_TYPE macros
03f,26nov96,vin  modified BSD to 44.
03e,23aug94,ism  added the MSW() and LSW() macros (SPR#3541)
03d,22sep92,rrr  added support for c++
03c,07sep92,smb  added #undef BSD to remove previous definition
03b,11jul92,smb  moved CPU definitions to types/vxCpu.h
03a,04jul92,jcf  cleaned up.
02q,28jun92,smb  reworked for ANSI lib.
02p,22jun92,rrr  added MIPS CPU_FAMILY
02o,16jun92,jwt  added (CPU_FAMILY = SPARC) for (CPU == SPARClite).
02n,26may92,rrr  the tree shuffle
02m,17mar92,yao  added _STACK_GROWS_DOWN/UP.  added macros ROUND_UP/DOWN,
		 ALIGNED, MEM_ROUND_UP/DOWN, STACK_ROUND_UP/DOWN, MEM_ALIGNED.
		 changed copyright notice.  included arch.h.  moved ffsMSB
		 definitions for 960 to changed BYTE_ORDER to _BYTE_ORDER,
		 BIG/LITTLE_ENDIAN to _BIG/LITTLE_ENDIAN.  arch960.h.  removed
		 conditional include of r3k/arch.h.
02l,09jan91,wmd  conditionalized out ffsMSB inline definition to shut up
 		 ansi warnings.
02k,11nov91,jpb  added definition for R3K CPU_FAMILY.
02j,28oct91,wmd  for lack of better place -  added Intel's 960 ffsMSB
		 inline definition here.
02i,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and ...
		  -changed copyright notice
02h,25sep91,yao  added CPU32.
02g,10sep91,wmd  added MIPS to conditional for BIG_ENDIAN byte order.
02f,20aug91,ajm  added MIPS_R3k support.
02e,14aug91,del  (intel) added I960KA/KB defines.
02d,29apr91,hdn  added defines and macros for TRON architecture
02c,31mar91,del  added #define of BYTE_ORDER for network etc.
02b,25oct90,dnw  changed to allow NULL to be redefined.
02a,25oct90,shl  combined CPU test for I960KB and I960CA into one.
...deleted pre v2a history - see RCS
*/

#define STATUS VOS_UINT16
#define UCHAR  VOS_UINT8
#define UINT16 VOS_UINT16
#define taskSpawn
#define FUNCPTR VOS_UINT32 *




#ifndef __INCvxWorksh
#define __INCvxWorksh

#ifdef __cplusplus
extern "C" {
#endif



/* BUS types */

#define BUS_TYPE_NONE       NONE
#define BUS_TYPE_VME        1
#define BUS_TYPE_MULTIBUS   2
#define BUS_TYPE_PCI        3

#define VME_BUS             BUS_TYPE_VME        /* for backward compat. */
#define MULTI_BUS           BUS_TYPE_MULTIBUS

#if !defined _WRS_CHANGE_INSN_SET_ENABLE
/* these macros can be used to switch target calls into an
 * alternate instruction set and back, e.g., ARM/Thumb, or MIPS16.
 * They are null functions by default.
*/
#define _WRS_CHANGE_INSN_SET(p) {}
#define _WRS_CHANGE_INSN_SET_PTR(p) (p)
#define _WRS_REV_INSN_SET_CHANGE(p) (p)
#endif /* _WRS_CHANGE_INSN_SET */

/* The _WRS_KERNEL macro must be defined for kernel-side builds only */

#ifndef _WRS_KERNEL
#define _WRS_KERNEL
#endif /* !_WRS_KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* __INCvxWorksh */
