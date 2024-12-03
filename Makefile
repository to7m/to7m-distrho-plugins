include DPF/Makefile.plugins.mk


.PHONY: record_xr18


all: record_xr18


record_xr18:
	$(MAKE) -C plugins/RecordXr18
