/************************************************************************
 *                                                                      *
 *                                                                      *
 *  Project Code:       VRP3.0                                          *
 *  Create Date:        2000/03/25                                      *
 *  Author:                                                             *
 *  Modify Date:                                                        *
 *  Document:                                                           *
 *  Function:                                                           *
 *  Others:                                                             *
 *----------------------------------------------------------------------*
 *                                                                      *
 *  Copyright 2000-2002 VRP3.0 Team Beijing Institute HuaWei Tech, Inc. *
 *                      ALL RIGHTS RESERVED                             *
 *                                                                      *
 *----------------------------------------------------------------------*
 *                                                                      *
 *                                                                      *
 ************************************************************************/
#ifndef _VOS_ERR_H_
#define _VOS_ERR_H_

#ifndef _VRPCFG_H_
#error "Please include vrpcfg.h before this file."
#endif

#ifdef	__cplusplus
extern "C"{
#endif

#include "v_typdef.h"
#include "v_modNum.h"
#include "v_queue.h"

#if 0 /* 20041126 VRPV5 SSP UPGRADE DELETE  不再需要 */
#define VOS_ERR_NO_PHYMEM	10
#endif
#define VOS_ERR_NULLPTR		20
#if 0 /* 20041126 VRPV5 SSP UPGRADE DELETE  不再需要 */
#define VOS_ERR_PTR			21
#define VOS_ERR_BADPTR		30


/*-----------------------系统层错误码--------------------------*/
#define VOS_SYS_ERR_NOTSUSP           2 /* The task is not suspended */
#define VOS_SYS_ERR_NO_EVENTS         3 /* No events posted(NO_WAITcase) */
#define VOS_SYS_ERR_NO_MSG            4 /* No messages in the Q(NO_WAITcase) */
#define VOS_SYS_ERR_NO_RSRC           5 /* OS resources already used up completely. */
#define VOS_SYS_ERR_NO_SEM            6 /* No semaphore(NO_WAITcase) */
#define VOS_SYS_ERR_NO_SUCH_Q         7 /* Such a Q does not exist */
#define VOS_SYS_ERR_NO_SUCH_SEM       8 /* No such semaphore exists */
#define VOS_SYS_ERR_NO_SUCH_TASK      9 /* Such a task does not exist */
#define VOS_SYS_ERR_PRIORITY         10 /* Invalid priority value */
#define VOS_SYS_ERR_QNF              11 /* No such named Q exists. */
#define VOS_SYS_ERR_Q_DELETED        12 /* Q deleted while waiting for messages. */
#define VOS_SYS_ERR_Q_FULL           13 /* Q already contains maxnumber of messages */
#define VOS_SYS_ERR_SEM_DELETED      14 /* Semaphore has been deleted while waiting. */
#define VOS_SYS_ERR_STK_SIZE         15 /* Stack size error(toosmall/toolarge) */
#define VOS_SYS_ERR_SUSP             16 /* Task already suspended */
#define VOS_SYS_ERR_TASKNF           17 /* No such named task exists */
#define VOS_SYS_ERR_TASK_ACTIVATION  18 /* Task Activation error */
#define VOS_SYS_ERR_TASK_MODE        19 /* Unsupported/impossible TaskMode */
#define VOS_SYS_ERR_TIMEOUT          20 /* Timeout occurred during event waiting. */
#define VOS_SYS_ERR_UNKNOWN          21 /* Unspecified error condition */
#define VOS_SYS_ERR_OS_DOES_NOT_SUPP 22
#define VOS_SYS_ERR_NO_SUCH_OBJ      23
#endif

#define VOS_SYS_ERR_PARA_ILLEGAL     24
#if 0 /* 20041126 VRPV5 SSP UPGRADE DELETE  不再需要 */
#define	VOS_SYS_ERR_TIME			 25
/*-----------------------系统层错误码--------------------------*/


/*-----------------------逻辑层错误码--------------------------*/

/*----------定时器逻辑层错误码----------*/
/* [VRPV5] */
/*----------定时器逻辑层错误码----------*/

/*----------队列逻辑层错误码----------*/
/*used in queue create function*/
#define VOS_QUE_ERR_PAR_ILLEGAL                     0x00000001
#endif
#define VOS_QUE_ERR_NO_SPACE                        0x00000002
#if 0 /* 20041126 VRPV5 SSP UPGRADE DELETE  不再需要 */
#define VOS_QUE_ERR_GEN_NAME_FAIL                   0x00000004
#define VOS_QUE_ERR_REG_NAME_FAIL                   0x00000008
#define VOS_QUE_ERR_MALLOC_FAIL                     0x00000010
#define VOS_QUE_ERR_CREATE_READ_SEM_FAIL            0x00000040
#define VOS_QUE_ERR_CREATE_WRITE_SEM_FAIL           0x00000100

/*used in queue delete function*/
#define VOS_QUE_ERR_ID_ILLEGAL                      0x00000200
#define VOS_QUE_ERR_QUEUE_DISABLE                   0x00000400
#define VOS_QUE_ERR_DEL_READ_SEM_FAIL               0x00000800
#define VOS_QUE_ERR_DEL_WRITE_SEM_FAIL              0x00001000
#endif
#define VOS_QUE_ERR_NO_MSG_TO_READ                    VOS_ERRNO_QUEUE_READ_NOMSG /*0x00002000*/
#if 0 /* 20041126 VRPV5 SSP UPGRADE DELETE  不再需要 */
#define VOS_QUE_ERR_READ_SEM_P_ERR                  0x00004000
#define VOS_QUE_ERR_READ_WRITE_CONFLICT             0x00008000
#if ( VRP_DEBUG_IS_ON(VRP_DEBUG_VOS_LOGIC) == VRP_YES )
#define VOS_QUE_ERR_DROPING                         0x00010000
#endif
#define VOS_QUE_ERR_QUEUE_FULL                      0x00020000
#define VOS_QUE_ERR_WRITE_P_ERR                     0x00040000
#define VOS_QUE_ERR_NOT_FOUND                       0x00080000
/*----------队列逻辑层错误码----------*/

/*----------信号量逻辑层错误码----------*/
#define VOS_SEM_ERR_PARA                            0x00000001
#define VOS_SEM_ERR_NO_SPACE                        0x00000002
#define VOS_SEM_ERR_GEN_NAME                        0x00000004
#define VOS_SEM_ERR_REG_NAME                        0x00000008
#define VOS_SEM_ERR_SYS_CREATE                      0x00000010

#define VOS_SEM_ERR_SEM_ID_ILLEGAL                  0x00000020
#define VOS_SEM_ERR_SEM_DISABLE                     0x00000040
#define VOS_SEM_ERR_DEL_SEM                         0x00000080
#define VOS_SEM_ERR_P                               0x00000100
#define VOS_SEM_ERR_V                               0x00000200
#define VOS_SEM_ERR_NOT_FOUND                       0x00000400
/*----------信号量逻辑层错误码----------*/

/*----------事件逻辑层错误码----------*/
#define VOS_EV_ERR_PAR_ERR                          0x00000001
#define VOS_EV_ERR_NO_WAIT_READ_FAIL                0x00000002
#define VOS_EV_ERR_WAIT_READ_FAIL                   0x00000004
#define VOS_EV_ERR_GET_EV_INFO_FAIL                 0x00000008    
#define VOS_EV_ERR_SET_EV_INFO_FAIL                 0x00000010    
#define VOS_ERR_TASK_NOT_EXIST                      0x00000020
/*----------事件逻辑层错误码----------*/

/*-----------------------逻辑层错误码--------------------------*/

ULONG  VOS_SYS_Error(ULONG ulError);
#endif

#ifdef	__cplusplus
}
#endif	/* end of __cplusplus */

#endif	/* end of _VOS_ERR_H_ */
