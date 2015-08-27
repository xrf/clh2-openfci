#include <stdio.h>
#include <stdlib.h>

static void error_sqlite(void *ctx, int err, const char *msg)
{
    (void)ctx;
    fprintf(stderr, PROG ": [sqlite] %s (%d)\n", msg, err);
    fflush(stderr);
}

static void panic(void)
{
    fprintf(stderr,
            PROG ": unexpected error\n"
            PROG ": run through a debugger for more info\n");
    fflush(stderr);
    abort();
}
