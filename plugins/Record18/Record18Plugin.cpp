#include "DistrhoPlugin.hpp"


START_NAMESPACE_DISTRHO


class Record18Plugin : public Plugin
{
public:
    Record18Plugin()
        : Plugin(0, 0, 0)
    {}

    ~Record18Plugin() override
    {}

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

    void run(const float** inputs, float**, uint32_t frames) override
    {}

private:
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Record18Plugin)
};


Plugin* createPlugin()
{
    return new Record18Plugin();
}


END_NAMESPACE_DISTRHO
