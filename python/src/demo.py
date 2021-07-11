import random

heavy = []
light = []
w = 8
h = 6
raw = [[0 for y in range(h)] for x in range(w)]
thr = 90
i = 0

def print_matrix(matrix):
    for y in range(0, h):
        s = " "
        for x in range(0, w):
            s += str(matrix[x][y]) + "\t"
        print(s)

def get_avg(x0, y0, x1, y1):
    s = 0
    for x in range(x0, x1):
        for y in range(y0, y1):
            s += raw[x][y]
    return int( s / ((x1 - x0) * (y1 - y0)))

def get_delta(x0, y0, x1, y1):
    max = raw[x0][y0]
    min = raw[x0][y0]
    for x in range(x0, x1):
        for y in range(y0, y1):
            if raw[x][y] > max:
                max = raw[x][y]
            if raw[x][y] < min:
                min = raw[x][y]
    return max - min

def find_mid(a, b):
    return a + int((b-a)/2)


def sectorize(x0, y0, x1, y1):
    global i, heavy
    print("sectorize", raw[x0][y0], raw[x1-1][y1-1], "-", i)
    delta = get_delta(x0, y0, x1, y1)
    if delta > thr or x1 - x0 > 256 or y1 - y0 > 256:
        i += 1
        if (x1 - x0) > (y1 - y0):
            #vertical split
            mid = find_mid(x0, x1)
            sectorize(x0, y0, mid, y1)
            sectorize(mid, y0, x1, y1)
        else:
            #horizontal split
            mid = find_mid(y0, y1)
            sectorize(x0, y0, x1, mid)
            sectorize(x0, mid, x1, y1)
    else:
        s = {}
        s["x0"] = x0
        s["y0"] = y0
        s["x1"] = x1
        s["y1"] = y1
        s["v"] = get_avg(x0, y0, x1, y1)
        heavy.append(s)

def find(x0, y0, items):
    return next((item for item in items if item["x0"] == x0 and item["y0"] == y0), None)

def occupy(s, mat):
    for x in range(s["x0"], s["x1"]):
        for y in range(s["y0"], s["y1"]):
            mat[x][y] = True
    #return x1+1, y1+1

def sort():
    inter = sorted(heavy, key=lambda k: k["x0"] * k["y0"] + k["x0"])
    for s in inter:
        ls = {}
        ls["x"] = s["x1"] - s["x0"]
        ls["y"] = s["y1"] - s["y0"]
        ls["v"] = s["v"]
        light.append(ls)

for x in range(0, w):
    for y in range(0, h):
        raw[x][y] = random.randint(0,100)

print_matrix(raw)

# SECTORIZE
sectorize(0, 0, w, h)

print(len(heavy))
for _h in heavy:
    print(_h)

# SERIALIZE
sort()

for _l in light:
    print(_l)