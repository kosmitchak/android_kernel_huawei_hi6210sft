/* taskLib.h - generic kernel interface header */

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
06w,09sep05,jln  added _WRS_ABSOLUTE_EXTERN(_vx_qNodeToPschedInfo)
06v,30aug05,yvp  Reworked VX_TASK macros to undo struct workaround.
06u,31aug05,jln  added TASK_QNODE_TO_PSCHEDINFO macro
06t,25aug05,kk   fix VX_TASK_INSTANTIATE macro
06s,11aug05,mmi  Change pad from int --> UINT32
06r,09aug05,mmi  Add pCpuPwrMgmtPState
06q,15aug05,yvp  Added include of sys/taskDefs.h.
06p,10aug05,yvp  Added macros VX_TASK and VX_TASK_INITIALIZE.
06o,04aug05,yvp  Added prototype for create/delete library init.
06n,02aug05,yvp  Converted while(0) to while ((0)) - suppress diab warnings.
06m,01apr05,kk   added kernelBaseLibP.h for IS_KERNEL_TASK, added some docs.
06l,04mar05,kk   added cpuTimeInfo1, cpuTimeInfo2, pSchedInfo to TCB
		 fixed apigen errors (SPR# 106451)
06k,25feb05,pcs  Modified prototype of fn. taskLibInit to accept distint stack
                 overflow and underflow sizes for both user and kernel tasks.
06j,01feb05,kk   added IS_KERNEL_TASK() (SPR# 104271)
06i,26oct04,fr   added taskOpenInit() routine (SPR 101491)
06h,04oct04,aeg  removed #ifndef _ASMLANGUAGE around #include msgQLibP.h
06g,03oct04,kk   surround include of msgQLibP.h by #ifndef _ASMLANGUAGE
06f,24sep04,dcc  added accessObjId and taskVarSemId to WIND_TCB.
06e,01oct04,pcm  increased the number of task create/delete hooks from 16 to 20
06d,22sep04,pcm  changed WIND_TCB field name msgPassing to msgQInfo
06c,22sep04,md   moved taskSafe()/taskUnsafe() to taskLibCommon.h
06b,02sep04,aeg  removed NULL pointer check from TASK_ID_VERIFY();
		 changed taskInit2() to taskInitExcStk().
06b,20jul04,tcr  fix TASK_LOCK/TASK_SAFE macros for WV_INSTRUMENTATION
06a,11jun04,aeg  changed definition of VX_DEALLOC_TCB (SPR #98106).
05z,15may04,hya  added VX_DEALLOC_TCB.
05y,05may04,cjj  removed I960 preprocessor directives
05x,30mar04,jb   Renaming I80X86 pad entry to reflect new use
05w,29mar04,dcc  brought back VX_TASK_OBJ_OWNER option bit.
05v,17feb04,dcc  adjusted offsets in WIND_TCB and added currentCtx member.
05u,22jan04,aeg  moved coprocessor related task option bit to taskLibCommon.h
04w,09dec03,dlk  Restored reserved2 field for I80X86 cpu family only.
04v,02apr03,dtr  Reseving TCB slot for compiler.
04u,03sep02,dtr  Adding VX_SPE_TASK option.
05t,07nov03,tcr  allow WV instrumentation to OBJ_VERIFY
05s,26aug03,tcr  update WindView instrumentation
05r,31oct03,dcc  added some members to the WIND_TCB: pEdrInfo, pCompilerData,  
		 pDbgInfo, pShellInfo and pUTcb. Reserved 8bytes for WRAMP use.
		 Removed windxLock member. Adjusted offsets as a result of 
		 OBJ_CORE expansion.
05q,30oct03,pcm  implemented changes from code review
05p,20oct03,pcm  added new TCB field <pExcStackStart>; replaced various
		 priority inheritance TCB fields with <pPriInheritData>
05p,09sep03,kk   change taskLibInit prototype, added excCnt to TCB, 
		 made VX_USER_MODE = 0, added VX_DEALLOC_EXC_STACK option bit,
		 added taskInit2(), renamed pKStackXXX to pExcStackXXX.
05o,05sep03,dcc  moved common definitions with user side to taskLibCommon.h
05n,19aug03,dbt  Added PAL coprocessor support for VxSim.
05m,24jul03,kam  updated offset comments for all simulators
05l,10apr03,pes  PAL Coprocessor Abstraction
05k,21jul03,kk   changed tcbPad[0,1] to pKStackBase and pKStackPtr
05j,08jul03,kk   added VX_USER_MODE option bit for user mode task
05i,15jul03,kam  rebased to Base6 integration branch
05h,23jun03,ymz  added 8-byte alignment padding for MIPS.
05g,20jun03,to   deleted dummyNode, move rtpId, resurrect reserved2.
05f,13may03,to   added rtpId to TCB, added task option VX_TASK_OBJ_OWNER,
		 deleted K&R style prototypes.
05e,14apr03,to   replaced activeNode with dummyNode in TCB for now
05d,14apr03,to   made task name a copy in TASK_DESC instead of a pointer.
05c,20mar03,ymz  moved OBJ_CORE as the first element of WIND_TCB 
05b,16jan02,to   added fpStatus to TCB for ARM.
05a,17apr03,dbt  Added SIMLINUX support.
04z,27mar03,jmp  removed unused fields from SIMNT TCB, removed SUNOS and HPPA
		 simulators refernces.
04y,14mar03,jmp  removed windowsThreadId and windowsThread from SIMNT TCB.
04x,19nov01,aeg  added td_events to TASK_DESC.
04w,09nov01,jhw  Revert WDB_INFO to reside inside WIND_TCB.
04v,06nov01,gls  cleaned up compilation warnings
04u,06nov01,dee  fixed offsets in coldfire specific portion
		 for EXC_INFO and REG_SET per sh-coldfire.tcl
04t,30oct01,jhw  Removed wdbInfo and wdbExitHook from WIND_TCB.
	    gsl  Reused wdbExitHook for pPthread.
04s,22oct01,dee  Mergeed from T2.1.0 ColdFire
04r,04oct01,yvp  Fixed SPR69306: added member pCoprocCtx to PPC section for
		 Altivec support.
04q,27sep01,aeg  changed comment on WIND_TCB reservedX fields.
04p,24sep01,s_l  Fixed a systax error on line "UINT32 regSetPad[2]" missing ';'
04o,18sep01,aeg  deleted selectSem, pad1, & pSelWakeupNode from WIND_TCB,
		 and added pSelContext, events, pWdbTaskInfo, & wdbExitHook;
		 added code barriers to TASK_LOCK and TASK_SAFE macros.
04n,01mar00,frf  Added SH4 support for T2.
04m,10sep98,kab  expanded WDB_INFO struct to match TOR2.x., moved pDspContext
		 to end of TCB, replaced spare4.
04l,22jul98,mem  added pDspContext to TCB, removed spare4. Added VX_DSP_TASK.
04k,23apr97,hk   added SH support.
04j,14mar01,pcs  Added VX_ALTIVEC_TASK.
04i,12mar99,dbs  added COM task-local storage to TCB
04h,29may98,cym  added windowsContextValid flag for SIMNT.
04g,24apr98,kkk  moved wdbInfo after ARM stuff in TCB. (bug created by merge)
04f,19aug97,cym  added SIMNT support.
04e,28nov96,cdp  added ARM support.
04d,06nov97,dbt  modified WIND_TCB for new debugger scheme. Removed Am29K
		 family support.
04c,22jul96,jmb  merged ease patch for HP Sim
04b,26may94,yao  added PPC support.
04a,26may95,ms   added wdbExitHook field to the TCB
03z,16may95,rrr  added WDB_INFO structure to TCB.
03y,12jul95,ism  added simsolaris support
03x,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
03w,15mar94,smb  modified TASK_SAFE macro
03v,24jan94,smb  added instrumentation macros
03u,10dec93,smb  instrumented TASK_LOCK macro
03t,02dec93,pme  added Am29K family support
03s,12nov93,hdn  added support for I80X86
03r,15oct93,cd   removed junk values from MIPS TCB.
03q,16sep93,jmm  added S_taskLib_ILLEGAL_PRIORITY and taskPriRangeCheck
03p,11aug93,gae  vxsim hppa.
03o,20jun93,rrr  vxsim.
03m,11feb93,jcf  added __PROTOTYPE_5_0 for compatibility.
03l,08feb93,smb  added a null check to taskIdVerify.
03k,13nov92,dnw  changed declaration of pSmObjTcb to struct sm_obj_tcb *
		 removed include of smObjTcbP.h (SPR #1768)
		 moved typedef of DBG_INFO here from dbgLib.h
		 removed include of dbgLib.h (SPR #1768)
03j,22sep92,rrr  added support for c++
03i,21sep92,smb  removed exit prototype and added include of stdlib.h
03h,02aug92,jcf  changed reserved3 field to pExcRegSet for exception handling.
03g,29jul92,smb  made modification for the stdio library.
03f,28jul92,jcf  added windxLock/reserved[12]; moved dbgInfo/pSmTcbObj;
		 removed funcRestart.
03e,12jul92,yao  replace pDbgState pointer with data structure DBG_INFO
		 in WIND_TCB.  added dbgPCWSave to i960.
03d,19jul92,pme  added shared memory objects support.
03c,10jul92,jwt  moved padding out of REG_SET in into WIND_TCB for SPARC.
03b,06jul92,ajm  removed taskSummary from forward declarations
03a,04jul92,jcf  cleaned up.
02y,16jun92,yao  made pDbgState available for all architectures.
02x,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
02w,19mar92,yao  added ANSI prototype for taskStackAllot().
02v,12mar92,yao  removed ifdef CPU.  added taskRegsShow().
02u,10jan92,jwt  added CPU_FAMILY==SPARC architecture dependent prototypes.
02t,11nov91,rrr  Added funcRestart to tcb for signals.
02s,28oct91,wmd  Added changes for i960KB from Intel.
02r,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed ASMLANGUAGE to _ASMLANGUAGE
		  -changed copyright notice
02q,20sep91,hdn  deleted foroff from WIND_TCB for TRON.
		 added pDbgState to WIND_TCB for TRON.
02p,20aug91,ajm  made architecture independant.
02o,10jun91,del  added pragma for gnu960 alignment.
02n,23may91,wmd  added defines and macros for SPARC architecture.
02m,29apr91,hdn  added defines and macros for TRON architecture.
02l,08apr91,jdi  added NOMANUAL to prevent mangen.
02k,24mar91,del  added pDbgState for use with new dbgLib. And I960 defines.
02j,16oct90,shl  made #else ANSI style.
02i,05oct90,dnw  deleted private functions.
		 made taskSpawn, taskInit, taskCreate take var args.
02h,05oct90,shl  added ANSI function prototypes.
		 made #endif ANSI style.
		 added copyright notice.
02g,01oct90,jcf  added addtional spares to WIND_TCB.
02f,03aug90,jcf  moved arch dependent portion of WIND_TCB to end of struct
02e,13jul90,rdc  added support for environment variables and additional
		 select functionality.
02d,30jun90,jcf  added assembly language defines.
02c,26jun90,jcf  removed obsolete generic status codes
		 changed inheritance protocol.
		 changed safetyQSem to a safetyQHead.
		 reworked priority mananagement.
		 changed topOfStack to endOfStack.
02b,17may90,rdc  changed select semaphores in tcb to be binary semaphores.
02a,17apr90,jcf  added error codes.
		 changed to wind 2.0.
01l,16mar90,rdc  added select semaphore to tcbx.
01k,25may89,gae  added VX_FORTRAN option.
01j,21apr89,jcf  added KERNEL_{UNINIT,VRTX,PSOS,WIND}.
01i,07nov88,rdc  added VX_ADA_DEBUG to task options.
01h,22jun88,dnw  name tweaks.
01g,30may88,dnw  changed to v4 names.
01f,28may88,dnw  deleted obsolete status values.
		 added EXC_INFO to tcbx.
01e,18may88,jcf  added psos semaphore head to tcbx.
		  extended maximum number of hooks to 10.
01d,13may88,rdc  added signal info to tcbx.
01c,28apr88,ecs  added IMPORTs of idle & taskName.
01b,13apr88,gae  added function declarations; option bit VX_STDIO;
		 taskStd[] to TCB extension.
01a,25jan88,jcf  written by extracting from vxLib.h v02l.
*/

#ifndef __INCtaskLibh
#define __INCtaskLibh

#ifdef __cplusplus
extern "C" {
#endif

#include <vxWorks.h>
/* miscellaneous */

#define MAX_TASK_ARGS		10	/* max args passed to a task */
#define VX_MAX_TASK_SWITCH_RTNS	16	/* max task switch callout routines */
#define VX_MAX_TASK_SWAP_RTNS	16	/* max task swap callout routines */
#define VX_MAX_TASK_DELETE_RTNS	20	/* max task delete callout routines */
#define VX_MAX_TASK_CREATE_RTNS	20	/* max task create callout routines */

/* kernel only task option bits (see taskLibCommon.h for common option bits) */

#define VX_UNBREAKABLE		0x0002	/* breakpoints ignored */

#define VX_STDIO		0x0010	/* OBSOLETE: need not be set for stdio*/
#define VX_ADA_DEBUG		0x0020	/* 1 = VADS debugger support */
#define VX_FORTRAN		0x0040	/* 1 = NKR FORTRAN support */

/* VX_PRIVATE_ENV		0x0080 	   defined in taskLibCommon.h */
/* VX_NO_STACK_FILL 		0x0100     defined in taskLibCommon.h */
/* VX_TASK_NOACTIVATE  		0x2000	   defined in taskLibCommon.h */
/* VX_NO_STACK_PROTECT		0x4000 	   defined in taskLibCommon.h */

/* the following are now defined in sys/taskDefs.h */

/* #define VX_USER_MODE		0x0000	   INTERNAL: tasks in user mode */
/* #define VX_SUPERVISOR_MODE	0x0001	   INTERNAL: tasks in sup mode */
/* #define VX_DEALLOC_STACK	0x0004	   INTERNAL: deallocate stack */
/* #define VX_TASK_OBJ_OWNER	0x0800	   INTERNAL: task owns objs directly */
/* #define VX_DEALLOC_EXC_STACK	0x1000	   INTERNAL: deallocate the exc stack */
/* #define VX_DEALLOC_TCB	0x8000	   INTERNAL: deallocate the task TCB */

/* typedefs */

#ifdef __cplusplus
}
#endif

#endif /* __INCtaskLibh */
