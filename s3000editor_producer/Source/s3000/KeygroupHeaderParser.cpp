#include "KeygroupHeaderParser.h"


KeygroupHeader KeygroupHeaderParser::parse(
    const std::vector<uint8_t>& d)
{
    KeygroupHeader k{};


    if (d.size() < 9)
        return k;


    k.id = d[0];


    k.nextBlock =
        d[1] |
        (d[2] << 7);


    k.lowNote = d[3];

    k.highNote = d[4];


    k.tune =
        d[5] |
        (d[6] << 8);


    k.filterFreq = d[7];

    k.filterKeyFollow = d[8];


    return k;
}