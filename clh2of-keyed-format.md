# Keyed formats (`clh2of-keyed`) v2

*Changes since v1: Fixed incomplete canonicalization conditions.*

In this document, formal grammar is specified using [W3C EBNF notation](https://www.w3.org/TR/REC-xml/#sec-notation).

## Mathematical definition

Refer to [Simple formats](clh2of-simple-format.md).

## Binary format (`clh2of-keyed-binary`)

A table consists of a sequence of [canonical](#canonicalization) matrix element entries.  In the binary format, the table is stored as a concatenation of all its entries with no separators.  Each entry is 16 bytes, thus a table with `N` entries would occupy exactly `16 × N` bytes.

    table ::= entry*

Entries within a table are ordered by key.

An entry consists of 2 fields.  The first 8 bytes store a 64-bit unsigned integer key, and the remaining 8 bytes contain the value of the matrix element as an IEEE 754 double-precision floating-point number.

    entry ::= key value
    key   ::= uint64
    value ::= float64

### Endianness

Refer to [Simple formats](clh2of-simple-format.md).

## SQLite3 format (`clh2of-keyed-sqlite3`)

The SQLite3 format uses the following schema:

```sql
CREATE TABLE data (key INTEGER PRIMARY KEY, value REAL);
```

where `key` is the integer [key](#key-function) and `value` is the value of the matrix element.  Each entry corresponds to a row in the SQLite3 table.

## Canonicalization

Refer to [Simple formats](clh2of-simple-format.md).

## Key function

After canonicalization, the indices `(p1, p2, p3, p4)` as defined in the canonicalization section are used to construct the key using bitwise operations:

    key = (p1 << 48) | (p2 << 32) | (p3 << 16) | p4

where `<<` denotes left shift and `|` denotes bitwise-OR.

### Example

A concrete implementation of the key function can be found in as `clh2of_table.py`, where it is called `_construct_key`.
