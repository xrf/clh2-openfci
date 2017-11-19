#!/usr/bin/env python
import random
import clh2of_table

def assert_approx_eq(x, y):
    assert abs(x - y) < 1e-8

def random_test(t):
    shells_max = 5
    x = 0.0
    for i in range(50000):
        ml4_max = 0
        ml4 = 1
        while abs(ml4) > ml4_max:
            n1 = random.randint(0, shells_max // 2)
            n2 = random.randint(0, shells_max // 2)
            n3 = random.randint(0, shells_max // 2)
            n4 = random.randint(0, shells_max // 2)
            ml1_max = shells_max - 2 * n1
            ml2_max = shells_max - 2 * n2
            ml3_max = shells_max - 2 * n3
            ml4_max = shells_max - 2 * n4
            ml1 = random.randint(-ml1_max, ml1_max)
            ml2 = random.randint(-ml2_max, ml2_max)
            ml3 = random.randint(-ml3_max, ml3_max)
            ml4 = ml1 + ml2 - ml3
        print(n1, ml1, n2, ml2, n3, ml3, n4, ml4, x)
        x += t.element(n1, ml1, n2, ml2, n3, ml3, n4, ml4)
    assert_approx_eq(x, 1881.814655568331)

t = clh2of_table.TableSqlite("shells5.sqlite3")
assert_approx_eq(t.element(0, 0, 0, 0, 0, 0, 0, 0), 1.2533141373154995)
assert_approx_eq(t.element(0, 0, 0, 0, 1, 0, 0, 0), 0.31332853432887514)
assert_approx_eq(t.element(1, 0, 1, 0, 1, 0, 1, 0), 0.7490510273799696)
assert_approx_eq(t.element(1, -1, 1, 1, 1, 0, 1, 0), 0.16217199530693752)

random.seed(1)
random_test(t)
