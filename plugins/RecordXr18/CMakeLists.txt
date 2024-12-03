# CMake file for DISTRHO Plugins #
# ------------------------------ #

dpf_add_plugin(record_xr18
  TARGETS ladspa lv2 vst2 vst3 clap jack
  FILES_DSP
      RecordXr18Plugin.cpp)

target_include_directories(
  record_xr18 PUBLIC ".")
