//#include <stdlab.h>
#include "skbuff.h"

void kfree(const void *p){
	free(p);
}

void *kmalloc(VOS_UINT32 size, VOS_UINT32 flags)
{
	return malloc(size);
}