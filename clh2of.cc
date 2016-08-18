#include <stdlib.h>
#include <algorithm>
#include <clh2of.h>
#include <quantumdot/QdotInteraction.hpp>

// calculate shell index
static int shell_index(unsigned char n, signed char ml) {
    return 2 * n + abs((int)ml);
}

struct clh2of {

    // sentinel value
    static const int initial_shell_index_cap = -1;

    // internal state used by OpenFCI
    quantumdot::QdotInteraction q;

    // maximum shell index allowed by `q` (be sure to keep this in sync)
    int shell_index_cap;

    clh2of(int shell_index_max = initial_shell_index_cap)
        : shell_index_cap(initial_shell_index_cap) {
        if (shell_index_max >= 0)
            setup(shell_index_max, true);
    }

    // can throw exceptions
    void setup(int shell_index_max, bool exact = false) {
        if (shell_index_cap == shell_index_max)
            return;
        if (exact) {
            shell_index_cap = shell_index_max;
        } else {
            if (shell_index_cap > shell_index_max)
                return;
            if (shell_index_cap == initial_shell_index_cap) {
                // must begin at a positive integer
                shell_index_cap = 2;
            }
            do {
                shell_index_cap *= 2;
            } while (shell_index_cap < shell_index_max);
        }
        // increase the shell index; note that we can't reuse the internal
        // state (things will break), so we must construct a new one
        q = quantumdot::QdotInteraction();
        q.setR(shell_index_cap);
        q.buildInteractionComBlocks();
    }

    // can throw exceptions
    double element(const struct clh2of_ix &ix) {
        const int N1 = shell_index(ix.n1, ix.ml1);
        const int N2 = shell_index(ix.n2, ix.ml2);
        const int N3 = shell_index(ix.n3, ix.ml3);
        const int N4 = shell_index(ix.n4, ix.ml4);
        const int N_max = std::max(std::max(N1, N2), std::max(N3, N4));
        setup(N_max);
        // a sign correction is needed when using `q.singleElement` (it is
        // not needed for `q.singleElementAnalytic` however)
        const int sign = (ix.n1 + ix.n2 + ix.n3 + ix.n4) % 2 ? -1 : 1;
        return sign * q.singleElement(N1, ix.ml1, N2, ix.ml2,
                                      N3, ix.ml3, N4, ix.ml4);
    }

};

extern "C" {

/* shell_index_max can be negative, in which case it's "auto" */
struct clh2of *clh2of_init(int shell_index_max)
{
    return new (std::nothrow) struct clh2of(shell_index_max);
}

void clh2of_setup(struct clh2of *self, int shell_index_max, int exact)
{
    return self->setup(shell_index_max, (bool)exact);
}

double clh2of_element(struct clh2of *self, const struct clh2of_ix *ix)
{
    return self->element(*ix);
}

void clh2of_free(struct clh2of *self)
{
    delete self;
}

}
