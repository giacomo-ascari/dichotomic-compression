from PIL import Image

def _draw(x0, y0, x1, y1, pix, mat, r, g, b):
    for x in range(x0, x1):
        for y in range(y0, y1):
            pix[x,y] = (r, g, b)
            mat[x][y] = True

def deserialize(input):
    light = []
    file = open(input, "rb")
    v = int.from_bytes(file.read(1), "big")
    thr = int.from_bytes(file.read(1), "big")
    w = int.from_bytes(file.read(4), "big")
    h = int.from_bytes(file.read(4), "big")
    print(v, thr, w, h)
    bytes = file.read(5)
    while bytes:
        ls = {}
        ls["x"] = bytes[0]
        ls["y"] = bytes[1]
        ls["r"] = bytes[2]
        ls["g"] = bytes[3]
        ls["b"] = bytes[4]
        light.append(ls)
        bytes = file.read(5)
    file.close()
    return w, h, light

def _x(i, w):
    return i % w

def _y(i, w):
    return int(i / w)

def render(w, h, light, output):
    img = Image.new('RGB', [w,h], 0)
    mat = [[False for y in range(h)] for x in range(w)]
    pix = img.load()
    n = 0
    for y in range(h):
        for x in range(w):
            if mat[x][y] == False:
                ls = light[n]
                _draw(x, y, x+ls["x"]+1, y+ls["y"]+1, pix, mat, ls["r"], ls["g"], ls["b"])
                n += 1
    img.save(output)
