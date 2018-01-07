#ifndef _INCLUDE_LINUX_ION_H_
#define _INCLUDE_LINUX_ION_H_
#include "../../drivers/staging/android/uapi/ion.h"
#ifdef CONFIG_ION_HI3XXX
#include "../../drivers/staging/android/ion-hi3xxx/ion.h"
#else
#include "../../drivers/staging/android/ion/ion.h"
#endif
#endif /* _INCLUDE_LINUX_ION_H_ */
