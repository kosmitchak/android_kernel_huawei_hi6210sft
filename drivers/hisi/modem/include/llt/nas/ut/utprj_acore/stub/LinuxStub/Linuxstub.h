#ifndef __LINUX_STUB_H__
#define __LINUX_STUB_H__

#include "winsock.h"
#include "limits.h"
#include "skbuff.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __CHECKER__
#define __force __attribute__((force))
#else
#define __force
#endif

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif


#ifndef __uid_t_defined
typedef __uid_t uid_t;
# define __uid_t_defined
#endif

# ifndef __blkcnt_t_defined
typedef __blkcnt_t blkcnt_t;	 /* Type to count number of disk blocks.  */
#  define __blkcnt_t_defined
# endif

#ifndef __dev_t_defined
typedef __dev_t dev_t;
# define __dev_t_defined
#endif

#ifndef __timespec_defined
struct timespec
{
    __time_t tv_sec;		/* Seconds.  */
    long int tv_nsec;		/* Nanoseconds.  */
};
# define __timespec_defined
#endif

#define __GFP_HIGH  ((__force gfp_t)0x20u)  /* Should access emergency pools? */

#define GFP_ATOMIC  (__GFP_HIGH)

#define __GFP_DMA   ((__force gfp_t)0x01u)

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

#define INT_MIN     (-INT_MAX - 1)

#define EOPNOTSUPP               (-45)               /* Operation not supported */
#define EADDRNOTAVAIL            (49)                /* Cannot assign requested address *//*在errno-base.h定义*/
#define ETH_ALEN                6
#define ETH_HLEN                14
#define IFF_UP                  0x1     /* interface is up      */

/* Backlog congestion levels */
#define NET_RX_SUCCESS      0   /* keep 'em coming, baby */
#define NET_RX_DROP         1   /* packet dropped */
#define O_NONBLOCK          0xfff0

typedef unsigned int mode_t;
#define IFNAMSIZ 16

#define PAGE_SHIFT  (12)
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))

enum kmemcheck_shadow {
    KMEMCHECK_SHADOW_UNALLOCATED,
    KMEMCHECK_SHADOW_UNINITIALIZED,
    KMEMCHECK_SHADOW_INITIALIZED,
    KMEMCHECK_SHADOW_FREED,
};

extern void mark_shadow(void *address, unsigned int n,
    enum kmemcheck_shadow status);

extern void set_bit(unsigned long nr, volatile void * addr);
extern void kmemcheck_mark_initialized(void *address, unsigned int n);


//#define netif_stop_queue(dev)   set_bit(0, &(dev)->tbusy)


#define time_after_eq(a,b) \
     ((long)(a) - (long)(b) >= 0)

extern unsigned long jiffies;



#define L1_CACHE_SHIFT 5
#define L1_CACHE_BYTES (1 << L1_CACHE_SHIFT)

#ifndef __user
#define __user
#endif

#ifndef SMP_CACHE_BYTES
#define SMP_CACHE_BYTES L1_CACHE_BYTES
#endif
#define SKB_DATA_ALIGN(X)   (((X) + (SMP_CACHE_BYTES - 1)) & \
                ~(SMP_CACHE_BYTES - 1))

#define kmemcheck_bitfield_begin(name)  \
    int name##_begin[0];

#define kmemcheck_bitfield_end(name)    \
    int name##_end[0];

#define MAYBE_BUILD_BUG_ON(cond) ((void)sizeof(char[1 - 2 * !!(cond)]))

#define kmemcheck_annotate_bitfield(ptr, name)              \
    do {                                \
        int _n;                         \
                                    \
        if (!ptr)                       \
            break;                      \
                                    \
        _n = (long) &((ptr)->name##_end)            \
            - (long) &((ptr)->name##_begin);        \
        MAYBE_BUILD_BUG_ON(_n < 0);             \
                                    \
        kmemcheck_mark_initialized(&((ptr)->name##_begin), _n); \
    } while (0)

#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef unsigned int u_int32_t;
typedef unsigned char u_int8_t;

typedef __loff_t loff_t;

#ifndef __loff_t_defined
//
# define __loff_t_defined
#endif

typedef int     __kernel_timer_t;

#define SIGEV_MAX_SIZE  64
#define __ARCH_SIGEV_PREAMBLE_SIZE  (sizeof(int) * 2 + sizeof(sigval_t))
#define SIGEV_PAD_SIZE  ((SIGEV_MAX_SIZE - __ARCH_SIGEV_PREAMBLE_SIZE) \
        / sizeof(int))
//#define _NSIG       64
#define __BITS_PER_LONG 64
#define _NSIG_BPW   __BITS_PER_LONG
#define _NSIG_WORDS (_NSIG / _NSIG_BPW)
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_STOP 5
#define NF_MAX_VERDICT NF_STOP
#ifndef O_RDWR
#define O_RDWR      2
#endif
enum {
    NFPROTO_UNSPEC =  0,
    NFPROTO_IPV4   =  2,
    NFPROTO_ARP    =  3,
    NFPROTO_BRIDGE =  7,
    NFPROTO_IPV6   = 10,
    NFPROTO_DECNET = 12,
    NFPROTO_NUMPROTO,
};
#define ERESTARTSYS 512
#define ERESTARTNOINTR  513
#define ERESTARTNOHAND  514 /* restart if no handler.. */
#define ENOIOCTLCMD 515 /* No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */
#define EBADHANDLE  521 /* Illegal NFS file handle */
#define ENOTSYNC    522 /* Update synchronization mismatch */
#define EBADCOOKIE  523 /* Cookie is stale */
#define ENOTSUPP    524 /* Operation is not supported */
#define ETOOSMALL   525 /* Buffer or request is too small */
#define ESERVERFAULT    526 /* An untranslatable error occurred */
#define EBADTYPE    527 /* Type not supported by server */
#define EJUKEBOX    528 /* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED 529 /* iocb queued, will get completion event */
#define EIOCBRETRY  530 /* iocb queued, will trigger a retry */
//#define S_IRUSR 00400
//#define S_IWUSR 00200
#define CLOCK_REALTIME          0
#define CLOCK_MONOTONIC         1
#define CLOCK_PROCESS_CPUTIME_ID    2
#define CLOCK_THREAD_CPUTIME_ID     3
#define CLOCK_MONOTONIC_RAW         4
#define CLOCK_REALTIME_COARSE       5
#define CLOCK_MONOTONIC_COARSE      6
//#define SIGEV_SIGNAL    0   /* notify via signal */
//#define SIGEV_NONE      1   /* other notification: meaningless */
//#define SIGEV_THREAD    2   /* deliver via thread creation */
//#define SIGEV_THREAD_ID 4   /* deliver to thread */
#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK        1    /* for unblocking signals */
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK        2    /* for setting the signal mask */
#endif
//#define SIGRTMIN    32
#ifndef EINVAL
#define EINVAL  22
#endif
#define NF_IP_PRE_ROUTING   0
#define NF_IP_POST_ROUTING  4
#define POLLIN      0x0001
#define POLLPRI     0x0002
#define POLLOUT     0x0004
#define POLLERR     0x0008
#define POLLHUP     0x0010
#define POLLNVAL    0x0020
#define POLLRDNORM  0x0040
#define POLLRDBAND  0x0080
#ifndef POLLWRNORM
#define POLLWRNORM  0x0100
#endif
#ifndef POLLWRBAND
#define POLLWRBAND  0x0200
#endif
#ifndef POLLMSG
#define POLLMSG     0x0400
#endif
#ifndef POLLREMOVE
#define POLLREMOVE  0x1000
#endif
#ifndef POLLRDHUP
#define POLLRDHUP       0x2000
#endif


#define EPERM        1  /* Operation not permitted */
#define ENOENT       2  /* No such file or directory */
#define ESRCH        3  /* No such process */
#define EINTR        4  /* Interrupted system call */
#define EIO      5  /* I/O error */
#define ENXIO        6  /* No such device or address */
#define E2BIG        7  /* Argument list too long */
#define ENOEXEC      8  /* Exec format error */
#define EBADF        9  /* Bad file number */
#define ECHILD      10  /* No child processes */
#define EAGAIN      11  /* Try again */
#define ENOMEM      12  /* Out of memory */
#define EACCES      13  /* Permission denied */
#define EFAULT      14  /* Bad address */
#define ENOTBLK     15  /* Block device required */
#define EBUSY       16  /* Device or resource busy */
#define EEXIST      17  /* File exists */
#define EXDEV       18  /* Cross-device link */
#define ENODEV      19  /* No such device */
#define ENOTDIR     20  /* Not a directory */
#define EISDIR      21  /* Is a directory */
#define EINVAL      22  /* Invalid argument */
#define ENFILE      23  /* File table overflow */
#define EMFILE      24  /* Too many open files */
#define ENOTTY      25  /* Not a typewriter */
#define ETXTBSY     26  /* Text file busy */
#define EFBIG       27  /* File too large */
#define ENOSPC      28  /* No space left on device */
#define ESPIPE      29  /* Illegal seek */
#define EROFS       30  /* Read-only file system */
#define EMLINK      31  /* Too many links */
#define EPIPE       32  /* Broken pipe */
#define EDOM        33  /* Math argument out of domain of func */
#define ERANGE      34  /* Math result not representable */


enum nf_ip_hook_priorities {
    NF_IP_PRI_FIRST = INT_MIN,
    NF_IP_PRI_CONNTRACK_DEFRAG = -400,
    NF_IP_PRI_RAW = -300,
    NF_IP_PRI_SELINUX_FIRST = -225,
    NF_IP_PRI_CONNTRACK = -200,
    NF_IP_PRI_MANGLE = -150,
    NF_IP_PRI_NAT_DST = -100,
    NF_IP_PRI_FILTER = 0,
    NF_IP_PRI_SECURITY = 50,
    NF_IP_PRI_NAT_SRC = 100,
    NF_IP_PRI_SELINUX_LAST = 225,
    NF_IP_PRI_CONNTRACK_CONFIRM = INT_MAX,
    NF_IP_PRI_LAST = INT_MAX,
};
enum nf_ip6_hook_priorities {
    NF_IP6_PRI_FIRST = INT_MIN,
    NF_IP6_PRI_CONNTRACK_DEFRAG = -400,
    NF_IP6_PRI_RAW = -300,
    NF_IP6_PRI_SELINUX_FIRST = -225,
    NF_IP6_PRI_CONNTRACK = -200,
    NF_IP6_PRI_MANGLE = -150,
    NF_IP6_PRI_NAT_DST = -100,
    NF_IP6_PRI_FILTER = 0,
    NF_IP6_PRI_SECURITY = 50,
    NF_IP6_PRI_NAT_SRC = 100,
    NF_IP6_PRI_SELINUX_LAST = 225,
    NF_IP6_PRI_LAST = INT_MAX,
};
#define NF_IP_PRE_ROUTING   0
#define NF_IP_LOCAL_IN      1
#define NF_IP_FORWARD       2
#define NF_IP_LOCAL_OUT     3
#define NF_IP_POST_ROUTING  4
#define NF_IP_NUMHOOKS      5
#define NF_IP6_PRE_ROUTING  0
#define NF_IP6_LOCAL_IN     1
#define NF_IP6_FORWARD      2
#define NF_IP6_LOCAL_OUT    3
#define NF_IP6_POST_ROUTING 4
#define NF_IP6_NUMHOOKS 5
#define NF_INET_PRE_ROUTING   0
#define NF_INET_LOCAL_IN      1
#define NF_INET_FORWARD       2
#define NF_INET_LOCAL_OUT     3
#define NF_INET_POST_ROUTING  4
#define NF_ARP_IN   0
#define NF_ARP_OUT  1
#define NF_ARP_FORWARD  2
#define NF_ARP_NUMHOOKS 3
#define NF_BR_PRE_ROUTING   0
#define NF_BR_LOCAL_IN  1
#define NF_BR_FORWARD   2
#define NF_BR_LOCAL_OUT 3
#define NF_BR_POST_ROUTING  4
#define NF_BR_BROUTING  5
#define NF_BR_NUMHOOKS  6
enum nf_br_hook_priorities {
    NF_BR_PRI_FIRST = INT_MIN,
    NF_BR_PRI_NAT_DST_BRIDGED = -300,
    NF_BR_PRI_FILTER_BRIDGED = -200,
    NF_BR_PRI_BRNF = 0,
    NF_BR_PRI_NAT_DST_OTHER = 100,
    NF_BR_PRI_FILTER_OTHER = 200,
    NF_BR_PRI_NAT_SRC = 300,
    NF_BR_PRI_LAST = INT_MAX,
};
/*****************************************************************************
   STRUCT定义
*****************************************************************************/
struct ethhdr {
    unsigned char   h_dest[ETH_ALEN];   /* destination eth addr */
    unsigned char   h_source[ETH_ALEN]; /* source ether addr    */
    __be16      h_proto;        /* packet type ID field */
};

enum netdev_tx {
    __NETDEV_TX_MIN  = INT_MIN, /* make sure enum is signed */
    NETDEV_TX_OK     = 0x00,    /* driver took care of packet */
    NETDEV_TX_BUSY   = 0x10,    /* driver tx path was busy*/
    NETDEV_TX_LOCKED = 0x20,    /* driver tx lock was already taken */
};
typedef enum netdev_tx netdev_tx_t;

struct pneigh_entry {
    struct pneigh_entry *next;
#ifdef CONFIG_NET_NS
    struct net      *net;
#endif
    struct net_device   *dev;
    u8          flags;
    u8          key;
};

struct work_struct{
    int         stub;
};

struct timer_list{
    int         stub;
};

struct delayed_work {
    struct work_struct work;
    struct timer_list timer;
};


/**
 * struct rcu_head - callback structure for use with RCU
 * @next: next update requests in a list
 * @func: actual update function to call after the grace period.
 */
struct rcu_head {
    struct rcu_head *next;
    void (*func)(struct rcu_head *head);
};

typedef struct spinlock {
    union {
        int rlock;
    };
} spinlock_t;

/*
 *  neighbour table manipulation
 */

struct neigh_parms {
#ifdef CONFIG_NET_NS
    struct net *net;
#endif
    struct net_device *dev;
    struct neigh_parms *next;
    int (*neigh_setup)(struct neighbour *);
    void    (*neigh_cleanup)(struct neighbour *);
    struct neigh_table *tbl;

    void    *sysctl_table;

    int dead;
    atomic_t refcnt;
    struct rcu_head rcu_head;

    int base_reachable_time;
    int retrans_time;
    int gc_staletime;
    int reachable_time;
    int delay_probe_time;

    int queue_len;
    int ucast_probes;
    int app_probes;
    int mcast_probes;
    int anycast_delay;
    int proxy_delay;
    int proxy_qlen;
    int locktime;
};

typedef struct {
    int stub;
} rwlock_t;

//typedef struct {
//    int stub;
//} kmem_cache;

struct neigh_statistics {
    unsigned long allocs;       /* number of allocated neighs */
    unsigned long destroys;     /* number of destroyed neighs */
    unsigned long hash_grows;   /* number of hash resizes */

    unsigned long res_failed;   /* number of failed resolutions */

    unsigned long lookups;      /* number of lookups */
    unsigned long hits;     /* number of hits (among lookups) */

    unsigned long rcv_probes_mcast; /* number of received mcast ipv6 */
    unsigned long rcv_probes_ucast; /* number of received ucast ipv6 */

    unsigned long periodic_gc_runs; /* number of periodic GC runs */
    unsigned long forced_gc_runs;   /* number of forced GC runs */

    unsigned long unres_discards;   /* number of unresolved drops */
};

#define __percpu

struct neigh_table {
    struct neigh_table  *next;
    int         family;
    int         entry_size;
    int         key_len;
    __u32           (*hash)(const void *pkey, const struct net_device *);
    int         (*constructor)(struct neighbour *);
    int         (*pconstructor)(struct pneigh_entry *);
    void            (*pdestructor)(struct pneigh_entry *);
    void            (*proxy_redo)(struct sk_buff *skb);
    char            *id;
    struct neigh_parms  parms;
    /* HACK. gc_* shoul follow parms without a gap! */
    int         gc_interval;
    int         gc_thresh1;
    int         gc_thresh2;
    int         gc_thresh3;
    unsigned long       last_flush;
    struct delayed_work gc_work;
    struct timer_list   proxy_timer;
    struct sk_buff_head proxy_queue;
    atomic_t        entries;
    rwlock_t        lock;
    unsigned long       last_rand;
    struct kmem_cache       *kmem_cachep;
    struct neigh_statistics __percpu *stats;
    struct neighbour    **hash_buckets;
    unsigned int        hash_mask;
    __u32           hash_rnd;
    struct pneigh_entry **phash_buckets;
};

#define MAX_ADDR_LEN    32      /* Largest hardware address length */

//typedef struct {
//    int stub;
//} hh_cache;

//typedef struct {
//    int stub;
//} neigh_ops;

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

struct neighbour {
    struct neighbour    *next;
    struct neigh_table  *tbl;
    struct neigh_parms  *parms;
    struct net_device       *dev;
    unsigned long       used;
    unsigned long       confirmed;
    unsigned long       updated;
    __u8            flags;
    __u8            nud_state;
    __u8            type;
    __u8            dead;
    atomic_t        probes;
    rwlock_t        lock;
    unsigned char       ha[ALIGN(MAX_ADDR_LEN, sizeof(unsigned long))];
    struct hh_cache     *hh;
    atomic_t        refcnt;
    int         (*output)(struct sk_buff *skb);
    struct sk_buff_head arp_queue;
    struct timer_list   timer;
    const struct neigh_ops  *ops;
    u8          primary_key;
};



/*
 *  Device mapping structure. I'd just gone off and designed a
 *  beautiful scheme using only loadable modules with arguments
 *  for driver options and along come the PCMCIA people 8)
 *
 *  Ah well. The get() side of this is good for WDSETUP, and it'll
 *  be handy for debugging things. The set side is fine for now and
 *  being very small might be worth keeping for clean configuration.
 */

struct ifmap {
    unsigned long mem_start;
    unsigned long mem_end;
    unsigned short base_addr;
    unsigned char irq;
    unsigned char dma;
    unsigned char port;
    /* 3 bytes spare */
};

typedef struct {
    unsigned short encoding;
    unsigned short parity;
} raw_hdlc_proto;

typedef struct {
    unsigned int interval;
    unsigned int timeout;
} cisco_proto;

typedef struct {
    unsigned int t391;
    unsigned int t392;
    unsigned int n391;
    unsigned int n392;
    unsigned int n393;
    unsigned short lmi;
    unsigned short dce; /* 1 for DCE (network side) operation */
} fr_proto;

typedef struct {
    unsigned int dlci;
} fr_proto_pvc;          /* for creating/deleting FR PVCs */

typedef struct {
    unsigned int dlci;
    char master[IFNAMSIZ];  /* Name of master FRAD device */
}fr_proto_pvc_info;     /* for returning PVC information only */

typedef struct {
    unsigned int clock_rate; /* bits per second */
    unsigned int clock_type; /* internal, external, TX-internal etc. */
    unsigned short loopback;
} sync_serial_settings;          /* V.35, V.24, X.21 */

typedef struct {
    unsigned int clock_rate; /* bits per second */
    unsigned int clock_type; /* internal, external, TX-internal etc. */
    unsigned short loopback;
    unsigned int slot_map;
} te1_settings;                  /* T1, E1 */

struct if_settings {
    unsigned int type;  /* Type of physical device or protocol */
    unsigned int size;  /* Size of the data allocated by the caller */
    union {
        /* {atm/eth/dsl}_settings anyone ? */
        raw_hdlc_proto      __user *raw_hdlc;
        cisco_proto     __user *cisco;
        fr_proto        __user *fr;
        fr_proto_pvc        __user *fr_pvc;
        fr_proto_pvc_info   __user *fr_pvc_info;

        /* interface settings */
        sync_serial_settings    __user *sync;
        te1_settings        __user *te1;
    } ifs_ifsu;
};


/*
 * Interface request structure used fsor socket
 * ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */

struct ifreq {
#define IFHWADDRLEN 6
    union
    {
        char    ifrn_name[IFNAMSIZ];        /* if name, e.g. "en0" */
    } ifr_ifrn;

    union {
        struct  sockaddr ifru_addr;
        struct  sockaddr ifru_dstaddr;
        struct  sockaddr ifru_broadaddr;
        struct  sockaddr ifru_netmask;
        struct  sockaddr ifru_hwaddr;
        short   ifru_flags;
        int ifru_ivalue;
        int ifru_mtu;
        struct  ifmap ifru_map;
        char    ifru_slave[IFNAMSIZ];   /* Just fits the size */
        char    ifru_newname[IFNAMSIZ];
        void __user *   ifru_data;
        struct  if_settings ifru_settings;
    } ifr_ifru;
};

#define ifr_name    ifr_ifrn.ifrn_name  /* interface name   */
#define ifr_hwaddr  ifr_ifru.ifru_hwaddr    /* MAC address      */
#define ifr_addr    ifr_ifru.ifru_addr  /* address      */
#define ifr_dstaddr ifr_ifru.ifru_dstaddr   /* other end of p-p lnk */
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr /* broadcast address    */
#define ifr_netmask ifr_ifru.ifru_netmask   /* interface net mask   */
#define ifr_flags   ifr_ifru.ifru_flags /* flags        */
#define ifr_metric  ifr_ifru.ifru_ivalue    /* metric       */
#define ifr_mtu     ifr_ifru.ifru_mtu   /* mtu          */
#define ifr_map     ifr_ifru.ifru_map   /* device map       */
#define ifr_slave   ifr_ifru.ifru_slave /* slave device     */
#define ifr_data    ifr_ifru.ifru_data  /* for use by interface */
#define ifr_ifindex ifr_ifru.ifru_ivalue    /* interface index  */
#define ifr_bandwidth   ifr_ifru.ifru_ivalue    /* link bandwidth   */
#define ifr_qlen    ifr_ifru.ifru_ivalue    /* Queue length     */
#define ifr_newname ifr_ifru.ifru_newname   /* New name     */
#define ifr_settings    ifr_ifru.ifru_settings  /* Device/proto settings*/

struct net_device_ops {
    int         (*ndo_init)(struct net_device *dev);
    void            (*ndo_uninit)(struct net_device *dev);
    int         (*ndo_open)(struct net_device *dev);
    int         (*ndo_stop)(struct net_device *dev);
    netdev_tx_t     (*ndo_start_xmit) (struct sk_buff *skb,
                           struct net_device *dev);
    u16         (*ndo_select_queue)(struct net_device *dev,
                            struct sk_buff *skb);
    void            (*ndo_change_rx_flags)(struct net_device *dev,
                               int flags);
    void            (*ndo_set_rx_mode)(struct net_device *dev);
    void            (*ndo_set_multicast_list)(struct net_device *dev);
    int         (*ndo_set_mac_address)(struct net_device *dev,
                               void *addr);
    int         (*ndo_validate_addr)(struct net_device *dev);
    int         (*ndo_do_ioctl)(struct net_device *dev,
                            struct ifreq *ifr, int cmd);
    int         (*ndo_set_config)(struct net_device *dev,
                              struct ifmap *map);
    int         (*ndo_change_mtu)(struct net_device *dev,
                          int new_mtu);
    int         (*ndo_neigh_setup)(struct net_device *dev,
                           struct neigh_parms *);
    void            (*ndo_tx_timeout) (struct net_device *dev);

    struct net_device_stats* (*ndo_get_stats)(struct net_device *dev);

    void            (*ndo_vlan_rx_register)(struct net_device *dev,
                                struct vlan_group *grp);
    void            (*ndo_vlan_rx_add_vid)(struct net_device *dev,
                               unsigned short vid);
    void            (*ndo_vlan_rx_kill_vid)(struct net_device *dev,
                                unsigned short vid);
#ifdef CONFIG_NET_POLL_CONTROLLER
    void                    (*ndo_poll_controller)(struct net_device *dev);
    void            (*ndo_netpoll_cleanup)(struct net_device *dev);
#endif
    int         (*ndo_set_vf_mac)(struct net_device *dev,
                          int queue, u8 *mac);
    int         (*ndo_set_vf_vlan)(struct net_device *dev,
                           int queue, u16 vlan, u8 qos);
    int         (*ndo_set_vf_tx_rate)(struct net_device *dev,
                              int vf, int rate);
    int         (*ndo_get_vf_config)(struct net_device *dev,
                             int vf,
                             struct ifla_vf_info *ivf);
    int         (*ndo_set_vf_port)(struct net_device *dev,
                           int vf,
                           struct nlattr *port[]);
    int         (*ndo_get_vf_port)(struct net_device *dev,
                           int vf, struct sk_buff *skb);
#if defined(CONFIG_FCOE) || defined(CONFIG_FCOE_MODULE)
    int         (*ndo_fcoe_enable)(struct net_device *dev);
    int         (*ndo_fcoe_disable)(struct net_device *dev);
    int         (*ndo_fcoe_ddp_setup)(struct net_device *dev,
                              u16 xid,
                              struct scatterlist *sgl,
                              unsigned int sgc);
    int         (*ndo_fcoe_ddp_done)(struct net_device *dev,
                             u16 xid);
#define NETDEV_FCOE_WWNN 0
#define NETDEV_FCOE_WWPN 1
    int         (*ndo_fcoe_get_wwn)(struct net_device *dev,
                            u64 *wwn, int type);
#endif
};



enum netdev_state_t
{
    __LINK_STATE_XOFF=0,
    __LINK_STATE_START,
    __LINK_STATE_PRESENT,
    __LINK_STATE_SCHED,
    __LINK_STATE_NOCARRIER,
    __LINK_STATE_RX_SCHED,
    __LINK_STATE_LINKWATCH_PENDING,
    __LINK_STATE_DORMANT,
    __LINK_STATE_QDISC_RUNNING,
};


#define __be32  u_int32_t

struct skb_shared_info {
    atomic_t    dataref;
    unsigned short  nr_frags;
    unsigned short  gso_size;
    /* Warning: this field is not always filled in (UFO)! */
    unsigned short  gso_segs;
    unsigned short  gso_type;
    __be32          ip6_frag_id;
    struct sk_buff  *frag_list;
};

struct pm_qos_request_list {
    int stub;
};

struct hlist_node {
    int stub;
};

struct list_head {
    int stub;
};

struct net_device_stats {
    unsigned long   rx_packets;     /* total packets received   */
    unsigned long   tx_packets;     /* total packets transmitted    */
    unsigned long   rx_bytes;       /* total bytes received     */
    unsigned long   tx_bytes;       /* total bytes transmitted  */
    unsigned long   rx_errors;      /* bad packets received     */
    unsigned long   tx_errors;      /* packet transmit problems */
    unsigned long   rx_dropped;     /* no space in linux buffers    */
    unsigned long   tx_dropped;     /* no space available in linux  */
    unsigned long   multicast;      /* multicast packets received   */
    unsigned long   collisions;

    /* detailed rx_errors: */
    unsigned long   rx_length_errors;
    unsigned long   rx_over_errors;     /* receiver ring buff overflow  */
    unsigned long   rx_crc_errors;      /* recved pkt with crc error    */
    unsigned long   rx_frame_errors;    /* recv'd frame alignment error */
    unsigned long   rx_fifo_errors;     /* recv'r fifo overrun      */
    unsigned long   rx_missed_errors;   /* receiver missed packet   */

    /* detailed tx_errors */
    unsigned long   tx_aborted_errors;
    unsigned long   tx_carrier_errors;
    unsigned long   tx_fifo_errors;
    unsigned long   tx_heartbeat_errors;
    unsigned long   tx_window_errors;

    /* for cslip etc */
    unsigned long   rx_compressed;
    unsigned long   tx_compressed;
};

struct netdev_hw_addr_list {
    struct list_head    list;
    int         count;
};

struct wireless_dev {
    int         stub;
};

struct netdev_queue {
    int         stub;
};

struct Qdisc {
    int         stub;
};

#ifndef bool
#define bool int
#endif

/* The device structure describes a single device. */
struct device {
    /* The linked-list pointer. */
    struct device *next;

    /* The device's descriptor, as mapped into the Guest. */
    struct lguest_device_desc *desc;

    /* We can't trust desc values once Guest has booted: we use these. */
    unsigned int feature_len;
    unsigned int num_vq;

    /* The name of this device, for --verbose. */
    const char *name;

    /* Any queues attached to this device */
    struct virtqueue *vq;

    /* Is it operational */
    bool running;

    /* Does Guest want an intrrupt on empty? */
    bool irq_on_empty;

    /* Device-specific data. */
    void *priv;
};

struct ethtool_rx_ntuple_list {
#define ETHTOOL_MAX_NTUPLE_LIST_ENTRY 1024
#define ETHTOOL_MAX_NTUPLE_STRING_PER_ENTRY 14
    struct list_head    list;
    unsigned int        count;
};

#if 0
struct sockaddr {
    sa_family_t sa_family;  /* address family, AF_xxx   */
    char        sa_data[14];    /* 14 bytes of protocol address */
};
#endif


struct net_device {

    /*
     * This is the first field of the "visible" part of this structure
     * (i.e. as seen by users in the "Space.c" file).  It is the name
     * the interface.
     */
    char        name[IFNAMSIZ];

    struct pm_qos_request_list *pm_qos_req;

    /* device name hash chain */
    struct hlist_node   name_hlist;
    /* snmp alias */
    char            *ifalias;

    /*
     *I/O specific fields
     *FIXME: Merge these and struct ifmap into one
     */
    unsigned long       mem_end;    /* shared mem end */
    unsigned long       mem_start;  /* shared mem start */
    unsigned long       base_addr;  /* device I/O address */
    unsigned int        irq;        /* device IRQ number */

    /*
     *Some hardware also needs these fields, but they are not
     *part of the usual set specified in Space.c.
     */

    unsigned char       if_port;    /* Selectable AUI, TP,..*/
    unsigned char       dma;        /* DMA channel      */

    unsigned long       state;

    struct list_head    dev_list;
    struct list_head    napi_list;
    struct list_head    unreg_list;

    /* Net device features */
    unsigned long       features;
#define NETIF_F_SG          1   /* Scatter/gather IO. */
#define NETIF_F_IP_CSUM     2   /* Can checksum TCP/UDP over IPv4. */
#define NETIF_F_NO_CSUM     4   /* Does not require checksum. F.e. loopack. */
#define NETIF_F_HW_CSUM     8   /* Can checksum all the packets. */
#define NETIF_F_IPV6_CSUM   16  /* Can checksum TCP/UDP over IPV6 */
#define NETIF_F_HIGHDMA     32  /* Can DMA to high memory. */
#define NETIF_F_FRAGLIST    64  /* Scatter/gather IO. */
#define NETIF_F_HW_VLAN_TX  128 /* Transmit VLAN hw acceleration */
#define NETIF_F_HW_VLAN_RX  256 /* Receive VLAN hw acceleration */
#define NETIF_F_HW_VLAN_FILTER  512 /* Receive filtering on VLAN */
#define NETIF_F_VLAN_CHALLENGED 1024    /* Device cannot handle VLAN packets */
#define NETIF_F_GSO         2048    /* Enable software GSO. */
#define NETIF_F_LLTX        4096    /* LockLess TX - deprecated. Please */
                /* do not use LLTX in new drivers */
#define NETIF_F_NETNS_LOCAL 8192    /* Does not change network namespaces */
#define NETIF_F_GRO     16384   /* Generic receive offload */
#define NETIF_F_LRO     32768   /* large receive offload */

/* the GSO_MASK reserves bits 16 through 23 */
#define NETIF_F_FCOE_CRC    (1 << 24) /* FCoE CRC32 */
#define NETIF_F_SCTP_CSUM   (1 << 25) /* SCTP checksum offload */
#define NETIF_F_FCOE_MTU    (1 << 26) /* Supports max FCoE MTU, 2158 bytes*/
#define NETIF_F_NTUPLE      (1 << 27) /* N-tuple filters supported */
#define NETIF_F_RXHASH      (1 << 28) /* Receive hashing offload */

    /* Segmentation offload features */
#define NETIF_F_GSO_SHIFT   16
#define NETIF_F_GSO_MASK    0x00ff0000
#define NETIF_F_TSO     (SKB_GSO_TCPV4 << NETIF_F_GSO_SHIFT)
#define NETIF_F_UFO     (SKB_GSO_UDP << NETIF_F_GSO_SHIFT)
#define NETIF_F_GSO_ROBUST  (SKB_GSO_DODGY << NETIF_F_GSO_SHIFT)
#define NETIF_F_TSO_ECN     (SKB_GSO_TCP_ECN << NETIF_F_GSO_SHIFT)
#define NETIF_F_TSO6        (SKB_GSO_TCPV6 << NETIF_F_GSO_SHIFT)
#define NETIF_F_FSO     (SKB_GSO_FCOE << NETIF_F_GSO_SHIFT)

    /* List of features with software fallbacks. */
#define NETIF_F_GSO_SOFTWARE    (NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6)


#define NETIF_F_GEN_CSUM    (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)
#define NETIF_F_V4_CSUM     (NETIF_F_GEN_CSUM | NETIF_F_IP_CSUM)
#define NETIF_F_V6_CSUM     (NETIF_F_GEN_CSUM | NETIF_F_IPV6_CSUM)
#define NETIF_F_ALL_CSUM    (NETIF_F_V4_CSUM | NETIF_F_V6_CSUM)

    /*
     * If one device supports one of these features, then enable them
     * for all in netdev_increment_features.
     */
#define NETIF_F_ONE_FOR_ALL (NETIF_F_GSO_SOFTWARE | NETIF_F_GSO_ROBUST | \
                    NETIF_F_SG | NETIF_F_HIGHDMA |  \
                    NETIF_F_FRAGLIST)

    /* Interface index. Unique device identifier    */
    int         ifindex;
    int         iflink;

    struct net_device_stats stats;

#ifdef CONFIG_WIRELESS_EXT
    /* List of functions to handle Wireless Extensions (instead of ioctl).
     * See <net/iw_handler.h> for details. Jean II */
    const struct iw_handler_def *   wireless_handlers;
    /* Instance data managed by the core of Wireless Extensions. */
    struct iw_public_data * wireless_data;
#endif
    /* Management operations */
#ifdef __UT_CENTER__
    struct net_device_ops *netdev_ops;
    struct ethtool_ops *ethtool_ops;
#else
    const struct net_device_ops *netdev_ops;
    const struct ethtool_ops *ethtool_ops;
#endif
    /* Hardware header description */
    const struct header_ops *header_ops;

    unsigned int        flags;  /* interface flags (a la BSD)   */
    unsigned short      gflags;
        unsigned short          priv_flags; /* Like 'flags' but invisible to userspace. */
    unsigned short      padded; /* How much padding added by alloc_netdev() */

    unsigned char       operstate; /* RFC2863 operstate */
    unsigned char       link_mode; /* mapping policy to operstate */

    unsigned int        mtu;    /* interface MTU value      */
    unsigned short      type;   /* interface hardware type  */
    unsigned short      hard_header_len;    /* hardware hdr length  */

    /* extra head- and tailroom the hardware may need, but not in all cases
     * can this be guaranteed, especially tailroom. Some cases also use
     * LL_MAX_HEADER instead to allocate the skb.
     */
    unsigned short      needed_headroom;
    unsigned short      needed_tailroom;

    struct net_device   *master; /* Pointer to master device of a group,
                        * which this device is member of.
                        */

    /* Interface address info. */
    unsigned char       perm_addr[MAX_ADDR_LEN]; /* permanent hw address */
    unsigned char       addr_len;   /* hardware address length  */
    unsigned short          dev_id;     /* for shared network cards */

    spinlock_t      addr_list_lock;
    struct netdev_hw_addr_list  uc; /* Unicast mac addresses */
    struct netdev_hw_addr_list  mc; /* Multicast mac addresses */
    int         uc_promisc;
    unsigned int        promiscuity;
    unsigned int        allmulti;


    /* Protocol specific pointers */

#ifdef CONFIG_NET_DSA
    void            *dsa_ptr;   /* dsa specific data */
#endif
    void            *atalk_ptr; /* AppleTalk link   */
    void            *ip_ptr;    /* IPv4 specific data   */
    void                    *dn_ptr;        /* DECnet specific data */
    void                    *ip6_ptr;       /* IPv6 specific data */
    void            *ec_ptr;    /* Econet specific data */
    void            *ax25_ptr;  /* AX.25 specific data */
    struct wireless_dev *ieee80211_ptr; /* IEEE 802.11 specific data,
                            assign before registering */

/*
 * Cache line mostly used on receive path (including eth_type_trans())
 */
    unsigned long       last_rx;    /* Time of last Rx  */
    /* Interface address info used in eth_type_trans() */
    unsigned char       *dev_addr;  /* hw address, (before bcast
                           because most packets are
                           unicast) */

    struct netdev_hw_addr_list  dev_addrs; /* list of device
                              hw addresses */

    unsigned char       broadcast[MAX_ADDR_LEN];    /* hw bcast add */

#ifdef CONFIG_RPS
    struct kset     *queues_kset;

    struct netdev_rx_queue  *_rx;

    /* Number of RX queues allocated at alloc_netdev_mq() time  */
    unsigned int        num_rx_queues;
#endif

    struct netdev_queue rx_queue;

    struct netdev_queue * ____cacheline_aligned_in_smp;

    /* Number of TX queues allocated at alloc_netdev_mq() time  */
    unsigned int        num_tx_queues;

    /* Number of TX queues currently active in device  */
    unsigned int        real_num_tx_queues;

    /* root qdisc from userspace point of view */
    struct Qdisc        *qdisc;

    unsigned long       tx_queue_len;   /* Max frames per queue allowed */
    spinlock_t      tx_global_lock;
/*
 * One part is mostly used on xmit path (device)
 */
    /* These may be needed for future network-power-down code. */

    /*
     * trans_start here is expensive for high speed devices on SMP,
     * please use netdev_queue->trans_start instead.
     */
    unsigned long       trans_start;    /* Time (in jiffies) of last Tx */

    int         watchdog_timeo; /* used by dev_watchdog() */
    struct timer_list   watchdog_timer;

    /* delayed register/unregister */
    struct list_head    todo_list;
    /* device index hash chain */
    struct hlist_node   index_hlist;

    struct list_head    link_watch_list;

    /* register/unregister state machine */
    enum { NETREG_UNINITIALIZED=0,
           NETREG_REGISTERED,   /* completed register_netdevice */
           NETREG_UNREGISTERING,    /* called unregister_netdevice */
           NETREG_UNREGISTERED, /* completed unregister todo */
           NETREG_RELEASED,     /* called free_netdev */
           NETREG_DUMMY,        /* dummy device for NAPI poll */
    } reg_state:16;

    enum {
        RTNL_LINK_INITIALIZED,
        RTNL_LINK_INITIALIZING,
    } rtnl_link_state:16;

    /* Called from unregister, can be used to call free_netdev */
    void (*destructor)(struct net_device *dev);

#ifdef CONFIG_NETPOLL
    struct netpoll_info *npinfo;
#endif

#ifdef CONFIG_NET_NS
    /* Network namespace this network device is inside */
    struct net      *nd_net;
#endif

    /* mid-layer private */
    void            *ml_priv;

    /* bridge stuff */
    struct net_bridge_port  *br_port;
    /* macvlan */
    struct macvlan_port *macvlan_port;
    /* GARP */
    struct garp_port    *garp_port;

    /* class/net/name entry */
    struct device   dev;
    /* space for optional device, statistics, and wireless sysfs groups */
    const struct attribute_group *sysfs_groups[4];

    /* rtnetlink link ops */
    const struct rtnl_link_ops *rtnl_link_ops;

    /* VLAN feature mask */
    unsigned long vlan_features;

    /* for setting kernel sock attribute on TCP connection setup */
#define GSO_MAX_SIZE        65536
    unsigned int        gso_max_size;

#ifdef CONFIG_DCB
    /* Data Center Bridging netlink ops */
    const struct dcbnl_rtnl_ops *dcbnl_ops;
#endif

#if defined(CONFIG_FCOE) || defined(CONFIG_FCOE_MODULE)
    /* max exchange id for FCoE LRO by ddp */
    unsigned int        fcoe_ddp_xid;
#endif
    /* n-tuple filter list attached to this device */
    struct ethtool_rx_ntuple_list ethtool_ntuple_list;
};

typedef enum _DEVICE_ID
{
    DEVICE_ID_USB = 1,  // USB device = 1
    DEVICE_ID_KEY,      // Keyboard device = 2
    DEVICE_ID_BATTERY,  // Battery device = 3
    DEVICE_ID_CHARGER,  // Charger device = 4
    DEVICE_ID_SD,       // SD card device =  5
    DEVICE_ID_PROCESS,  // Used for monitored process = 6
    DEVICE_ID_SCREEN,   // Screen device = 7
    DEVICE_ID_WLAN, // WLAN device = 8
    DEVICE_ID_OM,   // OM module = 9
    DEVICE_ID_TEMP, // Temperature = 10
    DEVICE_ID_WAN,  // WAN module = 11
    DEVICE_ID_SIM,  //SIM device = 12
    DEVICE_ID_UPDATEONLINE,  //Used for update online ;Add fansaihua
    DEVICE_ID_MAX
} DEVICE_ID;

// Define the device event structure (driver => APP)
typedef struct _DEVICE_EVENT
{
    DEVICE_ID device_id;
    int event_code;
    int len;
    char data[0];
} DEVICE_EVENT;

extern struct net_device *alloc_etherdev_mq(int sizeof_priv, unsigned int queue_count);
#define alloc_etherdev(sizeof_priv) alloc_etherdev_mq(sizeof_priv, 1)

#define HAVE_NET_DEVICE_OPS

struct module
{
    int state;
};
typedef unsigned int nf_hookfn(unsigned int hooknum,
                                struct sk_buff *skb,
                               const struct net_device *in,
                               const struct net_device *out,
                               int (*okfn)(struct sk_buff *));
struct nf_hook_ops {
    struct list_head list;
    nf_hookfn *hook;
    struct module *owner;
    u_int8_t pf;
    unsigned int hooknum;
    int priority;
};

//typedef union sigval
//{
//    int sival_int;
//    void __user *sival_ptr;
//} sigval_t;
//typedef struct sigevent {
//    sigval_t sigev_value;
//    int sigev_signo;
//    int sigev_notify;
//    union {
//        int _pad[SIGEV_PAD_SIZE];
//        int _tid;
//        struct {
//            void (*_function)(sigval_t);
//            void *_attribute;   /* really pthread_attr_t */
//        } _sigev_thread;
//    } _sigev_un;
//} sigevent_t;
#ifndef __ASSEMBLY__
typedef void __signalfn_t(int);
typedef __signalfn_t __user *__sighandler_t;

#endif
//typedef struct siginfo {
//    int si_signo;
//    int si_errno;
//    int si_code;
//};
//struct sigaction {
//    union {
//       __sighandler_t _sa_handler;
//        void (*_sa_sigaction)(int, struct siginfo *, void *);
//    } _u;
//    sigset_t sa_mask;
//    unsigned long sa_flags;
//    void (*sa_restorer)(void);
//};
//#define sa_handler  _u._sa_handler
//#define sa_sigaction    _u._sa_sigaction

struct __wait_queue_head {
    spinlock_t lock;
    struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;

typedef unsigned int      gid_t;
typedef unsigned short    umode_t;

struct mutex {
    atomic_t        count;
    spinlock_t      wait_lock;
    struct list_head    wait_list;
};
struct rw_semaphore {
    spinlock_t      wait_lock;
    struct list_head    wait_list;
};
struct semaphore {
    unsigned int        count;
};
struct inode_operations {
    void (*truncate) (struct inode *);
    int (*permission) (struct inode *, int);
    int (*check_acl)(struct inode *, int);
    void (*truncate_range)(struct inode *, loff_t, loff_t);
};
struct super_block {
    struct list_head    s_list; /* Keep this first */
    unsigned char       s_dirt;
    unsigned char       s_blocksize_bits;
    unsigned long       s_blocksize;
    loff_t              s_maxbytes; /* Max file size */
    char *s_options;
};
struct file_lock
{
     unsigned char       s_dirt;
};
struct inode {
    struct hlist_node   i_hash;
    struct list_head    i_list;     /* backing dev IO list */
    struct list_head    i_sb_list;
    struct list_head    i_dentry;
    unsigned long       i_ino;
    atomic_t            i_count;
    unsigned int        i_nlink;
    uid_t               i_uid;
    gid_t               i_gid;
    unsigned int        i_blkbits;
    u64                 i_version;
    loff_t              i_size;
    struct timespec     i_atime;
    struct timespec     i_mtime;
    struct timespec     i_ctime;
    blkcnt_t            i_blocks;
    unsigned short      i_bytes;
    umode_t             i_mode;
    spinlock_t          i_lock; /* i_blocks, i_bytes, maybe i_size */
    struct mutex        i_mutex;
    struct rw_semaphore i_alloc_sem;
    const struct inode_operations   *i_op;
    const struct file_operations    *i_fop; /* former ->i_op->default_file_ops */
    struct super_block  *i_sb;
    struct file_lock    *i_flock;
    __u32               i_generation;
    unsigned long       i_state;
    unsigned long       dirtied_when;   /* jiffies of first dirtying */
    unsigned int        i_flags;
    atomic_t            i_writecount;
    void                *i_private; /* fs or device private pointer */
};

//struct itimerspec {
//    struct timespec it_interval;    /* timer period */
//    struct timespec it_value;   /* timer expiration */
//};
extern void __wake_up(wait_queue_head_t *q, unsigned int mode);
//#define printk     printf
extern void __init_waitqueue_head(wait_queue_head_t *q, int mode);
extern void init_waitqueue_head(wait_queue_head_t *q);
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif
struct iphdr {
    __u8    tos;
    __be16  tot_len;
    __be16  id;
    __be16  frag_off;
    __u8    ttl;
    __u8    protocol;
    __be32  saddr;
    __be32  daddr;
};
struct udphdr {
    __be16  source;
    __be16  dest;
    __be16  len;
};
struct tcphdr {
    __be16  source;
    __be16  dest;
    __be32  seq;
    __be32  ack_seq;
    __be16  window;
    __be16  check;
    __be16  urg_ptr;
};
struct classStub
{
    const char      *name;
};
#define ntohl(x) (x)
#define ntohs(x) (x)

typedef __u32 __kernel_dev_t;

struct cdev {
    unsigned long  dev;
    unsigned int count;
    struct module *owner;
    const struct file_operations *ops;
};

struct tasklet_struct
{
    struct tasklet_struct *next;
    unsigned long state;
    unsigned long data;
};
typedef int clockid_t;
typedef void (*poll_queue_proc)(struct file *, wait_queue_head_t *, struct poll_table_struct *);
typedef struct poll_table_struct {
    poll_queue_proc qproc;
    unsigned long key;
} poll_table;
#undef __NFDBITS
#define __NFDBITS   (8 * sizeof(unsigned long))
#undef __FD_SETSIZE
#define __FD_SETSIZE    1024
#undef __FDSET_LONGS
#define __FDSET_LONGS   (__FD_SETSIZE/__NFDBITS)
typedef struct {
    unsigned long fds_bits [__FDSET_LONGS];
} fd_set1;
typedef unsigned int        DRV_SIZE_T;
struct proc_dir_entry {
unsigned long proc_entry;
};

#define mdelay(n)   udelay((n) * 1000)

#define LOOKUP_FOLLOW        1
#define LOOKUP_DIRECTORY     2
#define LOOKUP_CONTINUE      4
#define LOOKUP_PARENT       16
#define LOOKUP_REVAL        64

struct dentry {
    char *path;
    char *full_path;
    const char *filename;
    char *link;
    unsigned long size;
    u8 file_type;
    u16 mode;
    u16 uid;
    u16 gid;
    u32 *inode;
    u32 mtime;
};

struct path {
    //struct vfsmount *mnt;
    struct dentry *dentry;
};

#define THIS_MODULE  NULL
//#ifdef __PC_UT__
//
//#else
//
//struct class  *com_class;
//#define class_create(owner, name)  com_class;
//
//#endif

#define kmalloc(size, flags) malloc(size)
#define kfree(ptr) free(ptr)

#define MINORBITS    20
#define MINORMASK   ((1U << MINORBITS) - 1)

#define MAJOR(dev)   ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)   ((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi) (((ma) << MINORBITS) | (mi))

struct file {
    /*
     * fu_list becomes invalid after file_free is called and queued via
     * fu_rcuhead for RCU freeing
     */
    struct path       f_path;
    const struct file_operations    *f_op;
    unsigned int  	f_flags;
    loff_t          f_pos;
    /* needed for tty driver, and maybe others */
    void            *private_data;

    };

//typedef long              ssize_t;
//typedef unsigned long     size_t;
struct file_operations {
    struct module *owner;
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
};
#define KERN_ERR
#define __init
#define __exit

#define NETLINK_HW_LOGCAT   29

#define NLMSG_ALIGNTO	4U
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN	 ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len) + NLMSG_HDRLEN)
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)	 ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
				  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_GOODSIZE	4096

#define NLMSG_DEFAULT_SIZE (NLMSG_GOODSIZE - NLMSG_HDRLEN)

extern struct net init_net;

typedef unsigned int __kernel_uid32_t;
typedef unsigned int __kernel_gid32_t;

typedef uid_t kuid_t;
typedef gid_t kgid_t;

struct scm_creds {
	u32	pid;
	kuid_t	uid;
	kgid_t	gid;
};

struct netlink_skb_parms {
	struct scm_creds	creds;		/* Skb credentials	*/
	__u32			portid;
	__u32			dst_group;
	__u32			flags;
	struct sock		*sk;
};

#define NETLINK_CB(skb)		(*(struct netlink_skb_parms*)&((skb)->cb))

struct nlmsghdr {
	__u32		nlmsg_len;	/* Length of message including header */
	__u16		nlmsg_type;	/* Message content */
	__u16		nlmsg_flags;	/* Additional flags */
	__u32		nlmsg_seq;	/* Sequence number */
	__u32		nlmsg_pid;	/* Sending process port ID */
};


#define MSG_DONTWAIT 0x40

struct netlink_kernel_cfg {
	unsigned int	groups;
	unsigned int	flags;
	void		(*input)(struct sk_buff *skb);
/*
	struct mutex	*cb_mutex;
*/
	void		(*bind)(int group);
};


#ifndef _lint
#define __func__ __FUNCTION__
#endif

#define EMSGSIZE 90
#define ENOBUFS 105

#define LINUX_VERSION_CODE 232626
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))

/*
 * struct clk - an machine class defined object / cookie.
 */
struct clk;

/**
 * clk_get - lookup and obtain a reference to a clock producer.
 * @dev: device for clock "consumer"
 * @id: clock comsumer ID
 *
 * Returns a struct clk corresponding to the clock producer, or
 * valid IS_ERR() condition containing errno.  The implementation
 * uses @dev and @id to determine the clock consumer, and thereby
 * the clock producer.  (IOW, @id may be identical strings, but
 * clk_get may return different clock producers depending on @dev.)
 *
 * Drivers must assume that the clock source is not enabled.
 *
 * clk_get should not be called from within interrupt context.
 */
struct clk *clk_get(struct device *dev, const char *id);

/**
 * clk_enable - inform the system when the clock source should be running.
 * @clk: clock source
 *
 * If the clock can not be enabled/disabled, this should return success.
 *
 * Returns success (0) or negative errno.
 */
int clk_enable(struct clk *clk);

/**
 * clk_disable - inform the system when the clock source is no longer required.
 * @clk: clock source
 *
 * Inform the system that a clock source is no longer required by
 * a driver and may be shut down.
 *
 * Implementation detail: if the clock source is shared between
 * multiple drivers, clk_enable() calls must be balanced by the
 * same number of clk_disable() calls for the clock source to be
 * disabled.
 */
void clk_disable(struct clk *clk);

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern void free_netdev(struct net_device *dev);
extern void netif_carrier_on(struct net_device *dev);
extern void kfree_skbmem(struct sk_buff *skb);
extern void kfree_skb(struct sk_buff *skb);
extern struct sk_buff *dev_alloc_skb(unsigned int length);
extern void kmem_cache_free(struct kmem_cache *cachep, void *objp);
extern struct net_device *alloc_etherdev_mq(int sizeof_priv, unsigned int queue_count);
extern struct net_device *alloc_netdev_mq(int sizeof_priv, const char *name, void (*setup)(struct net_device *), unsigned int queue_count);
extern int register_netdev(struct net_device *dev);
extern int netif_rx(struct sk_buff *skb);
extern void unregister_netdev(struct net_device *dev);
extern void __kfree_skb(struct sk_buff *skb);
extern void netif_carrier_off(struct net_device *dev);
extern struct net_device *alloc_etherdev_mq(int sizeof_priv, unsigned int queue_count);
extern struct net_device *alloc_netdev_mq(int sizeof_priv, const char *name, void (*setup)(struct net_device *), unsigned int queue_count);
/*-------------static------*/
extern struct sk_buff *__dev_alloc_skb(unsigned int length, gfp_t gfp_mask);
extern struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);
extern void skb_reserve(struct sk_buff *skb, int len);
extern __u32 skb_queue_len(const struct sk_buff_head *list_);
extern struct sk_buff *skb_peek(struct sk_buff_head *list_);
extern struct sk_buff *skb_peek_tail(struct sk_buff_head *list_);
extern void skb_release_all(struct sk_buff *skb);
extern void skb_queue_head_init(struct sk_buff_head *list);
extern void __skb_queue_head_init(struct sk_buff_head *list);
extern struct sk_buff *__skb_dequeue(struct sk_buff_head *list);
extern struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list);
extern void __skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk);
extern void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);
extern void netif_stop_queue(struct net_device *dev);
extern void netif_start_queue(struct net_device *dev);
extern int is_valid_ether_addr(const u8 *addr);
extern void netif_wake_queue(struct net_device *dev);
extern void kmem_cache_free(struct kmem_cache *cachep, void *objp);
extern void udelay(int loops);
extern void *memcpy(void *to, const void *from, size_t n);
extern unsigned char *skb_pull(struct sk_buff *skb, unsigned int len);
extern __be16 eth_type_trans(struct sk_buff *skb, struct net_device *dev);
extern unsigned char *skb_put(struct sk_buff *skb, unsigned int len);
extern unsigned char *skb_push(struct sk_buff *skb, unsigned int len);

extern void *netdev_priv(struct net_device *dev);
extern int device_event_report(void *data, int len);
extern long copy_to_user(void *to,const void *from, unsigned long n);

extern unsigned long  copy_from_user(void *to, const void *from, unsigned long n);

extern struct proc_dir_entry *proc_create(const char *name, mode_t mode,struct proc_dir_entry *parent, const struct file_operations *proc_fops);
extern struct proc_dir_entry *proc_mkdir(const char *name,
    struct proc_dir_entry *parent);
extern int printk(const char *s, ...);
extern int kern_path(const char *name, unsigned int flags, struct path *path);
extern int netif_rx_ni_balong(struct sk_buff *skb);
extern unsigned int imajor(const struct inode *inode);
extern int cdev_add(struct cdev *p, dev_t devivce, unsigned int count);
#ifdef __PC_UT__

#else
extern void cdev_init(struct cdev *cdev, const struct file_operations *fops);

extern void cdev_del(struct cdev *p);
//extern void device_create(struct class *cls, struct device *parent,
//                    dev_t devt, void *drvdata,
//                    const char *fmt, ...);
#endif
extern int register_chrdev_region(dev_t dev, unsigned count, const char *name);
extern int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);

extern void unregister_chrdev_region(dev_t from, unsigned count);

extern void  wake_up_interruptible(wait_queue_head_t  *Read_Wait);

extern int wait_event_interruptible(wait_queue_head_t Read_Wait, VOS_UINT32 ulReadWakeUpFlg);

extern unsigned int phys_to_virt(unsigned int ulData);

extern unsigned int virt_to_phys(unsigned int ulData);

extern void sema_init(struct semaphore *sem,int val);
extern void down(struct semaphore  *sem);
extern void up(struct semaphore  *sem);
extern void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p);

extern struct sock *netlink_kernel_create(struct net *net, int unit, struct netlink_kernel_cfg *cfg);
extern void netlink_kernel_release(struct sock *sk);
extern int netlink_unicast(struct sock *ssk, struct sk_buff *skb, __u32 portid, int nonblock);
extern struct nlmsghdr *nlmsg_hdr(const struct sk_buff *skb);
extern struct sk_buff *nlmsg_new(size_t payload, gfp_t flags);
extern void *nlmsg_data(const struct nlmsghdr *nlh);
extern struct nlmsghdr *nlmsg_put(struct sk_buff *skb, u32 portid, u32 seq,
					 int type, int payload, int flags);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
