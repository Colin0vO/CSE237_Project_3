# This makefile was tested with Vivado HLS 2019.2
# and Vitis v2025.1
# 
#

# select the HLS compiler to use (vivado_hls or vitis)
TOP = dft
# -------------------------------------------------------

USE_VITIS=1

# Vitis environment setup script (used when invoking vitis-run directly)
SETTINGS64 ?= C:\\Xilinx\\2025.1\\Vitis\\settings64.bat
VITIS_RUN_CMD = cmd /c "call $(SETTINGS64) && vitis-run --mode hls --tcl"

ifeq ($(USE_VITIS),1)

# Prefer environment provided by settings64.bat
ifndef XILINX_VITIS
$(error XILINX_VITIS is not set. Please run C:\\Xilinx\\2025.1\\Vitis\\settings64.bat (or your installed version) to setup environment.)
endif

# Use Vitis root for HLS headers
VHLS := $(XILINX_VITIS)

HLS_COMPILER=vitis-run --mode hls --tcl 
else
VHLS := $(shell vivado_hls -r)
HLS_COMPILER=vivado_hls -f
endif


EXAMPLES_CPP := $(wildcard *_test.cpp)
EXAMPLES_BIN = $(patsubst %.cpp,%,$(EXAMPLES_CPP))
EXAMPLES_LOG = $(patsubst %.cpp,%.log,$(EXAMPLES_CPP))

CC = gcc
CXX = g++
PYTHON ?= python

# insert new LD path to the LD_LIBRARY_PATH if it is not already there
# 
LD_PATH_NEW = /usr/lib/x86_64-linux-gnu
LD_LIBRARY_PATH := $(shell echo $(value LD_LIBRARY_PATH) | grep -q $(LD_PATH_NEW) || echo $(LD_PATH_NEW):)$(value LD_LIBRARY_PATH)

# ------------ Set the script path to generate tcl scripts ----------------
GEN_TCL = $(CURDIR)/../../scripts/gen_hls_runner_script.py
# -------------------------------------------------------------------------

#HLS_CONFIG_FILE = $(CURDIR)/__hls_config__.ini
HLS_CONFIG_FILE = $(CURDIR)/__hls_config__.ini

#$(warning $(EXAMPLES_LOG))

%.o: %.cpp
	$(CXX) -I$(VHLS)/include -c $< -o $@


%_test.bin: %_test.cpp %.cpp 
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(CXX) -I$(VHLS)/include $? -g -o $@

#  missing cos_table and sin_table definition. No testbench files available
#	dft.c \
#	dft_precompute.c \


CXX_HLS_TARGET_FILES = \
	$(TOP).cpp

HLS_TARGETS = $(patsubst %.cpp,%.comp,$(CXX_HLS_TARGET_FILES))

%.comp: %.tcl
	$(HLS_COMPILER) $<


%.tcl: %.cpp
	$(PYTHON) $(GEN_TCL) -c $(HLS_CONFIG_FILE) -i $< -o $@

%.log: %.bin
	./$< > $@ 2>&1


# Default: run csim then csynth via dedicated Tcl files
all: csim hls

# Compatibility target: run synthesis using dft.tcl
hls: dft.tcl
	$(VITIS_RUN_CMD) dft.tcl

# C simulation only using dft_csim.tcl
csim: dft_csim.tcl
	$(VITIS_RUN_CMD) dft_csim.tcl

test: $(EXAMPLES_LOG)

report: hls
	cp -f $(TOP).comp/hls/syn/report/$(TOP)_csynth.rpt $(CURDIR) || true

$(CURDIR)/export_$(TOP).tcl:
	@{ \
	    echo "open_component $(TOP).comp"; \
		echo "export_design -format ip_catalog -rtl verilog -version 1.0 -ipname $(TOP) "; \
		echo "exit"; \
	} > $(CURDIR)/export_$(TOP).tcl


ip: hls $(CURDIR)/export_$(TOP).tcl
	$(HLS_COMPILER) $(CURDIR)/export_$(TOP).tcl

clean:
	-rm -rf *.o *.log *.bin *.tcl *.comp *.rpt logs hls out.dat *.jou

.PHONY: clean hls csim all test report ip

# Prevent make from deleting the .bin intermediate files
.PRECIOUS: %.bin %.tcl 