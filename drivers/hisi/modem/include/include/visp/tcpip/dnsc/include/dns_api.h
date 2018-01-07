/*******************************************************************************
*
*
*                Copyright 2008, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              dns_api.h
*
*  Project Code: VISPV1R7
*   Module Name: dns
*  Date Created: 2008-03-09
*        Author: wuhailan
*   Description: dns用户API接口文件,该头文件涉及到IPV6的地址数据结构VRP_IN6ADDR_S,需要先包含IPV6头文件
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  -----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
#ifndef _DNS_API_H_
#define _DNS_API_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* 域名列表中域名的最大长度*/
#ifndef DNS_DOMAIN_LIST_NAME_LENGTH
#define DNS_DOMAIN_LIST_NAME_LENGTH 20
#endif

/*
根据rfc882文档的描述，如下:
To simplify implementations, the total number of octets that
represent label octets and label lengths is limited to 255.  Thus
a printed domain name can be up to 254 characters.

定义VISP支持DNS域名最大长度为254
*/
#define DNS_DOMAIN_NAME_LENGTH 254

/* 域名列表中域名的最大个数*/
#define DNS_DOMAIN_LIST_NUM         10

/* 域名服务器的最大个数*/
#define DNS_DOMAIN_SERVER_NUM        6

#define DNS_MAX_INTERFACE_NAME_LEN  47  /*接口名长度,与MAX_INTERFACE_NAME_LEN值一样,若有改动需要同步更新*/

/*设置DNS域名服务器中使用的宏值*/
#define DNS_ADD_SERVER_IP      0  /*表示增加指定域名服务器地址*/
#define DNS_DEL_SERVER_IP      1  /*表示删除指定域名服务器地址*/
#define DNS_DEL_ALL_SERVER_IP  2  /*表示删除全部域名服务器地址.*/

#ifndef _VRP_IN6ADDR_S
#define _VRP_IN6ADDR_S
typedef struct tagVRPIN6Addr
{
    union
    {
        UCHAR   u6_ucaddr[16];
        USHORT  u6_usaddr[8];
        ULONG   u6_uladdr[4];
    } vrp_u6_addr;    /* 128-bit IP6 address */
}VRP_IN6ADDR_S;
#endif

typedef struct tagTCPIP_IPV6_DNS_SERVADDRINFO
{   
    VRP_IN6ADDR_S stIpAddr;
    ULONG         ulIfIndex;
    CHAR          szInterfaceName[DNS_MAX_INTERFACE_NAME_LEN + 1];   
}TCPIP_IPV6_DNS_SERVADDRINFO_S;

/*dns域名信息结构,与DomainInfo_S结构一致，如有修改，要求同步修改*/
typedef struct  tagTCPIP_DomainInfo
{
     ULONG  ulDomainServer[DNS_DOMAIN_SERVER_NUM];
     TCPIP_IPV6_DNS_SERVADDRINFO_S stDomainServer6[DNS_DOMAIN_SERVER_NUM];
     CHAR   szDomainList[DNS_DOMAIN_LIST_NUM][DNS_DOMAIN_LIST_NAME_LENGTH + 1];
     UCHAR  ucPadding[2]; 
}TCPIP_DomainInfo_S;


/*dns模块错误码定义*/
/*注意:如果修改错误码，请同步修改dns_sh_info.c 文件中的CHAR *Inf_Dnsc_En[]描述信息*/
enum enum_DnsErr_INFO
{
    /* success */
    DNSC_SUCCESS       = 0,                  /* 0 */

    /* error code */
    DNS_INVALID_V4_ADDR,                     /* 1 */
    DNS_INVALID_V6_ADDR,                     /* 2 */
    DNS_UNSPECIFIED_ADDR,                    /* 3 */
    DNS_MULTICAST_ADDR,                      /* 4 */
    DNS_NO_DOMAIN_SERVER,                    /* 5 */
    DNS_ILLEGAL_V6_SERVER_ADDR,              /* 6 */
    DNS_ILLEGAL_INTF_NAME,                   /* 7 */
    DNS_TOO_MANY_SERVERS,                    /* 8 */
    DNS_EXISTING_SERVER,                     /* 9 */
    DNS_SPECIFY_INTF_NAME,                   /* 10 */
    DNS_INVALID_INTF_NAME,                   /* 11 */
    DNS_INVALID_DOMAIN_NAME,                 /* 12，域名无效，可能过长或长度为0或含有非法字符 */
    DNS_NO_DOMAIN_NAME,                      /* 13 */
    DNS_TOO_MANY_DOMAINS,                    /* 14 */
    DNS_EXISTING_DOMAIN,                     /* 15 */

    /* info code */
    DNS_V4_SERVERS,                          /* 16 */
    DNS_DOMAIN_SERVER,                       /* 17 */
    DNS_IP_ADDR,                             /* 18 */
    DNS_NO_SERVERS,                          /* 19 */
    DNS_V6_SERVERS,                          /* 20 */
    DNS_IPv6_ADDR,                           /* 21 */
    DNS_INTF_NAME,                           /* 22 */
    DNS_NO,                                  /* 23 */
    DNS_DOMAIN_NAME,                         /* 24 */
    DNS_TTL,                                 /* 25 */
    DNS_ALIAS,                               /* 26 */
    DNS_STOP_SEARCH,                         /* 27 */
    DNS_TRY_SERVER,                          /* 28 */
    DNS_NO_FIRST,                            /* 29 */

    /* error code */
    DNS_NULL_POINTER,                        /* 30, 空指针参数. */
    DNS_ERR_WAITLIST,                        /* 31, 对waitlist register/unregister操作失败*/
    DNS_ERR_SETWAITLIST,                     /* 32, 对waitlist SET操作失败*/
    DNS_ERR_GETWAITLIST,                     /* 33, 对waitlist GET操作失败*/
    DNS_LEN_DOMAIN_NAME,                     /* 34，域名无效,域名过长*/
    DNS_LEN_INTF_NAME,                       /* 35，IFNET名字过长*/
    DNS_ADD_IPV6_INTF_NAME,                  /* 36，增加 ipv6 Domain Setver地址不合法*/
    DNS_DEL_IPV6_INTF_NAME,                  /* 37，删除 ipv6 Domain Setver地址不合法*/
    DNS_V6_TOO_MANY_SERVERS,                 /* 38，IPV6 服务器过多*/
    DNS_V4_TOO_MANY_SERVERS,                 /* 39，IPV4 服务器过多*/
    DNS_TOO_MANY_SERVERS_FORLINT,            /* 40，IPV4 服务器过多 for pclint*/
    DNS_DEL_V6_NO_DOMAIN_SERVER,             /* 41,  删除V6 DOMAIN SEVER不存在*/
    DNS_V6_EXISTING_SERVER,                  /* 42,  添加V6服务器存在.add by z62474 for A82D20453*/

    DNS_API_COM_NULL,                        /* 43, DNS组件为空*/
    DNS_API_PARA_WRONG,                      /* 44, 存入参数错误*/
    DNS_API_POINTER_NULL,                    /* 45, 指针参数为空*/
    
    DNS_LEN_DOMAIN_LABEL,                    /* 46, 域名Label过长 */
    DNS_API_IPV6ADDR_WRONG = 47,             /* 47, 存入IPv6地址错误*/
    DNS_API_IPV4ADDR_WRONG,                  /* 48, 存入IPv4地址错误*/
};
/*注意:如果修改错误码，请同步修改dns_sh_info.c 文件中的CHAR *Inf_Dnsc_En[]描述信息*/

/* 根据DNS server IP地址获取需要绑定的源IP地址, server IP地址和源IP地址均为主机序 */
typedef ULONG (*DNSCGetSrcIPHOOK)(ULONG ulDnsServerIP);

/*******************************************************************************
*    Func Name: TCPIP_ClrDnsDynamicHost
*  Description: 清除DNS缓存信息
*        Input: ULONG ulProtocolFlag:AF_INET 或 AF_INET6
*       Output: 无
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_ClrDnsDynamicHost(ULONG ulProtocolFlag);
/*******************************************************************************
*    Func Name: TCPIP_GetDnsDomainList
*  Description: 获取DNS域名列表信息
*        Input: TCPIP_DomainInfo_S *pstTcpipDomainInfo:DNS域名列表信息指针
*       Output: 无
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_GetDnsDomainList(TCPIP_DomainInfo_S *pstDomainInfo);
/*******************************************************************************
*    Func Name: TCPIP_GetDnsHost
*  Description: 获取DNS域名解析后的IP地址信息
*        Input: CHAR *pcDNSName:DNS域名
*               ULONG ulIPV6Flag:IPV6标记,0表示IPV4,非0表示IPV6
*       Output: VOID *pulIPAddr:解析后的IP地址,IPV4时传入ULONG *类型,IPV6时为CHAR ulIPAddr[46]类型
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_GetDnsHost(CHAR *pcDNSName, VOID *pulIPAddr, ULONG ulIPV6Flag);
/*******************************************************************************
*    Func Name: TCPIP_GetDnsServer
*  Description: 获取DNS域名服务器列表
*        Input: TCPIP_DomainInfo_S * pstTcpipDomainInfo:DNS域名服务器信息指针
*       Output: 无
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_GetDnsServer(TCPIP_DomainInfo_S * pstDomainInfo);
/*******************************************************************************
*    Func Name: TCPIP_SetDnsDomainSuffix
*  Description: 设置DNS域名前缀信息
*        Input: ULONG ulNoFlag:0为设置域名后缀，非0为删除域名后缀
*               CHAR  *pstDomainName:预添加或删除的域名后缀
*               ULONG ulDomainNameFlag:如果ulNoFlag非0且ulDomainNameFlag为0，则删除所有域名后缀
*       Output: 无
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_SetDnsDomainSuffix(ULONG ulNoFlag, 
                                      CHAR  *pstDomainName, 
                                      ULONG ulDomainNameFlag);
/*******************************************************************************
*    Func Name: TCPIP_SetDnsServer
*  Description: 设置DNS域名服务器
*        Input: ULONG ulOpMode:
*               设置模式:0表示增加指定域名服务器地址;
*                        1表示删除指定域名服务器地址;
*                        2表示删除全部域名服务器地址.
*               CHAR *pstServerIP:服务器地址
*               ULONG ulIPV6Flag:IPV6标记,0表示IPV4,非0表示IPV6
*       Output: 无
*       Return: 成功返回 DNSC_SUCCESS,其他返回失败
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern ULONG TCPIP_SetDnsServer(ULONG ulOpMode, CHAR *pstServerIP, ULONG ulIPV6Flag);
/*******************************************************************************
*    Func Name: TCPIP_ShowDnsInfo
*  Description: 显示DNS模块域名及缓存信息
*        Input: VOID
*       Output: 无
*       Return: VOID
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-03-09   wuhailan                Create
*
*******************************************************************************/
extern VOID TCPIP_ShowDnsInfo(VOID);
/*******************************************************************************
*    Func Name: TCPIP_SetDNSDebugSwitch
*  Description: 设置调试开关
*        Input: ulswitch: 0 打开  1 关闭
*       Output: NA
*       Return: VOS_OK 成功  其他 失败
*      Caution: NA
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME              DESCRIPTION    
*  ----------------------------------------------------------------------------
*  2008-04-10  wuhailan           Create the first version.
*
*******************************************************************************/
extern ULONG TCPIP_SetDNSDebugSwitch(ULONG ulswitch);
/*******************************************************************************
*    Func Name: TCPIP_GetDNSDebugSwitch
*  Description: 设置调试开关
*        Input: NA
*       Output: pulswitch: 调试开关
*       Return: VOS_OK 成功  其他 失败
*      Caution: NA
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME              DESCRIPTION    
*  ----------------------------------------------------------------------------
*  2008-04-10  wuhailan           Create the first version.
*
*******************************************************************************/
extern ULONG TCPIP_GetDNSDebugSwitch(ULONG *pulswitch);

/*******************************************************************************
*    Func Name: TCPIP_SetDnsTos
*  Description: 设置dns模块的tos值
*        Input: UCHAR ucTos: 要设置的tos值，值域有效
*       Output: 
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME             DESCRIPTION    
*  ----------------------------------------------------------------------------
*  2008-8-19    LY(57500)     Creat the first version.
*
*******************************************************************************/
extern ULONG TCPIP_SetDnsTos(UCHAR ucTos);
/*******************************************************************************
*    Func Name: TCPIP_GetDnsTos
*  Description: 获取dns模块的tos值
*        Input: UCHAR *pucTos: 保存输出tos值
*       Output: 
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME             DESCRIPTION    
*  ----------------------------------------------------------------------------
*  2008-8-19    LY(57500)     Creat the first version.
*
*******************************************************************************/
extern ULONG TCPIP_GetDnsTos(UCHAR *pucTos);

/*******************************************************************************
*    Func Name: TCPIP_RegFuncDNSCGetSrcIPHook
* Date Created: 2008-10-04
*       Author: z43740
*  Description: 注册根据DNS server的IP地址获取DNS域名解析请求报文的源地址的回调函数
*        Input: DNSCGetSrcIPHOOK pfHook: 回调函数
*       Output: 
*       Return: VOS_OK                  注册成功
*               DNS_NULL_POINTER        回调函数指针为空
*      Caution: 如果不需要手动指定DNS域名解析请求报文的源地址，返回0
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2008-10-04   z43740                  Create
*
*******************************************************************************/
extern ULONG TCPIP_RegFuncDNSCGetSrcIPHook(DNSCGetSrcIPHOOK pfHook);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* _DNS_API_H_ */

