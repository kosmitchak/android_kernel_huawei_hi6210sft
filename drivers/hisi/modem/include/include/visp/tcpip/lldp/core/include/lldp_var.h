
#ifndef _LLDP_VAR_H_
#define _LLDP_VAR_H_

#ifdef  __cplusplus
extern "C"{
#endif

extern LLDP_LOCAL_SYS_INFO_S    g_stLldpLocalSystemData;    /*本地设备系统信息*/
extern LLDP_LOCAL_SYS_STAT_S g_stLldpLocalSysStat;     /*系统统计信息*/
extern LLDP_LOCAL_PORT_S *g_stLldpLocalPortArray[LLDP_PORT_MAX_NUM];       /*本地端口表项链表LLDP_LOCAL_PORT_S*/
extern ULONG g_ulLldpLocalPortNum;      /*本地端口数量*/
extern BOOL_T g_bLLDP_ShellInit;   /*LLDP是否初始化成功*/

extern ULONG  g_ulLldpRemmTotal;             /*系统邻居总数*/


/* Global semaphore for LLDP */
extern ULONG g_ulSemForLldp ;

/*LLDPRUN任务的任务ID和消息队列ID*/
extern ULONG ulLLDPTaskID;


extern ULONG txFastInit;
extern ULONG reinitDelay;
extern ULONG msgTxInterval;

extern ULONG g_ulLLDPControlQueID;
extern ULONG g_ulLLDPTimerQueID;
extern ULONG g_ulLLDPRcvQueID;
extern ULONG g_ulLLDPTransmitTimerQueID;

extern ULONG g_ulLLDPDebugSwitch;
extern ULONG g_ulLLDPDebugIfIndexFilter;
extern ULONG g_ulLLDPDebugMACTypeFilter;
extern ULONG g_ulLLDPEtherType;
extern ULONG g_ulLLDPRemIndex;

extern ULONG g_ulLLDPSendPktFilter;
extern ULONG g_ulLLDPReceivePktFilter;

extern ULONG g_ulLLDPLinkAggrTlvSwitch;    /*值为0, 选择发送802.1中的规定; 值大于0，选择发送802.3中的规定。*/

extern UCHAR g_lldp_szEthDMA[LLDP_MAX_MACTYPE_SIZE][LLDP_MAC_ADDR_LEN] ;

extern LLDP_REM_INFO_S  *g_pstLLDPWarningRemInfo;

extern ULONG g_ulLLDPAddPortMode;
/*end of added by w00217009*/

/* Added by l00213099, for getting extended tlv, 2013/11/06 */
extern LLDP_GETEXTTLV_HOOK_FUNC g_stLLDPGetExtTlv;

/* Added by l00213099, for input LLDP packet to product, 2013/11/06 */
extern LLDP_INPUT_HOOK_FUNC g_stLLDPInputHook;

#ifdef  __cplusplus
}
#endif

#endif
