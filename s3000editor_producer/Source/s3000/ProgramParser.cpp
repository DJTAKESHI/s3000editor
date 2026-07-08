#include "ProgramParser.h"
#include "KeygroupParser.h"


using K = ProgramOffset;
using KG = KeygroupOffset;


int8_t ProgramParser::readS8(const std::vector<uint8_t>& d, size_t offset)
{
	return static_cast<int8_t>(d[offset]);
}


uint16_t ProgramParser::readU16(const std::vector<uint8_t>& d, size_t offset)
{
	return (uint16_t)(d[offset] | (d[offset + 1] << 8));
}

std::string ProgramParser::readName(const std::vector<uint8_t>& d, size_t offset, size_t len)
{
	std::string s;
	s.reserve(len);

	for (size_t i = 0; i < len; ++i)
	{
		char c = static_cast<char>(d[offset + i]);
		if (c == 0)
			break;

		s.push_back(c);
	}

	return s;
}

Program ProgramParser::parse(const std::vector<uint8_t>& d)
{
	Program p{};

	parseHeader(d, p);
	parseKeygroups(d, p);

	return p;
}

void ProgramParser::parseHeader(const std::vector<uint8_t>& d, Program& p)
{
	p.programNumber = d[K::General::Number];
	p.midiChannel = d[K::General::MidiChannel];
	p.polyphony = d[K::General::Polyphony];

	// ’Ç‰Á—\’è
}

void ProgramParser::parseKeygroups(const std::vector<uint8_t>& d, Program& p)
{
	const int numKeygroupgs = d[K::Keygroup::NumKeygroups];

	const size_t baseOffset = K::Keygroups::TempProgramNumber + 1;

	const size_t stride = 256;

	p.keygroups.clear();
	p.keygroups.reserve(numKeygroups);

	for (int i = 0; i < numKeygroupgs; ++i)
	{
		size_t offset = baseOffset + i * stride;

		std::vector<uint8_t> block(
			d.begin() + offset,
			d.begin() + offset + stride
		);

		p.keygroups.push_back(
			KeygroupParser::parse(block)
		);


	}
}