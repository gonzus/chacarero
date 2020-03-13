#include <stdio.h>
#include "gmem.h"

int gmem_unused = 0;

#if defined(GMEM_CHECK) && GMEM_CHECK >= 1

long gmem_alloc = 0;
long gmem_freed = 0;

static int gmem_inited = 0;

void gmem_init(void) {
    if (gmem_inited) {
        return;
    }

    atexit(gmem_fini);

    gmem_inited = 1;
    gmem_alloc = gmem_freed = 0;

#if defined(GMEM_CHECK) && GMEM_CHECK >= 1
    fprintf(stderr, "=== MEM BEG %ld %ld ===\n", gmem_alloc, gmem_freed);
#endif
}

void gmem_fini(void) {
    if (!gmem_inited) {
        return ;
    }

#if defined(GMEM_CHECK) && GMEM_CHECK >= 1
    fprintf(stderr, "=== MEM END %ld %ld ===\n", gmem_alloc, gmem_freed);
    if (gmem_alloc == gmem_freed) {
        fprintf(stderr, "=== MEM OK ===\n");
    } else {
        fprintf(stderr, "=== MEM ERR %ld BYTES ===\n", gmem_alloc - gmem_freed);
    }
#endif
    gmem_inited = 0;
}

int gmem_called_alloc(const char* file,
        int line,
        void* var,
        int count,
        long size) {
    if (!var) {
        return 0;
    }
    if (size <= 0 || count <= 0) {
        return 0;
    }

    if (!gmem_inited) {
        gmem_init();
    }

    long total = size * count;

#if defined(GMEM_CHECK) && GMEM_CHECK >= 2
    fprintf(stderr, "=== MEM MAL %s %d %p %d %ld %ld ===\n",
            file, line, var, count, size, total);
#endif

    gmem_alloc += total;
    return total;
}

int gmem_called_free(const char* file,
        int line,
        void* var,
        int count,
        long size) {
    if (!var) {
        return 0;
    }
    if (size <= 0 || count <= 0) {
        return 0;
    }

    if (!gmem_inited) {
        gmem_init();
    }

    long total = size * count;

#if defined(GMEM_CHECK) && GMEM_CHECK >= 2
    fprintf(stderr, "=== MEM FRE %s %d %p %d %ld %ld ===\n",
            file, line, var, count, size, total);
#endif

    gmem_freed += total;
    return total;
}

#else

void gmem_init(void) {
}

void gmem_fini(void) {
}

#endif /* #if defined(GMEM_CHECK) && GMEM_CHECK >= 1 */
