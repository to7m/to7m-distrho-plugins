NAME = record_xr18

FILES_DSP = plugins/RecordXr18/RecordXr18Plugin.cpp

include DPF/Makefile.plugins.mk

TARGETS += ladspa
TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap
TARGETS += jack

all: $(TARGETS)
