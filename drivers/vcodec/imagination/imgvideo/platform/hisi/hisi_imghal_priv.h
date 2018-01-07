#ifndef IMGHAL_PRIV_H_
#define IMGHAL_PRIV_H_

#include <gralloc_priv.h>

typedef struct private_handle_t IMGHAL_HANDLE_T; 
/*
#define IMGHAL_GetColorFormat(handle) (((struct private_handle_t *)(handle))->format)
#define IMGHAL_GetStride(handle) (((struct private_handle_t *)(handle))->stride)
#define IMGHAL_GetTileScheme(handle) (((struct private_handle_t *)(handle))->tileScheme)
#define IMGHAL_GetSize(handle) (((struct private_handle_t *)(handle))->size)
#define IMGHAL_GetChromaOffset(handle) (((struct private_handle_t *)(handle))->chromaOffset)

#define IMGHAL_Has128byteInterleave(handle) (((struct private_handle_t *)(handle))->has128byteInterleave)
#define IMGHAL_IsPacked(handle) (((struct private_handle_t *)(handle))->isPacked)
*/
#define IMGHAL_GetColorFormat(handle) (((IMGHAL_HANDLE_T *)(handle))->format)
#define IMGHAL_GetStride(handle) (((IMGHAL_HANDLE_T *)(handle))->byte_stride)
#define IMGHAL_GetTileScheme(handle) ((((IMGHAL_HANDLE_T *)(handle))->k3_alloc_type == K3_TILE_ALLOC_TYPE) ? 1 : 0)
#define IMGHAL_GetSize(handle) (((IMGHAL_HANDLE_T *)(handle))->size)
#define IMGHAL_GetChromaOffset(handle) 0

#define IMGHAL_Has128byteInterleave(handle) ((((IMGHAL_HANDLE_T *)(handle))->k3_alloc_type == K3_TILE_ALLOC_TYPE) ? 1 : 0)
#define IMGHAL_IsPacked(handle) 0

#define PLATFORM_PIXEL_FORMAT HAL_PIXEL_FORMAT_YCbCr_420_SP


#endif /* IMGHAL_PRIV_H_ */
