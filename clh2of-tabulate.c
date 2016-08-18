#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <clh2of.h>
#define PROG "clh2of-tabulate"
#include "utils.inl"

static unsigned shell_index(unsigned char n, signed char ml) {
    return 2 * n + (unsigned)abs((int)ml);
}

static unsigned nml_to_p(unsigned char n, signed char ml)
{
    const int k = shell_index(n, ml);
    return (unsigned)(k * (k + 2) + ml) / 2;
}

/* returns 0 if index has overflowed */
static int incr_nml(unsigned char *n, signed char *ml,
                    unsigned *shell_ix, unsigned shell_cap)
{
    if (*ml > -1) {
        if (!*n) {
            *ml = -*ml - 1;
            ++*shell_ix;
            if (*shell_ix >= shell_cap) {
                *ml = 0;
                *shell_ix = 0;
                return 1;
            }
            return 0;
        }
        --*n;
    } else if (*ml < -1) {
        ++*n;
    }
    *ml += 2;
    return 0;
}

static int outer_loop(struct clh2of_ix *ix,
                      unsigned shell_ix[4], unsigned shell_cap)
{
    return
        incr_nml(&ix->n3, &ix->ml3, &shell_ix[2], shell_cap) &&
        incr_nml(&ix->n2, &ix->ml2, &shell_ix[1], shell_cap) &&
        incr_nml(&ix->n1, &ix->ml1, &shell_ix[0], shell_cap);
}

static int inner_loop(struct clh2of_ix *ix,
                      unsigned shell_ix[4], unsigned shell_cap)
{
    ++ix->n4;
    shell_ix[3] += 2;
    while (shell_ix[3] >= shell_cap) {
        const int done = outer_loop(ix, shell_ix, shell_cap);
        ix->n4 = 0;
        ix->ml4 = ix->ml1 + ix->ml2 - ix->ml3;
        shell_ix[3] = abs(ix->ml4);
        if (done)
            return 1;
    }
    return 0;
}

static unsigned ordered_fuse(unsigned p1, unsigned p2)
{
    return p1 < p2 ? p1 << 16 ^ p2 : p2 << 16 ^ p1;
}

static sqlite3_int64 canonicalize(struct clh2of_ix *ix)
{
    const unsigned
        p1 = nml_to_p(ix->n1, ix->ml1),
        p2 = nml_to_p(ix->n2, ix->ml2),
        p3 = nml_to_p(ix->n3, ix->ml3),
        p4 = nml_to_p(ix->n4, ix->ml4);
    const unsigned long
        p12 = ordered_fuse(p1, p2),
        p34 = ordered_fuse(p3, p4);
    if ((p1 > p2 && (p1 == p2 || p3 > p4)) || p12 > p34)
        return -1;
    return
        (sqlite3_int64)p1 << 48 ^
        (sqlite3_int64)p2 << 32 ^
        (sqlite3_int64)p3 << 16 ^
        (sqlite3_int64)p4;
}

int main(int argc, char **argv)
{
    static const char *usage =
        "Usage: " PROG " NUM_SHELLS OUTPUT_FILENAME\n\n"
        "Tabulates matrix elements for NUM_SHELLS using OpenFCI.\n\n"
        "Output is stored in SQLite format.\n";

    struct clh2of *ctx;
    struct clh2of_ix ix = {0}, ixc;
    unsigned shell_cap, shell_ix[4] = {0};
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *str;
    const char *out_fn;

    /* print usage if needed */
    if (argc != 3) {
        fprintf(stderr, usage);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    /* parse arguments */
    shell_cap = strtol(argv[1], &str, 10);
    if (argv[1] == str) {
        fprintf(stderr, "clh2of-tabulate: MAX_NUM_SHELLS is invalid\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    ++shell_cap;
    out_fn = argv[2];

    printf("Tabulating matrix elements from shell #0 "
           "to shell #%i (inclusive).\n"
           "Initializing ...\n",
           shell_cap - 1);
    fflush(stdout);

    /* initialization */
    ctx = clh2of_init(shell_cap - 1);
    if (sqlite3_config(SQLITE_CONFIG_LOG, error_sqlite, NULL))
        panic();

    printf("Saving to %s ...\n", out_fn);
    fflush(stdout);

    if (sqlite3_open(out_fn, &db))
        panic();

    if (sqlite3_exec(
            db,
            "CREATE TABLE IF NOT EXISTS data "
            "(key INTEGER PRIMARY KEY, value REAL);",
            NULL, NULL, NULL))
        panic();

    if (sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL))
        panic();

    if (sqlite3_prepare_v2(
            db,
            "INSERT OR IGNORE INTO data (key, value) VALUES (?, ?);",
            -1, &stmt, NULL))
        panic();

    do {
        sqlite3_int64 key;
        double value;

        ixc = ix;
        key = canonicalize(&ixc);
        if (key < 0)
            continue;
        value = clh2of_element(ctx, &ixc);

        if (sqlite3_bind_int64(stmt, 1, key))
            panic();

        if (sqlite3_bind_double(stmt, 2, value))
            panic();

        if (sqlite3_step(stmt) != SQLITE_DONE)
            panic();

        if (sqlite3_reset(stmt))
            panic();

    } while (!inner_loop(&ix, shell_ix, shell_cap));

    if (sqlite3_finalize(stmt))
        panic();

    if (sqlite3_exec(db, "END; VACUUM;", NULL, NULL, NULL))
        panic();

    if (sqlite3_close(db))
        panic();

    clh2of_free(ctx);

    printf("Done.\n");
    return 0;
}
