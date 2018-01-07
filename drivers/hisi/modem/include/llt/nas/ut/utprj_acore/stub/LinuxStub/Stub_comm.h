/* 本文件只用于PC-Lint  */
/*------------------------------------------------------------------------------
 * @(#): stub_comm.h 0.01Beta (08/01/2000) xyang !
 *----------------------------------------------------------------------------*/
#if 0
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif
//#define VOS_WIN32                      1
//#define VOS_OS_VER VOS_WIN32

#include "vos.h"
#include "v_typdef.h"
//#include "ImmMem.h"

#ifdef NET_SKBUFF_DATA_USES_OFFSET
typedef unsigned int sk_buff_data_t;
#else
typedef unsigned char *sk_buff_data_t;
#endif

#define NET_SKB_PAD	64

#define GFP_ATOMIC  (0x20u)
#define printk      vos_printf
#define panic       vos_printf


extern void local_irq_save(unsigned long  ulSaveFlags);

extern void local_irq_restore(unsigned long  ulSaveFlags);

extern struct sk_buff *dev_alloc_skb_static_debug(unsigned long ulFileID, unsigned long ulLineNum,
                        unsigned short usLen);

extern struct sk_buff *dev_alloc_skb_static(unsigned int len);

extern struct sk_buff *dev_alloc_skb(unsigned int length);
extern struct sk_buff *dev_alloc_skb_ext(unsigned int len, unsigned char* pData, unsigned int HeadReserveLen,unsigned int TailReserveLen);

extern IMM_ZC_STRU * IMM_DataTranformImmZc_Debug(unsigned short usFileID,
        unsigned short usLineNum, const unsigned char *pucData, unsigned long ulLen, const void *pstTtfMem);


extern  struct sk_buff *skb_copy_exp(const struct sk_buff *skb);

extern void kfree_skb(struct sk_buff *skb);

extern void kfree_skb_head(struct sk_buff *skb);

extern VOS_VOID MMRB_RemoteFreeMem(const VOS_VOID *pucAddr);

extern unsigned char *skb_push(struct sk_buff *skb, unsigned int len);

extern unsigned char *skb_pull(struct sk_buff *skb, unsigned int len);

extern unsigned char *skb_put(struct sk_buff *skb, unsigned int len);

extern void skb_reserve(struct sk_buff *skb, int len);

extern unsigned int skb_headroom(const struct sk_buff *skb);

extern int skb_tailroom(const struct sk_buff *skb);


extern int skb_is_nonlinear(const struct sk_buff *skb);

extern __u32 skb_queue_len(const struct sk_buff_head *list_);


extern void skb_queue_head_init(struct sk_buff_head *list);

extern  void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk);
extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);

extern struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list);

extern struct sk_buff *skb_dequeue(struct sk_buff_head *list);


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif
/* 本文件只用于PC-Lint  */
#endif

