#pragma once

#include <vector>
#include "S3000Types.h"

class SampleParser
{
public:
	static Sample parse(
		const std::vector<uint8_t>& d
	);

private:
	static std::string readName(
		const std::vector<uint8_t>& d,
		size_t offset,
		size_t length
	);

};