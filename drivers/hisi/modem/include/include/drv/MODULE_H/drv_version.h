
#ifndef __DRV_VERSION_H__
#define __DRV_VERSION_H__
#include "drv_global.h"
/* 硬件产品信息适配 */
#define HW_VER_INVALID              (BSP_U16)0xFFFF

/* Porting板和P500板兼容 */
#define HW_VER_PRODUCT_PORTING      (BSP_U16)0xFFFE
#define HW_VER_PRODUCT_PV500        (BSP_U16)0xFFFD

/* 测试板 */
#define HW_VER_PRODUCT_UDP          (BSP_U16)0xFFFC

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)HW_VER_INVALID
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0001

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0000

/*Epad, S10 101u */
#define HW_VER_PRODUCT_S10_101U     (BSP_U16)0x0401

/*Epad, S10 102u */
#define HW_VER_PRODUCT_S10_102U      (BSP_U16)0x0406

/*Epad, S10 LTE */
#define HW_VER_PRODUCT_S10_101L     (BSP_U16)0x0501

/*Epad, S10 Modem */
#define HW_VER_PRODUCT_S10_MODEM     (BSP_U16)0x0000

/*Epad, S10 Temp */
#define HW_VER_PRODUCT_S10_TEMP     (BSP_U16)0x0005

#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

typedef struct {
    BSP_U32   bandGU;         	/*支持的GU频段*/
    BSP_U32   bamdLTELow;         /*支持LTE频段，B1~B32*/
    BSP_U32   bamdLTEHigh;        /*支持LTE频段，B33~B64*/
}BAND_INFO_T;

typedef struct {
    BSP_U16  index;         /*硬件版本号数值(大版本号1+大版本号2)，区分不同产品*/
    BSP_U16   hwIdSub;         /*硬件子版本号，区分产品的不同的版本*/
    BSP_CHAR* name;           /*内部产品名*/
    BSP_CHAR* namePlus;       /*内部产品名PLUS*/
    BSP_CHAR* hwVer;          /*硬件版本名称*/
    BSP_CHAR* dloadId;        /*升级中使用的名称*/
    BSP_CHAR* productId;      /*外部产品名*/
    BAND_INFO_T  band;            /*产品支持的频段*/
    BAND_INFO_T  bandDiv;         /*产品支持的分集*/
    BSP_CHAR reserve[4];      /*预留*/
}PRODUCT_INFO_T;

typedef struct  tagUE_SW_BUILD_VER_INFO_STRU
{
    BSP_U16 ulVVerNo;        /* V部分 */
    BSP_U16 ulRVerNo;        /*  R部分*/
    BSP_U16 ulCVerNo;        /* C部分*/
    BSP_U16 ulBVerNo;         /* B部分*/
    BSP_U16 ulSpcNo;          /* SPC部分*/
    BSP_U16 ulCustomVer;     /* 客户需定制部分, 针对硬件接口低8bit PCB号,高8bitHW号*/
    BSP_U32 ulProductNo;     /* such as porting,CPE, ... */
    BSP_S8 acBuildDate[BUILD_DATE_LEN];  /* build日期,*/
    BSP_S8 acBuildTime[BUILD_TIME_LEN];  /* build时间*/
} UE_SW_BUILD_VER_INFO_STRU; /* 内部版本*/

/* 当前版本是否支持某个模块 */
typedef enum tagBSP_MODULE_SUPPORT_E
{
    BSP_MODULE_SUPPORT     = 0,
    BSP_MODULE_UNSUPPORT   = 1,
    BSP_MODULE_SUPPORT_BUTTOM
}BSP_MODULE_SUPPORT_E;

/* 查询的模块类型 */
typedef enum tagBSP_MODULE_TYPE_E
{
    BSP_MODULE_TYPE_SD = 0x0,
    BSP_MODULE_TYPE_CHARGE,
    BSP_MODULE_TYPE_WIFI,
    BSP_MODULE_TYPE_OLED,
    BSP_MODULE_TYPE_HIFI,
    BSP_MODULE_TYPE_POWER_ON_OFF,
    BSP_MODULE_TYPE_HSIC,
    BSP_MODULE_TYPE_HSIC_NCM,
    BSP_MODULE_TYPE_LOCALFLASH,

    BSP_MODULE_TYPE_BUTTOM
}BSP_MODULE_TYPE_E;

typedef enum{
     BOARD_TYPE_LTE_ONLY    = 0,
     BOARD_TYPE_GUL
}BOARD_TYPE_E;

typedef enum{
     BOARD_TYPE_BBIT        = 0,
     BOARD_TYPE_SFT,
     BOARD_TYPE_ASIC,
     BOARD_TYPE_MAX
}BOARD_ACTUAL_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP              = 1,
     PV500_PILOT_CHIP,
     V7R1_PILOT_CHIP,
     PV500_V9R1ES_CHIP,

     V8R1_PILOT_CHIP     = 0x8,
     V8R2_PILOT_CHIP

}BSP_CHIP_TYPE_E;

/*组件类型*/
typedef enum
{
    VER_BOOTLOAD = 0,
    VER_BOOTROM =1,
    VER_NV =2 ,
    VER_VXWORKS =3,
    VER_DSP =4 ,
    VER_PRODUCT_ID =5 ,
    VER_WBBP =6 ,
    VER_PS =7,
    VER_OAM =8,
    VER_GBBP =9 ,
    VER_SOC =10,
    VER_HARDWARE =11,
    VER_SOFTWARE =12,
    VER_MEDIA =13,
    VER_APP =14,
    VER_ASIC =15,
    VER_RF =16,
    VER_PMU =17,
    VER_PDM = 18,
    VER_PRODUCT_INNER_ID = 19,
    VER_INFO_NUM =20
}COMP_TYPE_I;

/*****************************************************************************
* 函 数 名  : BSP_HwGetVerMain
* 功能描述  : 获取产品版本号
* 输入参数  :
* 输出参数  :
* 返 回 值  :
* 修改记录  :
*****************************************************************************/
BSP_U16 BSP_HwGetVerMain(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GetProductName
* 功能描述  : 获取产品名称
* 输入参数  : char* pProductName,字符串指针，保证不小于32字节
*             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
* 返 回 值  : 0：正确，非0: 失败
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  :
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_HwGetHwVersion
* 功能描述  : 获取硬件版本名称
* 输入参数  : BSP_CHAR* pHwVersion,字符串指针，保证不小于32字节
* 输出参数  : 无
* 返 回 值  : 无
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength);


/*****************************************************************************
* 函 数 名  : BSP_GetSupportBands
*
* 功能描述  : 获取支持的频段
*
* 输入参数  : BAND_INFO_T *pBandInfo,频段信息结构体指针
* 输出参数  :
*
 返 回 值  : 0：正确，非0: 失败
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 BSP_GetSupportBands( BAND_INFO_T *pBandInfo);

/*****************************************************************************
* 函 数 名  : BSP_HwIsSupportWifi
* 功能描述  : 打印硬件版本信息,打桩
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
BSP_BOOL BSP_HwIsSupportWifi(BSP_VOID);


/********************************************************************************************************
 函 数 名  : getHwVersionIndex
 功能描述  : 获取由HKADC读取的硬件ID经过拼装后的硬件索引值
 输入参数  : 无
 输出参数  : 硬件ID的索引值

 返 回 值  : 非-1:  操作成功；
             -1：操作失败。
 注    意  : 该接口仅在c核提供
********************************************************************************************************/
extern int getHwVersionIndex(void);

/*****************************************************************************
 函 数 名  : getHwProductID
 功能描述  :
 输入参数  : 无
 输出参数  : modem NV的索引值
             手机板：返回product ID
			 非手机板：返回硬件board ID
******************************************************************************/
extern int getHwProductID(void);
#define DRV_GET_HW_VERSION_INDEX()       getHwProductID()

/*****************************************************************************
 函 数 名  : VerifySIMLock
 功能描述  : 判断当前解锁码是否正确 .
 输入参数  : imei       - 单板IMEI号
             unlockcode - 解锁码
 输出参数  : 无。
 返 回 值  : 1：   解锁成功
             0:    解锁失败
*****************************************************************************/
extern int VerifySL(char* UnlockCode, char* Key);
#define DRV_CARDLOCK_MD5_VERIFY(unlockcode, imei)  VerifySL(unlockcode, imei)


extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*****************************************************************************
 函 数 名  : BSP_CheckModuleSupport
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType);

#define DRV_GET_BATTERY_SUPPORT()   BSP_CheckModuleSupport(BSP_MODULE_TYPE_CHARGE)

#define DRV_GET_WIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_WIFI)

#define DRV_GET_SD_SUPPORT()        BSP_CheckModuleSupport(BSP_MODULE_TYPE_SD)

#define DRV_GET_OLED_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_OLED)

#define DRV_GET_HIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HIFI)

#define DRV_GET_HSIC_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HSIC)


/*****************************************************************************
 函 数 名  : BSP_InitPlatformVerInfo
 功能描述  : 初始化芯片的版本号
 输入参数  : 无
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID BSP_InitPlatformVerInfo(BSP_VOID);
#define DRV_INIT_PLATFORM_VER() BSP_InitPlatformVerInfo()


extern BSP_S32  DRV_START_MODEGET(void);

/*****************************************************************************
 函 数 名  : BSP_GetPlatformInfo
 功能描述  : 获取芯片的版本号
 输入参数  : 无
 输出参数  : u32PlatformInfo:芯片的版本号
 返 回 值  : void
 注    意  : 该接口仅在c核提供
*****************************************************************************/
extern BSP_VOID BSP_GetPlatformInfo(unsigned int *u32PlatformInfo);
#define DRV_GET_PLATFORM_INFO(u32PlatformInfo) BSP_GetPlatformInfo(u32PlatformInfo)


/*add by shenwei*/
/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
static INLINE_ALWAYS BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID)
{
	return BOARD_TYPE_GUL;
}
#else
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID);
#endif
#define DRV_GET_BOARDTYPE()   BSP_OM_GetBoardType()

/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardActualType
*
* 功能描述  : 获取单板物理类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*             BBIT/SFT/ASIC
*
* 其它说明  : 该接口仅在C核提供，A核通过DTS来区分平台
*
*****************************************************************************/
#if defined (BSP_CORE_APP)
BOARD_ACTUAL_TYPE_E BSP_OM_GetBoardActualType(BSP_VOID);
#else
extern BOARD_ACTUAL_TYPE_E g_eBoardActualType;
static INLINE_ALWAYS BOARD_ACTUAL_TYPE_E BSP_OM_GetBoardActualType(BSP_VOID)
{
	return g_eBoardActualType;
}
#endif
#define DRV_GET_BOARDACTUALTYPE()   BSP_OM_GetBoardActualType()

/*****************************************************************************
* 函 数 名  : BSP_OM_GetChipType
*
* 功能描述  : 获取芯片类型
*
* 输入参数  : BSP_VOID
*
* 输出参数  : 无
*
* 返 回 值  : 芯片类型
*             PV500_CHIP:PV500芯片
*             V7R1_CHIP: V7R1芯片
*             V8R1_PILOT_CHIP: V8R1芯片
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);
#define DRV_GET_CHIPTYPE()    BSP_OM_GetChipType()

/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern BSP_U32 BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()

/*****************************************************************************
 函 数 名  : BSP_MNTN_MemVersionCtrl
 功能描述  : 组件版本读写接口。
 输入参数  : pcData：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存地址；
                     当Mode为写的时候，为调用者准备写入的组件版本信息的地址。
             ucLength：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存大小；
                       当Mode为写的时候，为调用者准备写入的组件版本信息的字符数（不包括'\0'）。
             ucType：版本信息ID，
                     0： BOOTLOAD；
                     1： BOOTROM；
                     2： NV；
                     3： VXWORKS；
                     4： DSP；
                     5； CDROMISO；
                     6： PHY；
                     7： PS；
                     8： COM；
                     9： RF；
                     10：SOC；
                     11：HARDWARE；
                     12：SOFTWARE；
                     13：MEDIA；
                     14：APP；
                     15：INFO_NUM。
             ucMode：0：读取指定ID的组件版本信息；1：写入指定ID的组件版本信息。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_MemVersionCtrl(signed char *pcData, unsigned char ucLength, unsigned char ucType, unsigned char ucMode);
#define DRV_MEM_VERCTRL(pcData,ucLength,ucType,ucMode)  \
			BSP_MNTN_MemVersionCtrl(pcData,ucLength,ucType,ucMode)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFullHwVersion
 功能描述  : 硬件完整版本读接口。
 输入参数  : pFullHwVersion：为调用者待保存返回的硬件完整版本信息的内存首地址；
             ulLength      ：为调用者待保存返回的硬件完整版本信息的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetFullHwVersion(char * pFullHwVersion,unsigned int ulLength);
#define  DRV_GET_FULL_HW_VER(pFullHwVersion,ulLength)  \
			BSP_MNTN_GetFullHwVersion(pFullHwVersion,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetProductIdInter
 功能描述  : 产品名称完整版本读接口。
 输入参数  : pProductIdInter：为调用者待保存返回的产品名称完整版本的内存首地址；
             ulLength       ：为调用者待保存返回的产品名称完整版本的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetProductIdInter(char * pProductIdInter, unsigned int ulLength);
#define  DRV_GET_PRODUCTID_INTER_VER(pProductIdInter,ulLength)  \
			BSP_MNTN_GetProductIdInter(pProductIdInter,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_VersionQueryApi
 功能描述  : 查询所有组件的版本号。
 输入参数  : ppVersionInfo：待保存的版本信息地址。
 输出参数  : ucLength：待返回的数据的字节数。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_VersionQueryApi(void ** ppVersionInfo, unsigned int * ulLength);
#define  DRV_VER_QUERY(ppVersionInfo,ulLength)  \
			BSP_MNTN_VersionQueryApi (ppVersionInfo,ulLength)

#endif

