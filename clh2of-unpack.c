#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#define PROG "clh2of-unpack"
#include "utils.inl"

struct buffered_reader {
    FILE *stream;
    size_t position, avail;
    char buffer[BUFSIZ];
};

static void br_init(struct buffered_reader *self, FILE *stream)
{
    self->stream = stream;
    self->position = 0;
    self->avail = 0;
}

/* returns (requested_size - actual_size_read) */
static size_t br_read(struct buffered_reader *self, void *data, size_t size)
{
    char *dest = (char *)data;
    while (1) {
        const char *const src = self->buffer + self->position;
        if (self->avail > size) {
            memcpy(dest, src, size);
            self->position += size;
            self->avail -= size;
            return 0;
        }
        size_t count;
        memcpy(dest, src, self->avail);
        size -= self->avail;
        dest += self->avail;
        count = fread(self->buffer, 1, BUFSIZ, self->stream);
        if (!count) {
            self->avail = 0;
            return size;
        }
        self->avail = count;
        self->position = 0;
    }
}

int main(int argc, char **argv)
{
    static const char *usage =
        "Usage: " PROG " INPUT_FILENAME OUTPUT_FILENAME\n\n"
        "Unpacks matrix elements into SQLite format.\n";

    struct buffered_reader br;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *in_fn, *out_fn;

    /* print usage if needed */
    if (argc != 3) {
        fprintf(stderr, "%s", usage);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    /* parse arguments */
    in_fn = argv[1];
    out_fn = argv[2];

    br_init(&br, fopen(in_fn, "r"));

    if (sqlite3_config(SQLITE_CONFIG_LOG, error_sqlite, NULL))
        panic();

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

    while (1) {
        struct {
            sqlite3_int64 key;
            double value;
        } row;

        const size_t r = br_read(&br, &row, sizeof(row));

        if (r == sizeof(row))
            break;
        else if (r)
            panic();

        if (sqlite3_bind_int64(stmt, 1, row.key))
            panic();

        if (sqlite3_bind_double(stmt, 2, row.value))
            panic();

        if (sqlite3_step(stmt) != SQLITE_DONE)
            panic();

        if (sqlite3_reset(stmt))
            panic();

    }

    if (sqlite3_finalize(stmt))
        panic();

    if (sqlite3_exec(db, "END; VACUUM;", NULL, NULL, NULL))
        panic();

    if (sqlite3_close(db))
        panic();

    fclose(br.stream);

    return 0;
}
