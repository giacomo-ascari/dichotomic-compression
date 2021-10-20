#include <string>
#include <iostream>
#include <getopt.h>

#include "dc/dc-compressor/dc-compressor.h"
#include "dc/dc-decompressor/dc-decompressor.h"

using namespace std;

// ./dc_core -c -f ../examples/vaporwave.jpg -t 30 -v 2 -V
// ./dc_core -d -f ../examples/vaporwave.jpg.dci -V

string error = "Error. Use 'dc_core -h' to check correct usage.\n";
string help =
" Usage: dc_core -c|-d -f FILENAME [-t THRESHOLD] [-v VERSION] [-h] [-V] \n\
\t-c: sets to compression mode, image file --> DCI file (exclusive with -d) \n\
\t-d: sets to decompression mode, DCI image --> bitmap image (exclusive with -c) \n\
\t-f FILENAME: specifies the input filename \n\
\t-t THRESHOLD: overrides default threshold (only in compr. mode) in range [0-255] \n\
\t-v VERSION: overrides default version (default is 2) (only in compr. mode). options: 1, 2 \n\
\t-V: shows verbose execution \n\
\t-h: shows this help panel \n";

int main(int argc, char**argv) {

    cout.setf(ios_base::unitbuf);

    // DECLARATIONS
    string file, out_file;
    unsigned char thr = 31;
    char mode = '\0';
    int verbose = 0;
    int version = 2;
    string c_ext = ".dci";
    string d_ext = ".png";

    // ARGUMENT READING
    int opt = 0;
    while ((opt = getopt(argc, argv, "hcdVf:t:v:")) != -1) { 
        switch (opt) {
            case 'h': cout << help << endl; exit(0);
            case 'c': mode = 'c'; break;
            case 'd': mode = 'd'; break;
            case 'V': verbose = 1; break;
            case 'f': file = optarg; break;
            case 't': thr = stoi(optarg); break;
            case 'v': version = stoi(optarg); break;
            default:
                cerr << error << endl;
                exit(1);
        }
    }

    // ARGUMENT CHECK
    if  (file[0] == '-'
        || file.size() == 0
        || mode == 0
        || version < 1
        || version > 2
        ) {
        cerr << error << endl;
        exit(1);
    }

    // INPUT and OUTPUT FILENAMES ADJUSTMENT
    out_file = file + (mode == 'c' ? c_ext : d_ext);

    // BASICALLY MAGIC, EVEN FOR ME
    if (mode == 'c') {
        dc_compressor compressor(file, out_file, verbose, version, thr);
        compressor.compress();
    } else {
        dc_decompressor decompressor(file, out_file, verbose);
        decompressor.decompress();
    }

    // BYE BYE
    return 0;
}