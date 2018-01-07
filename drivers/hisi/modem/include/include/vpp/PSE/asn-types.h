#ifndef	_IPSI_ASN_TYPES_H
#define	_IPSI_ASN_TYPES_H

#include "ipsi_pse_build_conf.h"
#include "ipsi_types.h"
#include "sec_sys.h"

/**
* @defgroup asn_types
* This section contains asn-types Structures and Enum.
*/



/**
* @defgroup asn_typesStructures
* @ingroup asn_types
* This section contains asn-types Structures.
*/

/**
* @defgroup SEC_AsnOcts_S
* @ingroup asn_typesStructures
* @code
* typedef struct stSEC_ASNOCTS
* {
* 	SEC_UINT32 octetLen;
* 	SEC_CHAR*  octs;
* } SEC_AsnOcts_S;
* @endcode
*
* @datastruct octetLen Length of the octet buffer.
* @datastruct octs Buffer for octets.
*/
typedef struct stSEC_ASNOCTS{
	SEC_UINT32 octetLen;
	SEC_CHAR* octs;
} SEC_AsnOcts_S;

typedef SEC_AsnOcts_S SEC_AsnOid_S;

/**
* @defgroup SEC_AsnBits_S
* @ingroup asn_typesStructures
* @code
* typedef struct stSEC_ASNBITS
* {
* 	SEC_UINT bitLen;
* 	SEC_CHAR *bits;
* } SEC_AsnBits_S;
* @endcode
*
* @datastruct bitLen Length of the bits buffer.
* @datastruct bits Buffer for bits.
*/
typedef struct stSEC_ASNBITS{
	SEC_UINT bitLen; /*Length of the bits buffer*/
	SEC_CHAR *bits;  /*Buffer for bits*/
	SEC_UINT noBytes; /*NO of bytes in th bits*/
} SEC_AsnBits_S;

//typedef struct AnyInfo SEC_ANYINFO_S;
typedef void* SEC_AsnAny_S;



typedef SEC_AsnAny_S SEC_AsnAnyDefinedBy_S;

/* ASN.1 Basic types */
//typedef unsigned short SEC_WCHAR;
typedef SEC_UCHAR SEC_AsnBool;
typedef SEC_UINT32 SEC_AsnLen;
/* ASN.1 integer needs to be 4 bytes for encoder */
typedef SEC_INT32 SEC_AsnInt;
typedef SEC_UINT32 SEC_AsnTag;
typedef SEC_AsnInt SEC_AsnEnum;
typedef SEC_CHAR SEC_AsnNull;
typedef SEC_AsnOcts_S SEC_AsnRelativeOid_S;

/* Basic ASN.1 string types */
typedef SEC_AsnOcts_S SEC_VisibleString_S;
typedef SEC_AsnOcts_S SEC_TeletexString_S;
typedef SEC_AsnOcts_S SEC_UTF8String_S;
typedef SEC_AsnOcts_S SEC_UniversalString_S;
typedef SEC_AsnOcts_S SEC_PrintableString_S;
typedef SEC_AsnOcts_S SEC_NumericString_S;
typedef SEC_AsnOcts_S SEC_IA5String_S;
typedef SEC_AsnOcts_S SEC_BMPString_S;
typedef SEC_AsnOcts_S SEC_UTCTime_S;
typedef SEC_AsnOcts_S SEC_GeneralizedTime_S;

#endif // _IPSI_ASN_TYPES_H

