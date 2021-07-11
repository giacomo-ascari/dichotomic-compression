from dc_compress import unpack, sectorize, sort, serialize
from dc_decompress import deserialize, render

def compress(input, output, thr=10):
    print("Starting compression of", input, "with thr:", thr)
    # UNPACK
    w, h, pix = unpack(input)
    # SECTORIZE
    heavy = []
    sectorize(heavy, pix, 0, 0, w, h, thr)
    # SORT
    light = sort(heavy, w, h)
    # SERIALIZE
    serialize(light, w, h, thr, output)
    print("Rate:", str(len(light)/(w*h)))

def decompress(input, output):
    print("Starting decompression of", input)
    # DESERIALIZATION
    w, h, light = deserialize(input)
    # RENDER
    render(w, h, light, output)

