#pragma once
#include <vector>
#include "S3000Types.h"
#include "../s3000/Offsets.h"
//#include "Keygroup.h"


class KeygroupParser
{
public:
    static Keygroup parse(const std::vector<uint8_t>& d);

private:
    template <typename T>
    static T read(const std::vector<uint8_t>& d, size_t offset)
    {
        return static_cast<T>(d[offset]);
    }
};



