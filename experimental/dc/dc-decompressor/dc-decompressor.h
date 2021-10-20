#ifndef _DC_DECOMPRESSOR_H
#define _DC_DECOMPRESSOR_H

#include <string>

#include "../dc.h"
#include "../dc-utils/dc-utils.h"

using namespace std;

class dc_decompressor: public dc {
    public:
        dc_decompressor(string _input, string _output, bool _verbose);
        ~dc_decompressor();
        void decompress();
    private:
        int input_filesize;
        void _deserialize();
        void _render(bool channel_sensitive);
        void _pack();
};

#endif