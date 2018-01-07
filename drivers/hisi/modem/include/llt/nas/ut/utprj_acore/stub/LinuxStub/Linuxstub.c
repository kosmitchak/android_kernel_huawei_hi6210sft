#include "Linuxstub.h"

int
test_and_set_bit(unsigned long nr, volatile void *addr)
{
    unsigned long oldbit;
    unsigned long temp;
    int *m = ((int *) addr) + (nr >> 5);

#ifndef __UT_CENTER__
    __asm__ __volatile__(
#ifdef CONFIG_SMP
    "   mb\n"
#endif
    "1: ldl_l %0,%4\n"
    "   and %0,%3,%2\n"
    "   bne %2,2f\n"
    "   xor %0,%3,%0\n"
    "   stl_c %0,%1\n"
    "   beq %0,3f\n"
    "2:\n"
#ifdef CONFIG_SMP
    "   mb\n"
#endif
    ".subsection 2\n"
    "3: br 1b\n"
    ".previous"
    :"=&r" (temp), "=m" (*m), "=&r" (oldbit)
    :"Ir" (1UL << (nr & 31)), "m" (*m) : "memory");
#endif

    return oldbit != 0;
}

static bool linkwatch_urgent_event(struct net_device *dev)
{
    return 1;
}

static void linkwatch_add_event(struct net_device *dev)
{
    return 1;
}

static void linkwatch_schedule_work(int urgent)
{
    return;
}

void linkwatch_fire_event(struct net_device *dev)
{
    bool urgent = linkwatch_urgent_event(dev);

    if (!test_and_set_bit(__LINK_STATE_LINKWATCH_PENDING, &dev->state)) {
        linkwatch_add_event(dev);
    } else if (!urgent)
        return;

    linkwatch_schedule_work(urgent);
}

void netif_carrier_off(struct net_device *dev)
{
    if (!test_and_set_bit(__LINK_STATE_NOCARRIER, &dev->state))
        linkwatch_fire_event(dev);
}

void set_bit(unsigned long nr, volatile void * addr)
{
    unsigned long temp;
    int *m = ((int *) addr) + (nr >> 5);

#ifndef __UT_CENTER__
    __asm__ __volatile__(
    "1: ldl_l %0,%3\n"
    "   bis %0,%2,%0\n"
    "   stl_c %0,%1\n"
    "   beq %0,2f\n"
    ".subsection 2\n"
    "2: br 1b\n"
    ".previous"
    :"=&r" (temp), "=m" (*m)
    :"Ir" (1UL << (nr & 31)), "m" (*m));
#endif
}

enum {
    SKB_FCLONE_UNAVAILABLE,
    SKB_FCLONE_ORIG,
    SKB_FCLONE_CLONE,
};

/*
 *Free an skbuff by memory without cleaning the state.
 */
void kfree_skbmem(struct sk_buff *skb)
{
    struct sk_buff *other;
    atomic_t *fclone_ref;

    switch (skb->fclone) {
    case SKB_FCLONE_UNAVAILABLE:
        kmem_cache_free(skbuff_head_cache, skb);
        break;

    case SKB_FCLONE_ORIG:
        fclone_ref = (atomic_t *) (skb + 2);
        if (atomic_dec_and_test(fclone_ref))
        kmem_cache_free(skbuff_fclone_cache, skb);
        break;

    case SKB_FCLONE_CLONE:
        fclone_ref = (atomic_t *) (skb + 1);
        other = skb - 1;

        /* The clone portion is available for
         * fast-cloning again.
         */
        skb->fclone = SKB_FCLONE_UNAVAILABLE;

        if (atomic_dec_and_test(fclone_ref))
            kmem_cache_free(skbuff_fclone_cache, other);
        break;
    }
}


#if 0
/**
    *free_skb - free an sk_buff
    *skb: buffer to free
    *
    *rop a reference to the buffer and free it if the usage count has
    *it zero.
 */
void kfree_skb(struct sk_buff *skb)
{
    if (unlikely(!skb))
        return;
    if (likely(atomic_read(&skb->users) == 1))
        smp_rmb();
    else if (likely(!atomic_dec_and_test(&skb->users)))
        return;
    trace_kfree_skb(skb, __builtin_return_address(0));
    __kfree_skb(skb);
}


#endif




/**
 *dev_alloc_skb - allocate an skbuff for receiving
 *length: length to allocate
 *
 *Allocate a new &sk_buff and assign it a usage count of one. The
 *buffer has unspecified headroom built in. Users should allocate
 *the headroom they think they need without accounting for the
 *built in space. The built in space is used for optimisations.
 *
 *NULL is returned if there is no free memory. Although this function
 *allocates memory it can be called from an interrupt.
 */
struct sk_buff *dev_alloc_skb(unsigned int length)
{
    /*
     * There is more code here than it seems:
     * __dev_alloc_skb is an inline
     */
    return __dev_alloc_skb(length, GFP_ATOMIC);
}





/**
 * kmem_cache_free - Deallocate an object
 * @cachep: The cache the allocation was from.
 * @objp: The previously allocated object.
 *
 * Free an object which was previously allocated from this
 * cache.
 */
void kmem_cache_free(struct kmem_cache *cachep, void *objp)
{
    unsigned long flags;

    local_irq_save(flags);
    debug_check_no_locks_freed(objp, obj_size(cachep));
    if (!(cachep->flags & SLAB_DEBUG_OBJECTS))
        debug_check_no_obj_freed(objp, obj_size(cachep));
    _cache_free(cachep, objp);
    local_irq_restore(flags);

    race_kmem_cache_free(_RET_IP_, objp);
}



/**
 * alloc_etherdev - Allocates and sets up an Ethernet device
 * @sizeof_priv: Size of additional driver-private structure to be allocated
 * for this Ethernet device
 *
 * Fill in the fields of the device structure with Ethernet-generic
 * values. Basically does everything except registering the device.
 *
 * Constructs a new net device, complete with a private data area of
 * size (sizeof_priv).  A 32-byte (not bit) alignment is enforced for
 * this private data area.
 */

struct net_device *alloc_etherdev_mq(int sizeof_priv, unsigned int queue_count)
{
    return alloc_netdev_mq(sizeof_priv, "eth%d", ether_setup, queue_count);
}
struct net_device *alloc_netdev_mq(int sizeof_priv, const char *name,
    void (*setup)(struct net_device *), unsigned int queue_count)
{
    struct netdev_queue *tx;
    struct net_device *dev;
    size_t alloc_size;
    struct net_device *p;
#ifdef CONFIG_RPS
    struct netdev_rx_queue *rx;
    int i;
#endif

    BUG_ON(strlen(name) >= sizeof(dev->name));

    alloc_size = sizeof(struct net_device);
    if (sizeof_priv) {
        /* ensure 32-byte alignment of private area */
        alloc_size = ALIGN(alloc_size, NETDEV_ALIGN);
        alloc_size += sizeof_priv;
    }
    /* ensure 32-byte alignment of whole construct */
    alloc_size += NETDEV_ALIGN - 1;

    p = kzalloc(alloc_size, GFP_KERNEL);
    if (!p) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate device.\n");
        return NULL;
    }

    tx = kcalloc(queue_count, sizeof(struct netdev_queue), GFP_KERNEL);
    if (!tx) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate "
                "tx qdiscs.\n");
        goto free_p;
    }

#ifdef CONFIG_RPS
    rx = kcalloc(queue_count, sizeof(struct netdev_rx_queue), GFP_KERNEL);
    if (!rx) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate "
               "rx queues.\n");
        goto free_tx;
    }

    tomic_set(&rx->count, queue_count);

    /*
     * Set a pointer to first element in the array which holds the
     * reference count.
     */
    for (i = 0; i < queue_count; i++)
        rx[i].first = rx;
#endif

    dev = PTR_ALIGN(p, NETDEV_ALIGN);
    dev->padded = (char *)dev - (char *)p;

    if (dev_addr_init(dev))
        goto free_rx;

    dev_mc_init(dev);
    dev_uc_init(dev);

    dev_net_set(dev, &init_net);

    dev->_tx = tx;
    dev->num_tx_queues = queue_count;
    dev->real_num_tx_queues = queue_count;

#ifdef CONFIG_RPS
    dev->_rx = rx;
    dev->num_rx_queues = queue_count;
#endif

    dev->gso_max_size = GSO_MAX_SIZE;

    netdev_init_queues(dev);

    INIT_LIST_HEAD(&dev->ethtool_ntuple_list.list);
    dev->ethtool_ntuple_list.count = 0;
    INIT_LIST_HEAD(&dev->napi_list);
    INIT_LIST_HEAD(&dev->unreg_list);
    INIT_LIST_HEAD(&dev->link_watch_list);
    dev->priv_flags = IFF_XMIT_DST_RELEASE;
    setup(dev);
    strcpy(dev->name, name);
    return dev;

free_rx:
#ifdef CONFIG_RPS
    kfree(rx);
free_tx:
#endif
    kfree(tx);
free_p:
    kfree(p);
    return NULL;
}


/**
 *register_netdev   - register a network device
 *@dev: device to register
 *
 *Take a completed network device structure and add it to the kernel
 *interfaces. A %NETDEV_REGISTER message is sent to the netdev notifier
 *chain. 0 is returned on success. A negative errno code is returned
 *on a failure to set up the device, or if the name is a duplicate.
 *
 *This is a wrapper around register_netdevice that takes the rtnl semaphore
 *and expands the device name if you passed a format string to
 *alloc_netdev.
 */
int register_netdev(struct net_device *dev)
{
    int err;

    rtnl_lock();

    /*
     * If the name is a format string the caller wants us to do a
     * name allocation.
     */
    if (strchr(dev->name, '%')) {
        err = dev_alloc_name(dev, dev->name);
        if (err < 0)
            goto out;
    }

    err = register_netdevice(dev);
out:
    rtnl_unlock();
    return err;
}




//#define netif_stop_queue(dev)   et_bit(0, &(dev)->tbusy)

/**
 *netif_rx  -   post buffer to the network code
 *skb: buffer to post
 *
 *This function receives a packet from a device driver and queues it for
 *the upper (protocol) levels to process.  It always succeeds. The buffer
 *may be dropped during processing for congestion control or by the
 *protocol layers.
 *
 *return values:
 *NET_RX_SUCCESS    (no congestion)
 *NET_RX_DROP     (packet was dropped)
 *
 */
int netif_rx(struct sk_buff *skb)
{
    int ret;

    /* if netpoll wants it, pretend we never saw it */
    if (netpoll_rx(skb))
        return NET_RX_DROP;

    if (netdev_tstamp_prequeue)
        net_timestamp_check(skb);

#ifdef CONFIG_RPS
    {
        struct rps_dev_flow voidflow, *rflow = &voidflow;
        int cpu;

        preempt_disable();
        rcu_read_lock();

        cpu = get_rps_cpu(skb->dev, skb, &rflow);
        if (cpu < 0)
            cpu = smp_processor_id();

        ret = enqueue_to_backlog(skb, cpu, &rflow->last_qtail);

        rcu_read_unlock();
        preempt_enable();
    }
#else
    {
        unsigned int qtail;
        ret = enqueue_to_backlog(skb, get_cpu(), &qtail);
        put_cpu();
    }
#endif
    return ret;
}



/**
 *unregister_netdev - remove device from the kernel
 *@dev: device
 *
 *This function shuts down a device interface and removes it
 *from the kernel tables. On success 0 is returned, on a failure
 *a negative errno code is returned.
 *
 *This is just a wrapper for unregister_netdevice that takes
 *the rtnl semaphore.  In general you want to use this and not
 *unregister_netdevice.
 */
void unregister_netdev(struct net_device *dev)
{
    rtnl_lock();
    unregister_netdevice(dev);
    rtnl_unlock();
}





    /**
     *  __kfree_skb - private function
     *  @skb: buffer
     *
     *  Free an sk_buff. Release anything attached to the buffer.
     *  Clean the state. This is an internal helper function. Users should
     *  always call kfree_skb
     */

void __kfree_skb(struct sk_buff *skb)
{
    skb_release_all(skb);
    kfree_skbmem(skb);
}



/**
 * alloc_etherdev_mq - Allocates and sets up an Ethernet device
 * @sizeof_priv: Size of additional driver-private structure to be allocated
 *  for this Ethernet device
 * @queue_count: The number of queues this device has.
 *
 * Fill in the fields of the device structure with Ethernet-generic
 * values. Basically does everything except registering the device.
 *
 * Constructs a new net device, complete with a private data area of
 * size (sizeof_priv).  A 32-byte (not bit) alignment is enforced for
 * this private data area.
 */

struct net_device *alloc_etherdev_mq(int sizeof_priv, unsigned int queue_count)
{
    return alloc_netdev_mq(sizeof_priv, "eth%d", ether_setup, queue_count);
}


/**
 *  alloc_netdev_mq - allocate network device
 *  @sizeof_priv:   size of private data to allocate space for
 *  @name:      device name format string
 *  @setup:     callback to initialize device
 *  @queue_count:   the number of subqueues to allocate
 *
 *  Allocates a struct net_device with private data area for driver use
 *  and performs basic initialization.  Also allocates subquue structs
 *  for each queue on the device at the end of the netdevice.
 */
struct net_device *alloc_netdev_mq(int sizeof_priv, const char *name,
        void (*setup)(struct net_device *), unsigned int queue_count)
{
    struct netdev_queue *tx;
    struct net_device *dev;
    size_t alloc_size;
    struct net_device *p;
#ifdef CONFIG_RPS
    struct netdev_rx_queue *rx;
    int i;
#endif

    BUG_ON(strlen(name) >= sizeof(dev->name));

    alloc_size = sizeof(struct net_device);
    if (sizeof_priv) {
        /* ensure 32-byte alignment of private area */
        alloc_size = ALIGN(alloc_size, NETDEV_ALIGN);
        alloc_size += sizeof_priv;
    }
    /* ensure 32-byte alignment of whole construct */
    alloc_size += NETDEV_ALIGN - 1;

    p = kzalloc(alloc_size, GFP_KERNEL);
    if (!p) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate device.\n");
        return NULL;
    }

    tx = kcalloc(queue_count, sizeof(struct netdev_queue), GFP_KERNEL);
    if (!tx) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate "
               "tx qdiscs.\n");
        goto free_p;
    }

#ifdef CONFIG_RPS
    rx = kcalloc(queue_count, sizeof(struct netdev_rx_queue), GFP_KERNEL);
    if (!rx) {
        printk(KERN_ERR "alloc_netdev: Unable to allocate "
               "rx queues.\n");
        goto free_tx;
    }

    atomic_set(&rx->count, queue_count);

    /*
     * Set a pointer to first element in the array which holds the
     * reference count.
     */
    for (i = 0; i < queue_count; i++)
        rx[i].first = rx;
#endif

    dev = PTR_ALIGN(p, NETDEV_ALIGN);
    dev->padded = (char *)dev - (char *)p;

    if (dev_addr_init(dev))
        goto free_rx;

    dev_mc_init(dev);
    dev_uc_init(dev);

    dev_net_set(dev, &init_net);

    dev->_tx = tx;
    dev->num_tx_queues = queue_count;
    dev->real_num_tx_queues = queue_count;

#ifdef CONFIG_RPS
    dev->_rx = rx;
    dev->num_rx_queues = queue_count;
#endif

    dev->gso_max_size = GSO_MAX_SIZE;

    netdev_init_queues(dev);

    INIT_LIST_HEAD(&dev->ethtool_ntuple_list.list);
    dev->ethtool_ntuple_list.count = 0;
    INIT_LIST_HEAD(&dev->napi_list);
    INIT_LIST_HEAD(&dev->unreg_list);
    INIT_LIST_HEAD(&dev->link_watch_list);
    dev->priv_flags = IFF_XMIT_DST_RELEASE;
    setup(dev);
    strcpy(dev->name, name);
    return dev;

free_rx:
#ifdef CONFIG_RPS
    kfree(rx);
free_tx:
#endif
    kfree(tx);
free_p:
    kfree(p);
    return NULL;
}

/**
 *_dev_alloc_skb - allocate an skbuff for receiving
 *length: length to allocate
 *gfp_mask: get_free_pages mask, passed to alloc_skb
 *
 *Allocate a new &sk_buff and assign it a usage count of one. The
 *buffer has unspecified headroom built in. Users should allocate
 *the headroom they think they need without accounting for the
 *built in space. The built in space is used for optimisations.
 *
 *NULL is returned if there is no free memory.
 */
inline struct sk_buff *__dev_alloc_skb(unsigned int length,
                          gfp_t gfp_mask)
{
    struct sk_buff *skb = alloc_skb(length + NET_SKB_PAD, gfp_mask);
    if (likely(skb))
        skb_reserve(skb, NET_SKB_PAD);
    return skb;
}

inline struct sk_buff *alloc_skb(unsigned int size,
                gfp_t priority)
{
    return __alloc_skb(size, priority, 0, -1);
}

/**
 *skb_reserve - adjust headroom
 *skb: buffer to alter
 *len: bytes to move
 *
 *Increase the headroom of an empty &sk_buff by reducing the tail
 *room. This is only allowed for an empty buffer.
 */
inline void skb_reserve(struct sk_buff *skb, int len)
{
    skb->data += len;
    skb->tail += len;
}

inline void skb_reserve_debug(unsigned short usFileID, unsigned short usLineNum,
                               struct sk_buff *skb, int len)
{
    skb->data += len;
    skb->tail += len;
}


/**
 *skb_queue_len - get queue length
 *list_: list to measure
 *
 *Return the length of an &sk_buff queue.
 */
inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
    return list_->qlen;
}


inline __u32 skb_queue_len_debug(unsigned short usFileID, unsigned short usLineNum, const struct sk_buff_head *list_)
{
    return list_->qlen;
}


/**
 *  skb_peek - peek at the head of an &sk_buff_head
 *  @list_: list to peek at
 *
 *  Peek an &sk_buff. Unlike most other operations you _MUST_
 *  be careful with this one. A peek leaves the buffer on the
 *  list and someone else may run off with it. You must hold
 *  the appropriate locks or have a private queue to do this.
 *
 *  Returns %NULL for an empty list or a pointer to the head element.
 *  The reference count is not incremented and the reference is therefore
 *  volatile. Use with caution.
 */
inline struct sk_buff *skb_peek(struct sk_buff_head *list_)
{
    struct sk_buff *list = ((struct sk_buff *)list_)->next;
    if (list == (struct sk_buff *)list_)
        list = NULL;
    return list;
}

/**
 *  skb_peek_tail - peek at the tail of an &sk_buff_head
 *  @list_: list to peek at
 *
 *  Peek an &sk_buff. Unlike most other operations you _MUST_
 *  be careful with this one. A peek leaves the buffer on the
 *  list and someone else may run off with it. You must hold
 *  the appropriate locks or have a private queue to do this.
 *
 *  Returns %NULL for an empty list or a pointer to the tail element.
 *  The reference count is not incremented and the reference is therefore
 *  volatile. Use with caution.
 */
inline struct sk_buff *skb_peek_tail(struct sk_buff_head *list_)
{
    struct sk_buff *list = ((struct sk_buff *)list_)->prev;
    if (list == (struct sk_buff *)list_)
        list = NULL;
    return list;
}

/* Free everything but the sk_buff shell. */
void skb_release_all(struct sk_buff *skb)
{
    skb_release_head_state(skb);
    skb_release_data(skb);
}

/**
 *free_netdev - free network device
 *@dev: device
 *
 *This function does the last stage of destroying an allocated device
 * nterface. The reference to the device object is released.
 *If this is the last reference then it will be freed.
 */
//#define free_netdev(dev)    kfree(dev)

void free_netdev(struct net_device *dev)
{
    struct napi_struct *p, *n;

    release_net(dev_net(dev));

    kfree(dev->_tx);

    /* Flush device addresses */
    dev_addr_flush(dev);

    /* Clear ethtool n-tuple list */
    ethtool_ntuple_flush(dev);

    list_for_each_entry_safe(p, n, &dev->napi_list, dev_list)
        netif_napi_del(p);

    /*  Compatibility with error handling in drivers */
    if (dev->reg_state == NETREG_UNINITIALIZED) {
        kfree((char *)dev - dev->padded);
        return;
    }

    BUG_ON(dev->reg_state != NETREG_UNREGISTERED);
    dev->reg_state = NETREG_RELEASED;

    /* will free via device release */
    put_device(&dev->dev);
}

inline void __skb_queue_head_init(struct sk_buff_head *list)
{
    list->prev = list->next = (struct sk_buff *)list;
    list->qlen = 0;
}

/**
 *_skb_dequeue - remove from the head of the queue
 *list: list to dequeue from
 *
 *emove the head of the list. This function does not take any locks
 *so must be used with appropriate locks held only. The head item is
 *returned or %NULL if the list is empty.
 */
inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
    struct sk_buff *skb = skb_peek(list);
    if (skb)
        __skb_unlink(skb, list);
    return skb;
}


/**
 *__skb_dequeue_tail - remove from the tail of the queue
 *list: list to dequeue from
 *
 *Remove the tail of the list. This function does not take any locks
 *so must be used with appropriate locks held only. The tail item is
 *returned or %NULL if the list is empty.
 */
inline struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list)
{
    struct sk_buff *skb = skb_peek_tail(list);
    if (skb)
        __skb_unlink(skb, list);
    return skb;
}


/**
 *__skb_queue_head - queue a buffer at the list head
 *list: list to use
 *newsk: buffer to queue
 *
 *Queue a buffer at the start of a list. This function takes no locks
 *and you must therefore hold required locks before calling it.
 *
 *A buffer cannot be placed on two lists at the same time.
 */
inline void __skb_queue_head(struct sk_buff_head *list,
                struct sk_buff *newsk)
{
    __skb_queue_after(list, (struct sk_buff *)list, newsk);
}


/**
 *__skb_queue_tail - queue a buffer at the list tail
 *list: list to use
 *newsk: buffer to queue
 *
 *Queue a buffer at the end of a list. This function takes no locks
 *and you must therefore hold required locks before calling it.
 *
 *A buffer cannot be placed on two lists at the same time.
 */
inline void __skb_queue_tail(struct sk_buff_head *list,
                   struct sk_buff *newsk)
{
    __skb_queue_before(list, (struct sk_buff *)list, newsk);
}

inline void skb_queue_head_init(struct sk_buff_head *list)
{
    __skb_queue_head_init(list);
}

inline void skb_queue_head_init_debug(unsigned short usFileID, unsigned short usLineNum, struct sk_buff_head *list)
{
    __skb_queue_head_init(list);
}

void skb_queue_tail_debug(unsigned short usFileID, unsigned short usLineNum,
                                  struct sk_buff_head *list, struct sk_buff *newsk)
{
    __skb_queue_tail(list, newsk);
}

struct sk_buff *skb_dequeue_tail_debug(unsigned short usFileID, unsigned short usLineNum, struct sk_buff_head *list)
{
    struct sk_buff *result;

    result = __skb_dequeue_tail(list);

    return result;
}

struct sk_buff *skb_dequeue_debug(unsigned short usFileID, unsigned short usLineNum, struct sk_buff_head *list)
{
    struct sk_buff *result;

    result = __skb_dequeue(list);

    return result;
}

void netif_carrier_on(struct net_device *dev)
{
    if (test_and_clear_bit(__LINK_STATE_NOCARRIER, &dev->state))
        linkwatch_fire_event(dev);
    if (netif_running(dev))
        __netdev_watchdog_up(dev);
}

void kmemcheck_mark_initialized(void *address, unsigned int n)
{
    mark_shadow(address, n, KMEMCHECK_SHADOW_INITIALIZED);
}

static void mark_shadow(void *address, unsigned int n,
    enum kmemcheck_shadow status)
{
    unsigned long addr = (unsigned long) address;
    unsigned long last_addr = addr + n - 1;
    unsigned long page = addr & PAGE_MASK;
    unsigned long last_page = last_addr & PAGE_MASK;
    unsigned int first_n;
    void *shadow;

    /* If the memory range crosses a page boundary, stop there. */
    if (page == last_page)
        first_n = n;
    else
        first_n = page + PAGE_SIZE - addr;

    shadow = kmemcheck_shadow_lookup(addr);
    if (shadow)
        memset(shadow, status, first_n);

    addr += first_n;
    n -= first_n;

    /* Do full-page memset()s. */
    while (n >= PAGE_SIZE) {
        shadow = kmemcheck_shadow_lookup(addr);
        if (shadow)
            memset(shadow, status, PAGE_SIZE);

        addr += PAGE_SIZE;
        n -= PAGE_SIZE;
    }

    /* Do the remaining page, if any. */
    if (n > 0) {
        shadow = kmemcheck_shadow_lookup(addr);
        if (shadow)
            memset(shadow, status, n);
    }
}


/*
 * Return the shadow address for the given address. Returns NULL if the
 * address is not tracked.
 *
 * We need to be extremely careful not to follow any invalid pointers,
 * because this function can be called for *any* possible address.
 */
void *kmemcheck_shadow_lookup(unsigned long address)
{
    pte_t *pte;
    struct page *page;

    if (!virt_addr_valid(address))
        return NULL;

    pte = kmemcheck_pte_lookup(address);
    if (!pte)
        return NULL;

    page = virt_to_page(address);
    if (!page->shadow)
        return NULL;
    return page->shadow + (address & (PAGE_SIZE - 1));
}


static inline void netif_stop_queue(struct net_device *dev)
{
    set_bit(__LINK_STATE_XOFF, &dev->state);
}

static inline void netif_start_queue(struct net_device *dev)
{
    dev->tbusy = 0;
    dev->interrupt = 0;
    dev->start = 1;
}

static inline int is_valid_ether_addr(const u8 *addr)
{
    /* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
     * explicitly check for it here. */
    return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

void *memcpy(void *to, const void *from, size_t n)
{
#ifdef CONFIG_X86_USE_3DNOW
    return __memcpy3d(to, from, n);
#else
    return __memcpy(to, from, n);
#endif
}

inline void *netdev_priv(struct net_device *dev)
{
    return (char *)dev + ((sizeof(struct net_device)
                    + NETDEV_ALIGN_CONST)
                & ~NETDEV_ALIGN_CONST);
}

static inline void netif_wake_queue(struct net_device *dev)
{
#ifdef CONFIG_NETPOLL_TRAP
    if (netpoll_trap()) {
        clear_bit(__LINK_STATE_XOFF, &dev->state);
        return;
    }
#endif
    if (test_and_clear_bit(__LINK_STATE_XOFF, &dev->state))
        __netif_schedule(dev);
}

int device_event_report(void *data, int len)
{
    return 0;
}

int kern_path(const char *name, unsigned int flags, struct path *path)
{
    return 0;
}

int netif_rx_ni_balong(struct sk_buff *skb)
{
    return 0;
}

inline unsigned int imajor(const struct inode *inode)
{
    return MAJOR(inode->i_rdev);
}


#ifdef __PC_UT__  

#else
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
{

}

int cdev_add(struct cdev *p, dev_t dev, unsigned count)
{
    return 0;
}

void cdev_del(struct cdev *p)
{

}

void device_create(struct class *cls, struct device *parent,
                    dev_t devt, void *drvdata,
                    const char *fmt, ...)
{

}
#endif

int register_chrdev_region(dev_t dev, unsigned count, const char *name)
{
    return 0;
}

int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)
{
    return 0;
}

void unregister_chrdev_region(dev_t from, unsigned count)
{

}

unsigned long copy_to_user(void  *to, const void *from, unsigned long n)
{
    VOS_MemCpy(to,from,n);
    return VOS_OK;
}

unsigned long copy_from_user(void *to, const void  *from,unsigned long n)
{
    VOS_MemCpy(to,from,n);
    return VOS_OK;
}

void sema_init(struct semaphore *sem,int val)
{
	return ;
}

void down(struct semaphore  *sem)
{
	return ;
}

void up(struct semaphore  *sem)
{
	return ;
}

void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p)
{
    return;
}

void *nlmsg_data(const struct nlmsghdr *nlh)
{
    return (unsigned char *) nlh + NLMSG_HDRLEN;
}

int nlmsg_msg_size(int payload)
{
    return NLMSG_HDRLEN + payload;
}

struct nlmsghdr * nlmsg_put(struct sk_buff *skb, u32 portid, u32 seq, int type, int len, int flags)
{
    struct nlmsghdr *nlh;
    int size = nlmsg_msg_size(len);

    nlh = (struct nlmsghdr*)skb_put(skb, NLMSG_ALIGN(size));
    nlh->nlmsg_type = type;
    nlh->nlmsg_len = size;
    nlh->nlmsg_flags = flags;
    nlh->nlmsg_pid = portid;
    nlh->nlmsg_seq = seq;

    return nlh;
}

struct nlmsghdr *nlmsg_hdr(const struct sk_buff *skb)
{
    return (struct nlmsghdr *)skb->data;
}

void netlink_kernel_release(struct sock *sk)
{
    return;
}

struct sock * netlink_kernel_create(struct net *net, int unit, struct netlink_kernel_cfg *cfg)
{
    return NULL;
}

int netlink_unicast(struct sock *ssk, struct sk_buff *skb, u32 portid, int nonblock)
{
    kfree_skb(skb);
    return 1;
}

int nlmsg_total_size(int payload)
{
    return NLMSG_ALIGN(nlmsg_msg_size(payload));
}

struct sk_buff *nlmsg_new(size_t payload, gfp_t flags)
{
    return IMM_ZcStaticAlloc(nlmsg_total_size(payload));
}