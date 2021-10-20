#include <string>
#include <iostream>

#include "dc.h"

using namespace std;

dc::dc() : dc("temp", "temp.dci", false) { }

dc::dc(string _input, string _output, bool _verbose) {
    input = _input;
    output = _output;
    verbose = _verbose;
    version = 0;
    thr = 0;
    sectors_count = 0;
}

dc::~dc() { }