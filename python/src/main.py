import dc

print("COMPRESS")
dc.compress('../examples/lines.png', '../examples/lines.dci', 40)

print("DECOMPRESS")
dc.decompress('../examples/lines.dci', '../examples/lines-decompressed.png')