#include "ProgramParser.h"
#include "KeygroupParser.h"
#include <juce_core/juce_core.h>

namespace P = ProgramOffset;
namespace KG = KeygroupOffset;


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
	if (d.size() <= P::General::Polyphony)
	{
		return;
	}

	uint16_t kgrpAddress =
		(d[0] << 8) | d[1];

	DBG(
		"KGRP1 address = "
		+ juce::String(kgrpAddress)
	);

	p.programNumber = d[P::General::Number];
	p.midiChannel = d[P::General::MidiChannel];
	p.polyphony = d[P::General::Polyphony];

	// ’Ç‰Á—\’è
}

void ProgramParser::parseKeygroups(const std::vector<uint8_t>& d, Program& p)
{
	if (d.size() <= P::Keygroups::NumKeygroups)
	{
		return;
	}

	const int numKeygroups = d[P::Keygroups::NumKeygroups];

	const size_t baseOffset = P::Keygroups::TempProgramNumber + 1;

	const size_t stride = 256;

	p.keygroups.clear();
	p.keygroups.reserve(numKeygroups);

	for (int i = 0; i < numKeygroups; ++i)
	{
		size_t offset = baseOffset + i * stride;

		if (offset + stride > d.size())
		{
			break;
		}

		std::vector<uint8_t> block(
			d.begin() + offset,
			d.begin() + offset + stride
		);

		p.keygroups.push_back(
			KeygroupParser::parse(block)
		);

		DBG(
			"Parsed Keygroup " + juce::String(i)
		);
	}
}