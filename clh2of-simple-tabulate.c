/*
  Generates tables of Coulomb matrix elements in a 2D harmonic oscillator
  basis.  The tables are stored in the following format:

      ((<n:uint8> <ml:int8>){4} <value:float64>)*

*/
#include <stdio.h>
#include <stdlib.h>
#include <clh2of.h>

static unsigned shell_index(unsigned char n, signed char ml)
{
    return 2 * n + (unsigned)abs(ml);
}

static unsigned nml_to_p(unsigned char n, signed char ml)
{
    const int k = (int)shell_index(n, ml);
    return (unsigned)(k * (k + 2) + ml) / 2;
}

/* returns 0 if index has overflowed */
static int incr_nml(unsigned char *n,
                    signed char *ml,
                    unsigned *shell_ix,
                    unsigned shell_cap)
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

static int iter_index(struct clh2of_ix *ix,
                      unsigned shell_ix[4],
                      unsigned shell_cap)
{
    ++ix->n4;
    shell_ix[3] += 2;
    while (shell_ix[3] >= shell_cap) {
        const int done =
            incr_nml(&ix->n3, &ix->ml3, &shell_ix[2], shell_cap) &&
            incr_nml(&ix->n2, &ix->ml2, &shell_ix[1], shell_cap) &&
            incr_nml(&ix->n1, &ix->ml1, &shell_ix[0], shell_cap);
        ix->n4 = 0;
        ix->ml4 = ix->ml1 + ix->ml2 - ix->ml3;
        shell_ix[3] = shell_index(ix->n4, ix->ml4);
        if (done) {
            return 0;
        }
    }
    return 1;
}

static unsigned long ordered_fuse(unsigned p1, unsigned p2)
{
    return
        p1 < p2 ?
        (unsigned long)p1 << 16 ^ p2 :
        (unsigned long)p2 << 16 ^ p1;
}

static int is_canonical(const struct clh2of_ix *ix)
{
    const unsigned
        p1 = nml_to_p(ix->n1, ix->ml1),
        p2 = nml_to_p(ix->n2, ix->ml2),
        p3 = nml_to_p(ix->n3, ix->ml3),
        p4 = nml_to_p(ix->n4, ix->ml4);
    const unsigned long
        p12 = ordered_fuse(p1, p2),
        p34 = ordered_fuse(p3, p4);
    return (p1 < p2 || (p1 == p2 && p3 <= p4)) && p12 <= p34;
}

#define PROG "clh2of-simple-tabulate"

static const char *usage =
    "Usage: " PROG " <num_shells> <out_file> [<lambda> [<beta>]]\n\n"
    "Tabulates matrix elements for <num_shells> using OpenFCI.\n\n"
    "Output is stored in the following binary format:\n"
    "((<n:uint8> <ml:int8>){4} <value:float64>)*\n";

int main(int argc, char **argv)
{
    FILE *f;
    char *str;
    const char *out_fn;
    double lambda = 1.0, beta = 0.0;
    unsigned shell_max, shell_ix[4] = {0};
    struct clh2of *ctx;
    static struct {
        struct clh2of_ix ix;
        double value;
    } entry;                       /* declared static to initialize to zero */

    /* print usage if needed */
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "%s", usage);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    /* parse arguments */
    shell_max = (unsigned)strtol(argv[1], &str, 10);
    if (argv[1] == str) {
        fprintf(stderr, PROG ": <num_shells> is invalid\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    out_fn = argv[2];
    if (argc >= 4) {
        lambda = strtod(argv[3], &str);
        if (argv[3] == str) {
            fprintf(stderr, PROG ": <lambda> is invalid\n");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
    }
    if (argc >= 5) {
        beta = strtod(argv[4], &str);
        if (argv[4] == str) {
            fprintf(stderr, PROG ": <beta> is invalid\n");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr,
            "Tabulating matrix elements from "
            "shell #0 to shell #%i (inclusive).\n"
            "lambda = %.17g\n"
            "beta = %.17g\n"
            "Saving to %s ...\n",
            shell_max,
            lambda,
            beta,
            out_fn);
    fflush(stderr);

    /* initialize and open file */
    ctx = clh2of_init(-1);
    if (!ctx ||
        !(f = fopen(out_fn, "wb"))) {
        abort();
    }
    clh2of_set_lambda(ctx, lambda);
    clh2of_set_beta(ctx, beta);
    clh2of_setup(ctx, (int)shell_max, 1);

    /* generate the matrix elements */
    do {
        if (!is_canonical(&entry.ix)) {
            continue;
        }
        entry.value = clh2of_element(ctx, &entry.ix);
        if (fwrite(&entry, sizeof(entry), 1, f) < 1) {
            abort();
        }
    } while (iter_index(&entry.ix, shell_ix, shell_max + 1));

    /* clean up */
    if (fclose(f)) {
        abort();
    }
    clh2of_free(ctx);

    fprintf(stderr, "Done.\n");
    fflush(stderr);
    return 0;
}
