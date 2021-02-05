#include <iomanip>
#include <sstream>
#include <sys/ioctl.h>

#include "utility.h"

#define countof(x) sizeof(x) / sizeof(x[0])
#define stringize(X) #X
#define ITEM(X) std::make_pair(TIOCM_##X,stringize(X))

void makeVector(std::vector<char> &inputVector, const char *data, std::size_t size)
{
    std::vector<char> createVector;
    inputVector.assign(data, data + size);
}

std::string modemStatusBitsToString(unsigned int data, bool add_hex)
{
    typedef std::pair<unsigned int, const char *> pair_t;
    const pair_t pairs[] = {
          ITEM(DTR)
        , ITEM(RTS)
        , ITEM(CTS)
        , ITEM(CAR)
        , ITEM(DSR)
    };
    std::ostringstream ostr;
    if( add_hex )
        ostr << std::hex << data;
    if( data || !add_hex ) {
        ostr << " (";
        bool at_least_one = false;
	    for(size_t i = 0; i < countof(pairs); ++i) {
            if( data & pairs[i].first ) {
                if( at_least_one )
                    ostr << '|';
                at_least_one = true;
                ostr << pairs[i].second;
            }
	    }
        ostr << ") ";
    }
	return ostr.str();
}

