#include "ProgramEncoder.h"
#include "Offsets.h"
#include <cmath>

static uint8_t encodeAkaiChar(char c)
{
    if (c >= '0' && c <= '9')
        return static_cast<uint8_t>(c - '0');

    if (c == ' ')
        return 10;

    if (c >= 'A' && c <= 'Z')
        return static_cast<uint8_t>(11 + (c - 'A'));

    if (c >= 'a' && c <= 'z')
        return static_cast<uint8_t>(11 + (c - 'a'));

    if (c == '#')
        return 37;

    if (c == '+')
        return 38;

    if (c == '-')
        return 39;

    if (c == '.')
        return 40;

    return 10;
}


std::vector<uint8_t>
ProgramEncoder::encode(
    const Program& program)
{
    auto data =
        program.rawData;

    if (data.size() < 192)
    {
        DBG(
            "PROGRAM ENCODER: INVALID RAW SIZE = "
            + juce::String((int)data.size())
        );

        return {};
    }

    // =========================
// Program Name
// =========================

    for (std::size_t i = 0;
        i < ProgramOffset::General::NameLength;
        ++i)
    {
        char c = ' ';

        if (i < program.name.size())
            c = program.name[i];

        data[
            ProgramOffset::General::Name + i
        ] = encodeAkaiChar(c);
    }

    // =========================
// MIDI Program Number
// PRGNUM offset 15
// =========================

    data[
        ProgramOffset::General::Number
    ] =
        static_cast<uint8_t>(
            juce::jlimit(
                0,
                128,
                program.programNumber
            )
            );



    // =========================
    // Keygroups
    // =========================

    data[
        ProgramOffset::Keygroups::NumKeygroups
    ] =
        static_cast<uint8_t>(
            program.groups
            );

        // =========================
        // General
        // =========================

        data[
            ProgramOffset::General::MidiChannel
        ] =
            static_cast<uint8_t>(
                program.midiChannel
                );

            data[
                ProgramOffset::General::Polyphony
            ] =
                static_cast<uint8_t>(
                    program.polyphony
                    );

                data[
                    ProgramOffset::General::Priority
                ] =
                    static_cast<uint8_t>(
                        program.priority
                        );

                    data[
                        ProgramOffset::General::PlayLow
                    ] =
                        static_cast<uint8_t>(
                            program.playLow
                            );

                        data[
                            ProgramOffset::General::PlayHigh
                        ] =
                            static_cast<uint8_t>(
                                program.playHigh
                                );

                            // =========================
                            // Output
                            // =========================

                            data[
                                ProgramOffset::Output::Output
                            ] =
                                static_cast<uint8_t>(
                                    program.output
                                    );

                                data[
                                    ProgramOffset::Output::Stereo
                                ] =
                                    static_cast<uint8_t>(
                                        program.stereoLevel
                                        );

                                    // Pan is stored as signed 8-bit
                                    data[
                                        ProgramOffset::Output::Pan
                                    ] =
                                        static_cast<uint8_t>(
                                            static_cast<int8_t>(
                                                program.pan
                                                )
                                            );

                                        data[
                                            ProgramOffset::Output::Loudness
                                        ] =
                                            static_cast<uint8_t>(
                                                program.loudness
                                                );

                                            data[
                                                ProgramOffset::Voice::VoiceOutput
                                            ] =
                                                static_cast<uint8_t>(
                                                    program.individualOutputLevel
                                                    );

                                                // Bend Up
                                                data[
                                                    ProgramOffset::Pitch::BendUp
                                                ] =
                                                    static_cast<uint8_t>(
                                                        program.bendUp
                                                        );

                                                    data[ProgramOffset::Output::VelocityLoudness] =
                                                        static_cast<uint8_t>(
                                                            static_cast<int8_t>(
                                                                program.velocityLoudness
                                                                )
                                                            );

                                                    data[ProgramOffset::Keygroups::KeygroupCrossfade] =
                                                        program.keygroupCrossfade ? 1 : 0;


                                                // =========================
                                                // Program Tune
                                                // =========================

                                                const double tune =
                                                    juce::jlimit(
                                                        -50.0,
                                                        50.0,
                                                        program.tune
                                                    );

                                                const int semitone =
                                                    static_cast<int>(
                                                        std::floor(tune)
                                                        );

                                                const double fraction =
                                                    tune
                                                    - static_cast<double>(
                                                        semitone
                                                        );

                                                const int fractionRaw =
                                                    juce::jlimit(
                                                        0,
                                                        255,
                                                        juce::roundToInt(
                                                            fraction * 256.0
                                                        )
                                                    );

                                                data[
                                                    ProgramOffset::Pitch::ProgramTune
                                                ] =
                                                    static_cast<uint8_t>(
                                                        fractionRaw
                                                        );

                                                    data[
                                                        ProgramOffset::LFO::LFO1Delay
                                                    ] =
                                                        static_cast<uint8_t>(
                                                            program.lfo1Delay
                                                            );

                                                    data[
                                                        ProgramOffset::Pitch::ProgramTune + 1
                                                    ] =
                                                        static_cast<uint8_t>(
                                                            static_cast<int8_t>(
                                                                semitone
                                                                )
                                                            );

                                                        data[
                                                            ProgramOffset::LFO::LFO2Rate
                                                        ] =
                                                            static_cast<uint8_t>(
                                                                program.lfo2Rate
                                                                );

                                                            data[
                                                                ProgramOffset::LFO::LFO2Depth
                                                            ] =
                                                                static_cast<uint8_t>(
                                                                    program.lfo2Depth
                                                                    );

                                                                data[
                                                                    ProgramOffset::LFO::LFO2Delay
                                                                ] =
                                                                    static_cast<uint8_t>(
                                                                        program.lfo2Delay
                                                                        );

                                                                    data[
                                                                        ProgramOffset::LFO::LFO2Wave
                                                                    ] =
                                                                        static_cast<uint8_t>(
                                                                            program.lfo2Wave
                                                                            );

                                                                        data[ProgramOffset::LFO::ModWheelDepth] =
                                                                            static_cast<uint8_t>(
                                                                                program.modWheelDepth
                                                                                );

                                                                        data[ProgramOffset::LFO::PressureDepth] =
                                                                            static_cast<uint8_t>(
                                                                                program.pressureDepth
                                                                                );

                                                                        data[ProgramOffset::LFO::VelocityDepth] =
                                                                            static_cast<uint8_t>(
                                                                                program.velocityDepth
                                                                                );

                                                        data[
                                                            ProgramOffset::LFO::LFO1Wave
                                                        ] =
                                                            static_cast<uint8_t>(
                                                                program.lfo1Wave
                                                                );


                                                            data[
                                                                ProgramOffset::SoftPedal::SoftLoudness
                                                            ] =
                                                                static_cast<uint8_t>(
                                                                    program.softLoudness
                                                                    );

                                                                data[
                                                                    ProgramOffset::SoftPedal::SoftAttack
                                                                ] =
                                                                    static_cast<uint8_t>(
                                                                        program.softAttack
                                                                        );

                                                                    data[
                                                                        ProgramOffset::SoftPedal::SoftFilter
                                                                    ] =
                                                                        static_cast<uint8_t>(
                                                                            program.softFilter
                                                                            );

                                                        data[
                                                            ProgramOffset::Pitch::PressurePitch
                                                        ] =
                                                            static_cast<uint8_t>(
                                                                program.pressurePitch
                                                                );

                                                            data[
                                                                ProgramOffset::Pitch::BendDown
                                                            ] =
                                                                static_cast<uint8_t>(
                                                                    program.bendDown
                                                                    );

                                                                data[
                                                                    ProgramOffset::Pitch::BendMode
                                                                ] =
                                                                    static_cast<uint8_t>(
                                                                        program.bendMode
                                                                        );

                                                                    data[
                                                                        ProgramOffset::Pitch::Transpose
                                                                    ] =
                                                                        static_cast<uint8_t>(
                                                                            static_cast<int8_t>(
                                                                                program.transpose
                                                                                )
                                                                            );

                                                                        data[
                                                                            ProgramOffset::Voice::Legato
                                                                        ] =
                                                                            static_cast<uint8_t>(
                                                                                program.legato ? 1 : 0
                                                                                );

                                                                            data[
                                                                                ProgramOffset::Voice::VoiceAssign
                                                                            ] =
                                                                                static_cast<uint8_t>(
                                                                                    program.voiceAssign
                                                                                    );

                                                                                data[
                                                                                    ProgramOffset::Voice::Desync
                                                                                ] =
                                                                                    static_cast<uint8_t>(
                                                                                        program.lfo1Desync ? 1 : 0
                                                                                        );

                                                                                    data[
                                                                                        ProgramOffset::LFO::LFO1Depth
                                                                                    ] =
                                                                                        static_cast<uint8_t>(
                                                                                            program.lfo1Depth
                                                                                            );

                                                        DBG(
                                                            "ENCODE PROGRAM TUNE = "
                                                            + juce::String(tune, 2)
                                                        );

                                                        DBG(
                                                            juce::String::formatted(
                                                                "ENCODE PTUNO RAW = %02X %02X",
                                                                (unsigned)data[
                                                                    ProgramOffset::Pitch::ProgramTune
                                                                ],
                                                                (unsigned)data[
                                                                    ProgramOffset::Pitch::ProgramTune + 1
                                                                ]
                                                            )
                                                        );

                                                        // =========================
// MODULATION SOURCES
// =========================

                                                        data[ProgramOffset::Mod::ModSPan1] =
                                                            static_cast<uint8_t>(program.modSPan1);

                                                        data[ProgramOffset::Mod::ModSPan2] =
                                                            static_cast<uint8_t>(program.modSPan2);

                                                        data[ProgramOffset::Mod::ModSPan3] =
                                                            static_cast<uint8_t>(program.modSPan3);


                                                        data[ProgramOffset::Mod::ModSAmp1] =
                                                            static_cast<uint8_t>(program.modSAmp1);

                                                        data[ProgramOffset::Mod::ModSAmp2] =
                                                            static_cast<uint8_t>(program.modSAmp2);


                                                        data[ProgramOffset::Mod::ModSLFO1Rate] =
                                                            static_cast<uint8_t>(program.modSLfo1Rate);

                                                        data[ProgramOffset::Mod::ModSLFO1Depth] =
                                                            static_cast<uint8_t>(program.modSLfo1Depth);

                                                        data[ProgramOffset::Mod::ModSLFO1Delay] =
                                                            static_cast<uint8_t>(program.modSLfo1Delay);


                                                        data[ProgramOffset::Mod::ModSFilter1] =
                                                            static_cast<uint8_t>(program.modSFilter1);

                                                        data[ProgramOffset::Mod::ModSFilter2] =
                                                            static_cast<uint8_t>(program.modSFilter2);

                                                        data[ProgramOffset::Mod::ModSFilter3] =
                                                            static_cast<uint8_t>(program.modSFilter3);


                                                        data[ProgramOffset::Mod::ModSPitch] =
                                                            static_cast<uint8_t>(program.modSPitch);

                                                        data[ProgramOffset::Mod::ModSAmp3] =
                                                            static_cast<uint8_t>(program.modSAmp3);


                                                        // =========================
                                                        // MODULATION AMOUNTS
                                                        // signed -50 .. +50
                                                        // =========================

                                                        data[ProgramOffset::Mod::ModVPan1] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVPan1)
                                                                );

                                                        data[ProgramOffset::Mod::ModVPan2] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVPan2)
                                                                );

                                                        data[ProgramOffset::Mod::ModVPan3] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVPan3)
                                                                );


                                                        data[ProgramOffset::Mod::ModVAmp1] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVAmp1)
                                                                );

                                                        data[ProgramOffset::Mod::ModVAmp2] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVAmp2)
                                                                );


                                                        data[ProgramOffset::Mod::ModVLFO1Rate] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVLfo1Rate)
                                                                );

                                                        data[ProgramOffset::Mod::ModVLFO1Depth] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVLfo1Depth)
                                                                );

                                                        data[ProgramOffset::Mod::ModVLFO1Delay] =
                                                            static_cast<uint8_t>(
                                                                static_cast<int8_t>(program.modVLfo1Delay)
                                                                );

                                                        // =========================
                                                        // Done
                                                        // =========================

                                                        return data;
}