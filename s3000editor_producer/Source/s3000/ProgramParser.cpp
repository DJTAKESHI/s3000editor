#include "ProgramParser.h"
#include "KeygroupParser.h"
#include <juce_core/juce_core.h>
#include "SysExUtils.h"

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

double ProgramParser::readTune(
    const std::vector<uint8_t>& d,
    size_t offset)
{
    const uint8_t fractionRaw =
        d[offset];

    const int8_t semitone =
        static_cast<int8_t>(
            d[offset + 1]
            );

    return static_cast<double>(semitone)
        + static_cast<double>(fractionRaw) / 256.0;
}

static char decodeAkaiChar(uint8_t v)
{
    if (v <= 9)
        return static_cast<char>('0' + v);

    if (v == 10)
        return ' ';

    if (v >= 11 && v <= 36)
        return static_cast<char>('A' + (v - 11));

    if (v == 37)
        return '#';

    if (v == 38)
        return '+';

    if (v == 39)
        return '-';

    if (v == 40)
        return '.';

    return '?';
}


Program ProgramParser::parse(const std::vector<uint8_t>& d)
{
    DBG("========== ProgramParser::parse CALLED ==========");
    DBG("PROGRAM PARSER INPUT SIZE = " + juce::String((int)d.size()));

	Program p{};

	parseHeader(d, p);
	//parseKeygroups(d, p);

	return p;
}

void ProgramParser::parseHeader(
    const std::vector<uint8_t>& d,
    Program& p)
{


    if (d.size() <= P::Keygroups::NumKeygroups)
    {
        DBG("PROGRAM HEADER TOO SMALL");
        return;
    }

    // raw‚ð•ÛŽ
    p.rawData = d;

    // ===== Program Name =====

    p.name.clear();

    for (std::size_t i = 0;
        i < ProgramOffset::General::NameLength;
        ++i)
    {
        p.name += decodeAkaiChar(
            d[ProgramOffset::General::Name + i]
        );
    }

    while (!p.name.empty() && p.name.back() == ' ')
        p.name.pop_back();




    // ===== General =====

    p.programNumber =
        d[ProgramOffset::General::Number];

    // ===== General =====

    p.programNumber =
        d[P::General::Number];

    p.midiChannel =
        d[P::General::MidiChannel];

    p.polyphony =
        d[P::General::Polyphony];

    p.priority =
        d[P::General::Priority];

    p.playLow =
        d[P::General::PlayLow];

    p.playHigh =
        d[P::General::PlayHigh];

    // ==============================
// Output
// ==============================

    p.output =
        d[P::Output::Output];

    p.stereoLevel =
        d[P::Output::Stereo];

    p.pan =
        readS8(
            d,
            P::Output::Pan
        );

    p.loudness =
        d[P::Output::Loudness];



    // ===== Keygroups =====

    p.groups =
        d[P::Keygroups::NumKeygroups];

    // ==============================
// LFO2
// ==============================

    p.lfo2Rate =
        d[P::LFO::LFO2Rate];

    p.lfo2Depth =
        d[P::LFO::LFO2Depth];

    p.lfo2Delay =
        d[P::LFO::LFO2Delay];


    // ==============================
    // LFO1
    // ==============================

    p.lfo1Rate =
        d[P::LFO::LFO1Rate];

    p.lfo1Depth =
        d[P::LFO::LFO1Depth];

    p.lfo1Delay =
        d[P::LFO::LFO1Delay];




    // ==============================
// LFO1 Depth Modulation
// ==============================

    p.modWheelDepth =
        d[P::LFO::ModWheelDepth];

    p.pressureDepth =
        d[P::LFO::PressureDepth];

    p.velocityDepth =
        d[P::LFO::VelocityDepth];

    // ==============================
// Pitch
// ==============================



    p.bendUp =
        d[P::Pitch::BendUp];

    p.pressurePitch =
        readS8(
            d,
            P::Pitch::PressurePitch
        );



    // ==============================
// Temperament
// ==============================

    for (int i = 0; i < 12; ++i)
    {
        p.temperament[i] =
            readS8(
                d,
                P::Temperament::Temperament + i
            );
    }


    // ==============================
// Voice
// ==============================

    p.lfo1Desync =
        d[P::Voice::Desync] != 0;

    p.voiceAssign =
        d[P::Voice::VoiceAssign];


    // ==============================
    // Soft Pedal
    // ==============================

    p.softLoudness =
        d[P::SoftPedal::SoftLoudness];

    p.softAttack =
        d[P::SoftPedal::SoftAttack];

    p.softFilter =
        d[P::SoftPedal::SoftFilter];

    // ==============================
// Portamento
// ==============================

    p.portamentoTime =
        d[P::Portamento::Time];

    p.portamentoType =
        d[P::Portamento::Type];

    p.portamentoEnabled =
        d[P::Portamento::Enable] != 0;




    p.tune =
        readTune(
            d,
            P::Pitch::ProgramTune
        );




    p.individualOutputLevel =
        d[P::Voice::VoiceOutput];

    p.legato =
        d[P::Voice::Legato] != 0;



    p.bendDown =
        d[P::Pitch::BendDown];



    p.bendMode =
        d[P::Pitch::BendMode];



    p.transpose =
        readS8(
            d,
            P::Pitch::Transpose
        );



    p.modSPan1 = d[P::Mod::ModSPan1];
    p.modSPan2 = d[P::Mod::ModSPan2];
    p.modSPan3 = d[P::Mod::ModSPan3];

    p.modSAmp1 = d[P::Mod::ModSAmp1];
    p.modSAmp2 = d[P::Mod::ModSAmp2];

    p.modSLfo1Rate = d[P::Mod::ModSLFO1Rate];
    p.modSLfo1Depth = d[P::Mod::ModSLFO1Depth];
    p.modSLfo1Delay = d[P::Mod::ModSLFO1Delay];

    p.modSFilter1 = d[P::Mod::ModSFilter1];
    p.modSFilter2 = d[P::Mod::ModSFilter2];
    p.modSFilter3 = d[P::Mod::ModSFilter3];

    p.modSPitch = d[P::Mod::ModSPitch];
    p.modSAmp3 = d[P::Mod::ModSAmp3];




    // ==============================
// Modulation Amounts
// signed -50 .. +50
// ==============================

    p.modVPan1 =
        readS8(
            d,
            P::Mod::ModVPan1
        );

    p.modVPan2 =
        readS8(
            d,
            P::Mod::ModVPan2
        );

    p.modVPan3 =
        readS8(
            d,
            P::Mod::ModVPan3
        );


    p.modVAmp1 =
        readS8(
            d,
            P::Mod::ModVAmp1
        );

    p.modVAmp2 =
        readS8(
            d,
            P::Mod::ModVAmp2
        );


    p.modVLfo1Rate =
        readS8(
            d,
            P::Mod::ModVLFO1Rate
        );

    p.modVLfo1Depth =
        readS8(
            d,
            P::Mod::ModVLFO1Depth
        );

    p.modVLfo1Delay =
        readS8(
            d,
            P::Mod::ModVLFO1Delay
        );

    p.lfo1Wave = d[P::LFO::LFO1Wave];
    p.lfo2Wave = d[P::LFO::LFO2Wave];

    if (d.size() > P::Mod::ModVPitch)
    {
        p.modVPitch =
            readS8(
                d,
                P::Mod::ModVPitch
            );
    }

    

    static const char* noteNames[12] =
    {
        "C", "C#", "D", "D#",
        "E", "F", "F#", "G",
        "G#", "A", "A#", "B"
    };

    for (int i = 0; i < 12; ++i)
    {
        DBG(
            juce::String("TEMPER ")
            + noteNames[i]
            + " = "
            + juce::String((int)p.temperament[i])
        );
    }

    


}

void ProgramParser::parseKeygroups(const std::vector<uint8_t>& d, Program& p, const std::map<int, juce::String>& residentSamples)
{
	if (d.size() <= P::Keygroups::NumKeygroups)
	{
		return;
	}

	if (d.size() < 128)
	{
		DBG("Program data too small");
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
			KeygroupParser::parse(
				block,
				residentSamples
			)
		);


	}
}