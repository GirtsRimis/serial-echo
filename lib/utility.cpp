#include <iomanip>
#include <sstream>
#include <sys/ioctl.h>

#include "utility.h"

#define countof(x) sizeof(x) / sizeof(x[0])
#define stringize(X) #X
#define ITEM(X) make_pair(TIOCM_##X,stringize(X))

using namespace std;

void makeVector(vector<char> &inputVector, const char *data, size_t size)
{
    vector<char> createVector;
    inputVector.assign(data, data + size);
}

using namespace OutputBitsFormat;
string modemStatusBitsToString(unsigned int data, uint8_t options)
{
    typedef pair<unsigned int, const char *> pair_t;
    const pair_t pairs[] = {
          ITEM(DTR)
        , ITEM(RTS)
        , ITEM(CTS)
        , ITEM(CAR)
        , ITEM(DSR)
    };
    const bool show_empty = true;
    ostringstream ostr;
    if( options & eShowHex )
        ostr << "0x" << setfill('0') << setw(4) << hex << data;
    if( data || !(options & eShowHex) ) {
        ostr << " (";
        bool at_least_one = false;
        for(size_t i = 0; i < countof(pairs); ++i) {
            bool bit_set = data & pairs[i].first;
            if( bit_set || (options & eShowZeroBits) ) {
                if( at_least_one ) {
                    ostr << '|';
                    if( i == 2 ) // separate output from input
                        ostr << '|';
                }
                at_least_one = true;
                ostr << (bit_set ? pairs[i].second : EMPTY);
            }
        }
        ostr << ") ";
    }
    return ostr.str();
}
