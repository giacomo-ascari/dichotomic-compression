#include "dc/dc.h"
#include <string.h>
#define N 200

// ./dc_core -c -f ../examples/vaporwave.jpg -t 30 -v
// ./dc_core -d -f ../examples/vaporwave.jpg.dci -v

char * error = "Error. Use 'dc_core -h' to check correct usage.\n";
char * help =
" Usage: dc_core -c|-d FILENAME [-t THRESHOLD] [-v] [-h] \n\
\t-c FILENAME: compress image to a DCI file (exclusive with -d) \n\
\t-d FILENAME: decompress a DCI image to a bitmap image (exclusive with -c) \n\
\t-t THRESHOLD: override default threshold (only for compression) in range [0-255] \n\
\t-v: show verbose execution \n\
\t-h: show this help panel \n";

int main(int argc, char**argv) {

    // DECLARATIONS
    char file[N] = "\0", out_file[N] = "\0";
    unsigned char thr = 23;
    char mode = '\0';
    int verbose = 0;

    char *c_ext = ".dci";
    char *d_ext = ".bmp";

    // ARGUMENT READING
    int opt = 0;
    while ((opt = getopt(argc, argv, "hcdvf:t:")) != -1) { 
        switch (opt) {
            case 'h': printf("%s", help); exit(0);
            case 'c': mode = 'c'; break;
            case 'd': mode = 'd'; break;
            case 'v': verbose = 1; break;
            case 'f': strcpy(file, optarg); break;
            case 't': thr = (unsigned char)strtol(optarg, (char **)NULL, 10); break;
            default:
                fprintf(stderr, "%s", error);
                exit(1);
        }
    }

    // ARGUMENT CHECK
    if  (file[0] == '-'
        || strlen(file) == 0
        || mode == 0
        ) {
        fprintf(stderr, "%s", error);
        exit(1);
    }

    // INPUT and OUTPUT FILENAMES ADJUSTMENT
    strcpy(out_file, file);
    if (mode == 'c') {
        strcat(out_file, c_ext);
    } else {
        strcat(out_file, d_ext);
    }

    // BASICALLY MAGIC, EVEN FOR ME
    if (mode == 'c') {
        dc_compress(file, out_file, thr, verbose);
    } else {
        dc_decompress(file, out_file, verbose);
    }

    // BYE BYE
    return 0;
}