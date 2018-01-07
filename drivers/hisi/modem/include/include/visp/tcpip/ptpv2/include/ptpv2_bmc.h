/*******************************************************************************
*
*
*                Copyright 2008, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              ptpv2_bmc.h
*
*  Project Code:
*   Module Name: PTPV2 bmc
*  Date Created: 2014-03-03
*        Author: luoagwoei & dutianyi
*   Description: 
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE            NAME                DESCRIPTION
*  -----------------------------------------------------------------------------
*  2014-03-03  luoagwoei & dutianyi      Create
*******************************************************************************/

#ifndef _PTPV2_BMC_H_
#define _PTPV2_BMC_H_

#ifdef __cplusplus
extern "C" {
#endif

ULONG PTPV2_1588_BMC(PTPV2_FOREIGN_MASTER_S** pstSelectMaster);


ULONG PTPV2_AddForeignMaster(PTPV2_PORT_S* pstPtpPort, TCPIP_PTP_ADDRINFO_S* pstAddrInfo,
                             PTPV2_FOREIGN_MASTER_S** ppstForeMaster);

ULONG PTPV2_DelForeignMaster(PTPV2_FOREIGN_MASTER_S* pstForeMaster);


ULONG PTPV2_DelForeignMaster_ByAddrr(TCPIP_PTP_PORT_ADDR_S* pstAddrInfo);

ULONG PTPV2_Update_ForeignMasterPara(PTPV2_PORT_S* pstPtpPort, PTPV2_FOREIGN_MASTER_S* pstForeMaster, 
                            PTPV2_ANNOUNCE_MSG_S* pstAnnMsg, TCPIP_PTP_ADDRINFO_S* pstAddrInfo);

ULONG PTPV2_BMCPreProcess(PTPV2_PORT_S* pstPtpPort, PTPV2_ANNOUNCE_MSG_S* pstAnnMsg, 
                            TCPIP_PTP_ADDRINFO_S* pstAddrInfo);

ULONG PTPV2_BMCProcess(PTPV2_FOREIGN_MASTER_S** pstSelectMaster);

ULONG PTPV2_1588_BMC(PTPV2_FOREIGN_MASTER_S** pstSelectMaster);

ULONG PTPV2_BMCNotify(PTPV2_FOREIGN_MASTER_S* pstSelectMaster);

ULONG PTPV2_BMCProcess(PTPV2_FOREIGN_MASTER_S** pstSelectMaster);

ULONG PTPV2_BMCNotify(PTPV2_FOREIGN_MASTER_S* pstSelectMaster);

VOID PTPV2_ForeMasterPrint(PTPV2_FOREIGN_MASTER_S *pstForeMaster);

/*two-way模式，允许sync和delay序号相同时,不判断Class范围*/
#define IS_PTPV2_FOREMASTER_CLASS_VALID(pstForeMaster) \
  ((g_ulPtpClassCheckSwitch) ? BOOL_TRUE :  \
  ((80 <= pstForeMaster->stGrandmasterClockQuality.ucClockClass)  \
        && (pstForeMaster->stGrandmasterClockQuality.ucClockClass <= 110)))

/*two-way模式，允许sync和delay序号相同时,直接返回2,避免TP5000对接不成功*/
#define PTPV2_CLASS_TO_MAPPING_INDEX(ptpclass)   \
  ((g_ulPtpClassCheckSwitch) ? 2 : \
  ((ptpclass - 80) / 2))

/*是否为优选源*/
#define PTPV2_IS_PRI_CLOCK_SOURCE(clkclass)\
    ((g_pstPtpGlobalInfo->stCfgDefaultDs.ucClass0 == clkclass)\
    || (g_pstPtpGlobalInfo->stCfgDefaultDs.ucClass1 == clkclass)\
    || (g_pstPtpGlobalInfo->stCfgDefaultDs.ucClass2 == clkclass)\
    || (g_pstPtpGlobalInfo->stCfgDefaultDs.ucClass3 == clkclass))\

#ifdef __cplusplus
}
#endif   /* end of __cplusplus          */

#endif   /* end of _PTPV2_BMC_h_   */

