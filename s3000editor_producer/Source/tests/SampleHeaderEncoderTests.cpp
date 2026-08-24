#include <JuceHeader.h>

#include "../s3000/SampleHeaderParser.h"
#include "../s3000/SampleHeaderEncoder.h"
#include "../s3000/S3000Types.h"

static std::vector<uint8_t> loadSampleFixture(
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

class SampleHeaderEncoderTest
    : public juce::UnitTest
{
public:
    SampleHeaderEncoderTest()
        : juce::UnitTest(
            "Sample Header Encoder",
            "S3000"
        )
    {
    }

    void runTest() override
    {
        beginTest(
            "Real S3000XL sample header binary round trips losslessly"
        );

        {
            juce::File fixture(
                "C:\\Users\\yasuto\\s3000editor_projucer\\"
                "s3000editor_producer\\Source\\tests\\fixtures\\"
                "sample_header_basic.bin"
            );





            auto original =
                loadSampleFixture(fixture);

            auto parsed =
                SampleHeaderParser::parse(
                    original,
                    0
                );

            auto encoded =
                SampleHeaderEncoder::encode(
                    parsed
                );

            expectEquals(
                static_cast<int>(encoded.size()),
                static_cast<int>(original.size())
            );

            for (int i = 0;
                i < static_cast<int>(original.size());
                ++i)
            {
                expectEquals(
                    static_cast<int>(encoded[i]),
                    static_cast<int>(original[i]),
                    "Round-trip mismatch at offset "
                    + juce::String(i)
                );
            }

            expect(
                !original.empty(),
                "Could not load sample header fixture"
            );

            if (original.empty())
                return;

            expectEquals(
                static_cast<int>(original.size()),
                141
            );

            // Parser / Encoderïîï™ÇÕ
            // é¿ç€ÇÃAPIÇ…çáÇÌÇπÇƒéüÇ…í«â¡
        }

        beginTest(
            "Changing sample start only changes START bytes"
        );

        {
            juce::File fixture(
                "C:\\Users\\yasuto\\s3000editor_projucer\\"
                "s3000editor_producer\\Source\\tests\\fixtures\\"
                "sample_header_basic.bin"
            );

            auto original =
                loadSampleFixture(fixture);

            expect(
                !original.empty(),
                "Could not load sample header fixture"
            );

            if (original.empty())
                return;

            auto baseline =
                SampleHeaderParser::parse(
                    original,
                    0
                );

            auto before =
                SampleHeaderEncoder::encode(
                    baseline
                );

            auto changed = baseline;

            // é¿ã@fixtureÇ≈ÇÕ START = 5
            changed.start = 10;

            auto after =
                SampleHeaderEncoder::encode(
                    changed
                );

            DBG("=== START MUTATION DIFF ===");

            int changedBytes = 0;

            for (int i = 0;
                i < static_cast<int>(before.size());
                ++i)
            {
                if (before[i] != after[i])
                {
                    ++changedBytes;

                    DBG(
                        "offset "
                        + juce::String(i)
                        + ": "
                        + juce::String((int)before[i])
                        + " -> "
                        + juce::String((int)after[i])
                    );
                }
            }

            expect(
                changedBytes > 0,
                "Changing START did not change encoded data"
            );

            auto reparsed =
                SampleHeaderParser::parse(
                    after,
                    0
                );

            expectEquals(
                static_cast<int>(reparsed.start),
                10
            );
        }

        beginTest(
            "Changing sample tune survives encode parse round trip"
        );

        {
            juce::File fixture(
                "C:\\Users\\yasuto\\s3000editor_projucer\\"
                "s3000editor_producer\\Source\\tests\\fixtures\\"
                "sample_header_basic.bin"
            );

            auto original =
                loadSampleFixture(fixture);

            expect(
                !original.empty(),
                "Could not load sample header fixture"
            );

            if (original.empty())
                return;

            auto baseline =
                SampleHeaderParser::parse(
                    original,
                    0
                );

            auto before =
                SampleHeaderEncoder::encode(
                    baseline
                );

            auto changed = baseline;

            changed.tune = 2.25;

            auto after =
                SampleHeaderEncoder::encode(
                    changed
                );

            DBG("=== TUNE MUTATION DIFF ===");

            int changedBytes = 0;

            for (int i = 0;
                i < static_cast<int>(before.size());
                ++i)
            {
                if (before[i] != after[i])
                {
                    ++changedBytes;

                    DBG(
                        "offset "
                        + juce::String(i)
                        + ": "
                        + juce::String((int)before[i])
                        + " -> "
                        + juce::String((int)after[i])
                    );
                }
            }

            expect(
                changedBytes > 0,
                "Changing tune did not change encoded data"
            );

            auto reparsed =
                SampleHeaderParser::parse(
                    after,
                    0
                );

            expectWithinAbsoluteError(
                reparsed.tune,
                2.25,
                0.01
            );
        }

    }
};

static SampleHeaderEncoderTest
sampleHeaderEncoderTest;