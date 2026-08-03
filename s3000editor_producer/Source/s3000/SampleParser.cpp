#include "SampleParser.h"
#include "Offsets.h"

namespace S = SampleOffset;

Sample SampleParser::parse(
	const std::vector<uint8_t>& d)
{
	Sample s{};

	if (d.size() < 64)
	{
		return s;
	}

	s.name = readName(
		d,
		S::Name,
		12
	);

	s.sampleRate = d[S::SampleRate];
	s.rootKey = d[S::RootKey];

	s.tune = static_cast<int8_t>(
		d[S::Tune]
		);

	s.loop = d[S::LoopEnable] != 0;

	return s;
}

std::string SampleParser::readName(
	const std::vector<uint8_t>& d,
	size_t offset,
	size_t length)
{
	std::string result;

	for (size_t i = 0; i < length; i++)
	{
		char c = static_cast<char>(
		d[offset + i]
			);

		if (c == 0)
		{
			break;
		}

		result += c;
	}

	return result;
}