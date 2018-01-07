/*******************************************************************************
*
*
*                Copyright 2008, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              diagnose.h
*
*  Project Code: VISPV100R007C02
*   Module Name: i3agent
*  Date Created: 2008-07-30
*        Author: L00105073
*   Description: 诊断信息配置API
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  -----------------------------------------------------------------------------
*  2008-07-29   L00105073               Create
*
*******************************************************************************/

#ifndef DIAGNOSE_H
#define DIAGNOSE_H

#ifdef  __cplusplus
    extern "C"{
#endif

#if 0
#ifndef ULONG
#define ULONG unsigned long
#endif

#ifndef CHAR
#define CHAR char
#endif

#ifndef VOID
#define VOID void
#endif


#define MAX_STOP_ON_ASSERTS 10  /* 停止诊断输出的断言次数的最大值 */

#define MIN_MEMORY_VERIFY_FREQUENCY 1   /* 内存验证频率的最小值 */
#define MAX_MEMORY_VERIFY_FREQUENCY 10  /* 内存验证频率的最大值 */

/* output infomation type */
#define NBB_TEXT    1
#define NBB_BINARY  2


/* diagnose 错误码类型 */
enum eDIAGERRCODE
{
    DIAG_OK,                     /*0功能正常*/
    DIAG_ERR,                    /*1功能错误,用于内部函数返回值*/
    DIAG_ERR_NULL_PTR,          /* 空指针 */
    DIAG_ERR_OUTOFBOUND_1,           /* 参数1超出取值范围 */
    DIAG_ERR_OUTOFBOUND_2,           /* 参数2超出取值范围 */
    DIAG_ERR_OUTOFBOUND_3,           /* 参数3超出取值范围 */
    DIAG_ERR_OUTOFBOUND_4,           /* 参数4超出取值范围 */
    DIAG_ERR_WRONGFORMAT,            /* 错误的参数格式 */
    DIAG_ERR_MUST_SET_BEFORE_DCL_INIT,   /* 该参数应该在DCL启动前设置 */
    DIAG_ERR_MUST_SET_AFTER_DCL_INIT,   /* 该参数应该在DCL启动前设置 */
    DIAG_ERR_NEEDENABLEIPSTRACE,     /* 使能EventTrace或TimerTrace必须同时使能IpsTrace */
    DIAG_ERR_MALLOCBUFFERFAIL,      /* 分配内存失败 */
    DIAG_ERR_SUPPORT_DEBUG_ONLY,    /* 只支持debug版本 */
    DIAG_ERR_MAXCODE             /*最大错误码,添加新错误码时,直接添加到后面*/
};


typedef struct tagTCPIP_DiagnosticsInfo
{
    ULONG ulTraceLevel;
    ULONG ulTraceLevelVerify;
    CHAR* pszTraceFilterInFile;
    CHAR* pszTraceFilterOutFile;
    CHAR* pszTraceFilterInFunc;
    CHAR* pszTraceFilterOutFunc;
    ULONG ulIpsInfo;
    ULONG ulEventInfo;
    ULONG ulTimerInfo;
    CHAR* pszIpsFilterInProc;
    CHAR* pszIpsFilterOutProc;
    CHAR* pszIpsFilterInType;
    CHAR* pszIpsFilterOutType;
    ULONG ulMemUsage;
    ULONG ulMemTune;
    ULONG ulVerifyMem;
    ULONG ulVerifyMemPerNSchedules;
    ULONG ulCheckMemory;
    ULONG ulAssertBehave;
    ULONG ulDumpOnAssert;
    ULONG ulPdLogLevel;
    ULONG ulPdLogDetail;
    ULONG ulPdLogDev;
    ULONG ulPdLogVerbose;
    CHAR* pszLogFilterInProc;
    CHAR* pszLogFilterOutProc;
    CHAR* pszLogFilterInMsgid;
    CHAR* pszLogFilterOutMsgid;
    CHAR* pszStopLogs;
    ULONG ulStopOnAssert;
}TCPIP_DIAGNOSTICS_INFO;



/* diagnose 断言处理方式 */
enum eDIAGASSERTACTION
{
    DIAG_ASSERT_ASSERT = 1,     /* 表示当出现断言时将此断言交给C断言函数处理：在Win32系统中，这会导致C运行库显示一个
                                对话框并调用JIT调试器（可选）；在UNIX系统中会导致内核转储 */              
    DIAG_ASSERT_EXIT,           /* 表示当出现断言时会将断言传送给标准输出并正常退出程序 */   
    DIAG_ASSERT_TRAP,           /* 在Win32系统中会调用JIT调试器但是不显示C运行库对话框 */
    DIAG_ASSERT_CONTINUE,       /* 表示当出现断言时会将断言传送给标准输出并使程序继续执行 */  
};

/* diagnose 日志级别 */
enum eDIAGLOGLEVEL
{
    DIAG_LOG_LEVEL_NONE = 0,  
    DIAG_LOG_LEVEL_PROBLEM,     
    DIAG_LOG_LEVEL_EXCEPTION,   
    DIAG_LOG_LEVEL_AUDIT,
};

/* diagnose 详细或摘要日志 */
enum eDIAGLOGDETAIL
{
    DIAG_LOG_DETAIL_SUMMARY = 1,    /* 摘要日志 */
    DIAG_LOG_DETAIL_FULL,           /* 详细日志 */
};

/* diagnose 详细或摘要日志 */
enum eDIAGENABLE
{
    DIAG_DISABLE = 0,       /* 去使能 */
    DIAG_ENABLE,            /* 使能 */
};

/* diagnose Trace 级别 */
enum eDIAGTRACE
{
    DIAG_TRACE_ENTRY_POINT      = 0,    /* 函数入口 */       
    DIAG_TRACE_ENTRY_EXIT       = 1,    /* 函数退出点 */
    DIAG_TRACE_DETAIL           = 2,    /* 细节 */
    DIAG_TRACE_FLOW             = 4,    /* 流程 */
    DIAG_TRACE_INTERFACE        = 6,    /* 接口 */
    DIAG_TRACE_STATISTICS       = 8,    /* 统计 */
    DIAG_TRACE_NO_TRC           = 10,   /* 屏蔽TRACE */
    DIAG_TRACE_DEFAULT_LEVEL    = 10,   /* 默认TRACE级别 */
};

/* diagnose flush type */
enum eDIAGFLUSHTYPE
{
    DIAG_FLUSH_ALL,     /* 转储所有信息 */
    DIAG_FLUSH_TRACE,   /* 转储TRACE信息 */
    DIAG_FLUSH_IPS,     /* 转储IPS信息 */
    DIAG_FLUSH_PD,      /* 转储日志信息 */
};

enum eDIAGOUTPUTTYPE
{
    DIAG_OUTPUT_TRACE,  /* trace information */
    DIAG_OUTPUT_IPS,    /* IPS information */
    DIAG_OUTPUT_PD,     /* log information */
    DIAG_OUTPUT_DMP,    /* dump information */
    DIAG_OUTPUT_PER,    /* performance information */
    DIAG_OUTPUT_MEM,    /* memory information */
    DIAG_OUTPUT_IPS_MON,/* ips monitor */
    DIAG_OUTPUT_PD_MON, /* pd monitor */
    DIAG_OUTPUT_MT_PER, /* performance information in mutilthread*/
};

//typedef VOID (*TCPIP_PROT_DIAGNOSTICS_HOOK_FUNC)(ULONG ulType,CHAR *pszOutputInfo);

typedef VOID (*TCPIP_PROT_DIAGNOSTICS_HOOK_FUNC)
    (ULONG ulType, ULONG ulTextOrBinary, ULONG ulLength, CHAR *pszOutputInfo);

extern TCPIP_PROT_DIAGNOSTICS_HOOK_FUNC g_diagnostics_output;

extern ULONG TCPIP_SetProtAssert(ULONG ulAssertDlg, ULONG ulDumpOnAssert);
extern ULONG TCPIP_SetProtBufSize(ULONG ulTrace, ULONG ulIps, ULONG ulPd);
extern ULONG Diag_CheckStopLogs(CHAR* pszStopLogs);
extern ULONG TCPIP_SetProtDiagnosticsStop(CHAR* pszStopLogs, ULONG ulStopOnAsserts);
extern ULONG TCPIP_SetProtIpsFilterProcess(CHAR* pszFilterInProc, CHAR* pszFilterOutProc);
extern ULONG TCPIP_SetProtIpsFilterType(CHAR* pszFilterInType, CHAR* pszFilterOutType);
extern ULONG TCPIP_SetProtIpsTrace(ULONG ulIpsTrace, ULONG ulEventTrace, ULONG ulTimerTrace);
extern ULONG TCPIP_SetProtLog(ULONG ulLogLevel, ULONG ulDetail, ULONG ulDev, ULONG ulVerbose);
extern ULONG TCPIP_SetProtLogFilterMsgId(CHAR* pszFilterInMsgId, CHAR*pszFilterOutMsgId);
extern ULONG TCPIP_SetProtLogFilterProcess(CHAR* pszFilterInProc, CHAR* pszFilterOutProc);
extern ULONG TCPIP_SetProtMemoryCheck(ULONG ulCheck);
extern ULONG TCPIP_SetProtMemoryVerify(ULONG ulVerify, ULONG ulFrequency);
extern ULONG TCPIP_SetProtTraceFilterFile(CHAR* pszFilterInFile, CHAR* pszFilterOutFile);
extern ULONG TCPIP_SetProtTraceFilterFunc(CHAR* pszFilterInFunc, CHAR* pszFilterOutFunc);
extern ULONG TCPIP_SetProtTraceLevel(ULONG ulTraceLevel);
extern ULONG TCPIP_SetProtVerifyTraceLevel(ULONG ulTraceLevel);
extern ULONG TCPIP_FlushMemoryBuffer(ULONG ulType);
extern ULONG TCPIP_RegFuncDiagnosticsOutputHook(TCPIP_PROT_DIAGNOSTICS_HOOK_FUNC pfHookFunc);
extern ULONG TCPIP_GetDiagnosticsPara(TCPIP_DIAGNOSTICS_INFO* pstDiagnosticsInfo);
extern VOID TCPIP_ShowDiagnosticsPara(VOID);
extern ULONG TCPIP_GetI3stubStatus (ULONG ulIndex, ULONG* pulOperStatus);
extern VOID TCPIP_ShowProtMomeryUsage(VOID);
extern VOID TCPIP_ShowProtMemeryTuning(ULONG ulCommonLen);

#endif
#ifdef  __cplusplus
}
#endif

#endif
