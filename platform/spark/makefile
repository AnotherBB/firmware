
$(MAKECMDGOALS) %: run_make

.PHONY: run_make

APPDIR = ../../firmware
SPARKDIR = $(APPDIR)/platform/spark
APP ?= controller

run_make:
	@$(MAKE) --debug=b -e -C ../../../spark-firmware/main APPDIR=$(APPDIR) \
	USER_MAKEFILE=$(APP).mk \
	TARGET_FILE=brewpi TARGET_DIR=$(SPARKDIR)/target $(MAKEOVERRIDES) $(MAKECMDGOALS) 
