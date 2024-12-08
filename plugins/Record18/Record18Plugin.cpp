#include <cmath>
#include "DistrhoPlugin.hpp"


START_NAMESPACE_DISTRHO


const uint NUM_OF_CHANNELS = 18;
const float FRAGMENT_DUR_S = 10;
const uint NUM_OF_FRAGMENTS_IN_MEM = 5;
const uint32_t MIN_SAMPLE_RATE = 48000;
const uint32_t MAX_SAMPLE_RATE = 48000;
const uint32_t MIN_BLOCK_SIZE = 16;
const uint32_t MAX_BLOCK_SIZE = 32768;

const uint32_t MAX_FRAGMENT_SIZE = MAX_SAMPLE_RATE * FRAGMENT_DUR_S + MAX_BLOCK_SIZE;
const uint32_t BUFFER_SIZE = MAX_FRAGMENT_SIZE * NUM_OF_FRAGMENTS_IN_MEM;
const uint32_t IS_FILLED_SIZE = MAX_SAMPLE_RATE * FRAGMENT_DUR_S / MIN_BLOCK_SIZE + 2;


class Record18Plugin : public Plugin
{
public:
    Record18Plugin()
        : Plugin(0, 0, 0),
          currentSampleRate(MIN_SAMPLE_RATE),
          currentBlockSize(MIN_BLOCK_SIZE),
          blockI(0),
          buffer(nullptr),
          isFilled(nullptr)
    {
        buffer = new float[BUFFER_SIZE]();
        isFilled = new bool[IS_FILLED_SIZE]();

        currentSampleRate = getSampleRate();
        environmentChanged();
    }

    ~Record18Plugin() override
    {
        delete[] buffer;
        delete[] isFilled;
    }

protected:
    const char* getLabel() const override
    {
        return "Record18";
    }

    const char* getDescription() const override
    {
        return "Record 18 channels of audio to disk in 10-second chunks.";
    }

    const char* getMaker() const override
    {
        return "to7m";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/to7m/to7m-distrho-plugins";
    }

    const char* getLicense() const override
    {
        return "AGPL-3.0+";
    }

    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('t', 'R', '1', '8');
    }

    void sampleRateChanged(double newSampleRate) override
    {
        currentSampleRate = newSampleRate;
        environmentChanged();
    }

    void run(const float** inputChannels, float**, uint32_t blockSize) override
    {
        if (blockSize != currentBlockSize) {
            currentBlockSize = blockSize;
            environmentChanged();
        }

        // debugging
        const uint32_t bufferSize = BUFFER_SIZE;
        uint32_t sampleOffsetFromBlock;
        float sample;

        const float* inputChannel;
        uint32_t startBufferI;
        uint32_t stopBufferI = blockI * totalBlockSize;
        for (uint32_t channelI = 0; channelI < NUM_OF_CHANNELS; channelI++)
        {
            inputChannel = inputChannels[channelI];
            startBufferI = stopBufferI;
            stopBufferI = startBufferI + blockSize;

            for (uint32_t i = startBufferI; i < stopBufferI; i++)
            {
                // debugging
                sampleOffsetFromBlock = i % blockSize;
                sample = inputChannel[sampleOffsetFromBlock];

                buffer[i] = sample;
            }
        }

        isFilled[blockI] = true;
        blockI++;
        blockI %= totalBlocks;
    }

private:
    uint currentSampleRate, currentBlockSize;
    uint32_t totalBlockSize, totalBlocks;
    uint32_t blockI;
    float* buffer;
    bool* isFilled;
    bool isEnvironmentChanged;

    void environmentChanged()
    {
        totalBlockSize = currentBlockSize * NUM_OF_CHANNELS;

        uint32_t samplesPerFragment = std::ceil(FRAGMENT_DUR_S * currentSampleRate);
        samplesPerFragment += samplesPerFragment % currentBlockSize;
        uint32_t blocksPerFragment = samplesPerFragment / currentBlockSize;
        totalBlocks = blocksPerFragment * NUM_OF_FRAGMENTS_IN_MEM;

        blockI = 0;
        isFilled[0] = false;
        isEnvironmentChanged = true;
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Record18Plugin)
};


Plugin* createPlugin()
{
    return new Record18Plugin();
}


END_NAMESPACE_DISTRHO
