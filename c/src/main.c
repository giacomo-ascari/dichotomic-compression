#include "dc/dc.h"
#include <string.h>
#define N 200

// ./main.out -c ../../examples/test.png ../../examples/test.dci
// ./main.out -d ../../examples/test.dci ../../examples/test_decompressed.bmp

int main(int argc, char**argv) {
    char input[N], output[N];
    if (argc == 4) {
        strcpy(input, argv[2]);
        strcpy(output, argv[3]);
        printf("%s | %s\n", input, output);
        if (strcmp("-c", argv[1])=='\0')
            dc_compress(input, output, 30, 1);
        else if (strcmp("-d", argv[1])=='\0')
            dc_decompress(input, output, 1);
    }
   
}