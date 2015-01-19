#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <exception>
#include <quantumdot/QdotInteraction.hpp>
#include <protocol.h>

static const char *prog;

// calculate shell index
static int shell_index(unsigned n, int ml) {
    return 2 * (int) n + abs(ml);
}

struct clh2_ctx {

    // sentinel value
    static const int initial_shell_index_cap = -1;

    // internal state used by OpenFCI
    quantumdot::QdotInteraction q;

    // maximum shell index allowed by `q` (be sure to keep this in sync)
    int shell_index_cap;

    clh2_ctx() : shell_index_cap(initial_shell_index_cap) {}

    // can throw exceptions
    void setup(int shell_index_max) {
        if (shell_index_cap >= shell_index_max)
            return;
        if (shell_index_cap == initial_shell_index_cap) {
            // must begin at a positive integer
            shell_index_cap = 2;
        }
        do {
            shell_index_cap *= 2;
        } while (shell_index_cap < shell_index_max);
        // increase the shell index; note that we can't reuse the internal
        // state (things will break), so we must construct a new one
        q = quantumdot::QdotInteraction();
        q.setR(shell_index_cap);
        q.buildInteractionComBlocks();
    }

    // must never throw exceptions
    double element(const struct clh2_indicesp &ix) {
        const int N1 = shell_index(ix.n1, ix.ml1);
        const int N2 = shell_index(ix.n2, ix.ml2);
        const int N3 = shell_index(ix.n3, ix.ml3);
        const int N4 = shell_index(ix.n4, ix.ml4);
        const int N_max = std::max(std::max(N1, N2), std::max(N3, N4));
        try {
            setup(N_max);
            // a sign correction is needed when using `q.singleElement` (it is
            // not needed for `q.singleElementAnalytic` however)
            const int sign = (ix.n1 + ix.n2 + ix.n3 + ix.n4) % 2 ? -1 : 1;
            return sign * q.singleElement(N1, ix.ml1, N2, ix.ml2,
                                          N3, ix.ml3, N4, ix.ml4);
        } catch (const std::exception& e) {
            (void) fprintf(stderr, "%s: warning: exception: %s\n",
                           prog, e.what());
        } catch (...) {
            (void) fprintf(stderr, "%s: warning: unknown exception\n", prog);
        }
        // since we've no idea if the internal state of `q` would remain valid
        // after an exception, let's just start over; hopefully the assignment
        // operator doesn't throw!
        q = quantumdot::QdotInteraction();
        shell_index_cap = initial_shell_index_cap;
        return NAN;
    }

};

int main(int argc, char **argv) {
    clh2_ctx ctx;
    clh2_main_init(&prog, &argc, &argv);

    for (; *argv; ++argv) {
        union clh2_cell *data, *p;
        size_t count;

        clh2_open_request(&data, &count, prog, *argv);
        for (p = data; p != data + count; ++p)
            p->value = ctx.element(p->indices);
        clh2_close_request(data, count);
    }

    return EXIT_SUCCESS;
}
