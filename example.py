import clh2of_table

def benchmark(t):
    import random
    x = 0
    for i in range(50000):
        n1 = random.randint(0, 5)
        n2 = random.randint(0, 5)
        n3 = random.randint(0, 5)
        n4 = random.randint(0, 5)
        x += t.element(n1, 0, n2, 0, n3, 0, n4, 0)
    print(x)

def example(t):
    print(t.element(0, 0, 0, 0, 0, 0, 0, 0))
    print(t.element(0, 0, 0, 0, 1, 0, 0, 0))
    print(t.element(1, 0, 1, 0, 1, 0, 1, 0))
    print(t.element(1, -1, 1, 1, 1, 0, 1, 1))

t = clh2of_table.TableSqlite("11.sqlite")
example(t)
