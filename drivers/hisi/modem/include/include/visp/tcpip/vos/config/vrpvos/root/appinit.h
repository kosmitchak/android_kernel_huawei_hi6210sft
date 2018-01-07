/************************************************************************
 *                                                                      *
 *                          APP_INIT.H                                  *
 *                                                                      *
 *  Project Code:       VRP5                                            *
 *  Create Date:        2003/07/10                                      *
 *  Author:             Terry                                           *
 *  Modify Date:                                                        *
 *  Document:                                                           *
 *  Function:           Application Register/Initializtion Supporting   *
 *  Others:                                                             *
 *----------------------------------------------------------------------*
 *                                                                      *
 *  Copyright 2002-2004, Huawei Tech Ltd Co, ALL RIGHTS RESERVED        *
 *                                                                      *
 *----------------------------------------------------------------------*
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#include "public/entry/tcpip_entry.h" /*Change into VISP file.20050512*/

#if 0 /*closed by VISP*/
#ifndef _APPINIT_H_
#define _APPINIT_H_
#ifdef  __cplusplus
extern "C"{
#endif

#define APP_MAX_REG_NUM         256 /* Also include old version apps!! */

#define APP_NAME_MAX_LEN        16
#define APP_TASK_NAME_LEN       4
#define APP_TASK_PARA_NUM       4

typedef VOID  (*APP_TASK_ENTRY) (ULONG ulArg0, ULONG ulArg1, ULONG ulArg2, ULONG ulArg3);
typedef ULONG (*APP_INIT_FUNC)();
typedef VOID  (*APP_MIB_REG)();
typedef VOID  (*APP_IFNET_HOT_NOTIFY_CB)(ULONG ulEvent, ULONG ulSlot, VOID * pstIfnet, ULONG ulIfIndex);
typedef ULONG (*APP_CFA_CB)(VOID *, VOID **);


typedef struct tagAPP_INIT_INFO
{
    ULONG                   ulMID;                          /*MID*/
    UCHAR                   szAppName[APP_NAME_MAX_LEN + 1];    /*Application name, '\0' terminated */
    VOID                   *pstComponentInfo;

    APP_TASK_ENTRY          pfTaskEntry;                    /*Task entry, NULL for no own task*/
    UCHAR                   szTaskName[APP_TASK_NAME_LEN + 1];  /*Task name, zero terminated*/
    ULONG                   ulTaskPriority;                 /*Task priority*/   
    ULONG                   ulTaskMode;                     /*task mode*/
    ULONG                   ulTaskStackSize;                /*task stack size */
    ULONG                   ulTaskArg[APP_TASK_PARA_NUM];   /*task entry parameters*/

    APP_INIT_FUNC           pfAppInit;
    APP_MIB_REG             pfMibReg;                       /*register MIB*/
    APP_IFNET_HOT_NOTIFY_CB pfIfnetHotNotifyCB;
    APP_CFA_CB              pfCfaCB;                        /*config process func on IO board*/

    CHAR                  **ppcExecInfor_EN;                /**/
    CHAR                  **ppcExecInfor_CN;                /**/
    CHAR                  **ppcExecInfor_EXTD;              /**/

    ULONG                   ulAppIndex;                     /*to store app id ??USHORT/ULONG??*/ 
    ULONG                   ulTaskId;                       /*to store the task ID*/
    ULONG                   ulExecInforIndex;               /*to store the execinfo index*/

}APP_INIT_INFO_S;

typedef enum tagSYS_INIT_ERROR
{
    SYS_INIT_OK = 0,
    SYS_INIT_ERR,
    SYS_INIT_NULL_PTR,
    SYS_INIT_NO_MEM,
    SYS_INIT_ERR_DUPLICATED,
    SYS_INIT_ERR_TOO_MANY_APPS,
    SYS_INIT_REG_COMP_FAIL,

    SYS_INIT_ERR_MAX
}SYS_INIT_ERROR_EN;

ULONG SYS_RegAppInfo(APP_INIT_INFO_S *);
extern ULONG g_ulRegAppNum;
extern APP_INIT_INFO_S * g_apstRegiseterApp[];
extern ULONG g_ulRegExecInfoNum;


#ifdef  __cplusplus
}
#endif
#endif
#endif
