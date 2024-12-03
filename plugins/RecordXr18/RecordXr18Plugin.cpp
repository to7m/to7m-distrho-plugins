#include "DistrhoPlugin.hpp"


START_NAMESPACE_DISTRHO


class RecordXr18 : public Plugin
{
public:
    RecordXr18()
        : Plugin(0, 0, 0)
    {}

    ~RecordXr18() override
    {}

protected:
    void run(const float** inputs, float**, uint32_t frames) override
    {}

private:
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordXr18)
};


Plugin* createPlugin()
{
    return new RecordXr18();
}


END_NAMESPACE_DISTRHO
