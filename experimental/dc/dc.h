#ifndef _DC_H
#define _DC_H

#include <string>
#include <vector>
#include <map>

#include "dc-utils/dc-utils.h"

using namespace std;

class dc {
    public:
        dc();
        dc(string _input, string _output, bool _verbose);
        ~dc();
    protected:
        bool verbose;
        int version;
        int thr;
        string input;
        string output;
        int sectors_count;
        dc_pixel_matrix pixel_matrix;
        map<size_t, dc_heavy_sector> h_sectors;
        vector<dc_light_sector> l_sectors;
};

#endif