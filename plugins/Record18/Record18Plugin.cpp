#include <cmath>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>
#include "DistrhoPlugin.hpp"
#include "Write.cpp"


START_NAMESPACE_DISTRHO


const uint NUM_OF_CHANNELS = 18;
const float FRAGMENT_DUR_S = 10;
const uint NUM_OF_FRAGMENTS_IN_MEM = 5;
const uint32_t MIN_SAMPLE_RATE = 48000;
const uint32_t MAX_SAMPLE_RATE = 48000;
const uint32_t MIN_BLOCK_SIZE = 16;
const uint32_t MAX_BLOCK_SIZE = 32768;
const float THREAD_SLEEP_DUR_S = 1;

const uint32_t MAX_CHANNEL_FRAGMENT_SIZE = MAX_SAMPLE_RATE * FRAGMENT_DUR_S + MAX_BLOCK_SIZE;
const uint32_t BUFFER_SIZE
    = MAX_CHANNEL_FRAGMENT_SIZE * NUM_OF_CHANNELS * NUM_OF_FRAGMENTS_IN_MEM;
const uint32_t IS_FILLED_SIZE = MAX_SAMPLE_RATE * FRAGMENT_DUR_S / MIN_BLOCK_SIZE + 2;


class Record18Plugin : public Plugin
{
public:
    Record18Plugin()
        : Plugin(0, 0, 0),
          currentBlockSize(MIN_BLOCK_SIZE),
          buffer(nullptr),
          isFilled(nullptr),
          isEnding(false)
    {
        buffer = new float[BUFFER_SIZE]();
        isFilled = new bool[IS_FILLED_SIZE]();

        currentSampleRate = getSampleRate();
        environmentChanged();

        writerThread = std::thread(&Record18Plugin::write, this);
    }

    ~Record18Plugin() override
    {
        isEnding = true;
        writerThread.join();
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
                buffer[i] = inputChannel[i % blockSize];
            }
        }

        isFilled[blockI] = true;
        blockI++;
        blockI %= totalBlocks;
    }

private:
    uint currentSampleRate, currentBlockSize;
    std::mutex lock;
    uint32_t totalBlockSize, blocksPerFragment, totalBlocks;
    uint32_t blockI;
    float* buffer;
    bool* isFilled;
    bool isEnding;
    std::thread writerThread;

    void verifyEnvironment()
    {
        if (currentSampleRate < MIN_SAMPLE_RATE || currentSampleRate > MAX_SAMPLE_RATE)
        {
            std::ostringstream err;
            err << "sample rate ("
                << currentSampleRate
                << ") should be in range ("
                << MIN_SAMPLE_RATE
                << " <= x <= "
                << MAX_SAMPLE_RATE
                << ")";
            throw std::runtime_error(err.str());
        }

        if (currentBlockSize < MIN_BLOCK_SIZE || currentBlockSize > MAX_BLOCK_SIZE)
        {
            std::ostringstream err;
            err << "block size ("
                << currentBlockSize
                << ") should be in range ("
                << MIN_BLOCK_SIZE
                << " <= x <= "
                << MAX_BLOCK_SIZE
                << ")";
            throw std::runtime_error(err.str());
        }
    }

    void environmentChanged()
    {
        verifyEnvironment();

        totalBlockSize = currentBlockSize * NUM_OF_CHANNELS;

        uint32_t samplesPerFragment = std::ceil(FRAGMENT_DUR_S * currentSampleRate);
        samplesPerFragment += samplesPerFragment % currentBlockSize;

        lock.lock();

        blocksPerFragment = samplesPerFragment / currentBlockSize;
        totalBlocks = blocksPerFragment * NUM_OF_FRAGMENTS_IN_MEM;

        blockI = 0;

        for (uint32_t i; i < IS_FILLED_SIZE; i++)
        {
            isFilled[i] = false;
        }

        lock.unlock();
    }

    void cleanup()
    {
        delete[] buffer;
        delete[] isFilled;
    }

    void write()
    {
        uint32_t blocksPerFragmentLocal, totalBlocksLocal;
        uint32_t lastBlockIInFragment;
        FragmentWriter fragmentWriter();
        while (!isEnding)
        {
            lock.lock();

            blocksPerFragmentLocal = blocksPerFragment;
            totalBlocksLocal = totalBlocks;

            lastBlockIInFragment = blocksPerFragment - 1;

            lock.unlock();

            if (isFilled[lastBlockIInFragment])
            {
                isFilled[lastBlockIInFragment] = false;

                fragmentWriter.tryWriteFragment();

                lastBlockIInFragment += localBlocksPerFragment;
                lastBlockIInFragment %= totalBlocksLocal;
            }

            std::this_thread::sleep_for(std::chrono::duration<float>(THREAD_SLEEP_DUR_S));
        }

        cleanup();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Record18Plugin)
};


Plugin* createPlugin()
{
    return new Record18Plugin();
}


END_NAMESPACE_DISTRHO
