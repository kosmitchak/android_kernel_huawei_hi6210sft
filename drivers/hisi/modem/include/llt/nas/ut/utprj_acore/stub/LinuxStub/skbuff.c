/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : skbuff.c
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2011年08月17日
  最近修改   :
  功能描述   :
  函数列表   :

  修改历史   :
  1.日    期   : 2011年08月17日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/slab.h>
#include <linux/kmemcheck.h>
#include <linux/slab_def.h>
#include <linux/gfp.h>
#include "DrvInterface.h"
#include "PsLog.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
SK_BUFF_CLUSTER_CFG_INFO_STRU              g_astaCpuUserDataBlkClusterTableInfo[] =
{
    {SKB_DATA_ALIGN(1500)+sizeof(struct skb_shared_info), MAXUSERDATABLOCK},     /*具体待定*/
};
DATA_CLUSTER_STRU  stUserDataCluster;
DATA_CLUSTER_STRU  *g_pstUserDataCluster;

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : __alloc_skb
 功能描述  : allocate a network buffer
 输入参数  : unsigned int size,gfp_t gfp_mask,int fclone,int node
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : 修改Linux内核
*****************************************************************************/
struct sk_buff *__alloc_skb(unsigned int size, gfp_t gfp_mask, int fclone, int node)
{
    struct kmem_cache *cache;
    struct skb_shared_info *shinfo;
    struct sk_buff *skb;
    u8 *data;

    /**/
    cache = fclone ? skbuff_fclone_cache : skbuff_head_cache;

    /* Get the HEAD */
    skb = kmem_cache_alloc_node(cache, gfp_mask & ~__GFP_DMA, node);
    if (!skb)
        goto out;
    prefetchw(skb);

    size = SKB_DATA_ALIGN(size);
    data = kmalloc_node_track_caller(size + sizeof(struct skb_shared_info),
            gfp_mask, node);
    if (!data)
        goto nodata;
    prefetchw(data + size);

    /*
     * Only clear those fields we need to clear, not those that we will
     * actually initialise below. Hence, don't put any more fields after
     * the tail pointer in struct sk_buff!
     */

    /*Linux add begin，caikai-191211 */
    skb->private_mem.enType     = DEFAULT_DEFINED;
    skb->private_mem.unRetaddr.pSkBuffAddr   = (VOS_VOID*)skb;
    /*0xFF????*/
    skb->private_mem.ucUserId               = INVALID_RABID_VALUE;
    /*Linux add end，caikai-191211*/

    memset(skb, 0, offsetof(struct sk_buff, tail));
    skb->truesize = size + sizeof(struct sk_buff);
    atomic_set(&skb->users, 1);
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
  	skb->end = skb->tail + size;
    kmemcheck_annotate_bitfield(skb, flags1);
    kmemcheck_annotate_bitfield(skb, flags2);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
    skb->mac_header = ~0U;
#endif

    /* make sure we initialize shinfo sequentially */
    shinfo = skb_shinfo(skb);
    memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
    atomic_set(&shinfo->dataref, 1);

    if (fclone) {
        struct sk_buff *child = skb + 1;
        atomic_t *fclone_ref = (atomic_t *) (child + 1);

        kmemcheck_annotate_bitfield(child, flags1);
        kmemcheck_annotate_bitfield(child, flags2);
        skb->fclone = SKB_FCLONE_ORIG;
        atomic_set(fclone_ref, 1);

        child->fclone = SKB_FCLONE_UNAVAILABLE;
    }
out:
    return skb;
nodata:
    kmem_cache_free(cache, skb);
    skb = NULL;
    goto out;
}

/*****************************************************************************
 函 数 名  :__alloc_skb_exp
 功能描述  :allocate a network buffer(Type类型为USER_DEFINED)
 输入参数  :unsigned int size,gfp_t gfp_mask,int fclone,int node
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
struct sk_buff *__alloc_skb_exp(unsigned int size, gfp_t gfp_mask,
                int fclone, int node)
{
    struct kmem_cache *cache;
    struct skb_shared_info *shinfo;
    struct sk_buff *skb;
    u8 *data;

    cache = fclone ? skbuff_fclone_cache : skbuff_head_cache;

    /* Get the HEAD */
    skb = kmem_cache_alloc_node(cache, gfp_mask & ~__GFP_DMA, node);
    if (!skb)
        goto out;
    prefetchw(skb);

    size = SKB_DATA_ALIGN(size);

    /*
     * Only clear those fields we need to clear, not those that we will
     * actually initialise below. Hence, don't put any more fields after
     * the tail pointer in struct sk_buff!
     */

    /* Linux BEGIN */
    data   = SkBuffGetUserDataBlk(size + sizeof(struct skb_shared_info));
    if(!data)
    {
        goto nodata;
    }

    /* 挂接从用户数据区获得的数据块 */
    prefetchw(data + size);

    /* ===========赋值私有结构============ */
    /* 数据块来自用户区 */
    skb->private_mem.enType =  USER_DEFINED;
    /* 返回地址指向数据块 */
    skb->private_mem.unRetaddr.pDataAddr = (VOS_VOID*)data;

    skb->private_mem.ucUserId            = INVALID_RABID_VALUE;

    memset(skb, 0, offsetof(struct sk_buff, tail));
    skb->truesize   = size + sizeof(struct sk_buff);
    atomic_set(&skb->users, 1);
    skb->head       = data;
    skb->data       = data;
    skb_reset_tail_pointer(skb);
    skb->end        = skb->tail + size;
    kmemcheck_annotate_bitfield(skb, flags1);
    kmemcheck_annotate_bitfield(skb, flags2);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
skb->mac_header = ~0U;
#endif

    /* make sure we initialize shinfo sequentially */
    shinfo = skb_shinfo(skb);
    memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
    atomic_set(&shinfo->dataref, 1);

    if (fclone) {
    struct sk_buff *child = skb + 1;
    atomic_t *fclone_ref = (atomic_t *) (child + 1);

    kmemcheck_annotate_bitfield(child, flags1);
    kmemcheck_annotate_bitfield(child, flags2);
    skb->fclone = SKB_FCLONE_ORIG;
    atomic_set(fclone_ref, 1);

    child->fclone = SKB_FCLONE_UNAVAILABLE;
}

    /* Linux END */
out:
    return skb;
nodata:
    kmem_cache_free(cache, skb);
    skb = NULL;
    goto out;
}

/*****************************************************************************
 函 数 名  :alloc_skb_exp
 功能描述  :allocate a network buffer(Type类型为USER_DEFINED)
 输入参数  :unsigned int size,gfp_t priority
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
static inline struct sk_buff*alloc_skb_exp(unsigned int size,gfp_t priority)
{
    return __alloc_skb_exp(size,priority,0,-1);
}

/*****************************************************************************
 函 数 名  :__dev_alloc_skb_exp
 功能描述  :allocate an skbuff for receiving(Type类型为USER_DEFINED)
 输入参数  :unsigned int length,gfp_t gfp_mask
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
static inline struct sk_buff*__dev_alloc_skb_exp(unsigned int len,gfp_t gfp_mask)
{
#ifdef WIFI_USED
    /*预留WIFI头部和尾部*/
    struct sk_buff *skb= alloc_skb_exp(len + WIFI_SKB_PAD + NET_SKB_PAD,gfp_mask);
    if (likely(skb))
    {
        skb_reserve(skb,WIFI_SKB_PAD/2 + NET_SKB_PAD);
    }
    return skb;
#else
    struct sk_buff *skb= alloc_skb_exp(len + NET_SKB_PAD,gfp_mask);
    if (likely(skb))
    {
        skb_reserve(skb,NET_SKB_PAD);
    }
    return skb;

#endif
}

/*****************************************************************************
 函 数 名  :dev_alloc_skb_exp
 功能描述  :allocate an skbuff for receiving(Type类型为USER_DEFINED)
 输入参数  :unsigned int length
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
inline struct sk_buff *dev_alloc_skb_exp(unsigned int len)
{
    return __dev_alloc_skb_exp(len,GFP_ATOMIC);
}

/*****************************************************************************
 函 数 名  :__alloc_skb_exp
 功能描述  :allocate a network buffer(Type类型为EXT_DEFINED)
 输入参数  :unsigned char *pData,unsigned int size,gfp_t gfp_mask,int fclone,int node
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
  1.日    期   : 2011年8月20日
    作    者   : yinweidong
    修改内容   : Modified


*****************************************************************************/
struct sk_buff *__alloc_skb_ext( IMC_DATA_STRU *pImcData,int len,gfp_t gfp_mask,
                int fclone, int node)
{
    unsigned int size;
    struct kmem_cache *cache;
    struct skb_shared_info *shinfo;
    struct sk_buff *skb;
    u8 *data;

    cache = fclone ? skbuff_fclone_cache : skbuff_head_cache;

    /* Get the HEAD */
    skb = kmem_cache_alloc_node(cache, gfp_mask & ~__GFP_DMA, node);
    if (!skb)
        goto out;
    prefetchw(skb);
    size = SKB_DATA_ALIGN(pImcData->usDataLen + len);
    /* Linux BEGIN */

    /*
     * Only clear those fields we need to clear, not those that we will
     * actually initialise below. Hence, don't put any more fields after
     * the tail pointer in struct sk_buff!
     */


    /* ===============挂接外部数据块========================*/
    data                                = pImcData->pDataAddr;
    if (!data)
        goto nodata;
//  data                                = DRV_PHY_TO_VIRT(data);

    prefetchw(data + size);
    skb->private_mem.enType             = EXT_DEFINED;

    memset(skb, 0, offsetof(struct sk_buff, tail));
    skb->truesize                       = size + sizeof(struct sk_buff);
    atomic_set(&skb->users, 1);

    /* head和data都指向CCPU提供的数据首地址 */
    skb->head                           = data;
    skb->data                           = data;
    skb_reset_tail_pointer(skb);
    skb->end                            = skb->tail + size;
    kmemcheck_annotate_bitfield(skb, flags1);
    kmemcheck_annotate_bitfield(skb, flags2);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
    skb->mac_header                     = ~0U;
#endif

    /* make sure we initialize shinfo sequentially */
    shinfo                              = skb_shinfo(skb);
    memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
    atomic_set(&shinfo->dataref, 1);

    if (fclone) {
        struct sk_buff *child = skb + 1;
        atomic_t *fclone_ref = (atomic_t *) (child + 1);

        kmemcheck_annotate_bitfield(child, flags1);
        kmemcheck_annotate_bitfield(child, flags2);
        skb->fclone = SKB_FCLONE_ORIG;
        atomic_set(fclone_ref, 1);

        child->fclone = SKB_FCLONE_UNAVAILABLE;
    }
out:
    return skb;
nodata:
    kmem_cache_free(cache, skb);
    skb = NULL;
    goto out;
}

/*****************************************************************************
 函 数 名  :alloc_skb_exp
 功能描述  :allocate a network buffer(Type类型为EXT_DEFINED)
 输入参数  :unsigned char *pData,unsigned int size,gfp_t priority
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
static inline struct sk_buff*alloc_skb_ext(IMC_DATA_STRU *pImcData,int len,gfp_t priority)
{
    return __alloc_skb_ext(pImcData,len,priority,0,-1);
}

/*****************************************************************************
 函 数 名  :__dev_alloc_skb_exp
 功能描述  :allocate an skbuff for receiving(Type类型为EXT_DEFINED)
 输入参数  :unsigned char *pData,unsigned int length,gfp_t gfp_mask
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
static inline struct sk_buff*__dev_alloc_skb_ext(IMC_DATA_STRU *pImcData,gfp_t gfp_mask)
{
#ifdef WIFI_USED
    struct sk_buff *skb= alloc_skb_ext(pImcData,WIFI_SKB_PAD+NET_SKB_PAD, gfp_mask);
    if (likely(skb))
    {
        skb_reserve(skb,WIFI_SKB_PAD/2 + NET_SKB_PAD);
    }
#else
    struct sk_buff *skb= alloc_skb_ext(pImcData,NET_SKB_PAD,gfp_mask);
    if (likely(skb))
    {
        skb_reserve(skb,NET_SKB_PAD);
    }
#endif
    return skb;
}

/*****************************************************************************
 函 数 名  :dev_alloc_skb_ext
 功能描述  :allocate an skbuff for receiving(Type类型为EXT_DEFINED)
 输入参数  :unsigned char *pData,unsigned int length
 输出参数  : 无
 返 回 值  : sk_buff*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
inline struct sk_buff *dev_alloc_skb_ext(IMC_DATA_STRU *pImcData)
{
    return __dev_alloc_skb_ext(pImcData,GFP_ATOMIC);
}

/*****************************************************************************
 函 数 名  : SkBuffUserDataBlkInit
 功能描述  : SkBuffUserDataBlk总初始化函数
 输入参数  : VOS_UINT8 **ppucDataMemAddr
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年08月12日
    作    者   : 殷卫东
    修改内容   : 新生成函数

*****************************************************************************/

/*考虑4M内存如何划分????*/
VOS_UINT32 SkBuffUserDataBlkInit(VOS_UINT8 **ppucDataMemAddr)
{
    VOS_UINT32                          ulSkbuffCnt;

    g_pstUserDataCluster              = &stUserDataCluster;
    stUserDataCluster.apstFreeStack   = ppucDataMemAddr;
    stUserDataCluster.usFreeCnt       = g_astaCpuUserDataBlkClusterTableInfo[0].usCnt;
    stUserDataCluster.usTotalCnt      = g_astaCpuUserDataBlkClusterTableInfo[0].usCnt;
    stUserDataCluster.usLen           = g_astaCpuUserDataBlkClusterTableInfo[0].usSize ;

    for ( ulSkbuffCnt = 0; ulSkbuffCnt < g_astaCpuUserDataBlkClusterTableInfo[0].usCnt; ulSkbuffCnt++ )
    {
       stUserDataCluster.UsrDataAddr[ulSkbuffCnt] = (VOS_UINT8*)SKB_DATA_ALIGN((VOS_UINT32)*ppucDataMemAddr);
       *ppucDataMemAddr                           = g_astaCpuUserDataBlkClusterTableInfo[0].usSize + stUserDataCluster.UsrDataAddr[ulSkbuffCnt] ;
    }
    return PS_SUCC;
}

/*****************************************************************************
 函 数 名  : SkBuffGetUserDataBlk
 功能描述  : 从用户数据区获得数据块
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年08月12日
    作    者   : 殷卫东
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID *SkBuffGetUserDataBlk(VOS_INT32 lLen)
{
    VOS_VOID                           *pMem;
    DATA_CLUSTER_STRU                  *pstUserDataCluster;
    VOS_INT32                           lLockKey;

    pstUserDataCluster          = g_pstUserDataCluster;

    if ( pstUserDataCluster->usLen  < lLen )
    {
        return VOS_NULL_PTR;
    }

    lLockKey = VOS_SplIMP();
    if ( 0 != pstUserDataCluster->usFreeCnt )
    {
        pMem    = (VOS_VOID*)pstUserDataCluster->apstFreeStack[--pstUserDataCluster->usFreeCnt];
    }
    else
    {
        pMem = VOS_NULL_PTR;
    }
    VOS_Splx(lLockKey);
    return pMem;
}

/*****************************************************************************
 函 数 名  : SkBuffRelUserDataBlk
 功能描述  : 数据块放回用户数据区
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年08月12日
    作    者   : 殷卫东
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID SkBuffRelUserDataBlk(VOS_VOID *pMem)
{
    VOS_INT32       lLockKey;
    DATA_CLUSTER_STRU                  *pstUserDataCluster;
    if( !pMem )
    {
        PS_LOG(UEPS_PID_RNIC, 0, PS_PRINT_WARNING,
                "sk_buff, SkBuffRelUserDataBlk : ERROR : pMem is NULL!" );
        return;
    }
    pstUserDataCluster          = g_pstUserDataCluster;
    lLockKey = VOS_SplIMP();

    pstUserDataCluster->apstFreeStack[pstUserDataCluster->usFreeCnt]  = (VOS_UINT8*)pMem;
    pstUserDataCluster->usFreeCnt++;
    VOS_Splx(lLockKey);
    return;
}

/*****************************************************************************
 函 数 名  : skb_release_data
 功能描述  : Free an sk_buff. Release anything attached to the buffer.
 输入参数  : struct sk_buff *skb
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月9日
    作    者   : 殷卫东
    修改内容   : 修改
*****************************************************************************/
static void skb_release_data(struct sk_buff *skb)
{
    if (!skb->cloned ||
        !atomic_sub_return(skb->nohdr ? (1 << SKB_DATAREF_SHIFT) + 1 : 1,
                   &skb_shinfo(skb)->dataref)) {
        if (skb_shinfo(skb)->nr_frags) {
            int i;
            for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
                put_page(skb_shinfo(skb)->frags[i].page);
        }

        if (skb_has_frags(skb))
            skb_drop_fraglist(skb);

        /*Linux modified begin，yinweidong-171741*/
        if (skb->private_mem.enType == USER_DEFINED)
        {
            /* 释放用户数据块 */
            SkBuffRelUserDataBlk(skb->private_mem.unRetaddr.pDataAddr);
        }
        else if (skb->private_mem.enType == DEFAULT_DEFINED)
        {
            kfree(skb->head);
        }
        else
        {
            IMC_RemoteFreeTtfMem(skb->private_mem.unRetaddr.pTtfMemAddr);
        }
        /*Linux modified end，yinweidong-171741*/
    }
}

/*****************************************************************************
 函 数 名  :skb_insert_head
 功能描述  :queue a buffer at the list head
 输入参数  :struct sk_buff_head *list, struct sk_buff *newsk
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
void skb_insert_head(struct sk_buff_head *list, struct sk_buff *newsk)
{
    VOS_INT32                           lLockKey;
    lLockKey        = VOS_SplIMP();
    __skb_queue_head(list, newsk);
    VOS_Splx(lLockKey);
}

/*****************************************************************************
 函 数 名  :skb_insert_tail
 功能描述  :queue a buffer at the list tail
 输入参数  :struct sk_buff_head *list, struct sk_buff *newsk
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
   1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
void skb_insert_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
    VOS_INT32                           lLockKey;
    lLockKey    = VOS_SplIMP();
    __skb_queue_tail(list, newsk);
    VOS_Splx(lLockKey);
}

/*****************************************************************************
 函 数 名  :skb_remove
 功能描述  :Remove the head of the list.
 输入参数  :struct sk_buff_head *list
 输出参数  : 无
 返 回 值  : sk_buff *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
struct sk_buff *skb_remove(struct sk_buff_head *list)
{
    struct sk_buff                     *result;
    VOS_INT32                           lLockKey;
    lLockKey    = VOS_SplIMP();
    result      = __skb_dequeue(list);
    VOS_Splx(lLockKey);
    return result;
}

/*****************************************************************************
 函 数 名  :skb_remove_tail
 功能描述  :Remove the tail of the list.
 输入参数  :struct sk_buff_head *list
 输出参数  : 无
 返 回 值  : sk_buff *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
struct sk_buff *skb_remove_tail(struct sk_buff_head *list)
{
    struct sk_buff                     *result;
    VOS_INT32                           lLockKey;
    lLockKey    = VOS_SplIMP();
    result      = __skb_dequeue_tail(list);
    VOS_Splx(lLockKey);
    return result;
}

/*****************************************************************************
 函 数 名  :skb_copy_exp
 功能描述  :create private copy of an sk_buff
 输入参数  :const struct sk_buff *skb, gfp_t gfp_mask
 输出参数  : 无
 返 回 值  : sk_buff *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月16日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
struct sk_buff *skb_copy_exp(const struct sk_buff *skb, gfp_t gfp_mask)
{
    int headerlen = skb->data - skb->head;
    /*
    Allocate the copy buffer
    */
    struct sk_buff *n;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
    n = alloc_skb_exp(skb->end + skb->data_len, gfp_mask);
#else
    n = alloc_skb_exp(skb->end - skb->head + skb->data_len, gfp_mask);
#endif
    if (!n)
        return NULL;

    /* Set the data pointer */
    skb_reserve(n, headerlen);
    /* Set the tail pointer and length */
    skb_put(n, skb->len);

    if (skb_copy_bits(skb, -headerlen, n->head, headerlen + skb->len))
        BUG();

    copy_skb_header(n, skb);
    return n;
}


char *skb_put(struct sk_buff *skb, unsigned int len)
{
    unsigned char *tmp = skb_tail_pointer(skb);

    skb->tail += len;
    skb->len  += len;

    return tmp;
}

unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
    return skb->tail;
}

char *skb_put_debug(unsigned long ulFileID, unsigned long ulLineNum,
                           struct sk_buff *skb, unsigned int len)
{
    unsigned char *tmp = skb_tail_pointer(skb);

    skb->tail += len;
    skb->len  += len;

    return tmp;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

