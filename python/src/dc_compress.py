from PIL import Image

def _get_avg(pix, x0, y0, x1, y1):
    s = [0 for i in range(3)]
    denom = (x1 - x0) * (y1 - y0)
    for x in range(x0, x1):
        for y in range(y0, y1):
            for c in range(3):
                s[c] += pix[x, y][c]
    for c in range(3):
        s[c] = int(s[c] / denom)
    return s[0], s[1], s[2]

def _get_delta(pix, x0, y0, x1, y1):
    max = [0 for i in range(3)]
    min = [255 for i in range(3)]
    if len(pix[0, 0]) == 4: offset = 1
    else: offset = 0
    for x in range(x0, x1):
        for y in range(y0, y1):
            for c in range(3):
                if pix[x, y][offset+c] > max[c]:
                    max[c] = pix[x, y][offset+c]
                if pix[x, y][offset+c] < min[c]:
                    min[c] = pix[x, y][offset+c]
    delta = 0
    for c in range(3):
        delta += max[c] - min[c]
    return int(delta / 3)

def _get_mid(a, b):
    return a + int((b - a) / 2)

def _find(x0, y0, items):
    return next((item for item in items if item["x0"] == x0 and item["y0"] == y0), None)

def _occupy(s, mat):
    for x in range(s["x0"], s["x1"]):
        for y in range(s["y0"], s["y1"]):
            mat[x][y] = True

def unpack(input):
    img = Image.open(input)
    img = img.convert("RGB")
    pix = img.load()
    return img.size[0], img.size[1], pix

def sectorize(heavy, pix, x0, y0, x1, y1, thr):
    delta = _get_delta(pix, x0, y0, x1, y1)
    area = (x1 - x0) * (y1 - y0)
    if area > 1 and (delta > thr or x1 - x0 > 256 or y1 - y0 > 256):
        if (x1 - x0) > (y1 - y0):
            #vertical split
            mid = _get_mid(x0, x1)
            sectorize(heavy, pix, x0, y0, mid, y1, thr)
            sectorize(heavy, pix, mid, y0, x1, y1, thr)
        else:
            #horizontal split
            mid = _get_mid(y0, y1)
            sectorize(heavy, pix, x0, y0, x1, mid, thr)
            sectorize(heavy, pix, x0, mid, x1, y1, thr)
    else:
        s = {}
        s["x0"] = x0
        s["y0"] = y0
        s["x1"] = x1
        s["y1"] = y1
        s["r"], s["g"], s["b"] = _get_avg(pix, x0, y0, x1, y1)
        heavy.append(s)

def sort(heavy, w, h):
    inter = sorted(heavy, key=lambda k: w * k["y0"] + k["x0"])
    light = []
    for s in inter:
        ls = {}
        ls["x"] = s["x1"] - s["x0"] - 1
        ls["y"] = s["y1"] - s["y0"] - 1
        ls["r"], ls["g"], ls["b"] = s["r"], s["g"], s["b"]
        light.append(ls)
    return light

def serialize(light, w, h, thr, output):
    file = open(output, "wb")
    file.write(bytearray([1, thr]))
    file.write(int.to_bytes(w, 4, "big"))
    file.write(int.to_bytes(h, 4, "big"))
    for l in light:
        b = [l["x"], l["y"], l["r"], l["g"], l["b"]]
        file.write(bytearray(b))
    file.close()
