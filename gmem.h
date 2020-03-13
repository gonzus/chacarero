#ifndef GMEM_H_
#define GMEM_H_

/*
 * A set of macros / functions to allocate and release dynamic memory.
 *
 * When compiling with GMEM_CHECK defined, the macros keep track of all memory
 * allocation and deallocation, and a summary is printed out at the end; when
 * it is undefined, the macros are equivalent to raw calls to malloc / realloc
 * / free, thus incurring no runtime cost; additionally, in this case when
 * freeing, the freed variable is set to zero.
 *
 * Examples for calling the macros:
 *
 *   GMEM_MALLOC(header, Header*, sizeof(Header))
 *         => header = (Header*) malloc(sizeof(Header));
 *
 *   GMEM_REALLOC(data, Data*, 20, 30)
 *         => data = (Data*) realloc(data, 30); // set last 10 elements to 0
 *
 *   GMEM_FREE(header, Header*, sizeof(Header))
 *         => free(header); header = 0;
 *
 *   GMEM_STR_DUP(str, "hello", 4)
 *         => str = malloc(4+1); memcpy(str, "hello", 4); str[4] = '\0';
 *   GMEM_STR_DUP(str, "hello", 0)
 *         => l = strlen("hello"); str = malloc(l+1); memcpy(str, "hello", l); str[l] = '\0';
 *
 *   GMEM_STR_FREE(str, 0)
 *         => free(str); str = 0;
 */

#include <stdlib.h>
#include <string.h>

#define _GMEM_RAW_MALLOC(scalar, type, size) \
    do { \
        scalar = (type) malloc(size); \
    } while (0)
#define _GMEM_RAW_REALLOC(scalar, type, osize, nsize) \
    do { \
        scalar = (type) realloc(scalar, nsize); \
    } while (0)
#define _GMEM_RAW_FREE(scalar, type, size) \
    do { \
        free(scalar); \
        scalar = 0; \
    } while (0)

void gmem_init(void);
void gmem_fini(void);

#if !defined(GMEM_CHECK) || GMEM_CHECK < 1

#define GMEM_MALLOC(scalar, type, size)           _GMEM_RAW_MALLOC(scalar, type, size)
#define GMEM_REALLOC(scalar, type, osize, nsize)  _GMEM_RAW_REALLOC(scalar, type, osize, nsize)
#define GMEM_FREE(scalar, type, size)             _GMEM_RAW_FREE(scalar, type, size)

#define GMEM_STR_DUP(tgt, src, len) \
    do { \
        (tgt) = 0; \
        if (!(src)) { \
            break; \
        } \
        int l = (len) <= 0 ? strlen(src) : (len); \
        _GMEM_RAW_MALLOC(tgt, char*, l+1); \
        memcpy(tgt, src, l); \
        (tgt)[l] = '\0'; \
    } while (0)
#define GMEM_STR_FREE(str, len) \
    do { \
        if (!(str)) { \
            break; \
        } \
        int l = (len) <= 0 ? strlen(str) : (len); \
        _GMEM_RAW_FREE(str, char*, l+1); \
    } while (0)

#else

#define GMEM_MALLOC(scalar, type, size) \
    do { \
        _GMEM_RAW_MALLOC(scalar, type, size); \
        gmem_called_alloc(__FILE__, __LINE__, scalar, 1, size); \
    } while (0)
#define GMEM_REALLOC(scalar, type, osize, nsize) \
    do { \
        gmem_called_free(__FILE__, __LINE__, scalar, 1, osize); \
        _GMEM_RAW_REALLOC(scalar, type, osize, nsize); \
        gmem_called_alloc(__FILE__, __LINE__, scalar, 1, nsize); \
    } while (0)
#define GMEM_FREE(scalar, type, size) \
    do { \
        gmem_called_free(__FILE__, __LINE__, scalar, 1, size); \
        _GMEM_RAW_FREE(scalar, type, size); \
    } while (0)
#define GMEM_STR_DUP(tgt, src, len) \
    do { \
        (tgt) = 0; \
        if (!(src)) { \
            break; \
        } \
        int l = (len) <= 0 ? strlen(src) : (len); \
        _GMEM_RAW_MALLOC(tgt, char*, l+1); \
        gmem_called_alloc(__FILE__, __LINE__, tgt, l+1, 1); \
        memcpy(tgt, src, l); \
        (tgt)[l] = '\0'; \
    } while (0)
#define GMEM_STR_FREE(str, len) \
    do { \
        if (!(str)) { \
            break; \
        } \
        int l = (len) <= 0 ? strlen(str) : (len); \
        gmem_called_free(__FILE__, __LINE__, str, l+1, 1); \
        _GMEM_RAW_FREE(str, char*, l+1); \
    } while (0)

extern long gmem_alloc;
extern long gmem_freed;

int gmem_called_alloc(const char* file,
        int line,
        void* var,
        int count,
        long size);
int gmem_called_free(const char* file,
        int line,
        void* var,
        int count,
        long size);

#endif /* #if !defined(GMEM_CHECK) || GMEM_CHECK < 1 */

#endif
