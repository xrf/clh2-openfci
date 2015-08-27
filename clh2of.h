#ifndef G_5BX2DN6LUMV5PK267O26LTBVFOMZR
#define G_5BX2DN6LUMV5PK267O26LTBVFOMZR
#ifdef __cplusplus
extern "C" {
#endif

struct clh2of_ix {
    unsigned char n1;
    signed char ml1;
    unsigned char n2;
    signed char ml2;
    unsigned char n3;
    signed char ml3;
    unsigned char n4;
    signed char ml4;
};

/** `shell_index_max` can be omitted by providing a negative value */
struct clh2of *clh2of_init(int shell_index_max);

void clh2of_setup(struct clh2of *self, int shell_index_max, int exact);

double clh2of_element(struct clh2of *self, const struct clh2of_ix *ix);

void clh2of_free(struct clh2of *self);

#ifdef __cplusplus
}
#endif
#endif
