#ifndef _DC_COMPRESSOR_H
#define _DC_COMPRESSOR_H

#include <string>

#include "../dc.h"
#include "../dc-utils/dc-utils.h"

using namespace std;

class dc_compressor: public dc {
    public:
        dc_compressor(string _input, string _output, bool _verbose, int _version, int _thr);
        ~dc_compressor();
        void compress();
    private:
        int input_filesize;
        float compression_rate;
        void _unpack(bool channel_sensitive);
        void _sectorize(dc_corners cor, int splits, dc_channels channel);
        void _serialize(bool channel_sensitive);
};

#endif