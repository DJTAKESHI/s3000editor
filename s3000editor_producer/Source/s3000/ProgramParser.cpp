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
    DBG("========== ProgramParser::parseHeader CALLED ==========");
    DBG("PROGRAM HEADER DATA SIZE = " + juce::String((int)d.size()));

    DBG(
        "REQUIRED LAST OFFSET = "
        + juce::String((int)P::Keygroups::NumKeygroups)
    );

    // ç≈í·Ç≈Ç‡ GROUPS(offset 42) Ç‹Ç≈ïKóv
    if (d.size() <= P::Keygroups::NumKeygroups)
    {
        DBG("PROGRAM HEADER TOO SMALL");
        return;
    }

    // rawÇï€éù
    p.rawData = d;

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

    p.tune =
        readTune(
            d,
            P::Pitch::ProgramTune
        );

    DBG(
        "PROGRAM TUNE = "
        + juce::String(p.tune, 2)
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

    p.modVPan1 = d[P::Mod::ModVPan1];
    p.modVPan2 = d[P::Mod::ModVPan2];
    p.modVPan3 = d[P::Mod::ModVPan3];

    p.modVAmp1 = d[P::Mod::ModVAmp1];
    p.modVAmp2 = d[P::Mod::ModVAmp2];

    p.modVLfo1Rate = d[P::Mod::ModVLFO1Rate];
    p.modVLfo1Depth = d[P::Mod::ModVLFO1Depth];
    p.modVLfo1Delay = d[P::Mod::ModVLFO1Delay];

    p.lfo1Wave = d[P::LFO::LFO1Wave];
    p.lfo2Wave = d[P::LFO::LFO2Wave];

    p.velocityLoudness =
        readS8(
            d,
            P::Output::VelocityLoudness
        );

    DBG(
        "VELOCITY -> LOUDNESS = "
        + juce::String(p.velocityLoudness)
    );





    // ===== Debug =====

    DBG("=== PROGRAM HEADER ===");

    DBG(
        "PROGRAM NUMBER = "
        + juce::String(p.programNumber)
    );

    DBG(
        "MIDI CHANNEL = "
        + juce::String(p.midiChannel)
    );

    DBG(
        "POLYPHONY RAW = "
        + juce::String(p.polyphony)
    );

    DBG(
        "PRIORITY = "
        + juce::String(p.priority)
    );

    DBG(
        "PLAY LOW = "
        + juce::String(p.playLow)
    );

    DBG(
        "PLAY HIGH = "
        + juce::String(p.playHigh)
    );

    DBG(
        "GROUPS = "
        + juce::String(p.groups)
    );

    DBG(
        "OUTPUT = "
        + juce::String(p.output)
    );

    DBG(
        "STEREO LEVEL = "
        + juce::String(p.stereoLevel)
    );

    DBG(
        "PAN = "
        + juce::String(p.pan)
    );

    DBG(
        "LOUDNESS = "
        + juce::String(p.loudness)
    );
    DBG(
        "LFO2 RATE = "
        + juce::String(p.lfo2Rate)
    );

    DBG(
        "LFO2 DEPTH = "
        + juce::String(p.lfo2Depth)
    );

    DBG(
        "LFO2 DELAY = "
        + juce::String(p.lfo2Delay)
    );

    DBG(
        "LFO1 RATE = "
        + juce::String(p.lfo1Rate)
    );

    DBG(
        "LFO1 DEPTH = "
        + juce::String(p.lfo1Depth)
    );

    DBG(
        "LFO1 DELAY = "
        + juce::String(p.lfo1Delay)
    );

    DBG(
        "MODWHEEL -> LFO1 DEPTH = "
        + juce::String(p.modWheelDepth)
    );

    DBG(
        "PRESSURE -> LFO1 DEPTH = "
        + juce::String(p.pressureDepth)
    );

    DBG(
        "VELOCITY -> LFO1 DEPTH = "
        + juce::String(p.velocityDepth)
    );

    DBG(
        "BEND UP = "
        + juce::String(p.bendUp)
    );

    DBG(
        "PRESSURE -> PITCH = "
        + juce::String(p.pressurePitch)
    );

    p.keygroupCrossfade =
        d[P::Keygroups::KeygroupCrossfade] != 0;

    DBG(
        "KEYGROUP CROSSFADE = "
        + juce::String(
            p.keygroupCrossfade ? "ON" : "OFF"
        )
    );


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

    DBG(
        "LFO1 DESYNC = "
        + juce::String(
            p.lfo1Desync ? "ON" : "OFF"
        )
    );

    DBG(
        "VOICE STEAL = "
        + juce::String(
            p.voiceAssign == 0
            ? "OLDEST"
            : "QUIETEST"
        )
    );

    DBG(
        "SOFT PEDAL LOUDNESS = "
        + juce::String(p.softLoudness)
    );

    DBG(
        "SOFT PEDAL ATTACK = "
        + juce::String(p.softAttack)
    );

    DBG(
        "SOFT PEDAL FILTER = "
        + juce::String(p.softFilter)
    );

    DBG(
        "PROGRAM TUNE = "
        + juce::String(p.tune, 2)
    );

    DBG(
        juce::String::formatted(
            "PTUNO RAW = %02X %02X",
            (unsigned)d[P::Pitch::ProgramTune],
            (unsigned)d[P::Pitch::ProgramTune + 1]
        )
    );


    DBG(
        "INDIVIDUAL OUTPUT LEVEL = "
        + juce::String(p.individualOutputLevel)
    );

    DBG(
        "LEGATO = "
        + juce::String(p.legato ? "ON" : "OFF")
    );

    DBG(
        "BEND DOWN = "
        + juce::String(p.bendDown)
    );

    DBG(
        "BEND MODE = "
        + juce::String(p.bendMode)
    );

    DBG(
        "TRANSPOSE = "
        + juce::String(p.transpose)
    );

    DBG("=== MODULATION RAW VALUES ===");

    DBG("MOD S PAN1 = " + juce::String(p.modSPan1));
    DBG("MOD S PAN2 = " + juce::String(p.modSPan2));
    DBG("MOD S PAN3 = " + juce::String(p.modSPan3));

    DBG("MOD S AMP1 = " + juce::String(p.modSAmp1));
    DBG("MOD S AMP2 = " + juce::String(p.modSAmp2));

    DBG("MOD S LFO1 RATE = "
        + juce::String(p.modSLfo1Rate));

    DBG("MOD S LFO1 DEPTH = "
        + juce::String(p.modSLfo1Depth));

    DBG("MOD S LFO1 DELAY = "
        + juce::String(p.modSLfo1Delay));

    DBG("MOD S FILTER1 = " + juce::String(p.modSFilter1));
    DBG("MOD S FILTER2 = " + juce::String(p.modSFilter2));
    DBG("MOD S FILTER3 = " + juce::String(p.modSFilter3));

    DBG("MOD S PITCH = " + juce::String(p.modSPitch));
    DBG("MOD S AMP3 = " + juce::String(p.modSAmp3));


    DBG("MOD V PAN1 = " + juce::String(p.modVPan1));
    DBG("MOD V PAN2 = " + juce::String(p.modVPan2));
    DBG("MOD V PAN3 = " + juce::String(p.modVPan3));

    DBG("MOD V AMP1 = " + juce::String(p.modVAmp1));
    DBG("MOD V AMP2 = " + juce::String(p.modVAmp2));

    DBG("MOD V LFO1 RATE = "
        + juce::String(p.modVLfo1Rate));

    DBG("MOD V LFO1 DEPTH = "
        + juce::String(p.modVLfo1Depth));

    DBG("MOD V LFO1 DELAY = "
        + juce::String(p.modVLfo1Delay));

    DBG("LFO1 WAVE = " + juce::String(p.lfo1Wave));
    DBG("LFO2 WAVE = " + juce::String(p.lfo2Wave));


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

		DBG(
			"Parsed Keygroup " + juce::String(i)
		);
	}
}