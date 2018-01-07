/* hi3xxx_mailbox_debugfs.h */

#ifndef __HI3XXX_MAILBOX_DEBUGFS_H__
#define __HI3XXX_MAILBOX_DEBUGFS_H__

#ifdef CONFIG_HI3XXX_MAILBOX_DEBUGFS
extern void mbox_debugfs_register(struct hisi_mbox_device **list);
extern void mbox_debugfs_unregister(void);
#else
static inline void mbox_debugfs_register(struct hisi_mbox_device **list)
{
	return;
}

static inline void mbox_debugfs_unregister(void)
{
	return;
}
#endif /* CONFIG_HI3XXX_MAILBOX_DEBUGFS */

#endif /* hi3xxx_mailbox_debugfs.h */
