#include <JuceHeader.h>

#include "../s3000/KeygroupEncoder.h"
#include "../s3000/Offsets.h"
#include "../s3000/S3000Types.h"
#include "../s3000/KeygroupParser.h"

static std::vector<uint8_t> loadFixture(
    const juce::File& file)
{
    juce::MemoryBlock block;

    if (!file.loadFileAsData(block))
        return {};

    const auto* data =
        static_cast<const uint8_t*>(
            block.getData()
            );

    return std::vector<uint8_t>(
        data,
        data + block.getSize()
    );
}


class KeygroupEncoderTest
    : public juce::UnitTest
{
public:
    KeygroupEncoderTest()
        : juce::UnitTest(
            "Keygroup Encoder",
            "S3000"
        )
    {
    }

    void runTest() override
    {
        beginTest(
            "Velocity zone is encoded correctly"
        );

        Keygroup kg;

        kg.rawData.resize(192, 0);

        kg.zones[0].lowVel = 30;
        kg.zones[0].highVel = 100;

        auto encoded =
            KeygroupEncoder::encode(kg);

        expectEquals(
            static_cast<int>(
                encoded[
                    KeygroupFullOffset::Zone::LOVEL[0]
                ]
                ),
            30
        );

        expectEquals(
            static_cast<int>(
                encoded[
                    KeygroupFullOffset::Zone::HIVEL[0]
                ]
                ),
            100
        );


        beginTest(
            "Velocity zone 2 is encoded correctly"
        );

        Keygroup kg2;

        kg2.rawData.resize(192, 0);

        kg2.zones[1].lowVel = 40;
        kg2.zones[1].highVel = 110;

        auto encoded2 =
            KeygroupEncoder::encode(kg2);

        expectEquals(
            static_cast<int>(
                encoded2[
                    KeygroupFullOffset::Zone::LOVEL[1]
                ]
                ),
            40
        );

        expectEquals(
            static_cast<int>(
                encoded2[
                    KeygroupFullOffset::Zone::HIVEL[1]
                ]
                ),
            110
        );

        beginTest(
            "Changing low velocity only changes target byte"
        );

        {
            Keygroup baseline;
            baseline.rawData.resize(192, 0);

            baseline.zones[0].lowVel = 20;
            baseline.zones[0].highVel = 100;

            auto before =
                KeygroupEncoder::encode(baseline);

            Keygroup changed = baseline;
            changed.zones[0].lowVel = 30;

            auto after =
                KeygroupEncoder::encode(changed);

            const int targetOffset =
                KeygroupFullOffset::Zone::LOVEL[0];

            expectEquals(
                static_cast<int>(after[targetOffset]),
                30
            );

            for (int i = 0;
                i < static_cast<int>(after.size());
                ++i)
            {
                if (i == targetOffset)
                {
                    expect(
                        before[i] != after[i],
                        "Target byte did not change"
                    );
                }
                else
                {
                    expectEquals(
                        static_cast<int>(after[i]),
                        static_cast<int>(before[i]),
                        "Unexpected change at offset "
                        + juce::String(i)
                    );
                }
            }
        }

        beginTest(
            "Changing high velocity only changes target byte"
        );

        {
            Keygroup baseline;
            baseline.rawData.resize(192, 0);

            baseline.zones[0].lowVel = 20;
            baseline.zones[0].highVel = 100;

            auto before =
                KeygroupEncoder::encode(baseline);

            Keygroup changed = baseline;
            changed.zones[0].highVel = 110;

            auto after =
                KeygroupEncoder::encode(changed);

            const int targetOffset =
                KeygroupFullOffset::Zone::HIVEL[0];

            expectEquals(
                static_cast<int>(after[targetOffset]),
                110
            );

            for (int i = 0;
                i < static_cast<int>(after.size());
                ++i)
            {
                if (i == targetOffset)
                {
                    expect(
                        before[i] != after[i],
                        "Target byte did not change"
                    );
                }
                else
                {
                    expectEquals(
                        static_cast<int>(after[i]),
                        static_cast<int>(before[i]),
                        "Unexpected change at offset "
                        + juce::String(i)
                    );
                }
            }
        }

        beginTest(
            "Changing pan only changes target byte"
        );

        Keygroup baselinePan;
        baselinePan.rawData.resize(192, 0);

        baselinePan.zones[0].pan = 0;

        auto beforePan =
            KeygroupEncoder::encode(baselinePan);

        Keygroup changedPan = baselinePan;
        changedPan.zones[0].pan = 25;

        auto afterPan =
            KeygroupEncoder::encode(changedPan);

        const int panOffset =
            KeygroupFullOffset::Zone::VPANO[0];

        expectEquals(
            static_cast<int>(afterPan[panOffset]),
            25
        );

        for (int i = 0;
            i < static_cast<int>(afterPan.size());
            ++i)
        {
            if (i == panOffset)
            {
                expect(
                    beforePan[i] != afterPan[i],
                    "Pan target byte did not change"
                );
            }
            else
            {
                expectEquals(
                    static_cast<int>(afterPan[i]),
                    static_cast<int>(beforePan[i]),
                    "Unexpected change at offset "
                    + juce::String(i)
                );
            }
        }

//        juce::UnitTestRunner runner;


    }
};

static KeygroupEncoderTest
keygroupEncoderTest;
