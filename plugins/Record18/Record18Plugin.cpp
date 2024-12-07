#include <cmath>
#include "DistrhoPlugin.hpp"


START_NAMESPACE_DISTRHO


const NUM_OF_CHANNELS = 18;
const DEFAULT_SAMPLE_RATE = 48000;
const DEFAULT_FRAMES = 64;
const FRAGMENT_DUR_S = 10;
const NUM_OF_FRAGMENTS_IN_MEM = 5;


class Record18Plugin : public Plugin
{
public:
    Record18Plugin()
        : Plugin(0, 0, 0),
          currentSampleRate(DEFAULT_SAMPLE_RATE),
          currentFrames(DEFAULT_FRAMES),
          totalFragmentSize(0),
          totalBlockSize(0),
          blocksPerFragment(0),
          buffer(nullptr),
          isFilled(nullptr)
    {
        initialise();
        isFilled = new bool[NUM_OF_FRAGMENTS_IN_MEM];
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
        initialise();
    }

    void run(const float** inputs, float**, uint32_t frames) override
    {
        if (frames != prevFrames) {
            currentFrames = frames;
            initialise();
        }

        uint32_t startBufferI;
        uint32_t stopBufferI = fragmentI * totalFragmentSize + blockI * totalBlockSize;

        for (uint32_t channelNum; channelNum < NUM_OF_CHANNELS; channelNum++)
        {
            input = inputs[channelNum];
            startBufferI = stopBufferI;
            stopBufferI = startBufferI + currentFrames;

            for (uint32_t i = startBufferI; i < stopBufferI; i++)
            {
                buffer[i] = input[i % currentFrames];
            }
        }
    }

private:
    uint32_t currentSampleRate, currentFrames;
    uint32_t totalFragmentSize, totalBlockSize, blocksPerFragment;
    uint32_t fragmentI, blockI;
    float* buffer;
    bool* isFilled;

    void initialise()
    {
        totalBlockSize = NUM_OF_CHANNELS * currentFrames;
        totalFragmentSize = std::ceil(FRAGMENT_DUR_S * currentSampleRate);
        totalFragmentSize += totalFragmentSize % totalBlockSize;
        blocksPerFragment = totalFragmentSize / totalBlockSize;

        fragmentI = blockI = 0;

        uint32_t bufferSize = totalFragmentSize * NUM_OF_FRAGMENTS_IN_MEM;
        delete[] buffer;
        buffer = new float[bufferSize]();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Record18Plugin)
};


Plugin* createPlugin()
{
    return new Record18Plugin();
}


END_NAMESPACE_DISTRHO
