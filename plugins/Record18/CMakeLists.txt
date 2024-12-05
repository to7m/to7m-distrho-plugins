# CMake file for DISTRHO Plugins #
# ------------------------------ #

dpf_add_plugin(record_18
  TARGETS ladspa lv2 vst2 vst3 clap jack
  FILES_DSP
      Record18Plugin.cpp)

target_include_directories(
  record_18 PUBLIC ".")
