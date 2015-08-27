#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#define PROG "clh2of-pack"
#include "utils.inl"

struct buffered_writer {
    FILE *stream;
    size_t position;
    char buffer[BUFSIZ];
};

static void bw_init(struct buffered_writer *self, FILE *stream)
{
    self->stream = stream;
    self->position = 0;
}

static int bw_write(struct buffered_writer *self,
                    const void *data,
                    size_t size)
{
    const char *src = (const char *)data;
    while (1) {
        const size_t remaining = BUFSIZ - self->position;
        char *const dest = self->buffer + self->position;
        if (remaining > size) {
            memcpy(dest, src, size);
            self->position += size;
            return 0;
        }
        int r;
        memcpy(dest, src, remaining);
        size -= remaining;
        src += remaining;
        r = fwrite(self->buffer, BUFSIZ, 1, self->stream);
        if (!r)
            return 1;
        self->position = 0;
    }
}

static int bw_flush(struct buffered_writer *self)
{
    const int r = fwrite(self->buffer, self->position, 1, self->stream);
    if (!r)
        return 1;
    self->position = 0;
    return 0;
}

int main(int argc, char **argv)
{
    static const char *usage =
        "Usage: " PROG " INPUT_FILENAME OUTPUT_FILENAME\n\n"
        "Packs matrix elements into binary format: "
        "(<key:int64> <value:float64>)*\n";

    struct buffered_writer bw;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *in_fn, *out_fn;

    /* print usage if needed */
    if (argc != 3) {
        fprintf(stderr, usage);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    /* parse arguments */
    in_fn = argv[1];
    out_fn = argv[2];

    bw_init(&bw, fopen(out_fn, "w"));

    if (sqlite3_config(SQLITE_CONFIG_LOG, error_sqlite, NULL))
        panic();

    if (sqlite3_open(in_fn, &db))
        panic();

    if (sqlite3_prepare_v2(db, "SELECT key, value FROM data;",
                           -1, &stmt, NULL))
        panic();

    while (1) {
        struct {
            sqlite3_int64 key;
            double value;
        } row;

        const int e = sqlite3_step(stmt);
        if (e == SQLITE_DONE)
            break;
        else if (e != SQLITE_ROW)
            panic();

        row.key = sqlite3_column_int64(stmt, 0);
        row.value = sqlite3_column_double(stmt, 1);

        if (bw_write(&bw, &row, sizeof(row)))
            panic();

    }

    if (sqlite3_finalize(stmt))
        panic();

    if (sqlite3_close(db))
        panic();

    if (bw_flush(&bw))
        panic();

    fclose(bw.stream);

    return 0;
}
