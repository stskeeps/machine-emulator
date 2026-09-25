# Copyright Cartesi and individual authors (see AUTHORS)
# SPDX-License-Identifier: LGPL-3.0-or-later

ROOT := ../..
BUILDDIR ?= build
TARGET := $(BUILDDIR)/cartesi-machine-load-first-cycle
CXX ?= g++
CXXFLAGS ?= -O3 -g -std=gnu++23 -fopenmp -Wall -Wextra -Wpedantic
LDLIBS ?= -lslirp -fopenmp

.PHONY: all emulator-libs clean

all: $(TARGET)

emulator-libs:
	$(MAKE) -C $(ROOT)/src libcartesi.a libcartesi_hash_tree.a

$(TARGET): cartesi-machine-load-first-cycle.cpp emulator-libs
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(ROOT)/src -o $@ $< \
		$(ROOT)/src/libcartesi.a $(ROOT)/src/libcartesi_hash_tree.a $(LDLIBS)

clean:
	rm -f $(TARGET)
