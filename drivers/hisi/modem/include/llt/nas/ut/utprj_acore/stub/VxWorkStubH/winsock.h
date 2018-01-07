

#ifndef __WINSOCK
#define __WINSOCK

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr {
    unsigned short int sa_family;              /* address family */
    char    sa_data[14];            /* up to 14 bytes of direct address */
};

#ifdef __cplusplus
}
#endif

#endif /* __WINSOCK */
