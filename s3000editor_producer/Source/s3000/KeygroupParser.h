#pragma once
#include <vector>
#include "S3000Types.h"
//#include "Keygroup.h"


class KeygroupParser
{
public:
    static Keygroup parse(const std::vector<uint8_t>& d);

private:
    static VelocityZone parseZone(
        const std::vector<uint8_t>& d,
        size_t base
    );

    template <typename T>
    static T read(const std::vector<uint8_t>& d, size_t offset)
    {
        return static_cast<T>(d[offset]);
    }
};



