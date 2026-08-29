#include "ProgramEncoder.h"
#include "Offsets.h"
#include <cmath>

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
                                                        ProgramOffset::Pitch::ProgramTune + 1
                                                    ] =
                                                        static_cast<uint8_t>(
                                                            static_cast<int8_t>(
                                                                semitone
                                                                )
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
                                                        // Done
                                                        // =========================

                                                        return data;
}