

#ifndef __HW_ALAN_KERNEL_COMPAT32_H__
#define __HW_ALAN_KERNEL_COMPAT32_H__

#include <linux/videodev2.h>
#include <linux/compat.h>
#include <linux/uaccess.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>

struct v4l2_event32 {
	__u32				type;
	union {
		struct v4l2_event_vsync		vsync;
		struct v4l2_event_ctrl		ctrl;
		struct v4l2_event_frame_sync	frame_sync;
		__u8			data[128];
	} u;
	__u32				pending;
	__u32				sequence;
#ifdef CONFIG_COMPAT	
	struct compat_timespec		timestamp;
#else
	struct timespec		timestamp;
#endif
	__u32				id;
	__u32				reserved[8];
};

struct v4l2_clip32 {
	struct v4l2_rect        c;
	compat_caddr_t 		next;
};

struct v4l2_window32 {
	struct v4l2_rect        w;
	__u32		  	field;	/* enum v4l2_field */
	__u32			chromakey;
	compat_caddr_t		clips; /* actually struct v4l2_clip32 * */
	__u32			clipcount;
	compat_caddr_t		bitmap;
};


struct v4l2_format32 {
	__u32	type;	/* enum v4l2_buf_type */
	union {
		struct v4l2_pix_format	pix;
		struct v4l2_pix_format_mplane	pix_mp;
		struct v4l2_window32	win;
		struct v4l2_vbi_format	vbi;
		struct v4l2_sliced_vbi_format	sliced;
		__u8	raw_data[200];        /* user-defined */
	} fmt;
};

#define VIDIOC_G_FMT32		_IOWR('V',  4, struct v4l2_format32)
#define VIDIOC_S_FMT32		_IOWR('V',  5, struct v4l2_format32)

#define	VIDIOC_DQEVENT32	_IOR ('V', 89, struct v4l2_event32)
#define HWCAM_V4L2_IOCTL_REQUEST_ACK32   _IOW('A', BASE_VIDIOC_PRIVATE + 0x20, struct v4l2_event32)
#define HWCAM_V4L2_IOCTL_NOTIFY32	_IOW('A', BASE_VIDIOC_PRIVATE + 0x21, struct v4l2_event32)

long compat_get_v4l2_event_data(struct v4l2_event *kp, struct v4l2_event32 __user *up);
long compat_put_v4l2_event_data(struct v4l2_event *kp, struct v4l2_event32 __user *up);
long compat_get_v4l2_format_data(struct v4l2_format *kp, struct v4l2_format32 __user *up);
long compat_put_v4l2_format_data(struct v4l2_format *kp, struct v4l2_format32 __user *up);


#endif
