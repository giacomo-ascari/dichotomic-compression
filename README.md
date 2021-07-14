# dichotomic-compression
Image compression algorithm, developed for research and educational purposes

## Overview
The dichotomic compression aims to minimize the size of a raster image by dinamically reducing the resolution:
- joining regions of pixels with similar color (under a specified threshold) in a "sector"
- creating a list of sectors with the least amount of information necessary

## Workflow
This workflow doesn't have to implemented strictly: depending on the implementation, some steps can be (or must be) integrated with one another.

![workflow](docs/workflow.png)

## Sectorization
TODO

## .DCI format
DCI (dichotmic compression image) is the format chosen to store the compressed image.
- Metadata (10 bytes)
  - version (1 byte): specifies the version used
  - threshold (1 byte): specifies the chosen threshold
  - width (4 bytes): width of the original image
  - height (4 bytes): height of the original image
- Sectors (variable length depending on version)
  - size/position: any data that can give information about the position of the sector
  - color channels (RGB)
![format](docs/format.png)

## .DCI versions
- Version 1 (implemented in Python)
  - Size/position composed of width (1 byte) and height (1 byte) of the sector (maximum 256x256 px)
  - RGB stored as 3 bytes, one for each color channel
  - Final sector "weight": 5 bytes
- Version 2 (implemented in C)
  - Size/position composed of the index of sectorizations done before getting to the sector itself (1 byte)
  - RGB stores as 3 1-byte channels
  - Final sector "weight": 4 bytes (32 bits)
- Version 3 (to be implemented in C)
  - Size/position composed of the index of sectorizations done before getting to the sector itself (7 bits)
  - RGB stores as 3 7-bits channels
  - Final sector "weight": 3.5 bytes (28 bits)
