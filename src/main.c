#include "dc/dc.h"
#include <string.h>
#define N 200

// ./dc_core -c -f ../examples/vaporwave.jpg -t 30 -v
// ./dc_core -d -f ../examples/vaporwave.jpg.dci -v

int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

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
    while ((opt = getopt(argc, argv, "cdvf:t:")) != -1) { 
        switch (opt) {
            case 'c': mode = 'c'; break;
            case 'd': mode = 'd'; break;
            case 'v': verbose = 1; break;
            case 'f': strcpy(file, optarg); break;
            case 't': thr = (unsigned char)strtol(optarg, (char **)NULL, 10); break;
            default:
                fprintf(stderr, "Error\n");
                exit(1);
        }
    }

    // ARGUMENT CHECK
    if  (file[0] == '-'
        || strlen(file) == 0
        || mode == 0
        ) {
        fprintf(stderr, "Error\n");
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