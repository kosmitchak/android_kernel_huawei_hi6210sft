/*!
 *****************************************************************************
 *
 * @file       msvdx_int.c
 *
 * Low-level MSVDX interface component
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 *
 * The contents of this file are subject to the MIT license as set out below.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above.
 *
 * If you wish to allow use of your version of this file only under the terms
 * of GPL, and not to allow others to use your version of this file under the
 * terms of the MIT license, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by GPL
 * as set out in the file called “GPLHEADER” included in this distribution. If
 * you do not delete the provisions above, a recipient may use your version of
 * this file under the terms of either the MIT license or GPL.
 *
 * This License is also included in this distribution in the file called
 * "MIT_COPYING".
 *
 *****************************************************************************/

#include "img_defs.h"
#include "msvdx_int.h"
#include "msvdx_ctx.h"
#include "msvdx_msg.h"
#include "report_api.h"
#include "secure_defs.h"
#include "dbgopt_api_km.h"
#include "sysos_api_km.h"
#include "vdecdd_mmu.h"
#include "msvdx_ext_reg.h"

#ifdef SECURE_MEDIA_SUPPORT
    #include "secureapi.h"
    #include "vxd_secure_msg.h"
#else
    #include "tal.h"
    #include "vxd_io.h"
#endif

#include "reg_io2.h"
#ifdef VDEC_USE_PVDEC
#include "hwdefs/msvdx_cmds.h"
#else
#include "msvdx_cmds_io2.h"
#endif
#include "msvdx_core_regs_io2.h"
#include "msvdx_vec_reg_io2.h"
#include "msvdx_vdmc_reg_io2.h"
#include "msvdx_mtx_reg_io2.h"
#include "img_soc_dmac_regs_io2.h"
#include "msvdx_test_reg_io2.h"
#include "msvdx_pin_test_regs.h"
#include "msvdx_vdeb_reg_io2.h"

#ifdef __FAKE_MTX_INTERFACE__
    #include "vdecfw_fakemtx.h"
#endif

#ifdef USE_REAL_FW
    #include "vdecfw_bin.h"
#endif
#ifdef POST_TEST
    /* CORE test definitions */
    #define GROUP_ID_DEFAULT_VALUE            ((IMG_UINT32)3)
    #define CORE_ID_DEFAULT_VALUE             ((IMG_UINT32)3)
    #define INVALID_32BIT_REG_VALUE           ((IMG_UINT32)0xFFFFFFFF)

    /* Registers test definitions */
    /* Test fields values */
    #define TEST_REG_FIELD_1BIT_VALUE         ((IMG_UINT32)1)
    #define TEST_REG_FIELD_2BITS_VALUE        ((IMG_UINT32)3)
    #define TEST_REG_FIELD_3BITS_VALUE        ((IMG_UINT32)7)
    #define TEST_REG_FIELD_8BITS_VALUE        ((IMG_UINT32)0xFF)
    #define TEST_REG_FIELD_26BITS_VALUE       ((IMG_UINT32)0x03FFFFFF)
    /* Default registers values */
    #define CR_MTX_REG_RW_REQ_DEF_VALUE       ((IMG_UINT32)0x80000000)
    #define CR_VEC_RENDEC_CONTROL1_DEF_VALUE  ((IMG_UINT32)0x00050001)
    #define CR_MMU_BANK_INDEX_DEF_VALUE       ((IMG_UINT32)0x00000000)
    #define CR_VDMC_2D_FLT_DATA_DEF_VALUE     ((IMG_UINT32)0x00000000)
    #define CR_VDEB_VIRT_CTRL_DEF_VALUE       ((IMG_UINT32)0x00000000)
#endif

#define GET_BITS(v, lb, n)   ((v >> lb) & ((1 << n) - 1))

#define FROM_REV(maj, min, maint)    ((ui32MajRev > maj) || \
                                     ((ui32MajRev == maj) && (ui32MinRev > min)) || \
                                     ((ui32MajRev == maj) && (ui32MinRev == min) && ((IMG_INT32)(ui32MaintRev) >= maint)))

#define BEFORE_REV(maj, min, maint)  (((IMG_INT32)(ui32MajRev) < maj) || \
                                     ((ui32MajRev == maj) && ((IMG_INT32)(ui32MinRev) < min)) || \
                                     ((ui32MajRev == maj) && (ui32MinRev == min) && ((IMG_INT32)(ui32MaintRev) < maint)))

#define AT_REV(maj, min, maint)      ((ui32MajRev == (IMG_UINT32)maj) && (ui32MinRev == (IMG_UINT32)min) && (ui32MaintRev == (IMG_UINT32)maint))


/*
******************************************************************************
 Cache Config
 ******************************************************************************/
#define MSVDX_CACHE_REF_OFFSET_V100     (72L)
#define MSVDX_CACHE_ROW_OFFSET_V100     (4L)

#define MSVDX_CACHE_REF_OFFSET_V550     (144L)
#define MSVDX_CACHE_ROW_OFFSET_V550     (8L)


/*!
******************************************************************************
 This enum defines an MSVDX specific parameter
*****************************************************************************/
typedef enum
{
    VDEC_MSVDX_ASYNC_NORMAL,              //!< VDMC and VDEB
    VDEC_MSVDX_ASYNC_VDMC,                //!< VDMC only
    VDEC_MSVDX_ASYNC_VDEB,                //!< VDEB only

} VDEC_eMsvdxAsyncMode;


/*!
******************************************************************************
 This struct contains Scaler parameters prepared for hardware
******************************************************************************/
typedef struct
{
    IMG_UINT32  aui32CmdHorizLumaCoeff[VDECFW_NUM_SCALE_COEFFS];    //!< HORIZONTAL_LUMA_COEFFICIENTS
    IMG_UINT32  aui32CmdVertLumaCoeff[VDECFW_NUM_SCALE_COEFFS];     //!< VERTICAL_LUMA_COEFFICIENTS
    IMG_UINT32  aui32CmdHorizChromaCoeff[VDECFW_NUM_SCALE_COEFFS];  //!< HORIZONTAL_CHROMA_COEFFICIENTS
    IMG_UINT32  aui32CmdVertChromaCoeff[VDECFW_NUM_SCALE_COEFFS];   //!< VERTICAL_CHROMA_COEFFICIENTS

} MSVDX_sScalerCoeffCmds;


/*!
******************************************************************************
 This struct contains REAL RPR parameters
******************************************************************************/
typedef struct
{
    IMG_INT32 i32AxInitial;
    IMG_INT32 i32AxIncrement;
    IMG_INT32 i32AyInitial;
    IMG_INT32 i32AyIncrement;

} TRANSLATION_sRprParams;


/*!
******************************************************************************
 MSVDX core version information.
 NOTE: It is assumed that all cores on a device are the same revision.
******************************************************************************/
static IMG_UINT32  ui32GroupId;   /*!< IMG Group ID.                   */
static IMG_UINT32  ui32CoreId;    /*!< VXD Core ID.                    */
static IMG_UINT32  ui32MajRev;    /*!< VXD Core major revision.        */
static IMG_UINT32  ui32MinRev;    /*!< VXD Core minor revision.        */
static IMG_UINT32  ui32MaintRev;  /*!< VXD Core maintenance revision.  */
static IMG_UINT32  ui32IntRev;    /*!< VXD internal revision.          */


/*!
******************************************************************************
 This type defines the Codec Mode and should be ordered as #VDEC_eVidStd.
*****************************************************************************/
static IMG_UINT32 aui32MsvdxCodecMode[VDEC_STD_MAX] =
{
    -1,     //!< Invalid
     3,     //!< MPEG2
     4,     //!< MPEG4
     4,     //!< H263
     1,     //!< H264
     2,     //!< VC1
     5,     //!< AVS
     8,     //!< RealVideo (8)
     0,     //!< JPEG
     10,    //!< On2 VP6
     11,    //!< On2 VP8
     4,     //!< Sorenson
     12,    //!< HEVC
};


#ifndef IMG_KERNEL_MODULE
/*!
******************************************************************************

 @Function              msvdx_PollPdump

******************************************************************************/
static IMG_RESULT
msvdx_PollPdump(
    const MSVDX_sContext  * m,
    MEM_REGIONS             eMemRegion,
    IMG_UINT32              ui32Offset,
    IMG_UINT32              ui32RequValue,
    IMG_UINT32              ui32Enable
)
{
    IMG_UINT32      ui32Result = IMG_SUCCESS;

#ifdef SECURE_MEDIA_SUPPORT

    IMG_ASSERT("Pdump is not available with secure media"==0);
    return IMG_ERROR_FATAL;

#else
    ui32Result = VXDIO_PDUMPVerifPoll(m->hMsvdxIo,
                                    eMemRegion,
                                    ui32Offset,
                                    ui32RequValue,
                                    ui32Enable);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);

    return ui32Result;
}
#endif


/*!
******************************************************************************

 @Function              msvdx_Poll

******************************************************************************/
static IMG_RESULT
msvdx_Poll(
    const MSVDX_sContext  * m,
    MEM_REGIONS             eMemRegion,
    IMG_UINT32              ui32Offset,
    IMG_UINT32              ui32RequValue,
    IMG_UINT32              ui32Enable
)
{
    IMG_UINT32      ui32Result = IMG_SUCCESS;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sPollArgs sPollArgs;
    sPollArgs.ui32MemRegion = (IMG_UINT32)eMemRegion;
    sPollArgs.ui32Offset = ui32Offset;
    sPollArgs.ui32RequValue = ui32RequValue;
    sPollArgs.ui32Enable = ui32Enable;

    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&sPollArgs,
                                    sizeof(sPollArgs),
                                    ENDPOINT_VXD_POLL_NOPDUMP);
#else
    ui32Result = VXDIO_Poll(m->hMsvdxIo,
                            (IMG_UINT32)eMemRegion,
                            ui32Offset,
                            ui32RequValue,
                            ui32Enable,
                            VXDIO_POLL_EQUAL);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);

    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdx_GetCoreProperties

******************************************************************************/
static IMG_RESULT
msvdx_GetCoreProperties(
    const MSVDX_sContext  * m,
    MSVDX_sCoreProps      * p
)
{
    IMG_UINT32      ui32Reg ,ui32RegInternal;
    IMG_UINT32      ui32CoreConfig;
    IMG_UINT32      ui32LatencyConfig;
    IMG_UINT32      ui32MmuStatusReg;

    IMG_BOOL        bBifWithMmu = IMG_FALSE;
    IMG_BOOL        bMmu36bitCapable;
    IMG_BOOL        bMmu40bitCapable;

    DBGOPT_sValue sVal;
    DBGOPT_eType eType;
    MSVDX_sCoreProps sCoreProps;
    VDEC_eVidStd eVidStd;

    /* Core Revision Information */
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CORE_REV);
    ui32MajRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MAJOR_REV);
    ui32MinRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MINOR_REV);
    ui32MaintRev = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CORE_REV, CR_MSVDX_MAINT_REV);

    /* MSVDX IMG Internal */
    ui32RegInternal = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERNAL);
    ui32IntRev = REGIO_READ_FIELD(ui32RegInternal,
        MSVDX_CORE, CR_MSVDX_INTERNAL, CR_MSVDX_INTERNAL);

    /* MSVDX latency config */
    ui32LatencyConfig = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_LATENCY_CONFIG);

    /* MMU status */
    ui32MmuStatusReg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_STATUS);

    /* Core ID */
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CORE_ID);
    ui32GroupId = REGIO_READ_FIELD(ui32Reg,
        MSVDX_CORE, CR_MSVDX_CORE_ID, CR_GROUP_ID);
    ui32CoreId = REGIO_READ_FIELD(ui32Reg,
        MSVDX_CORE, CR_MSVDX_CORE_ID, CR_CORE_ID);

    // Ensure that the core is IMG Video Decoder (MSVDX).
    if (ui32GroupId != 3 || ui32CoreId != 3)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
            "IMG Video Decoder (VXD) not detected");
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

#ifndef IMG_KERNEL_MODULE
    {
        IMG_UINT32      ui32Result;

        VXDIO_PDUMPComment(m->hMsvdxIo, REGION_VXDSYS_REGSPACE, "Reading group ID");
        // Insert Group and Core Id into PDUMP
        ui32Result = msvdx_PollPdump(m,
                                REGION_VXDSYS_REGSPACE,
                                MSVDX_CORE_CR_MSVDX_CORE_ID_OFFSET,
                                ui32GroupId << MSVDX_CORE_CR_MSVDX_CORE_ID_CR_GROUP_ID_SHIFT,
                                MSVDX_CORE_CR_MSVDX_CORE_ID_CR_GROUP_ID_MASK);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);

        VXDIO_PDUMPComment(m->hMsvdxIo, REGION_VXDSYS_REGSPACE, "Reading core ID");
        ui32Result = msvdx_PollPdump(m,
                                REGION_VXDSYS_REGSPACE,
                                MSVDX_CORE_CR_MSVDX_CORE_ID_OFFSET,
                                ui32CoreId << MSVDX_CORE_CR_MSVDX_CORE_ID_CR_CORE_ID_SHIFT,
                                MSVDX_CORE_CR_MSVDX_CORE_ID_CR_CORE_ID_MASK);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
    }
#endif

    ui32CoreConfig = REGIO_READ_FIELD(ui32Reg,
        MSVDX_CORE, CR_MSVDX_CORE_ID, CR_MSVDX_CORE_CONFIG);

    if (DBGOPTKM_GetSize("CoreConfig", DBGOPT_TYPE_BUF) == sizeof(sCoreProps) &&
        DBGOPTKM_Get("CoreConfig", &eType, &sVal))
    {
        *p = *((MSVDX_sCoreProps *) sVal.sBufVal.pvVal);

        DEBUG_REPORT(REPORT_MODULE_VXDIO,
                     "Setting core configuration from debug options");

        goto setting_props_ending;
    }

    VDEC_BZERO(p);

    // Construct core version name.
    sprintf(p->acVersion, "%d.%d.%d",
            ui32MajRev, ui32MinRev, ui32MaintRev);

    //////////////////////////////////////
    //  Default settings                //
    //////////////////////////////////////

    // Default minimum coded size is 6x1 MBs.
    p->ui32MinWidth = 6 * VDEC_MB_DIMENSION;
    p->ui32MinHeight = VDEC_MB_DIMENSION;

    for (eVidStd = VDEC_STD_UNDEFINED; eVidStd < VDEC_STD_MAX; eVidStd++)
    {
        // Default HD resolution (8k MBs), also supports 1920x1088
        p->aui32MaxWidth[eVidStd] = 2048;
        p->aui32MaxHeight[eVidStd] = 2048;

        if (eVidStd == VDEC_STD_H264)
        {
            p->aui32MaxWidth[eVidStd] = 1920;
        }
    }

    // Maximum 8k MBs.
    p->ui32MaxMbs = 2048 * 1024;

    // Default bit-depth
    if (BEFORE_REV(6,0,0))
    {
        p->ui32MaxBitDepthLuma      =   8;
        p->ui32MaxBitDepthChroma    =   8;
    }
    else
    {
        p->ui32MaxBitDepthLuma      =   10;
        p->ui32MaxBitDepthChroma    =   10;
    }



    // Default latency.
    p->ui32Latency = 128;

    // Default VDMC cache size.
    p->ui32VdmcCacheSize = 1;
    p->bVdmcBurst4 = IMG_FALSE;

    // SR DMA burst size
    // Value must be remap`ed for hardware use (2 = 1, 4 = 2, 8 = 3). CR_VEC_CONTROL_2, SR_BURST_SIZE,
    p->ui32SrDmaBurstSize = 4;

    // Default master.
    p->ui32NumCores = 1;

    // Populate the core properties.
    if (GET_BITS(ui32CoreConfig, 8, 1))
    {
        // Bit implemented in 3.0.2 but
        // some earlier cores (2.x.x onwards)
        // may have rotation capability.
        p->bRotationSupport = IMG_TRUE;
    }
    if (GET_BITS(ui32CoreConfig, 9, 1))
    {
        // Bit implemented in 3.0.2
        p->bScalingSupport = IMG_TRUE;
    }
    if (GET_BITS(ui32CoreConfig, 11, 1))
    {
        // Bit implemented in 2.0.3
        p->bHdSupport = IMG_TRUE;
    }
    if (GET_BITS(ui32CoreConfig, 15, 1))
    {
        bBifWithMmu = IMG_TRUE;
    }

    p->ui32NumStreams = GET_BITS(ui32CoreConfig, 12, 2) + 1;

    bMmu36bitCapable = REGIO_READ_FIELD(ui32MmuStatusReg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_36BIT_CAPABLE);
    bMmu40bitCapable = GET_BITS(ui32LatencyConfig, 26, 1);

    if (bBifWithMmu)
    {
        p->eMmuType = MMU_TYPE_32BIT;

        if (bMmu40bitCapable)
        {
            p->eMmuType = MMU_TYPE_40BIT;
        }
        else if (bMmu36bitCapable)
        {
            p->eMmuType = MMU_TYPE_36BIT;
        }
    }
    else
    {
        p->eMmuType = MMU_TYPE_NONE;
    }

    // Video standards.
    p->bMpeg2 = GET_BITS(ui32CoreConfig, 0, 1) ? IMG_TRUE : IMG_FALSE;
    p->bMpeg4 = GET_BITS(ui32CoreConfig, 1, 1) ? IMG_TRUE : IMG_FALSE;
    p->bH264  = GET_BITS(ui32CoreConfig, 2, 1) ? IMG_TRUE : IMG_FALSE;
    p->bVc1   = GET_BITS(ui32CoreConfig, 3, 1) ? IMG_TRUE : IMG_FALSE;
    p->bAvs   = GET_BITS(ui32CoreConfig, 10, 1) ? IMG_TRUE : IMG_FALSE;
    if (BEFORE_REV(5,0,0))         // real support is being read from MSVDX_INTERNAL register core before to 5.0.0
        p->bReal  = GET_BITS(ui32RegInternal, 23, 1) ? IMG_TRUE : IMG_FALSE;
    else
        p->bReal  = GET_BITS(ui32LatencyConfig, 30, 1) ? IMG_TRUE : IMG_FALSE;

    p->bJpeg  = GET_BITS(ui32LatencyConfig, 31, 1) ? IMG_TRUE : IMG_FALSE;
    p->bVp6   = GET_BITS(ui32LatencyConfig, 29, 1) ? IMG_TRUE : IMG_FALSE;
    p->bVp8   = GET_BITS(ui32LatencyConfig, 27, 1) ? IMG_TRUE : IMG_FALSE;




    //////////////////////////////////////
    //  Version specific settings       //
    //////////////////////////////////////

    if (FROM_REV(2,0,0))
    {
        p->bAutoClockGatingSupport = IMG_TRUE;
    }

    if (AT_REV(2,1,3))
    {
        p->bRotationSupport = IMG_TRUE;
    }

    if (FROM_REV(3,0,0))
    {
        p->b64ByteFixedStrides = IMG_TRUE;
        p->bExtendedStrides = IMG_TRUE;
        p->ui32CmdBufferSize = 64;
    }
    else
    {
        p->BRN26832 = IMG_TRUE;
        p->ui32CmdBufferSize = 16;
    }

    if (AT_REV(3,0,2) || AT_REV(3,0,5))
    {
        p->BRN29797 = IMG_TRUE;
        p->BRN29870 = IMG_TRUE;
        p->BRN29871 = IMG_TRUE;
        p->BRN30095 = IMG_TRUE;
        p->BRN30178 = IMG_TRUE;
        p->BRN30306 = IMG_TRUE;
    }

    if (BEFORE_REV(3,0,4))
    {
        p->BRN28888 = IMG_TRUE;

        if (AT_REV(3,0,3) && ui32IntRev >= 515)
        {
            p->BRN28888 = IMG_FALSE;
        }
    }

    if (FROM_REV(3,0,5))
    {
        p->bErrorHandling = IMG_TRUE;
        p->bErrorConcealment = IMG_TRUE;
    }

    if (BEFORE_REV(4,0,0))
    {
        p->BRN32651 = IMG_TRUE;
    }

    if (FROM_REV(4,0,3))
    {
        p->bNewTestRegSpace = IMG_TRUE;
    }

    if (FROM_REV(5,0,0))
    {
        p->b4kScalingCoeffs = IMG_TRUE;
        p->bScalingWithOold = IMG_TRUE;
        p->bExtClockGating = IMG_TRUE;
        p->bAuxLineBufSupported = IMG_TRUE;

        p->bImprovedErrorConcealment = IMG_TRUE;

        p->ui32Latency = 384;
        p->ui32VdmcCacheSize = 2;

        for (eVidStd = VDEC_STD_UNDEFINED; eVidStd < VDEC_STD_MAX; eVidStd++)
        {
            p->aui32MaxWidth[eVidStd] = 4096;
            p->aui32MaxHeight[eVidStd] = 4096;
        }

        // Maximum 64k MBs.
        p->ui32MaxMbs = 4096 * 4096;
    }
    else
    {
        p->BRN29688 = IMG_TRUE;
    }

    // Tourmaline, VXD393
    if (FROM_REV(5,5,0))
    {
        p->bNewCacheSettings = IMG_TRUE;
        p->ui32VdmcCacheSize = 4;
        p->bVdmcBurst4 = IMG_TRUE;
        p->bLossless = IMG_TRUE;
    }

    if (FROM_REV(5,5,2))
    {
        p->ui32SrDmaBurstSize = 8;
    }

    //Enabling multicore from rev 5.6.x
    if (FROM_REV(5,6,0))
    {
        p->b8bitHighColour = IMG_TRUE;
        p->ui32ScalingPrecision = 12;

        p->bMultiCoreSupport = IMG_TRUE;
        // This may not work/be invalid for slave cores.
        ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_MULTICORE_STATUS);
        p->ui32NumCores += REGIO_READ_FIELD(ui32Reg,
            MSVDX_CORE, CR_MSVDX_MULTICORE_STATUS, CR_MSVDX_NUM_SLAVES);
    }
    else
    {
        p->ui32ScalingPrecision = 11;
    }

    if(FROM_REV(5,6,0) && BEFORE_REV(6,0,0))
    {
        //SunStone/Verdalite
        p->bChromaUpsample = IMG_TRUE;
    }

    if (FROM_REV(6,0,0))
    {
        p->BRN40493 = IMG_FALSE;
    }
    else
    {
        p->BRN40493 = IMG_TRUE;
    }

    if (FROM_REV(6,1,2))
    {
        p->ui32MaxBitDepthLuma = 10;
        p->ui32MaxBitDepthChroma = 10;
    }

    //////////////////////////////////////
    //  Override settings               //
    //////////////////////////////////////

    if (!p->bAuxLineBufSupported)
    {
        p->BRN31777 = IMG_TRUE;
    }

    if (p->ui32Latency > 512 && p->ui32Latency <= 1024)
    {
        p->ui32SrDmaBurstSize = 8; //DMAC_BURST_8;
    }

setting_props_ending:
    // Write trusted host pin
    //TALREG_WriteWord32(m->hPinTestMemSpace, 0x148, 1);

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              msvdx_GetCoreInit

 @Description

 This function prepares data for core initialisation in the firmware.

 @Input     p               : Pointer to MSVDX core properties.

 @Input     psRendec        : Pointer to rendec information.

 @Output    psCoreInitData  : Pointer to core initialisation data.

 @Return    IMG_RESULT      : This function returns either IMG_SUCCESS or an
                              error code.

******************************************************************************/
static IMG_RESULT
msvdx_GetCoreInit(
    const MSVDX_sCoreProps    * p,
    HWCTRL_sRendec            * psRendec,
    VDECFW_sCoreInitData      * psCoreInitData
)
{
    IMG_UINT32              ui32SrBurstSize = 2;
    VDECFW_sMmuConfig     * psMmuConfig;
    VDECFW_sRendecConfig  * psRendecData;

    IMG_ASSERT(psCoreInitData);

    psMmuConfig = &psCoreInitData->sMmuConfig;
    psRendecData = &psCoreInitData->sRendecConfig;

    VDEC_BZERO(psMmuConfig);
    VDEC_BZERO(psRendecData);

#ifdef IMG_KERNEL_MODULE
    IMG_ASSERT(p->eMmuType != MMU_TYPE_NONE);
#endif

    if (p->eMmuType == MMU_TYPE_40BIT)
    {
        REGIO_WRITE_FIELD(psMmuConfig->ui32MmuControl2,
            MSVDX_CORE, CR_MMU_CONTROL2, CR_MMU_ENABLE_40BIT_ADDRESSING,
            1);
    }
    else if (p->eMmuType == MMU_TYPE_36BIT)
    {
        //TCONF_sWrapperControl sWrapperControl;

        REGIO_WRITE_FIELD(psMmuConfig->ui32MmuControl2,
            MSVDX_CORE, CR_MMU_CONTROL2, CR_MMU_ENABLE_36BIT_ADDRESSING,
            1);

        /*if (DBGOPTKM_GetSize("36BitTwiddle", DBGOPT_TYPE_BOOL) &&
            DBGOPTKM_Get("36BitTwiddle", &eType, &sVal))
        {
            psMmuConfig->b36bitMemTwiddle = sVal.bVal;
        }*/

        // Check 36bit memory twiddle in config file and apply test bit accordingly (in FW)
        // NOTE: this might need to be moved up to the application
        //TCONF_GetWrapperControlInfo(&sWrapperControl);
        /*if (sWrapperControl.ui32Flags & TAL_WRAPFLAG_MEM_TWIDDLE)
        {
            psMmuConfig->b36bitMemTwiddle = IMG_TRUE;
        }*/
    }


    // Rendec Configuration.
    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecBufferSize,
        MSVDX_VEC, CR_VEC_RENDEC_BUFFER_SIZE, RENDEC_BUFFER_SIZE0,
        psRendec->aui32BufferSize[0] / (4*1024));
    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecBufferSize,
        MSVDX_VEC, CR_VEC_RENDEC_BUFFER_SIZE, RENDEC_BUFFER_SIZE1,
        psRendec->aui32BufferSize[1] / (4*1024));

    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecControl1,
        MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_DECODE_START_SIZE,
        psRendec->ui32DecodeStartSize);
    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecControl1,
        MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_BURST_SIZE_W,
        psRendec->ui8BurstSizeWrite);
    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecControl1,
        MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_BURST_SIZE_R,
        psRendec->ui8BurstSizeRead);
    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecControl1,
        MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_EXTERNAL_MEMORY,
        psRendec->bInUse);

    psRendecData->aui32RendecInitialContext[0] = psRendec->aui32InitialContext[0];
    psRendecData->aui32RendecInitialContext[1] = psRendec->aui32InitialContext[1];
    psRendecData->aui32RendecInitialContext[2] = psRendec->aui32InitialContext[2];
    psRendecData->aui32RendecInitialContext[3] = psRendec->aui32InitialContext[3];
    psRendecData->aui32RendecInitialContext[4] = psRendec->aui32InitialContext[4];
    psRendecData->aui32RendecInitialContext[5] = psRendec->aui32InitialContext[5];

    REGIO_WRITE_FIELD(psRendecData->ui32RegVecRendecControl0,
        MSVDX_VEC, CR_VEC_RENDEC_CONTROL0, RENDEC_INITIALISE,
        1);

    /* calculate the value to write */
    switch (p->ui32SrDmaBurstSize)
    {
    case 2:
        ui32SrBurstSize = 1;
        break;

    case 4:
        ui32SrBurstSize = 2;
        break;

    case 8:
        ui32SrBurstSize = 3;
        break;

    default:
        IMG_ASSERT(0);  // this burst size should not be used
        break;
    }

    psCoreInitData->ui32RegVecControl2 = 0;
    REGIO_WRITE_FIELD(psCoreInitData->ui32RegVecControl2,
        MSVDX_VEC, CR_VEC_CONTROL_2, SR_BURST_SIZE,
        ui32SrBurstSize);

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              msvdx_CoreReset

******************************************************************************/
static IMG_RESULT
msvdx_CoreReset(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32        ui32Reg;
    IMG_UINT32        i;
    IMG_UINT32        ui32Result = IMG_SUCCESS;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    //if (!m->bInitialised)
    //{
    //    REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
    //           "An MSVDX context not initialised");
    //    ui32Result = IMG_ERROR_NOT_INITIALISED;
    //    goto error;
    //}

    DEBUG_REPORT(REPORT_MODULE_VXDIO, "Doing MSVDX Reset");

    /*
     * At this point psHwCtx->bPower is already set to false,
     * so there is no need to disable interrupts by SYSOSKM_DisableInt
     * (even if triggered, it won't do a thing)
     */

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    // Enable Manual clocks. Required in the reset mechanism in the TRM.
    m->sEnableClocksArgs.bAutoClockGatingSupport = m->sProps.bAutoClockGatingSupport;
    m->sEnableClocksArgs.bExtClockGating = m->sProps.bExtClockGating;
    m->sEnableClocksArgs.bForceManual = IMG_TRUE;
    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&m->sEnableClocksArgs,
                                    sizeof(m->sEnableClocksArgs),
                                    ENDPOINT_VXD_ENABLE_CLOCKS);
#else
    ui32Result = VXDIO_EnableClocks(m->hMsvdxIo,
                                    m->sProps.bAutoClockGatingSupport,
                                    m->sProps.bExtClockGating,
                                    IMG_TRUE);
#endif
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }

    /*
     * Clear the interrupt enable to minimize chance of getting spurious interrupts.
     * Thanks to psHwCtx->bPower set to false we should handle such interrupts
     * correctly, but let's do it anyway.
     */
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE, 0);

    // Pause the MMU.
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_CONTROL0, CR_MMU_PAUSE, 1);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0, ui32Reg);

    // Stop MTX.
    ui32Reg = REGIO_READ_REGISTER(m, MTX_CORE, CR_MTX_SOFT_RESET);
    REGIO_WRITE_FIELD(ui32Reg, MTX_CORE, CR_MTX_SOFT_RESET, MTX_RESET, 1);
    REGIO_WRITE_REGISTER(m, MTX_CORE, CR_MTX_SOFT_RESET, ui32Reg);

    /* Handle MMU faults. */
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS);
    if (ui32Reg & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK)
    {
        IMG_UINT32 ui32PageFault;

        /* Obtain MMU status. */
        ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_STATUS);
        ui32PageFault = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_PF_N_RW);

        if (ui32PageFault)
        {
            // write a valid PTD to the correct base address(es)
            //for (i = 0; i < MSVDX_CORE_CR_MMU_DIR_LIST_BASE_NO_ENTRIES; i++)
            //{
            //    REGIO_WRITE_TABLE_REGISTER(m, MSVDX_CORE, CR_MMU_DIR_LIST_BASE, i,
            //        psCoreInitData->sMmuConfig.aui32PTD[i]);
            //}

            /* Invalidate the MMU. */
            ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_CONTROL0, CR_MMU_FLUSH, 1);
            REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0, ui32Reg);

            /* Clear all MMU faults. */
            ui32Reg = 0;
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS, CR_MMU_FAULT_IRQ, 0xf);
            REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERRUPT_CLEAR, ui32Reg);
        }
    }

    // Check that the outstanding read requests has stabilised at zero.
    for (i = 0; i < 10; i++)
    {
#ifndef IMG_KERNEL_MODULE
        VXDIO_PDUMPComment(m->hMsvdxIo, REGION_VXDSYS_REGSPACE, "Wait for all outstanding bus reads to complete");
#endif
        /* Wait for all outstanding bus reads to complete. */
        ui32Result = msvdx_Poll(m,
                                REGION_VXDSYS_REGSPACE,
                                MSVDX_CORE_CR_MMU_MEM_REQ_OFFSET,
                                0,
                                MSVDX_CORE_CR_MMU_MEM_REQ_CR_MEM_REQ_STAT_READS_MASK);
        IMG_ASSERT((ui32Result == IMG_SUCCESS) || (ui32Result == IMG_ERROR_TIMEOUT));
        if ((ui32Result != IMG_SUCCESS) && (ui32Result != IMG_ERROR_TIMEOUT))
        {
            goto error_reset;
        }
    }
    if (ui32Result == IMG_ERROR_TIMEOUT)
    {
        goto error_reset;
    }

    // Disconnect RENDEC decoder from memory.
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_DEC_DISABLE, 1);
    REGIO_WRITE_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, ui32Reg);

    /* Issue a s/w reset...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_FE_SOFT_RESET, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_BE_SOFT_RESET, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VDMC_VDEB_SOFT_RESET, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VEC_MEMIF_SOFT_RESET, 1);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VEC_RENDEC_DEC_SOFT_RESET, 1);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL, ui32Reg);

    // Read back the MSVDX control register to allow the hardware to detect the reset.
    REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL);

    /* Manually clear all the reset bits. */
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_FE_SOFT_RESET, 0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_BE_SOFT_RESET, 0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VDMC_VDEB_SOFT_RESET, 0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VEC_MEMIF_SOFT_RESET, 0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_VEC_RENDEC_DEC_SOFT_RESET, 0);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL, ui32Reg);

    // Check that the outstanding read requests has stabilised at zero.
    for (i = 0; i < 10; i++)
    {
#ifndef IMG_KERNEL_MODULE
        VXDIO_PDUMPComment(m->hMsvdxIo, REGION_VXDSYS_REGSPACE, "Wait for all outstanding bus reads to complete");
#endif
        /* Wait for all outstanding bus reads to complete. */
        ui32Result = msvdx_Poll(m,
                                REGION_VXDSYS_REGSPACE,
                                MSVDX_CORE_CR_MMU_MEM_REQ_OFFSET,
                                0,
                                MSVDX_CORE_CR_MMU_MEM_REQ_CR_MEM_REQ_STAT_READS_MASK);
        IMG_ASSERT((ui32Result == IMG_SUCCESS) || (ui32Result == IMG_ERROR_TIMEOUT));
        if ((ui32Result != IMG_SUCCESS) && (ui32Result != IMG_ERROR_TIMEOUT))
        {
            goto error_reset;
        }
    }
    if(ui32Result == IMG_ERROR_TIMEOUT)
    {
        goto error_reset;
    }

    /* Reset the core (whilst idle). */
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_MSVDX_SOFT_RESET, 1);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL, ui32Reg);

#ifndef IMG_KERNEL_MODULE
    VXDIO_PDUMPComment(m->hMsvdxIo, REGION_VXDSYS_REGSPACE, "Wait for the reset to complete");
#endif
    /* Wait for the reset to complete. */
    ui32Result = msvdx_Poll(m,
                            REGION_VXDSYS_REGSPACE,
                            MSVDX_CORE_CR_MSVDX_CONTROL_OFFSET,
                            0,
                            MSVDX_CORE_CR_MSVDX_CONTROL_CR_MSVDX_SOFT_RESET_MASK);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_reset;
    }

    /* Set control register...*/
    ui32Reg = 0;
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MSVDX_CONTROL, CR_ENDIAN, MSVDX_ENDIAN);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_CONTROL, ui32Reg);

    /* Clear the interrupt enable...*/
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE, 0);

    /* Clear any pending interrupts...*/
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERRUPT_CLEAR, 0xFFFFFFFF);

error_reset:
    /* Registering for intercepting MMU interrupts on the host */
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE) ;
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE,
        ui32Reg | MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK );

    DEBUG_REPORT(REPORT_MODULE_VXDIO, "MSVDX Reset Completed");


error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              msvdx_RtmWriteRead

******************************************************************************/
static IMG_RESULT
msvdx_RtmWriteRead(
    const MSVDX_sContext  * m,
    IMG_UINT32              ui32Select,
    IMG_UINT32            * pui32Val
)
{
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MSVDX_RTM, ui32Select);

    *pui32Val = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_RTM);

    return IMG_SUCCESS;
}


#ifndef SYSBRG_BRIDGING
/*
******************************************************************************

 @Function              msvdx_CheckInterruptFunc

 @Description

 Check interrupt function.


******************************************************************************/
static IMG_BOOL
msvdx_CheckInterruptFunc(
    IMG_HANDLE  ui32MemSpaceId,
    IMG_VOID *  pCallbackParameter
)
{
    IMG_UINT32      ui32IntStatus = 0;
    IMG_HANDLE      hCoreContext = (IMG_HANDLE)pCallbackParameter;

    /* Disable interrupts.  */
    SYSOSKM_DisableInt();

    /* Read the status register. */
    ui32IntStatus = VXDIO_ReadRegister(hCoreContext, REGION_VXDSYS_REGSPACE, MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_OFFSET, MSVDX_DEF_WR_PIPE);

    /* Re enable interrupts */
    SYSOSKM_EnableInt();

    return (ui32IntStatus != 0);
}

#endif /* not SYSBRG_BRIDGING */


/*
******************************************************************************

 @Function              scaler_ApplyRestrictions

******************************************************************************/
static IMG_RESULT
scaler_ApplyRestrictions(
    const SCALER_sConfig  * psScalerConfig,
    SCALER_sParams        * psParams
)
{
    const MSVDX_sCoreProps * p = psScalerConfig->psCoreProps;

    // When display picture height is 2048, vertical start position must
    // be set to zero to avoid overflow when calculating last pixel position (BRN29870).
    if (p->BRN29870 && psScalerConfig->ui32ReconHeight == 2048)
    {
        psParams->iVertStartPos = 0;
    }

    return IMG_SUCCESS;
}


/*
******************************************************************************

 @Function              MSVDX_GetScalerCoeffCmds

******************************************************************************/
IMG_RESULT
MSVDX_GetScalerCoeffCmds(
    const SCALER_sCoeffs  * psScalerCoeffs,
    IMG_UINT32            * pui32PictCmds
)
{
    IMG_UINT32 i, j;
    MSVDX_sScalerCoeffCmds  sCoeffCmds;
    MSVDX_sScalerCoeffCmds * psCoeffCmds = &sCoeffCmds;

    if (psScalerCoeffs->paui8HorizLuma)
    {
        for (i = 0; i < 4; i++)
        {
            j = 1 + (2 * i);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizLumaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_LUMA_COEFFICIENTS, HOR_LUMA_COEFF_3,
                psScalerCoeffs->paui8HorizLuma[0][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizLumaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_LUMA_COEFFICIENTS, HOR_LUMA_COEFF_2,
                psScalerCoeffs->paui8HorizLuma[1][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizLumaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_LUMA_COEFFICIENTS, HOR_LUMA_COEFF_1,
                psScalerCoeffs->paui8HorizLuma[2][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizLumaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_LUMA_COEFFICIENTS, HOR_LUMA_COEFF_0,
                psScalerCoeffs->paui8HorizLuma[3][j]);
        }
    }
    else
    {
        psCoeffCmds->aui32CmdHorizLumaCoeff[0] = 0;
        psCoeffCmds->aui32CmdHorizLumaCoeff[1] = 0;
        psCoeffCmds->aui32CmdHorizLumaCoeff[2] = 0;
        psCoeffCmds->aui32CmdHorizLumaCoeff[3] = 0x00400000;
    }


    if (psScalerCoeffs->paui8HorizChroma)
    {
        for (i = 0; i < 4; i++)
        {
            j = 1 + (2 * i);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizChromaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_CHROMA_COEFFICIENTS, HOR_CHROMA_COEFF_3,
                psScalerCoeffs->paui8HorizChroma[0][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizChromaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_CHROMA_COEFFICIENTS, HOR_CHROMA_COEFF_2,
                psScalerCoeffs->paui8HorizChroma[1][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizChromaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_CHROMA_COEFFICIENTS, HOR_CHROMA_COEFF_1,
                psScalerCoeffs->paui8HorizChroma[2][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdHorizChromaCoeff[i],
                MSVDX_CMDS, HORIZONTAL_CHROMA_COEFFICIENTS, HOR_CHROMA_COEFF_0,
                psScalerCoeffs->paui8HorizChroma[3][j]);
        }
    }
    else
    {
        psCoeffCmds->aui32CmdHorizChromaCoeff[0] = 0;
        psCoeffCmds->aui32CmdHorizChromaCoeff[1] = 0;
        psCoeffCmds->aui32CmdHorizChromaCoeff[2] = 0;
        psCoeffCmds->aui32CmdHorizChromaCoeff[3] = 0x00400000;
    }

    if (psScalerCoeffs->paui8VertLuma)
    {
        for (i = 0; i < 4; i++)
        {
            j = 1 + (2 * i);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertLumaCoeff[i],
                MSVDX_CMDS, VERTICAL_LUMA_COEFFICIENTS, VER_LUMA_COEFF_3,
                psScalerCoeffs->paui8VertLuma[0][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertLumaCoeff[i],
                MSVDX_CMDS, VERTICAL_LUMA_COEFFICIENTS, VER_LUMA_COEFF_2,
                psScalerCoeffs->paui8VertLuma[1][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertLumaCoeff[i],
                MSVDX_CMDS, VERTICAL_LUMA_COEFFICIENTS, VER_LUMA_COEFF_1,
                psScalerCoeffs->paui8VertLuma[2][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertLumaCoeff[i],
                MSVDX_CMDS, VERTICAL_LUMA_COEFFICIENTS, VER_LUMA_COEFF_0,
                psScalerCoeffs->paui8VertLuma[3][j]);
        }
    }
    else
    {
        psCoeffCmds->aui32CmdVertLumaCoeff[0] = 0;
        psCoeffCmds->aui32CmdVertLumaCoeff[1] = 0;
        psCoeffCmds->aui32CmdVertLumaCoeff[2] = 0;
        psCoeffCmds->aui32CmdVertLumaCoeff[3] = 0x00400000;
    }


    if (psScalerCoeffs->paui8VertChroma)
    {
        for (i = 0; i < 4; i++)
        {
            j = 1 + (2 * i);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertChromaCoeff[i],
                MSVDX_CMDS, VERTICAL_CHROMA_COEFFICIENTS, VER_CHROMA_COEFF_3,
                psScalerCoeffs->paui8VertChroma[0][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertChromaCoeff[i],
                MSVDX_CMDS, VERTICAL_CHROMA_COEFFICIENTS, VER_CHROMA_COEFF_2,
                psScalerCoeffs->paui8VertChroma[1][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertChromaCoeff[i],
                MSVDX_CMDS, VERTICAL_CHROMA_COEFFICIENTS, VER_CHROMA_COEFF_1,
                psScalerCoeffs->paui8VertChroma[2][j]);

            REGIO_WRITE_FIELD(psCoeffCmds->aui32CmdVertChromaCoeff[i],
                MSVDX_CMDS, VERTICAL_CHROMA_COEFFICIENTS, VER_CHROMA_COEFF_0,
                psScalerCoeffs->paui8VertChroma[3][j]);
        }
    }
    else
    {
        // Scaler by-pass
        psCoeffCmds->aui32CmdVertChromaCoeff[0] = 0;
        psCoeffCmds->aui32CmdVertChromaCoeff[1] = 0;
        psCoeffCmds->aui32CmdVertChromaCoeff[2] = 0;
        psCoeffCmds->aui32CmdVertChromaCoeff[3] = 0x00400000;
    }

    // Copy the coefficients into the commands array.
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_LUMA_COEFFICIENTS_0] = psCoeffCmds->aui32CmdHorizLumaCoeff[0];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_LUMA_COEFFICIENTS_1] = psCoeffCmds->aui32CmdHorizLumaCoeff[1];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_LUMA_COEFFICIENTS_2] = psCoeffCmds->aui32CmdHorizLumaCoeff[2];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_LUMA_COEFFICIENTS_3] = psCoeffCmds->aui32CmdHorizLumaCoeff[3];
    pui32PictCmds[VDECFW_CMD_VERTICAL_LUMA_COEFFICIENTS_0] = psCoeffCmds->aui32CmdVertLumaCoeff[0];
    pui32PictCmds[VDECFW_CMD_VERTICAL_LUMA_COEFFICIENTS_1] = psCoeffCmds->aui32CmdVertLumaCoeff[1];
    pui32PictCmds[VDECFW_CMD_VERTICAL_LUMA_COEFFICIENTS_2] = psCoeffCmds->aui32CmdVertLumaCoeff[2];
    pui32PictCmds[VDECFW_CMD_VERTICAL_LUMA_COEFFICIENTS_3] = psCoeffCmds->aui32CmdVertLumaCoeff[3];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_CHROMA_COEFFICIENTS_0] = psCoeffCmds->aui32CmdHorizChromaCoeff[0];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_CHROMA_COEFFICIENTS_1] = psCoeffCmds->aui32CmdHorizChromaCoeff[1];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_CHROMA_COEFFICIENTS_2] = psCoeffCmds->aui32CmdHorizChromaCoeff[2];
    pui32PictCmds[VDECFW_CMD_HORIZONTAL_CHROMA_COEFFICIENTS_3] = psCoeffCmds->aui32CmdHorizChromaCoeff[3];
    pui32PictCmds[VDECFW_CMD_VERTICAL_CHROMA_COEFFICIENTS_0] = psCoeffCmds->aui32CmdVertChromaCoeff[0];
    pui32PictCmds[VDECFW_CMD_VERTICAL_CHROMA_COEFFICIENTS_1] = psCoeffCmds->aui32CmdVertChromaCoeff[1];
    pui32PictCmds[VDECFW_CMD_VERTICAL_CHROMA_COEFFICIENTS_2] = psCoeffCmds->aui32CmdVertChromaCoeff[2];
    pui32PictCmds[VDECFW_CMD_VERTICAL_CHROMA_COEFFICIENTS_3] = psCoeffCmds->aui32CmdVertChromaCoeff[3];

    return IMG_SUCCESS;
}


/*
******************************************************************************

 @Function              MSVDX_GetScalerCmds

******************************************************************************/
IMG_RESULT
MSVDX_GetScalerCmds(
    const SCALER_sConfig    * psScalerConfig,
    const SCALER_sPitch     * psPitch,
    const SCALER_sFilter    * psFilter,
    const PIXEL_sPixelInfo  * psOutLoopPixelInfo,
    SCALER_sParams          * psParams,
    IMG_UINT32              * pui32PictCmds
)
{
    const MSVDX_sCoreProps * p = psScalerConfig->psCoreProps;

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, ALT_OUTPUT_FORMAT,
        PIXEL_GET_HW_CHROMA_FORMAT_IDC(psOutLoopPixelInfo->eChromaFormatIdc));

#ifdef FXDPNT_SCALER_CALC

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_CHROMA_RESAMP_ONLY,
        (psPitch->iHorizLuma == FIXED(1,HIGHP)) && (psPitch->iVertLuma == FIXED(1,HIGHP)));

#else

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_CHROMA_RESAMP_ONLY,
        (psPitch->fHorizLuma == 1) && (psPitch->fVertLuma == 1));

#endif

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, ALT_MEMORY_PACKING,
        PIXEL_GET_HW_MEMORY_PACKING(psOutLoopPixelInfo->eMemoryPacking) );

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, ALT_BIT_DEPTH_LUMA_MINUS8,
        psOutLoopPixelInfo->ui32BitDepthY - 8);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, ALT_BIT_DEPTH_CHROMA_MINUS8,
        psOutLoopPixelInfo->ui32BitDepthC - 8);

    /* Scale luma bifilter is always 0 for now */
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_LUMA_BIFILTER_HORIZ,
        0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_LUMA_BIFILTER_VERT,
        0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_CHROMA_BIFILTER_HORIZ,
        psFilter->bHorizBilinear ? 1 : 0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, SCALE_CHROMA_BIFILTER_VERT,
        psFilter->bVertBilinear ? 1 : 0);

    // for cores 7.x.x and more, precision 3.13
    if (p->b8kScalingCoeffs)
    {
        psParams->iFixedPointShift = 13;
    }
    // for cores 5.x.x and more, precision 3.12
    else if (p->b4kScalingCoeffs)
    {
        psParams->iFixedPointShift = 12;
    }
    else
    {
        psParams->iFixedPointShift = 11;
    }

#ifdef FXDPNT_SCALER_CALC
    /* Calculate the fixed-point versions for use by the hardware. */
    psParams->iVertPitch = (int)((psPitch->iVertLuma + (1 << (HIGHP - psParams->iFixedPointShift - 1))) >> (HIGHP - psParams->iFixedPointShift));
    psParams->iVertStartPos = psParams->iVertPitch >> 1;
    psParams->iVertPitchChroma = (int)((psPitch->iVertChroma + (1 << (HIGHP - psParams->iFixedPointShift - 1))) >> (HIGHP - psParams->iFixedPointShift));
    psParams->iVertStartPosChroma = psParams->iVertPitchChroma >> 1;
    psParams->iHorzPitch = (int)(psPitch->iHorizLuma >> (HIGHP - psParams->iFixedPointShift));
    psParams->iHorzStartPos = psParams->iHorzPitch >> 1;
    psParams->iHorzPitchChroma = (int)(psPitch->iHorizChroma >> (HIGHP - psParams->iFixedPointShift));
    psParams->iHorzStartPosChroma = psParams->iHorzPitchChroma >> 1;

#else

    psParams->iVertPitch = (int)(psPitch->fVertLuma * (1 << psParams->iFixedPointShift) + 0.5f);
    psParams->iVertStartPos = (int)(psPitch->fVertLuma * 0.5f * (1 << psParams->iFixedPointShift) + 0.5f);
    psParams->iVertPitchChroma = (int)(psPitch->fVertChroma * (1 << psParams->iFixedPointShift) + 0.5f);
    psParams->iVertStartPosChroma = (int)(psPitch->fVertChroma * 0.5f * (1 << psParams->iFixedPointShift) + 0.5f);
    psParams->iHorzPitch = (int)(psPitch->fHorizLuma * (1 << psParams->iFixedPointShift));
    psParams->iHorzStartPos = (int)(psPitch->fHorizLuma * 0.5f * (1 << psParams->iFixedPointShift));
    psParams->iHorzPitchChroma = (int)(psPitch->fHorizChroma * (1 << psParams->iFixedPointShift));
    psParams->iHorzStartPosChroma = (int)(psPitch->fHorizChroma * 0.5f * (1 << psParams->iFixedPointShift));

#endif

#if 0
    psParams->iVertPitch = 0x1000;
    psParams->iVertStartPos = 0x800;
    psParams->iVertPitchChroma = 0x1000;
    psParams->iVertStartPosChroma = 0x800;
    psParams->iHorzPitch = 0x1000;
    psParams->iHorzStartPos = 0x800;
    psParams->iHorzPitchChroma = 0x1000;
    psParams->iHorzStartPosChroma = 0x800;

#endif

    scaler_ApplyRestrictions(psScalerConfig, psParams);

#ifdef HAS_HEVC
    if (psScalerConfig->eVidStd == VDEC_STD_HEVC)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALED_DISPLAY_SIZE],
            MSVDX_CMDS, PVDEC_SCALED_DISPLAY_SIZE, PVDEC_SCALE_DISPLAY_WIDTH,
            psScalerConfig->ui32ReconWidth - 1);
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALED_DISPLAY_SIZE],
            MSVDX_CMDS, PVDEC_SCALED_DISPLAY_SIZE, PVDEC_SCALE_DISPLAY_HEIGHT,
            psScalerConfig->ui32ReconHeight - 1);
    }
    else
#endif
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALED_DISPLAY_SIZE],
            MSVDX_CMDS, SCALED_DISPLAY_SIZE, SCALE_DISPLAY_WIDTH,
            psScalerConfig->ui32ReconWidth - 1);
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALED_DISPLAY_SIZE],
            MSVDX_CMDS, SCALED_DISPLAY_SIZE, SCALE_DISPLAY_HEIGHT,
            psScalerConfig->ui32ReconHeight - 1);
    }

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_OUTPUT_SIZE],
        MSVDX_CMDS, SCALE_OUTPUT_SIZE, SCALE_OUTPUT_WIDTH_MIN1,
        psScalerConfig->ui32ScaleWidth - 1);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_OUTPUT_SIZE],
        MSVDX_CMDS, SCALE_OUTPUT_SIZE, SCALE_OUTPUT_HEIGHT_MIN1,
        psScalerConfig->ui32ScaleHeight - 1);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_HORIZONTAL_SCALE_CONTROL],
        MSVDX_CMDS, HORIZONTAL_SCALE_CONTROL, HORIZONTAL_SCALE_PITCH,
        psParams->iHorzPitch);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_HORIZONTAL_SCALE_CONTROL],
        MSVDX_CMDS, HORIZONTAL_SCALE_CONTROL, HORIZONTAL_INITIAL_POS,
        psParams->iHorzStartPos);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_HORIZONTAL_CHROMA],
        MSVDX_CMDS, SCALE_HORIZONTAL_CHROMA, CHROMA_HORIZONTAL_PITCH,
        psParams->iHorzPitchChroma);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_HORIZONTAL_CHROMA],
        MSVDX_CMDS, SCALE_HORIZONTAL_CHROMA, CHROMA_HORIZONTAL_INITIAL,
        psParams->iHorzStartPosChroma);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_VERTICAL_SCALE_CONTROL],
        MSVDX_CMDS, VERTICAL_SCALE_CONTROL, VERTICAL_SCALE_PITCH,
        psParams->iVertPitch);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_VERTICAL_SCALE_CONTROL],
        MSVDX_CMDS, VERTICAL_SCALE_CONTROL, VERTICAL_INITIAL_POS,
        psParams->iVertStartPos);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_VERTICAL_CHROMA],
        MSVDX_CMDS, SCALE_VERTICAL_CHROMA, CHROMA_VERTICAL_PITCH, psParams->iVertPitchChroma);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_SCALE_VERTICAL_CHROMA],
        MSVDX_CMDS, SCALE_VERTICAL_CHROMA, CHROMA_VERTICAL_INITIAL, psParams->iVertStartPosChroma);

    return IMG_SUCCESS;
}



/*
******************************************************************************

 @Function              msvdx_GetProfile

 @Description

 Obtains the hardware defined video profile.

 @Input     eVidStd :   Width of the reference picture

 @Input     ui32StdProfile :   Height of the reference picture

 @Return    IMG_UINT32 : Video Profile as defined by hardware.

******************************************************************************/
static IMG_UINT32
msvdx_GetProfile(
    VDEC_eVidStd        eVidStd,
    IMG_UINT32          ui32StdProfile
)
{
    IMG_UINT32 ui32Profile = 0;

    switch (eVidStd)
    {
#ifdef HAS_AVS
        case VDEC_STD_AVS:
            ui32Profile = 0;
            break;
#endif /* HAS_AVS */
#ifdef HAS_VC1
        case VDEC_STD_VC1:
            if (ui32StdProfile == VC1_PROFILE_SIMPLE)
            {
                ui32Profile = 0;
            }
            else if (ui32StdProfile == VC1_PROFILE_MAIN)
            {
                ui32Profile = 1;
            }
            else if (ui32StdProfile == VC1_PROFILE_ADVANCED)
            {
                ui32Profile = 2;
            }
            else
            {
                IMG_ASSERT(IMG_FALSE);
            }
            break;
#endif /* HAS_VC1 */

#ifdef HAS_H264
        case VDEC_STD_H264:
            switch (ui32StdProfile)
            {
            case H264_PROFILE_BASELINE:
                ui32Profile = 0;
                break;

            /* Extended may be attempted as Baseline or Main depending on the constraint_set_flags */
            case H264_PROFILE_EXTENDED:
            case H264_PROFILE_MAIN:
                ui32Profile = 1;
                break;

            case H264_PROFILE_HIGH:
            case H264_PROFILE_HIGH444:
            case H264_PROFILE_HIGH422:
            case H264_PROFILE_HIGH10:
            case H264_PROFILE_CAVLC444:
            case H264_PROFILE_MVC_HIGH:
            case H264_PROFILE_STEREO_HIGH:
                ui32Profile = 2;
                break;

            default:
                ui32Profile = 2;
                IMG_ASSERT(IMG_FALSE);
                break;
            }
            break;
#endif /* HAS_H264 */
#ifdef HAS_VP6
        case VDEC_STD_VP6:
            ui32Profile = 0;
            break;
#endif /* HAS_VP6 */
#ifdef HAS_VP8
        case VDEC_STD_VP8:
            ui32Profile = ui32StdProfile;
            break;
#endif /* HAS_VP8 */
#ifdef HAS_MPEG2
        case VDEC_STD_MPEG2:
            ui32Profile = 1;
            break;
#endif /* HAS_MPEG2 */
        default:
            ui32Profile = 0;
            break;
    }

    return ui32Profile;
}


/*!
******************************************************************************

 @Function              MSVDX_SetReconPictCmds

******************************************************************************/
IMG_RESULT
MSVDX_SetReconPictCmds(
    const VDECDD_sStrUnit         * psStrUnit,
    const VDEC_sStrConfigData     * psStrConfigData,
    const VDEC_sStrOutputConfig   * psOutputConfig,
    const MSVDX_sCoreProps        * p,
    const MSVDX_sBuffers          * psBuffers,
    IMG_UINT32                    * pui32PictCmds
)
{
    PIXEL_sPixelInfo  * psPixelInfo;
    IMG_UINT32          ui32RowStrideCode;
    IMG_UINT32          ui32CacheRefOffset;
    IMG_UINT32          ui32CacheRowOffset;
    IMG_BOOL            bEnableAuxLineBuf = IMG_TRUE;

    IMG_UINT32          ui32CodedHeight;
    IMG_UINT32          ui32CodedWidth;
    IMG_UINT32          ui32DisplayHeight;
    IMG_UINT32          ui32DisplayWidth;

#if 0
    TRANSLATION_sRprParams    sRprParams;
    IMG_BOOL                  bRPR = IMG_FALSE;
#endif

    // Ensure that a reconstructed buffer has been provided.
    IMG_ASSERT(psBuffers->psReconPict);

    // Display and coded picture size commands are not considered by H/W when decoding JPEG.
    // Fields of VDECFW_CMD_DISPLAY_PICTURE and VDECFW_CMD_CODED_PICTURE commands are 12bit,
    // and JPEG size can be up to 32k, so it wouldn't fit. Set those sizes to 0 to avoid assert.
#ifdef HAS_JPEG
    if (psStrConfigData->eVidStd == VDEC_STD_JPEG)
    {
        ui32DisplayHeight = 0;
        ui32DisplayWidth = 0;
        ui32CodedHeight = 0;
        ui32CodedWidth = 0;
    }
    else
#endif /* HAS_JPEG */
    {
        ui32CodedHeight = VDEC_ALIGN_SIZE(psStrUnit->psPictHdrInfo->sCodedFrameSize.ui32Height,
                                          (psStrUnit->psPictHdrInfo->bField) ? 2*VDEC_MB_DIMENSION : VDEC_MB_DIMENSION);
        ui32CodedHeight -= 1;   // Hardware field is coded size - 1

        ui32CodedWidth = VDEC_ALIGN_SIZE(psStrUnit->psPictHdrInfo->sCodedFrameSize.ui32Width, VDEC_MB_DIMENSION);
        ui32CodedWidth -= 1;    // Hardware field is coded size - 1

        ui32DisplayHeight = psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Height +  psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32TopOffset - 1;
        ui32DisplayWidth = psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Width + psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32LeftOffset - 1;
    }

    // Display picture size (DISPLAY_PICTURE)
    //The display to be written is not the actual video size to be displayed but a number that has to differ from
    //the coded pixel size by less than 1MB (coded_size-display_size <= 0x0F). Because H264 can have a different
    //display size, we need to check and write the coded_size again in the display_size register if this condition
    //is not fulfilled.
    // For HEVC "PVDEC Display Picture Size" is used

    if (psStrConfigData->eVidStd != VDEC_STD_HEVC)
    {
        if( VDEC_STD_VC1 != psStrConfigData->eVidStd && ( ( ui32CodedHeight - ui32DisplayHeight ) > 0x0F ))
        {
            REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_DISPLAY_PICTURE],
                    MSVDX_CMDS, DISPLAY_PICTURE_SIZE, DISPLAY_PICTURE_HEIGHT,
                    ui32CodedHeight);
        }
        else
        {
            REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_DISPLAY_PICTURE],
                    MSVDX_CMDS, DISPLAY_PICTURE_SIZE, DISPLAY_PICTURE_HEIGHT,
                    ui32DisplayHeight);
        }

        if( VDEC_STD_VC1 != psStrConfigData->eVidStd && ( ( ui32CodedWidth - ui32DisplayWidth ) > 0x0F))
        {
            REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_DISPLAY_PICTURE],
                    MSVDX_CMDS, DISPLAY_PICTURE_SIZE, DISPLAY_PICTURE_WIDTH,
                    ui32CodedWidth);
        }
        else
        {
            REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_DISPLAY_PICTURE],
                    MSVDX_CMDS, DISPLAY_PICTURE_SIZE, DISPLAY_PICTURE_WIDTH,
                    ui32DisplayWidth);
        }

        // Coded picture size (CODED_PICTURE), for HEVC "PVDEC Coded Picture Size" is used
        REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_CODED_PICTURE],
            MSVDX_CMDS, CODED_PICTURE_SIZE, CODED_PICTURE_HEIGHT,
            ui32CodedHeight);
        REGIO_WRITE_FIELD_LITE(pui32PictCmds[VDECFW_CMD_CODED_PICTURE],
            MSVDX_CMDS, CODED_PICTURE_SIZE, CODED_PICTURE_WIDTH,
            ui32CodedWidth);
    }

    // Real video might have reference picture resampling
    if (psStrConfigData->eVidStd == VDEC_STD_REAL)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXTENDED_ROW_STRIDE],
            MSVDX_CMDS, EXTENDED_ROW_STRIDE, RPR_ROW_STRIDE,
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_Y].ui32Stride >> 6);
    }


    // Cache Configuration (MC_CACHE_CONFIGURATION)

    if (p->bNewCacheSettings)
    {
/*      #ifdef VDEC_USE_PVDEC_COMPATIBILITY
        ui32CacheRefOffset = 0x3C0; //Taken from DEVA
        ui32CacheRowOffset = 0xC; //Taken from DEVA
*/
        ui32CacheRefOffset = MSVDX_CACHE_REF_OFFSET_V550;
        ui32CacheRowOffset = MSVDX_CACHE_ROW_OFFSET_V550;
    }
    else
    {
        ui32CacheRefOffset = MSVDX_CACHE_REF_OFFSET_V100;
        ui32CacheRowOffset = MSVDX_CACHE_ROW_OFFSET_V100;
    }
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_MC_CACHE_CONFIGURATION],
        MSVDX_CMDS, MC_CACHE_CONFIGURATION, CONFIG_REF_OFFSET,
        ui32CacheRefOffset);
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_MC_CACHE_CONFIGURATION],
        MSVDX_CMDS, MC_CACHE_CONFIGURATION, CONFIG_ROW_OFFSET,
        ui32CacheRowOffset);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
        MSVDX_CMDS, OPERATING_MODE, CODEC_MODE,
        aui32MsvdxCodecMode[psStrConfigData->eVidStd]);
    if (psStrConfigData->eVidStd == VDEC_STD_REAL)
    {
        if (psStrUnit->psSequHdrInfo->sComSequHdrInfo.ui32CodecProfile == RV_VIDEO_30)
        {
            REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE], MSVDX_CMDS, OPERATING_MODE, CODEC_MODE, 0x8);
        }
        if (psStrUnit->psSequHdrInfo->sComSequHdrInfo.ui32CodecProfile == RV_VIDEO_40)
        {
            REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE], MSVDX_CMDS, OPERATING_MODE, CODEC_MODE, 0x9);
        }
    }

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
        MSVDX_CMDS, OPERATING_MODE, CODEC_PROFILE,
        msvdx_GetProfile(psStrConfigData->eVidStd, psStrUnit->psSequHdrInfo->sComSequHdrInfo.ui32CodecProfile));

    psPixelInfo = &psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo;
    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
        MSVDX_CMDS, OPERATING_MODE, CHROMA_FORMAT,
        psPixelInfo->bChromaFormat);

    if (psStrConfigData->eVidStd != VDEC_STD_JPEG)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXT_OP_MODE],
                MSVDX_CMDS, EXT_OP_MODE, CHROMA_FORMAT_IDC,
                PIXEL_GET_HW_CHROMA_FORMAT_IDC(psPixelInfo->eChromaFormatIdc));

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXT_OP_MODE],
            MSVDX_CMDS, EXT_OP_MODE, MEMORY_PACKING,
            psPixelInfo->eMemoryPacking == PIXEL_BIT10_MP ? 1 : 0);

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXT_OP_MODE],
            MSVDX_CMDS, EXT_OP_MODE, BIT_DEPTH_LUMA_MINUS8,
            psPixelInfo->ui32BitDepthY - 8);

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXT_OP_MODE],
            MSVDX_CMDS, EXT_OP_MODE, BIT_DEPTH_CHROMA_MINUS8,
            psPixelInfo->ui32BitDepthC - 8);
    }
    else
    {
        pui32PictCmds[VDECFW_CMD_EXT_OP_MODE] = 0;
    }

    if (psStrConfigData->eVidStd != VDEC_STD_JPEG)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, CHROMA_INTERLEAVED,
            PIXEL_GET_HW_CHROMA_INTERLEAVED(psPixelInfo->eChromaInterleaved));
    }
#ifdef HAS_AVS
    if (psStrConfigData->eVidStd == VDEC_STD_AVS)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE, VDEC_MSVDX_ASYNC_NORMAL);
    }
    else
#endif /* HAS_AVS */
#ifdef HAS_REAL
    if (psStrConfigData->eVidStd == VDEC_STD_REAL)
    {
            REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, DEBLOCK_STRENGTH,
            0);
    }
    else
#endif /* HAS_REAL */
#ifdef HAS_MPEG4
    if (psStrConfigData->eVidStd == VDEC_STD_MPEG4 ||
        psStrConfigData->eVidStd == VDEC_STD_H263 ||
        psStrConfigData->eVidStd == VDEC_STD_SORENSON)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE, VDEC_MSVDX_ASYNC_VDMC);
    }
    else
#endif /* HAS_MPEG4 */
#ifdef HAS_VP6
    if (psStrConfigData->eVidStd == VDEC_STD_VP6)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
        MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE, VDEC_MSVDX_ASYNC_VDMC)
    }
    else
#endif /* HAS_VP6 */
#ifdef HAS_VP8
    if (psStrConfigData->eVidStd == VDEC_STD_VP8)
    {
    }
    else
#endif /* HAS_VP8 */
#ifdef HAS_JPEG
    if (psStrConfigData->eVidStd == VDEC_STD_JPEG)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE, VDEC_MSVDX_ASYNC_VDMC)
    }
    else
#endif /* HAS_JPEG */
#ifdef HAS_MPEG2
    if (psStrConfigData->eVidStd == VDEC_STD_MPEG2)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE, VDEC_MSVDX_ASYNC_VDMC)
    }
    else
#endif /* HAS_MPEG2 */
#ifdef HAS_H264
    if (psStrConfigData->eVidStd == VDEC_STD_H264)
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ASYNC_MODE,
            psStrUnit->psPictHdrInfo->bDiscontinuousMbs ? VDEC_MSVDX_ASYNC_VDMC : VDEC_MSVDX_ASYNC_NORMAL)
    }
    else
#endif /* HAS_H264 */
    {};  /* Dummy, to allow for #ifdefs above. */

    // Stride (reconstructed buffer)
    if (p->bExtendedStrides && VDEC_IS_EXTENDED_STRIDE(psBuffers->psReconPict->sRendInfo))
    {
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, USE_EXT_ROW_STRIDE,
            1);

        // 64-byte (min) aligned luma stride value.
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_EXTENDED_ROW_STRIDE],
            MSVDX_CMDS, EXTENDED_ROW_STRIDE, EXT_ROW_STRIDE,
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_Y].ui32Stride >> 6);

        // 64-byte (min) aligned chroma stride value.
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_CHROMA_ROW_STRIDE],
            MSVDX_CMDS, CHROMA_ROW_STRIDE, CHROMA_ROW_STRIDE,
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_UV].ui32Stride >> 6);
    }
    else
    {
        // Obtain the code for buffer stride
        ui32RowStrideCode = VDECDDUTILS_GetStrideCode(
            psStrConfigData->eVidStd,
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_Y].ui32Stride);

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
            MSVDX_CMDS, OPERATING_MODE, ROW_STRIDE,
            ui32RowStrideCode & 0x7);

        if (psStrConfigData->eVidStd == VDEC_STD_JPEG)
        {
            // Use the unused chroma interleaved flag to hold MSB of row stride code
            IMG_ASSERT(ui32RowStrideCode < 16);
            REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_OPERATING_MODE],
                MSVDX_CMDS, OPERATING_MODE, CHROMA_INTERLEAVED,
                ui32RowStrideCode >> 3);
        }
        else
        {
            IMG_ASSERT(ui32RowStrideCode < 8);
        }
    }
    pui32PictCmds[VDECFW_CMD_LUMA_RECONSTRUCTED_PICTURE_BASE_ADDRESS] =
            (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psReconPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[0].ui32Offset;

    pui32PictCmds[VDECFW_CMD_CHROMA_RECONSTRUCTED_PICTURE_BASE_ADDRESS] =
            (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psReconPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[1].ui32Offset;

    pui32PictCmds[VDECFW_CMD_CHROMA2_RECONSTRUCTED_PICTURE_BASE_ADDRESS] =
            (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psReconPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
            psBuffers->psReconPict->sRendInfo.asPlaneInfo[2].ui32Offset;

    pui32PictCmds[VDECFW_CMD_INTRA_BUFFER_BASE_ADDRESS] = (IMG_UINT32)GET_HOST_ADDR(psBuffers->psIntraBufInfo, IMG_TRUE);
    pui32PictCmds[VDECFW_CMD_AUX_LINE_BUFFER_BASE_ADDRESS] = (IMG_UINT32)GET_HOST_ADDR(psBuffers->psAuxLineBufInfo, IMG_TRUE);

#ifdef HAS_VC1
    if (psStrConfigData->eVidStd == VDEC_STD_VC1)
    {
        pui32PictCmds[VDECFW_CMD_AUX_MSB_BUFFER] = (IMG_NULL == psBuffers->psBEBufInfo) ?
            0 : ((IMG_UINT32)(GET_HOST_ADDR(psBuffers->psBEBufInfo, IMG_TRUE))+VC1_MSB_BUFFER_OFFSET);
    }
#endif /* HAS_VC1 */

    if (p->bAuxLineBufSupported)
    {
#ifndef VDEC_USE_PVDEC
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
            MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, USE_AUX_LINE_BUF,
            bEnableAuxLineBuf ? 1 : 0);
#else
        // this is causing corruption in RV40 streams with scaling/rotation enabled on Coral, so setting to 0
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
            MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, USE_AUX_LINE_BUF,
            (bEnableAuxLineBuf && (psStrConfigData->eVidStd != VDEC_STD_REAL))? 1 : 0);
#endif
    }

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MSVDX_SetAltPictCmds

******************************************************************************/
IMG_RESULT
MSVDX_SetAltPictCmds(
    const VDECDD_sStrUnit       * psStrUnit,
    const VDEC_sStrConfigData   * psStrConfigData,
    const VDEC_sStrOutputConfig * psOutputConfig,
    const MSVDX_sCoreProps      * p,
    const MSVDX_sBuffers        * psBuffers,
    IMG_UINT32                  * pui32PictCmds
)
{
    IMG_UINT32    ui32RowStrideCode;
    IMG_BOOL      bReconWrite = IMG_TRUE;
    IMG_BOOL      bUseScalerBypass = IMG_FALSE;

    if(!psOutputConfig->bForceOold && !psOutputConfig->bScale &&
        psOutputConfig->eRotMode == VDEC_ROTATE_0 &&
        psOutputConfig->sPixelInfo.eChromaFormatIdc == psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo.eChromaFormatIdc)
    {
        // When supplied with an alternate output buffer and not using it for
        // transformation (e.g. scaling. rotating or up/down-sampling) signal
        // to use the scaler as a means of generating output with an arbitrary stride.
        if(VDEC_STD_VC1 == psStrConfigData->eVidStd)
        {
            if(psStrUnit->psSequHdrInfo->sComSequHdrInfo.bPostProcessing && !psStrUnit->psPictHdrInfo->bPostProcessing)
            {
                bUseScalerBypass = IMG_TRUE;
            }
        }
        else
        {
            bUseScalerBypass = IMG_TRUE;
        }
    }

    // Stride (Out-of-loop buffer)
    if (p->bExtendedStrides && VDEC_IS_EXTENDED_STRIDE(psBuffers->psAltPict->sRendInfo))
    {
        // Ensure that it's supported by the hardware.
        IMG_ASSERT(p->bExtendedStrides);

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
            MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, USE_EXT_ROT_ROW_STRIDE,
            1);

        // 64-byte (min) aligned luma stride value.
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
            MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, EXT_ROT_ROW_STRIDE,
            psBuffers->psAltPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_Y].ui32Stride >> 6);

        // 64-byte (min) aligned chroma stride value.
        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_CHROMA_ROW_STRIDE],
            MSVDX_CMDS, CHROMA_ROW_STRIDE, ALT_CHROMA_ROW_STRIDE,
            psBuffers->psAltPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_UV].ui32Stride >> 6);
    }
    else
    {
        // Obtain the code for buffer stride (must be less than 8, i.e. not JPEG strides)
        ui32RowStrideCode = VDECDDUTILS_GetStrideCode(
            psStrConfigData->eVidStd,
            psBuffers->psAltPict->sRendInfo.asPlaneInfo[VDEC_PLANE_VIDEO_Y].ui32Stride);
        IMG_ASSERT(ui32RowStrideCode < 8);

        REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
            MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, ROTATION_ROW_STRIDE,
            ui32RowStrideCode & 0x7);
    }

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, SCALE_INPUT_SIZE_SEL,
        (psOutputConfig->bScale ||
        (psOutputConfig->sPixelInfo.eChromaFormatIdc != psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo.eChromaFormatIdc))
        ? 1 : 0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, PACKED_422_OUTPUT,
        (psOutputConfig->sPixelInfo.eChromaFormatIdc == PIXEL_FORMAT_422 &&
         psOutputConfig->sPixelInfo.ui32NoPlanes == 1) ? 1 : 0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_CONTROL],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_CONTROL, ALT_OUTPUT_FORMAT,
        PIXEL_GET_HW_CHROMA_FORMAT_IDC(psOutputConfig->sPixelInfo.eChromaFormatIdc));


    if (bUseScalerBypass ||
        psOutputConfig->bScale ||
        (psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo.eChromaFormatIdc != psOutputConfig->sPixelInfo.eChromaFormatIdc))
    {
        SCALER_sConfig      sScalerConfig;

        // Must include cropped region in top-left since scaler must use this data.
        sScalerConfig.ui32ReconWidth = psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Width + psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32LeftOffset;
        sScalerConfig.ui32ReconHeight = psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Height + psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32TopOffset;

        // Adjust recon width to solve the odd width issue with HW. (applies to cores rev 3 and earlier)
        // May need to be disabled for later cores
        // This is BRN32651 so ...
        if (p->BRN32651)
        {
            sScalerConfig.ui32ReconWidth &= ~1;
        }

        sScalerConfig.ui32MbWidth = VDEC_ALIGN_SIZE(psStrUnit->psPictHdrInfo->sCodedFrameSize.ui32Width, VDEC_MB_DIMENSION);
        sScalerConfig.ui32MbHeight = VDEC_ALIGN_SIZE(psStrUnit->psPictHdrInfo->sCodedFrameSize.ui32Height,
                                            (psStrUnit->psPictHdrInfo->bField) ? 2*VDEC_MB_DIMENSION : VDEC_MB_DIMENSION);

        sScalerConfig.ui32ScaleWidth = psOutputConfig->bScale ?
            psOutputConfig->sScaledPictSize.ui32Width :
            psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Width;

        sScalerConfig.ui32ScaleHeight = psOutputConfig->bScale ?
            psOutputConfig->sScaledPictSize.ui32Height :
            psStrUnit->psPictHdrInfo->sDispInfo.sEncDispRegion.ui32Height;

        sScalerConfig.bFieldCoded = psStrUnit->psPictHdrInfo->bField;

        sScalerConfig.psInPixelInfo = &psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo;
        //IMG_ASSERT(psPictBuf->sDispPictRendInfo.ePixelFormat == psStrUnit->psSequHdrInfo->sComSequHdrInfo.ePixelFormat);

        sScalerConfig.psOutPixelInfo = &psOutputConfig->sPixelInfo;
        //IMG_ASSERT(psAltPictBuf->sDispPictRendInfo.ePixelFormat == psOutputConfig->ePixelFormat);

        sScalerConfig.psCoreProps = p;
        sScalerConfig.eVidStd = psStrConfigData->eVidStd;

        SCALER_GetScalerCmds(&sScalerConfig, pui32PictCmds);
    }


    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, ALT_PICTURE_ENABLE, 1);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, SCALE_ENABLE,
        (bUseScalerBypass ||
         psOutputConfig->bScale ||
         (psStrUnit->psSequHdrInfo->sComSequHdrInfo.sPixelInfo.eChromaFormatIdc != psOutputConfig->sPixelInfo.eChromaFormatIdc)) ? 1 : 0);

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, ROTATION_MODE,
        psOutputConfig->eRotMode);

    if (bUseScalerBypass || psOutputConfig->bScale || psOutputConfig->eRotMode != VDEC_ROTATE_0)
    {
        bReconWrite = IMG_TRUE;
    }

    if ((bUseScalerBypass || psOutputConfig->bScale) &&
        psStrConfigData->eVidStd == VDEC_STD_VC1 && p->BRN31777 && psStrUnit->psPictHdrInfo->bPostProcessing)
    {
        bReconWrite =  IMG_TRUE;
    }

    if (psBuffers->bTwoPass)
    {
        bReconWrite = IMG_TRUE;
    }

    REGIO_WRITE_FIELD(pui32PictCmds[VDECFW_CMD_ALTERNATIVE_OUTPUT_PICTURE_ROTATION],
        MSVDX_CMDS, ALTERNATIVE_OUTPUT_PICTURE_ROTATION, RECON_WRITE_DISABLE,
        bReconWrite ? 0 : 1);


    pui32PictCmds[VDECFW_CMD_LUMA_ALTERNATIVE_PICTURE_BASE_ADDRESS] = (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psAltPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
                                                            psBuffers->psAltPict->sRendInfo.asPlaneInfo[0].ui32Offset;

    pui32PictCmds[VDECFW_CMD_CHROMA_ALTERNATIVE_PICTURE_BASE_ADDRESS] = (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psAltPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
                                                            psBuffers->psAltPict->sRendInfo.asPlaneInfo[1].ui32Offset;

    pui32PictCmds[VDECFW_CMD_CHROMA2_ALTERNATIVE_PICTURE_BASE_ADDRESS] = (IMG_UINT32)GET_HOST_ADDR(&psBuffers->psAltPict->psPictBuf->sDdBufInfo, IMG_TRUE) +
                                                            psBuffers->psAltPict->sRendInfo.asPlaneInfo[2].ui32Offset;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MSVDX_DeInitialise

******************************************************************************/
IMG_RESULT
MSVDX_DeInitialise(
    IMG_HANDLE          hVxd
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32          ui32Result = IMG_SUCCESS;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
    /* Reset the MTX-MSVDX first. */
    /* When running against CSim, we don't have to reset the core when deinitializing. */
    if(!m->bCSim)
    {
        ui32Result = msvdx_CoreReset(hVxd);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }
    }

#ifndef SYSBRG_BRIDGING
    ui32Result = TALINTERRUPT_UnRegisterCheckFunc(m->hSysRegSpace);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif /* not SYSBRG_BRIDGING */

    // Disable the clocks
#ifdef SECURE_MEDIA_SUPPORT
    {
        VXD_sDisableClocksArgs  sDisableClocksArgs;

        //Using Secure Interface
        ui32Result = SECURE_SendMessage(m->hSecure,
                                        (IMG_BYTE *)&sDisableClocksArgs,
                                        sizeof(sDisableClocksArgs),
                                        ENDPOINT_VXD_DISABLE_CLOCKS);
    }
#else
    /*if CSim clocks are already disabled */
    if(!m->bCSim)
    {
    ui32Result = VXDIO_DisableClocks(m->hMsvdxIo);
    }
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif /* VDEC_MSVDX_HARDWARE */

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDX_GetMmuTileConfig

******************************************************************************/
IMG_RESULT MSVDX_GetMmuTileConfig(
    VDEC_eTileScheme        eTileScheme,
    IMG_BOOL                b128ByteInterleaved,
    VDECFW_sMmuTileConfig * psMmuTileConfig
)
{
    //printk("====eTileScheme is %d , b128ByteInterleaved is %d  ,psMmuTileConfig  %p\n",eTileScheme,b128ByteInterleaved,psMmuTileConfig);
    IMG_RESULT              ui32Result;
    MMU_sHeapTilingInfo     asTilingInfo[MMU_HEAP_MAX];
    IMG_UINT32              ui32NumTiledHeaps;
    IMG_UINT32              i;
    IMG_BOOL                bTiling = (eTileScheme == VDEC_TS_NONE) ? IMG_FALSE : IMG_TRUE;

    VDEC_BZERO(psMmuTileConfig);
    //printk("====MSVDX_GetMmuTileConfig    %s   %d\n",__FUNCTION__,__LINE__);
    // Tile scehem. This is a field inside control2 mmu register (defaults to 0)
    // and will be written in the firmware when updating tile config
    psMmuTileConfig->ui8TilingScheme = (eTileScheme == VDEC_TS1_512x8) ? 1 : 0;
    if(bTiling)
    {
    //printk("====MSVDX_GetMmuTileConfig    %s   %d\n",__FUNCTION__,__LINE__);

        ui32Result = MMU_GetTiledHeaps(eTileScheme, &ui32NumTiledHeaps, asTilingInfo);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }
    //printk("====MSVDX_GetMmuTileConfig    %s   %d\n",__FUNCTION__,__LINE__);

        if(ui32NumTiledHeaps > MSVDX_CORE_CR_MMU_TILE_NO_ENTRIES)
        {
            return IMG_ERROR_GENERIC_FAILURE;
        }

        for (i = 0; i < ui32NumTiledHeaps; i++)
        {
            IMG_UINT32 ui32Reg = 0;

            REGIO_WRITE_FIELD(ui32Reg,
                MSVDX_CORE, CR_MMU_TILE, CR_TILE_CFG,
                (b128ByteInterleaved << 0x4) |(bTiling << 0x3) | asTilingInfo[i].ui32HwTileStride);
            REGIO_WRITE_FIELD(ui32Reg,
                MSVDX_CORE, CR_MMU_TILE, CR_TILE_MAX_ADDR,
                (asTilingInfo[i].ui32StartOffset + asTilingInfo[i].ui32Size - 1) >> 20);
            REGIO_WRITE_FIELD(ui32Reg,
                MSVDX_CORE, CR_MMU_TILE, CR_TILE_MIN_ADDR,
                asTilingInfo[i].ui32StartOffset >> 20);

            psMmuTileConfig->aui32MmuTiling[i] = ui32Reg;
        }
    }

    return IMG_SUCCESS;
}



#ifdef USE_MTX_BREAK_TRAP

/*
******************************************************************************

 @Function              msvdx_SetMTXBreakTrap

 @Description

 This function is used to enable/disable MTX breakpoint trap.
 When trap is enabled, it is signalled to MTX with VDECFW_sFirmwareState
 in VLR RAM, that it should enter NOP loop just after starting. MTX breakpoints
 can be set then. When trap is disabled, MTX jump out of the loop and starts
 normal operation.
 Trap can be enabled only before starting MTX and can be disabled only once
 after MTX is started. Can not be used in any other circumstances.

 @Input     psContext :     A pointer to the device context.

 @Input     bEnable   :     IMG_TRUE to enable trap, IMG_FALSE to tell MTX to
                            jump out of the trap loop.


******************************************************************************/
static IMG_VOID
msvdx_SetMTXBreakTrap(
    MSVDX_sContext    * psContext,
    IMG_BOOL            bEnable
)
{
    IMG_UINT32 ui32Result = IMG_SUCCESS;
    IMG_UINT32 ui32Val = bEnable ? VDECFE_FWACT_BREAK_TRAP : 0;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sVlrWriteArgs   sVlrWriteArgs;
    sVlrWriteArgs.ui32MemRegion = (IMG_UINT32)REGION_VLRFE_REGSPACE;
    sVlrWriteArgs.ui32Addr = VLR_FIRMWARE_STATE_AREA_BASE_ADDR + offsetof(VDECFW_sFirmwareState, eFirmwareAction);
    sVlrWriteArgs.ui32NumWords = sizeof(((VDECFW_sFirmwareState *)0)->eFirmwareAction)>>2;
    sVlrWriteArgs.pui32Values = &ui32Val;

    ui32Result = SECURE_SendMessage(psContext->hSecure,
                                    &sVlrWriteArgs,
                                    sizeof(sVlrWriteArgs),
                                    ENDPOINT_VXD_VLR_WRITE);
#else
    ui32Result = VXDIO_VLRWriteWords(psContext->hMsvdxIo,
                                     REGION_VLRFE_REGSPACE,
                                     VLR_FIRMWARE_STATE_AREA_BASE_ADDR + offsetof(VDECFW_sFirmwareState, eFirmwareAction),
                                     sizeof(((VDECFW_sFirmwareState *)0)->eFirmwareAction)>>2,
                                     &ui32Val);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
}

#endif /* USE_MTX_BREAK_TRAP */



/*!
******************************************************************************

 @Function              MSVDX_ReadVLR

******************************************************************************/
IMG_RESULT MSVDX_ReadVLR(
    const IMG_HANDLE    hVxd,
    IMG_UINT32          ui32Offset,
    IMG_UINT32        * pui32,
    IMG_UINT32          ui32Words
)
{
    MSVDX_sContext * psCtx = (MSVDX_sContext *)hVxd;
    IMG_UINT32 ui32Result = IMG_SUCCESS;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sVlrReadArgs sArgs;

    sArgs.ui32MemRegion = (IMG_UINT32)REGION_VLRFE_REGSPACE;
    sArgs.ui32Addr = ui32Offset;
    sArgs.pui32Values = pui32;
    sArgs.ui32NumWords = ui32Words;
    ui32Result = SECURE_SendMessage(psCtx->hSecure,
                                    (IMG_BYTE *)&sArgs,
                                    sizeof(sArgs),
                                    ENDPOINT_VXD_VLR_READ);
#else
    ui32Result = VXDIO_VLRReadWords(psCtx->hMsvdxIo,
                                    REGION_VLRFE_REGSPACE,
                                    ui32Offset,
                                    ui32Words,
                                    pui32,
                                    IMG_TRUE);
#endif
    return ui32Result;
}

/*!
******************************************************************************

 @Function              MSVDX_WriteVLR

******************************************************************************/
IMG_RESULT MSVDX_WriteVLR(
    const IMG_HANDLE    hVxd,
    IMG_UINT32          ui32Offset,
    IMG_UINT32        * pui32,
    IMG_UINT32          ui32Words
)
{

    MSVDX_sContext * psCtx = (MSVDX_sContext *)hVxd;
    IMG_UINT32 ui32Result = IMG_SUCCESS;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sVlrWriteArgs sArgs;
    sArgs.ui32MemRegion = (IMG_UINT32)REGION_VLRFE_REGSPACE;
    sArgs.pui32Values = pui32;
    sArgs.ui32Addr = ui32Offset;
    sArgs.ui32NumWords = ui32Words;

    ui32Result = SECURE_SendMessage(psCtx->hSecure,
                                    (IMG_BYTE *)&sArgs,
                                    sizeof(sArgs),
                                    ENDPOINT_VXD_VLR_WRITE);
#else
    ui32Result = VXDIO_VLRWriteWords(psCtx->hMsvdxIo,
                                     REGION_VLRFE_REGSPACE,
                                     ui32Offset,
                                     ui32Words,
                                     pui32);
#endif

    return ui32Result;

}
/*
******************************************************************************

 @Function              msvdx_DumpRegSpace

******************************************************************************/
static IMG_RESULT
msvdx_DumpRegSpace(
    const MSVDX_sContext  * m,
    IMG_UINT32              ui32Start,
    IMG_UINT32              ui32End,
    const IMG_CHAR        * pszRegSpaceName,
    MEM_REGIONS             eMemRegion
)
{
    IMG_UINT32 i;
    IMG_UINT32 ui32Result = IMG_SUCCESS;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    IMG_UINT8 ui8NumRegs = ((ui32End - ui32Start) / 4);
    READNREGS * psRegRead = (READNREGS *)IMG_MALLOC(ui8NumRegs * sizeof(READNREGS));
    IMG_MEMSET(psRegRead, 0, ui8NumRegs * sizeof(READNREGS));
    if(IMG_NULL != psRegRead)
    {
        for (i = ui32Start; i < ui32End; i += 4)
        {
            //Populating the offset list
            psRegRead[i/4].ui32Offset = i;
            psRegRead[i/4].ui8Pipe = 0;
        }

        ui32Result = SECURE_ReadNRegs(m->hSecure,
                                      ui8NumRegs,
                                      eMemRegion,
                                      psRegRead);

        for (i = ui32Start; i < ui32End; i += 4)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                   "%s [0x%04X]: 0x%08X",
                   pszRegSpaceName,
                   i,
                   psRegRead[i/4].ui32Result);
        }
        IMG_FREE(psRegRead);
    }
    else
    {
        ui32Result = IMG_ERROR_MALLOC_FAILED;
    }
#else
    for (i = ui32Start; i < ui32End; i += 4)
    {
        ui32Result = VXDIO_ReadRegister(m->hMsvdxIo,
                                        eMemRegion,
                                        i,
                                        MSVDX_DEF_WR_PIPE);

        REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                     "%s [0x%04X]: 0x%08X",
                     pszRegSpaceName,
                     i,
                     ui32Result);
    }
#endif

    return ui32Result;
}


/*
******************************************************************************

 @Function              MSVDX_DumpRegisters

******************************************************************************/
IMG_RESULT MSVDX_DumpRegisters(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
    VXDIO_sState        sState;
    IMG_UINT32          ui32Result;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE

    // MSVDX Core
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nMSVDX Core Register Space 0x0600 - 0x06FF");
    msvdx_DumpRegSpace(m, 0, 0x0100, "MSVDX Core", REGION_VXDSYS_REGSPACE);

    // MTX Registers
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nMTX Register Space 0x0000 - 0x0354");
    msvdx_DumpRegSpace(m, 0, 0x0354, "MTX", REGION_VXDMTX_REGSPACE);

    ui32Result = MSVDX_GetCoreState(hVxd, &sState);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }

    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nCORE %d STATUS\n", m->ui32CoreNum);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "==========");
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX PC:  0x%08X", sState.sEPRuntimeStatus.ui32MTX_PC);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX PCX: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_PCX);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX Stack Pointer: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_A0StP);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX Frame Pointer: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_A0FrP);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX STATUS BITS: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_STATUS_BITS);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX FAULT0: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_FAULT0);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX ENABLE: 0x%08X", sState.sEPRuntimeStatus.ui32MTX_ENABLE);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "MTX STATUS END");
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "==============");

    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nDMAC STATUS");
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "===========");
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC SETUP[0]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_SETUP[0]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC SETUP[1]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_SETUP[1]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC SETUP[2]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_SETUP[2]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC COUNT[0]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_COUNT[0]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC COUNT[1]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_COUNT[1]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC COUNT[2]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_COUNT[2]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC PERIPHERAL ADDR[0]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[0]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC PERIPHERAL ADDR[1]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[1]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC PERIPHERAL ADDR[2]: 0x%08X", sState.sEPRuntimeStatus.ui32DMA_PERIPHERAL_ADDR[2]);
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "DMAC STATUS END");
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "===============");

    // VEC (excluding 0x98: rendec data)
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nVEC Register Space 0x0800 - 0x0FFF");
    msvdx_DumpRegSpace(m, 0, 0x98, "VEC", REGION_VEC_REGSPACE);
    msvdx_DumpRegSpace(m, 0x9c, 0x0100, "VEC", REGION_VEC_REGSPACE);

    // VDMC
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nVDMC Register Space 0x0400 - 0x047F");
    msvdx_DumpRegSpace(m, 0, 0x58, "VDMC", REGION_VDMC_REGSPACE);

    // VDEB
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nVDEB Register Space 0x0480 - 0x04FF");
    msvdx_DumpRegSpace(m, 0, 0x50, "VDEB", REGION_VDEB_REGSPACE);

    // DMAC
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nDMAC Register Space 0x0500 - 0x05FF");
    msvdx_DumpRegSpace(m, 0, 0x0100, "DMAC", REGION_DMAC_REGSPACE);

    // VLR
    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE, "\nVLR Register Space 0x2000 - 0x2FFF");
    msvdx_DumpRegSpace(m, 0, 0x01000, "VLR", REGION_VLR_REGSPACE);

    // Also dump RTM stuff for hardware debugging
    {
        IMG_CHAR * apszModuleName[] = {"CORE", "VEC", "VDMC", "VDEB"};
        IMG_UINT32 x, y, z;
        IMG_UINT32 ui32Select, ui32Val;
        const IMG_UINT32 ui32ModASigMask = 0x00000007;
        const IMG_UINT32 ui32ModBSigMask = 0x00000038;

        REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,  "\nRTM Dump");
        for (x=0; x<4; x++)
        {
            // select module A/B
            ui32Select = (x<<8) | (x<<6);

            for (y=0; y<8; y++)
            {
                // select module B signals
                ui32Select &= ~ui32ModBSigMask;
                ui32Select |= (y<<3);

                for (z=0; z<8; z++)
                {
                    // select module A signals
                    ui32Select &= ~ui32ModASigMask;
                    ui32Select |= z;

                    //TAL_WriteWord(i, j, ui32Select);
                    //TAL_ReadWord(i, j, &ui32Val);
                    msvdx_RtmWriteRead(m, ui32Select, &ui32Val);
                    IMG_ASSERT(apszModuleName[(ui32Select>>6)&0x3] == apszModuleName[(ui32Select>>8)&0x3]);
                    REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                                 "MSVDX RTM: Select[%08x] Value[%08x] # RTM %s, A[0x%01X]:0x%02X & B[0x%01X]:0x%02X",
                                 ui32Select, ui32Val, apszModuleName[(ui32Select>>6)&0x3],
                                 z, (ui32Val>>16)&0xff, y, (ui32Val>>24)&0xff);
                }
            }
        }
    }

#endif /* VDEC_MSVDX_HARDWARE */

    return IMG_SUCCESS;

error:
    return ui32Result;
}



/*!
******************************************************************************

 @Function              MSVDX_GetCoreState

******************************************************************************/
IMG_RESULT
MSVDX_GetCoreState(
    const IMG_HANDLE    hVxd,
    VXDIO_sState      * psState
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32          ui32Result;

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sGetStateArgs   sGetStateArgs;
#endif

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    sGetStateArgs.psState = psState;
    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&sGetStateArgs,
                                    sizeof(sGetStateArgs),
                                    ENDPOINT_VXD_GET_STATE);
#else
    ui32Result = VXDIO_GetCoreState(m->hMsvdxIo,
                                    psState);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDX_FlushMmuCache

******************************************************************************/
IMG_RESULT
MSVDX_FlushMmuCache(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
#ifdef VDEC_MSVDX_HARDWARE
    IMG_UINT32          ui32Reg;
#endif
    IMG_UINT32          ui32Result = IMG_SUCCESS;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
    ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0);
    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_CONTROL0, CR_MMU_INVALDC, 1);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0, ui32Reg);

    REGIO_WRITE_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_CONTROL0, CR_MMU_INVALDC, 0);
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0, ui32Reg);
#endif /* VDEC_MSVDX_HARDWARE */

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDX_HandleInterrupts

******************************************************************************/
IMG_RESULT
MSVDX_HandleInterrupts(
    const IMG_HANDLE    hVxd,
    VXD_sIntStatus    * psIntStatus
)
{
    MSVDX_sContext            * m = (MSVDX_sContext *)hVxd;
#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    VXD_sHandleInterruptsArgs   sHandleInterruptsArgs;
#endif
    IMG_UINT32                  ui32Result = IMG_SUCCESS;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
    /* Disable interrupts */
    SYSOSKM_DisableInt();

#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    sHandleInterruptsArgs.psIntStatus = psIntStatus;
    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&sHandleInterruptsArgs,
                                    sizeof(sHandleInterruptsArgs),
                                    ENDPOINT_VXD_HANDLE_INTERRUPTS);
#else
    ui32Result = VXDIO_HandleInterrupts(m->hMsvdxIo,
                                        psIntStatus);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_inthandle;
    }

error_inthandle:
    /* Re enable interrupts */
    SYSOSKM_EnableInt();
#endif /* VDEC_MSVDX_HARDWARE */

error:
    return ui32Result;
}

/*!
******************************************************************************

 @Function              MSVDX_HandleInterrupts

******************************************************************************/
IMG_RESULT
MSVDX_GetIntStatus(
    const IMG_HANDLE    hVxd,
    VXD_sIntStatus    * psIntStatus
)
{
    IMG_UINT32        ui32Reg;
    IMG_UINT32        ui32Enable;
    IMG_UINT32        ui32Result = IMG_SUCCESS;
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    psIntStatus->ui32Pending = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERRUPT_STATUS);
    ui32Enable = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_HOST_INTERRUPT_ENABLE);

    psIntStatus->ui32Pending &= ui32Enable;

    if (psIntStatus->ui32Pending & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK)
    {
        ui32Reg = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_STATUS);
        psIntStatus->ui32Requestor = (psIntStatus->ui32Pending & MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_MASK)
                                            >> MSVDX_CORE_CR_MSVDX_INTERRUPT_STATUS_CR_MMU_FAULT_IRQ_SHIFT;
        psIntStatus->MMU_FAULT_ADDR = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_FAULT_ADDR) << 12 ;
        psIntStatus->MMU_FAULT_RNW = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_FAULT_RNW);
        psIntStatus->MMU_PF_N_RW = REGIO_READ_FIELD(ui32Reg, MSVDX_CORE, CR_MMU_STATUS, CR_MMU_PF_N_RW);
    }

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDX_SendFirmwareMessage

******************************************************************************/
IMG_RESULT
MSVDX_SendFirmwareMessage(
    const IMG_HANDLE        hVxd,
    VXD_eCommsArea          eArea,
    const IMG_VOID        * psMsgHdr
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32        ui32Result;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    m->sFwMsgArgs.eArea = eArea;
    m->sFwMsgArgs.psMsgHdr = psMsgHdr;
    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&m->sFwMsgArgs,
                                    sizeof(m->sFwMsgArgs),
                                    ENDPOINT_VXD_INPUT);
#else
    ui32Result = VXDIO_SendFirmwareMessage(m->hMsvdxIo,
                                           eArea,
                                           psMsgHdr);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif

    return IMG_SUCCESS;

error:
    return ui32Result;
}


#ifdef POST_TEST
/*!
******************************************************************************

 @Function              checkCoreRevision

 @Description

 This function checks the core revision. If the core is the expected one
 than return IMG_SUCCESS else IMG_ERROR_DEVICE_NOT_FOUND

******************************************************************************/
IMG_RESULT
checkCoreRevision(
    IMG_HANDLE hVxd
)
{
    IMG_RESULT ui32Result;
    IMG_UINT32 ui32Register;
    IMG_UINT32 ui32RegField;
    MSVDX_sContext *m = (MSVDX_sContext *)hVxd;

    /* Init result to PASSED */
    ui32Result = IMG_SUCCESS;

    /* Get Core Revision Information */
    ui32Register = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CORE_REV);
    /* Check if the register is different than 0xFFFFFFFF at least */
    if(INVALID_32BIT_REG_VALUE == ui32Register)
    {
        /* the test is FAILED */
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }

    /* Get MSVDX IMG Internal register */
    ui32Register = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_INTERNAL);
    /* Check if the register is different than 0xFFFFFFFF at least */
    if(INVALID_32BIT_REG_VALUE == ui32Register)
    {
        /* the test is FAILED */
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }

    /* Get MSVDX latency config */
    ui32Register = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_LATENCY_CONFIG);
    /* Check if the register is different than 0xFFFFFFFF at least */
    if(INVALID_32BIT_REG_VALUE == ui32Register)
    {
        /* the test is FAILED */
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }

    /* Get Core ID register */
    ui32Register = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MSVDX_CORE_ID);
    /* Get and check Group ID field */
    ui32RegField = REGIO_READ_FIELD(ui32Register, MSVDX_CORE, CR_MSVDX_CORE_ID, CR_GROUP_ID);
    /* Check the value */
    if(GROUP_ID_DEFAULT_VALUE != ui32RegField)
    {
        /* the test is FAILED */
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }
    /* Get and check Core ID field */
    ui32RegField = REGIO_READ_FIELD(ui32Register, MSVDX_CORE, CR_MSVDX_CORE_ID, CR_CORE_ID);
    /* Check the value */
    if(CORE_ID_DEFAULT_VALUE != ui32RegField)
    {
        /* the test is FAILED */
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }

    /* Return result value */
    return ui32Result;
}




/*!
******************************************************************************

 @Function              executeRegTest

 @Description

 This function executes POST registers test. It returns IMG_SUCCESS if
 test passed else IMG_ERROR_TEST_MISMATCH

******************************************************************************/
IMG_RESULT
executeRegTest(
    IMG_HANDLE hVxd
)
{
    IMG_UINT32 ui32RegValue;
    IMG_UINT32 ui32OldRegValue;
    IMG_RESULT ui32Result;
    MSVDX_sContext *m = (MSVDX_sContext *)hVxd;

    /* init test result to pass */
    ui32Result = IMG_SUCCESS;

    /* TEST ON MTX_ENABLE register */

    /* read the CR_MTX_SYSC_CDMAA register and store the default value */
    ui32OldRegValue = REGIO_READ_REGISTER(m, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST);
    /* check the read value with the default value */
    if(CR_MTX_REG_RW_REQ_DEF_VALUE != ui32OldRegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* reset register value to update: can be equal to old value */
    ui32RegValue = 0;
    /* write the test value to the writable register field */
    REGIO_WRITE_FIELD(ui32RegValue, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER, TEST_REG_FIELD_3BITS_VALUE);
    /* write the register */
    REGIO_WRITE_REGISTER(m, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32RegValue);

    /* read the register again */
    ui32RegValue = REGIO_READ_REGISTER(m, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST);
    /* read the register field */
    ui32RegValue = REGIO_READ_FIELD(ui32RegValue, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, MTX_RSPECIFIER);
    /* check the read value with the expected written value */
    if(TEST_REG_FIELD_3BITS_VALUE != ui32RegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* re-write the register with the default value */
    REGIO_WRITE_REGISTER(m, MTX_CORE, CR_MTX_REGISTER_READ_WRITE_REQUEST, ui32OldRegValue);




    /* TEST ON CR_VEC_RENDEC_CONTROL1 register */

    /* read the VEC_RENDEC_CONTROL1 register and store the default value */
    ui32OldRegValue = REGIO_READ_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1);
    /* check the read value with the default value */
    if(CR_VEC_RENDEC_CONTROL1_DEF_VALUE != ui32OldRegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* reset register value to update: can be equal to old value */
    ui32RegValue = 0;
    /* write the test value to the writable register field */
    REGIO_WRITE_FIELD(ui32RegValue, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_DECODE_START_SIZE, TEST_REG_FIELD_8BITS_VALUE);
    /* write the test value to the register */
    REGIO_WRITE_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, ui32RegValue);

    /* read the register again */
    ui32RegValue = REGIO_READ_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1);
    /* read the register field */
    ui32RegValue = REGIO_READ_FIELD(ui32RegValue, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, RENDEC_DECODE_START_SIZE);
    /* check the read value with the expected written value */
    if(TEST_REG_FIELD_8BITS_VALUE != ui32RegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: Do nothing */
    }

    /* re-write the register with the default value */
    REGIO_WRITE_REGISTER(m, MSVDX_VEC, CR_VEC_RENDEC_CONTROL1, ui32OldRegValue);




    /* TEST ON CR_MMU_BANK_INDEX register */

    /* read the CR_MMU_BANK_INDEX register and store the default value */
    ui32OldRegValue = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_BANK_INDEX);
    /* check the read value with the default value */
    if(CR_MMU_BANK_INDEX_DEF_VALUE != ui32OldRegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* reset register value to update: can be equal to old value */
    ui32RegValue = 0;
    /* write the test value to the writable register field */
    REGIO_WRITE_FIELD(ui32RegValue, MSVDX_CORE, CR_MMU_BANK_INDEX, CR_MMU_BANK_N_INDEX_M, TEST_REG_FIELD_2BITS_VALUE);
    /* write the test value to the register */
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_BANK_INDEX, ui32RegValue);

    /* read the register again */
    ui32RegValue = REGIO_READ_REGISTER(m, MSVDX_CORE, CR_MMU_BANK_INDEX);
    /* read the register field */
    ui32RegValue = REGIO_READ_FIELD(ui32RegValue, MSVDX_CORE, CR_MMU_BANK_INDEX, CR_MMU_BANK_N_INDEX_M);
    /* check the read value with the expected written value */
    if(TEST_REG_FIELD_2BITS_VALUE != ui32RegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: Do nothing */
    }

    /* re-write the register with the default value */
    REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_BANK_INDEX, ui32OldRegValue);




    /* TEST ON CR_VDMC_CHROMA_ERROR_BASE_ADDR register */

    /* read the CR_VDMC_CHROMA_ERROR_BASE_ADDR register and store the default value */
    ui32OldRegValue = REGIO_READ_REGISTER(m, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR);
    /* check the read value with the default value */
    if(CR_VDMC_2D_FLT_DATA_DEF_VALUE != ui32OldRegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* reset register value to update: can be equal to old value */
    ui32RegValue = 0;
    /* write the test value to the writable register field */
    REGIO_WRITE_FIELD(ui32RegValue, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR, CR_VDMC_CHROMA_ERROR_BASE, TEST_REG_FIELD_26BITS_VALUE);
    /* write the test value to the register */
    REGIO_WRITE_REGISTER(m, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR, ui32RegValue);

    /* read the register again */
    ui32RegValue = REGIO_READ_REGISTER(m, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR);

    /* read the register field */
    ui32RegValue = REGIO_READ_FIELD(ui32RegValue, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR, CR_VDMC_CHROMA_ERROR_BASE);
    /* check the read value with the expected written value */
    if(TEST_REG_FIELD_26BITS_VALUE != ui32RegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: Do nothing */
    }

    /* re-write the register with the default value */
    REGIO_WRITE_REGISTER(m, MSVDX_VDMC, CR_VDMC_CHROMA_ERROR_BASE_ADDR, ui32OldRegValue);




    /* TEST ON CR_VDEB_VIRTUAL_FIFO_CONTROL register */

    /* read the CR_VDEB_VIRTUAL_FIFO_CONTROL register and store the default value */
    ui32OldRegValue = REGIO_READ_REGISTER(m, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL);
    /* check the read value with the default value */
    if(CR_VDEB_VIRT_CTRL_DEF_VALUE != ui32OldRegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: do nothing */
    }

    /* reset register value to update: can be equal to old value */
    ui32RegValue = 0;
    /* write the test value to the writable register field */
    REGIO_WRITE_FIELD(ui32RegValue, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL, CR_VDEB_VF_FLOWRATE_DIV, TEST_REG_FIELD_8BITS_VALUE);
    /* write the test value to the register */
    REGIO_WRITE_REGISTER(m, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL, ui32RegValue);

    /* read the register again */
    ui32RegValue = REGIO_READ_REGISTER(m, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL);
    /* read the register field */
    ui32RegValue = REGIO_READ_FIELD(ui32RegValue, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL, CR_VDEB_VF_FLOWRATE_DIV);
    /* check the read value with the expected written value */
    if(TEST_REG_FIELD_8BITS_VALUE != ui32RegValue)
    {
        /* register test fail */
        ui32Result = IMG_ERROR_DEVICE_UNAVAILABLE;
    }
    else
    {
        /* register test passed: Do nothing */
    }

    /* re-write the register with the default value */
    REGIO_WRITE_REGISTER(m, MSVDX_VDEB, CR_VDEB_VIRTUAL_FIFO_CONTROL, ui32OldRegValue);

    return ui32Result;
}
#endif


#ifdef STACK_USAGE_TEST
/*!
******************************************************************************

 @Function              MSVDX_GetStackUsage

 @Description

 Function to get stack usage from MSVDXIO layer.

******************************************************************************/
IMG_RESULT
MSVDX_GetStackUsage(
    const IMG_HANDLE    hVxd,
    IMG_UINT32        * paui32StackInfo
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32        ui32Result = IMG_SUCCESS;

    /* call VXDIO function to get stack usage */
    ui32Result = VXDIO_GetStackUsage(m->hMsvdxIo, paui32StackInfo);

    return ui32Result;
}
#endif


/*!
******************************************************************************

 @Function              MSVDX_Initialise

******************************************************************************/
IMG_RESULT
MSVDX_Initialise(
    const IMG_HANDLE        hVxd,
    VDECFW_sCoreInitData  * psInitConfig
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32          ui32Result;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* Initialise the core. */
    ui32Result = msvdx_GetCoreInit(&m->sProps,
                                   &m->sRendec,
                                   psInitConfig);
    if (IMG_SUCCESS != ui32Result)
    {
        REPORT(REPORT_MODULE_HWCTRL,
                REPORT_ERR,
                "Failed initialising core %d",
                m->ui32CoreNum);
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
    // Reset MSVDX: the core may have been powered down.
    ui32Result = msvdx_CoreReset(hVxd);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }

#ifdef __FAKE_MTX_INTERFACE__
    /* If fake MTX is to be used... */
    if (m->bFakeMtx)
    {
        /* ...start it now. */
        ui32Result = VDECFW_StartFakeMTX(m->ui32CoreNum);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }
    }
#endif


#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    // Enable the clocks since the core could have been powered down.
    m->sEnableClocksArgs.bAutoClockGatingSupport = m->sProps.bAutoClockGatingSupport;
    m->sEnableClocksArgs.bExtClockGating = m->sProps.bExtClockGating;
    m->sEnableClocksArgs.bForceManual = IMG_FALSE;
    ui32Result = SECURE_SendMessage(m->hSecure,
                                    (IMG_BYTE *)&m->sEnableClocksArgs,
                                    sizeof(m->sEnableClocksArgs),
                                    ENDPOINT_VXD_ENABLE_CLOCKS);
#else
    ui32Result = VXDIO_EnableClocks(m->hMsvdxIo,
                                    m->sProps.bAutoClockGatingSupport,
                                    m->sProps.bExtClockGating,
                                    IMG_FALSE);
#endif

    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if(ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#ifdef POST_TEST
    if(IMG_TRUE == m->bPost)
    {
        /* Execute core ID test */
        ui32Result = checkCoreRevision(m);
        if(IMG_SUCCESS == ui32Result)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                "***** POST: Core ID Test PASSED");
        }
        else
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
                "***** POST: Core ID Test FAILED");
            goto error;
        }

#ifdef SEC_USE_REAL_FW
        /* Execute registers test */
        /* ATTENTION: this tests must not be performed with CSIM. Elsewhere the software remains blocked. */
        ui32Result = executeRegTest(m);
        if(IMG_SUCCESS == ui32Result)
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_NOTICE,
                "***** POST: Register Test PASSED");
        }
        else
        {
            REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
                "***** POST: Register Test FAILED");
            goto error;
        }
#endif

    }
    else
    {
        /* do nothing */
    }
#endif

#ifdef USE_REAL_FW
#ifdef USE_MTX_DMA_LOAD
#ifdef __FAKE_MTX_INTERFACE__
    if (!m->bFakeMtx)
#endif /* __FAKE_MTX_INTERFACE__ */
    {
        /* Configure the MMU to DMA the firmware base component. */
        REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL2, psInitConfig->sMmuConfig.ui32MmuControl2);
        /* Set base address of PTD */
#ifdef SYSBRG_BRIDGING
        REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_DIR_LIST_BASE, m->sPtdInfo.ui32PtdPhysAddr);
#else /* not SYSBRG_BRIDGING */
        TALINTVAR_WriteToReg32(m->hSysRegSpace,
                               MSVDX_CORE_CR_MMU_DIR_LIST_BASE_OFFSET,
                               m->sPtdInfo.hPtdMemSpace,
                               m->sPtdInfo.ui32IntRegNum);
#endif /* not SYSBRG_BRIDGING */
        /* Enable mmu for all requestors */
        REGIO_WRITE_REGISTER(m, MSVDX_CORE, CR_MMU_CONTROL0, 0);
    }
#endif /* USE_MTX_DMA_LOAD */
#endif /* USE_REAL_FW */


#ifdef USE_REAL_FW
#ifdef USE_MTX_BREAK_TRAP
    msvdx_SetMTXBreakTrap(m, IMG_TRUE);
#endif /* USE_MTX_BREAK_TRAP */
#endif

    /* Load the firmware base component. */
#ifdef SECURE_MEDIA_SUPPORT
    //Using Secure Interface
    ui32Result = SECURE_LoadCoreFW(m->hSecure);
#else
    ui32Result = VXDIO_LoadBaseFirmware(m->hMsvdxIo);
#endif
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        REPORT(REPORT_MODULE_HWCTRL, REPORT_ERR,
                "Failed loading firmware base component for core %d",
                m->ui32CoreNum);
        goto error;
    }
    else
    {
        REPORT(REPORT_MODULE_HWCTRL, REPORT_NOTICE,
                "Passed loading firmware base component for core %d",
                m->ui32CoreNum);
    }

#ifdef USE_REAL_FW
#ifdef USE_MTX_BREAK_TRAP
    // You can break on this function to set breakpoints in MTX.
    // Once it's executed, MTX will jump out of the break trap loop and begin
    // normal operation.
    msvdx_SetMTXBreakTrap(m, IMG_FALSE);
#endif /* USE_MTX_BREAK_TRAP */
#endif

#ifndef SYSBRG_BRIDGING
    ui32Result = TALINTERRUPT_RegisterCheckFunc(m->hSysRegSpace,
                                                msvdx_CheckInterruptFunc,
                                                m->hMsvdxIo);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif /* not SYSBRG_BRIDGING */
#endif /* VDEC_MSVDX_HARDWARE */

    return IMG_SUCCESS;

error:
    return ui32Result;
}



/*!
******************************************************************************

 @Function              MSVDX_PrepareFirmware

******************************************************************************/
IMG_RESULT
MSVDX_PrepareFirmware(
    const IMG_HANDLE            hVxd,
    const MSVDXIO_sFw         * psFw,
    const VXDIO_sPtdInfo      * psPtdInfo
)
{
    MSVDX_sContext    * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32          ui32Result;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    /* Prepare the firmware for loading to MTX. */
#ifdef VDEC_MSVDX_HARDWARE
#ifdef USE_REAL_FW
#ifdef SECURE_MEDIA_SUPPORT
    {
        AUTHENTICATIONKEY   psKeyFw = IMG_NULL;

        //Using Secure Interface
        ui32Result = SECURE_PrepareFirmware(m->hSecure,
                                            (IMG_BYTE *)psFw,
                                            0,
                                            VXD_CORE0 | VXD_CORE1,
                                            psKeyFw);
    }
#else
    ui32Result = VXDIO_PrepareFirmware(m->hMsvdxIo,
                                       (IMG_HANDLE)psFw);
#endif
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error;
    }
#endif /* USE_REAL_FW */
#endif /* VDEC_MSVDX_HARDWARE */

    // Copy the device PTD for power-up initialisation.
    m->sPtdInfo = *psPtdInfo;

    return IMG_SUCCESS;

error:
    return ui32Result;
}

/*!
******************************************************************************

 @Function              MSVDX_Destroy

******************************************************************************/
IMG_RESULT
MSVDX_Destroy(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;
    IMG_UINT32        ui32Result;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "An MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

#ifdef VDEC_MSVDX_HARDWARE
    if (m->eDecodeLevel > VDECDD_DECODELEVEL_DECODER)
    {
#ifdef SECURE_MEDIA_SUPPORT
        VXD_sDeInitialiseArgs   sDeInitialiseArgs;
#endif

        MSVDX_DeInitialise(hVxd);

        /* Attempt to initialise MSVDX if it is used. */
#ifdef __FAKE_MTX_INTERFACE__
        /* If fake MTX is used... */
        if (m->bFakeMtx)
        {
            /* ...stop it now. */
            ui32Result = VDECFW_StopFakeMTX(m->ui32CoreNum);
            if (ui32Result != IMG_SUCCESS)
            {
                goto error;
            }
        }
#endif

#ifdef SECURE_MEDIA_SUPPORT
        //Using Secure Interface
        ui32Result = SECURE_SendMessage(m->hSecure,
                                        (IMG_BYTE *)&sDeInitialiseArgs,
                                        sizeof(sDeInitialiseArgs),
                                        ENDPOINT_VXD_DEINITIALISE);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

        ui32Result = SECURE_ReleaseHandle(m->hSecure);
#else
        //Using Secure Interface
        ui32Result = VXDIO_DeInitialise(m->ui32CoreNum,
                                        m->hMsvdxIo);
#endif
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }
    }
#endif

    IMG_FREE(m);

    return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              MSVDX_Create

******************************************************************************/
IMG_RESULT
MSVDX_Create(
    const VDECDD_sDdDevConfig * psDdDevConfig,
    IMG_UINT32                  ui32CoreNum,
    MSVDX_sCoreProps          * psCoreProps,
    HWCTRL_sRendec            * psRendec,
    IMG_HANDLE                * phMsvdxContext,
    HWCTRL_sFunctions         * psVxdFuncs
)
{
    MSVDX_sContext    * m;
    IMG_UINT32          ui32Result;

    IMG_ASSERT(psCoreProps);
    IMG_ASSERT(psRendec);

    if (phMsvdxContext == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A pointer to MSVDX context handle must be provided");
        ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        goto error;
    }

    VDEC_MALLOC(m);
    IMG_ASSERT(m);
    if(m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
            "Failed to allocate memory for MSVDX context");
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    VDEC_BZERO(m);
    *phMsvdxContext = m;

    m->ui32CoreNum = ui32CoreNum;
    m->bFakeMtx = psDdDevConfig->bFakeMtx;
    m->eDecodeLevel = psDdDevConfig->eDecodeLevel;
    m->bCSim = psDdDevConfig->bCSim;
#ifdef POST_TEST
    /* update POST flag */
    m->bPost = psDdDevConfig->bPost;
#endif
#ifdef STACK_USAGE_TEST
    /* update STACK USAGE TEST flag */
    m->bStackUsageTest = psDdDevConfig->bStackUsageTest;
#endif

#ifdef VDEC_MSVDX_HARDWARE
    if (psDdDevConfig->eDecodeLevel > VDECDD_DECODELEVEL_DECODER)
    {
#ifdef SECURE_MEDIA_SUPPORT
        VXD_sInitialiseArgs     sInitialiseArgs;

        IMG_MEMSET(&sInitialiseArgs, 0, sizeof(sInitialiseArgs));

        //Using Secure Interface
        switch (ui32CoreNum)
        {
        case 0:
            m->eSecureCore = VXD_CORE0;
            break;

        case 1:
            m->eSecureCore = VXD_CORE1;
            break;

        default:
            IMG_ASSERT(IMG_FALSE);
            break;
        }

        ui32Result = SECURE_GetHandle(m->eSecureCore, &m->hSecure);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);

        /* Attempt to initialise MSVDX if it is used. */
#ifdef __FAKE_MTX_INTERFACE__
        sInitialiseArgs.bFakeMtx = psDdDevConfig->bFakeMtx;
#endif
        sInitialiseArgs.eDeviceType = VXD_DEV_MSVDX;
        //Using Secure Interface
        ui32Result = SECURE_SendMessage(m->hSecure,
                                        (IMG_BYTE *)&sInitialiseArgs,
                                        sizeof(sInitialiseArgs),
                                        ENDPOINT_VXD_INITIALISE);
#else
#ifdef POST_TEST
#ifdef STACK_USAGE_TEST
        ui32Result = VXDIO_Initialise(psDdDevConfig->bFakeMtx,
                                      psDdDevConfig->bPost,
                                      psDdDevConfig->bStackUsageTest,
                                      ui32CoreNum,
                                      VXD_DEV_MSVDX,
                                      &m->hMsvdxIo);
#else
        ui32Result = VXDIO_Initialise(psDdDevConfig->bFakeMtx,
                                      psDdDevConfig->bPost,
                                      IMG_FALSE,
                                      ui32CoreNum,
                                      VXD_DEV_MSVDX,
                                      &m->hMsvdxIo);
#endif
#else
#ifdef STACK_USAGE_TEST
        ui32Result = VXDIO_Initialise(psDdDevConfig->bFakeMtx,
                                      IMG_FALSE,
                                      psDdDevConfig->bStackUsageTest,
                                      ui32CoreNum,
                                      VXD_DEV_MSVDX,
                                      &m->hMsvdxIo);
#else
        ui32Result = VXDIO_Initialise(psDdDevConfig->bFakeMtx,
                                      IMG_FALSE,
                                      IMG_FALSE,
                                      ui32CoreNum,
                                      VXD_DEV_MSVDX,
                                      &m->hMsvdxIo);
#endif
#endif
#endif
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

        ui32Result = msvdx_GetCoreProperties(m, &m->sProps);
        if (ui32Result != IMG_SUCCESS)
        {
            goto error;
        }

        if (psDdDevConfig->ui32RendecSize)
        {
            /* Create two buffers with a 4:1 split both multiples of 4k */
            m->sRendec.aui32BufferSize[1] = psDdDevConfig->ui32RendecSize / 5;
            m->sRendec.aui32BufferSize[1] &= ~0xfff;
            m->sRendec.aui32BufferSize[0] = psDdDevConfig->ui32RendecSize - m->sRendec.aui32BufferSize[1];
            m->sRendec.aui32BufferSize[0] &= ~0xfff;

            m->sRendec.aui32InitialContext[0] = 0x00101010;
            m->sRendec.aui32InitialContext[1] = 0x00101010;
            m->sRendec.aui32InitialContext[2] = 0x00101010;
            m->sRendec.aui32InitialContext[3] = 0x00101010;
            m->sRendec.aui32InitialContext[4] = 0x00101010;
            m->sRendec.aui32InitialContext[5] = 0x00101010;

            m->sRendec.bInUse = IMG_TRUE;
        }

        if (psDdDevConfig->ui32MemoryStallingMin &&
            psDdDevConfig->ui32MemoryStallingMax &&
            psDdDevConfig->ui32MemoryStallingRatio)
        {
            IMG_UINT32 ui32Reg;

            IMG_ASSERT(psDdDevConfig->ui32MemoryStallingMin <= psDdDevConfig->ui32MemoryStallingMax);
            IMG_ASSERT(psDdDevConfig->ui32MemoryStallingRatio < 10);

            // Configure MSVDX memory stalling with the min, max and ratio of accesses.
            // Ratio: Percentage of transactions to stall (in 10% steps, i.e. takes values 0-10).
            ui32Reg = REGIO_READ_REGISTER(m, MSVDX_TEST, MEM_CTRL);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_LATENCY_MIN,
                              psDdDevConfig->ui32MemoryStallingMin);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_LATENCY_MAX,
                              psDdDevConfig->ui32MemoryStallingMax);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_STALL_RATIO,
                              psDdDevConfig->ui32MemoryStallingRatio);
            REGIO_WRITE_REGISTER(m, MSVDX_TEST, MEM_CTRL, ui32Reg);
        }
        /*else
        {
            // Disable memory stalling if not defined.
            ui32Reg = REGIO_READ_REGISTER(m, MSVDX_TEST, MEM_CTRL);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_LATENCY_MIN, 0);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_LATENCY_MAX, 0);
            REGIO_WRITE_FIELD(ui32Reg, MSVDX_TEST, MEM_CTRL, MEM_STALL_RATIO, 0);
            REGIO_WRITE_REGISTER(m, MSVDX_TEST, MEM_CTRL, ui32Reg);
        }*/
    }
    else
#endif /* #ifdef VDEC_MSVDX_HARDWARE */
    {
        DBGOPT_sValue sVal;
        DBGOPT_eType eType;

        if (DBGOPTKM_GetSize("CoreConfig", DBGOPT_TYPE_BUF) == sizeof(m->sProps) &&
            DBGOPTKM_Get("CoreConfig", &eType, &sVal))
        {
            m->sProps = *((MSVDX_sCoreProps *) sVal.sBufVal.pvVal);

            DEBUG_REPORT(REPORT_MODULE_HWCTRL,
                         "Setting core configuration from debug options");
        }
        else
        {
            VDEC_eVidStd eVidStd;

            m->sProps.bH264 = IMG_TRUE;
            m->sProps.bHdSupport = IMG_TRUE;
            m->sProps.bRotationSupport = IMG_TRUE;
            m->sProps.bScalingSupport = IMG_TRUE;
            m->sProps.bScalingWithOold = IMG_TRUE;
            m->sProps.bChromaUpsample = IMG_TRUE;
            m->sProps.b64ByteFixedStrides = IMG_TRUE;
            m->sProps.bExtendedStrides = IMG_TRUE;

            if (ui32CoreNum == VDECDD_MSVDX_CORE_MASTER)
            {
                m->sProps.bMpeg2 = IMG_TRUE;
                m->sProps.bMpeg4 = IMG_TRUE;
                m->sProps.bVc1 = IMG_TRUE;
                m->sProps.bAvs = IMG_TRUE;
                m->sProps.bReal = IMG_TRUE;
                m->sProps.bJpeg = IMG_TRUE;
                m->sProps.bVp6 = IMG_TRUE;
                m->sProps.bVp8 = IMG_TRUE;
            }

            m->sProps.ui32MaxBitDepthLuma = 10;
            m->sProps.ui32MaxBitDepthChroma = 10;

            for (eVidStd = VDEC_STD_UNDEFINED; eVidStd < VDEC_STD_MAX; eVidStd++)
            {
                m->sProps.aui32MaxWidth[eVidStd] = 4096;
                m->sProps.aui32MaxHeight[eVidStd] = 4096;
            }

            m->sProps.b8bitHighColour = IMG_TRUE;
            m->sProps.ui32NumCores = 1;
            m->sProps.ui32NumStreams = 1;
            m->sProps.eMmuType = MMU_TYPE_40BIT;
        }
    }

    HWCTRL_PrintMSVDXProps(&m->sProps, m->ui32CoreNum);

#ifndef SYSBRG_BRIDGING
    {
        IMG_CHAR    acMemSpceName[32];

        // Create the memory space name prefix based upon the core number.
        sprintf(acMemSpceName, "C%d_", ui32CoreNum);
        strncat(acMemSpceName, "REG_MSVDX_SYS", 16);
        m->hSysRegSpace = TAL_GetMemSpaceHandle(acMemSpceName);
    }
#endif

    *psCoreProps = m->sProps;
    *psRendec = m->sRendec;

    psVxdFuncs->pfnReadVLR = MSVDX_ReadVLR;
    psVxdFuncs->pfnWriteVLR = MSVDX_WriteVLR;
    psVxdFuncs->pfnHandleInt = MSVDX_HandleInterrupts;
    psVxdFuncs->pfnGetIntStatus = MSVDX_GetIntStatus;
    psVxdFuncs->pfnDumpRegisters = MSVDX_DumpRegisters;
    psVxdFuncs->pfnSendFirmwareMessage = MSVDX_SendFirmwareMessage;
    psVxdFuncs->pfnGetCoreState = MSVDX_GetCoreState;
    psVxdFuncs->pfnFlushMmuCache = MSVDX_FlushMmuCache;
    psVxdFuncs->pfnInitialise = MSVDX_Initialise;
    psVxdFuncs->pfnDeInitialise = MSVDX_DeInitialise;
    psVxdFuncs->pfnDestroy = MSVDX_Destroy;
    psVxdFuncs->pfnPrepareFirmware = MSVDX_PrepareFirmware;
    psVxdFuncs->pfnGetRegsOffsets = IMG_NULL;
    psVxdFuncs->pfnGetInitMessage = MSVDX_GetInitMessage;
    psVxdFuncs->pfnGetDecodeMessage = MSVDX_GetDecodeMessage;
    psVxdFuncs->pfnGetBatchMessage = MSVDX_GetBatchMessage;
    psVxdFuncs->pfnGetEndBytesMessage = MSVDX_GetEndBytesMessage;
    psVxdFuncs->pfnGetFragmentMessage = MSVDX_GetFragmentMessage;
    psVxdFuncs->pfnReturnMsg = MSVDX_ReturnMsg;
    psVxdFuncs->pfnProcessMessage = MSVDX_ProcessMessage;
#ifndef IMG_KERNEL_MODULE
    psVxdFuncs->pfnPDUMPLock = MSVDX_PDUMPLock;
    psVxdFuncs->pfnPDUMPUnlock = MSVDX_PDUMPUnLock;
    psVxdFuncs->pfnPDUMPSync = MSVDX_PDUMPSync;
#endif
    return IMG_SUCCESS;

error:

    if(m)
    {
        IMG_FREE(m);
    }
    return ui32Result;
}

#ifndef IMG_KERNEL_MODULE
/*!
******************************************************************************

 @Function              MSVDX_PDUMPSync

******************************************************************************/
IMG_RESULT MSVDX_PDUMPSync(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A pointer to MSVDX context handle must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return VXDIO_PDUMPSync(m->hMsvdxIo);
}
/*!
******************************************************************************

 @Function              MSVDX_PDUMPLock

******************************************************************************/
IMG_RESULT MSVDX_PDUMPLock(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A pointer to MSVDX context handle must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return VXDIO_PDUMPLock(m->hMsvdxIo);
}


/*!
******************************************************************************

 @Function              MSVDX_PDUMPUnLock

******************************************************************************/
IMG_RESULT MSVDX_PDUMPUnLock(
    const IMG_HANDLE  hVxd
)
{
    MSVDX_sContext  * m = (MSVDX_sContext *)hVxd;;

    if (m == IMG_NULL)
    {
        REPORT(REPORT_MODULE_VXDIO, REPORT_ERR,
               "A pointer to MSVDX context handle must be provided");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return VXDIO_PDUMPUnLock(m->hMsvdxIo);
}

#endif

/*!
******************************************************************************

 @Function              MSVDX_GetCodedPicSize

******************************************************************************/
IMG_UINT32 MSVDX_GetCodedPicSize(
    IMG_UINT16   ui16WidthMin1,
    IMG_UINT16   ui16HeightMin1
)
{
    IMG_UINT32 ui32Reg = 0;

    REGIO_WRITE_FIELD_LITE(ui32Reg, MSVDX_CMDS, CODED_PICTURE_SIZE,
                          CODED_PICTURE_WIDTH, ui16WidthMin1);
    REGIO_WRITE_FIELD_LITE(ui32Reg, MSVDX_CMDS, CODED_PICTURE_SIZE,
                           CODED_PICTURE_HEIGHT, ui16HeightMin1);

    return ui32Reg;
}


/* EOF */
