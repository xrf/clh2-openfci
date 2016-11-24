/*
  Tables are in the following format:

      ((<n:uint8> <ml:int8>){4} <value:float64>)*

  Commands:

  add: Reads two tables and merges them.
       Both tables must contain the indices in the same ordering.

  sort: Reads a table and sort it in place by the first 64-bits of each table
        entry, interpreted as a uint64_t.

  sortk: Reads a table and sort it in place by the highest shell index among
         the four indices.  (This is not a stable sort.)

 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "clh2of.h"

struct entry {
    struct clh2of_ix index;
    double value;
};

static unsigned shell_index(unsigned char n, signed char ml)
{
    return 2 * n + (unsigned)abs(ml);
}

static unsigned max_shell_index(struct clh2of_ix ix)
{
    unsigned k1 = shell_index(ix.n1, ix.ml1);
    unsigned k2 = shell_index(ix.n2, ix.ml2);
    unsigned k3 = shell_index(ix.n3, ix.ml3);
    unsigned k4 = shell_index(ix.n4, ix.ml4);
    unsigned k12 = k1 >= k2 ? k1 : k2;
    unsigned k34 = k3 >= k4 ? k3 : k4;
    unsigned k1234 = k12 >= k34 ? k12 : k34;
    return k1234;
}

static int main_add(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: ... add <file1> <file2> <file3>\n");
        return EXIT_FAILURE;
    }
    FILE *f1 = fopen(argv[1], "rb");
    FILE *f2 = fopen(argv[2], "rb");
    FILE *f3 = fopen(argv[3], "wb");
    if (f1 == NULL || f2 == NULL || f3 == NULL) {
        fprintf(stderr, "failed to open file(s)\n");
        return EXIT_FAILURE;
    }

    unsigned long i;
    for (i = 0; ; ++i) {
        struct entry e1, e2;
        size_t n1 = fread(&e1, sizeof(struct entry), 1, f1);
        size_t n2 = fread(&e2, sizeof(struct entry), 1, f2);
        if (n1 != n2) {
            fprintf(stderr, "one file is shorter than other\n");
            return EXIT_FAILURE;
        }
        if (memcmp(&e1.index, &e2.index, sizeof(struct clh2of_ix)) != 0) {
            fprintf(stderr,
                    "entry #%lu: indices do not match: "
                    "%u %i %u %i %u %i %u %i != "
                    "%u %i %u %i %u %i %u %i\n",
                    i,
                    e1.index.n1, e1.index.ml1,
                    e1.index.n2, e1.index.ml2,
                    e1.index.n3, e1.index.ml3,
                    e1.index.n4, e1.index.ml4,
                    e2.index.n1, e2.index.ml1,
                    e2.index.n2, e2.index.ml2,
                    e2.index.n3, e2.index.ml3,
                    e2.index.n4, e2.index.ml4);
            return EXIT_FAILURE;
        }
        if (n1 != 1) {
            break;
        }
        struct entry e3 = {e1.index, e1.value + e2.value};
        fwrite(&e3, sizeof(struct entry), 1, f3);
    }

    return 0;
}

static void *mmap_file(const char *fn, size_t *size_out)
{
    int fd = open(fn, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "can't open file\n");
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) != 0) {
        fprintf(stderr, "can't stat file\n");
        return NULL;
    }
    size_t size = (size_t)st.st_size;

    if (size % sizeof(struct entry) != 0) {
        fprintf(stderr, "invalid file length\n");
        return NULL;
    }

    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) {
        fprintf(stderr, "can't mmap file\n");
        return NULL;
    }
    *size_out = size;
    return ptr;
}

static int comp(const void *leftptr, const void *rightptr)
{
    uint64_t left = *(const uint64_t *)leftptr;
    uint64_t right = *(const uint64_t *)rightptr;
    return (right < left) - (left < right);
}

static int main_sort(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: ... sort <file> #in-place!\n");
        return EXIT_FAILURE;
    }
    const char *fn = argv[1];
    size_t size = 0;
    void *ptr = mmap_file(fn, &size);
    size_t count = size / sizeof(struct entry);
    if (!ptr) {
        return EXIT_FAILURE;
    }

    qsort(ptr, count, sizeof(struct entry), &comp);

    munmap(ptr, size);
    return 0;
}

static int compk(const void *leftptr, const void *rightptr)
{
    unsigned left = max_shell_index(*(const struct clh2of_ix *)leftptr);
    unsigned right = max_shell_index(*(const struct clh2of_ix *)rightptr);
    return (right < left) - (left < right);
}

static int main_sortk(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: ... sortk <file> #in-place!\n");
        return EXIT_FAILURE;
    }
    const char *fn = argv[1];
    size_t size = 0;
    void *ptr = mmap_file(fn, &size);
    size_t count = size / sizeof(struct entry);
    if (!ptr) {
        return EXIT_FAILURE;
    }

    qsort(ptr, count, sizeof(struct entry), &compk);

    munmap(ptr, size);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        goto usage;
    } else if (strcmp(argv[1], "add") == 0) {
        return main_add(argc - 1, argv + 1);
    } else if (strcmp(argv[1], "sort") == 0) {
        return main_sort(argc - 1, argv + 1);
    } else if (strcmp(argv[1], "sortk") == 0) {
        return main_sortk(argc - 1, argv + 1);
    } else {
        goto usage;
    }
usage:
    fprintf(stderr, "Usage: clh2of-simple-mod (add|sort) ...\n");
    return EXIT_FAILURE;
}
