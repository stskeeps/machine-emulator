# Copyright 2019 Cartesi Pte. Ltd.
#
# This file is part of the machine-emulator. The machine-emulator is free
# software: you can redistribute it and/or modify it under the terms of the GNU
# Lesser General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# The machine-emulator is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the machine-emulator. If not, see http://www.gnu.org/licenses/.
#

# Every new emulator release should bump this constant when its RISC-V interpreter change
EMULATOR_MARCHID=15

# Every new emulator release should bump these constants
EMULATOR_VERSION_MAJOR=0
EMULATOR_VERSION_MINOR=14
EMULATOR_VERSION_PATCH=0
EMULATOR_VERSION_LABEL=

UNAME:=$(shell uname)

BUILDDIR ?= $(abspath ../build/$(UNAME)_$(shell uname -m))

LUA_INC:=$(shell pkg-config --cflags-only-I lua5.3)
LUA_LIB:=-llua5.3
LUA_BIN?=lua5.3
GRPC_DIR:=../lib/grpc-interfaces

PROTOC:=$(BUILDDIR)/bin/protoc
GRPC_CPP_PLUGIN=$(BUILDDIR)/bin/grpc_cpp_plugin

# So we can use gcc to check if there are unused symbols
ifeq ($(gc),yes)
GCFLAGS=-fno-inline-small-functions -fno-inline -fdata-sections -ffunction-sections
GCLDFLAGS=-Wl,--gc-sections,--print-gc-sections
endif

# Mac OS X specific setup
SOLDFLAGS_Darwin:=-bundle -undefined dynamic_lookup
CC_Darwin=clang
CXX_Darwin=clang++
INCS_Darwin=

ifeq ($(UNAME),Darwin)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
export MACOSX_DEPLOYMENT_TARGET := $(shell sw_vers -productVersion | sed -r  "s/([[:digit:]]+)\.([[:digit:]]+)\..+/\1.\2.0/")
endif
SOLDFLAGS_Darwin+=-Wl,-rpath,$(BUILDDIR)/lib -Wl,-rpath,$(CURDIR)
# Homebrew installation
ifneq (,$(wildcard /usr/local/opt/boost/lib))
BOOST_LIB_DIR_Darwin=-L/usr/local/opt/boost/lib
BOOST_INC_Darwin=-I/usr/local/opt/boost/include
CRYPTOPP_LIB_Darwin:=-L/usr/local/opt/cryptopp/lib -lcryptopp
CRYPTOPP_INC_Darwin:=-I/usr/local/opt/cryptopp/include
NLOHMANN_JSON_INC_Darwin:=-I/usr/local/opt/nlohmann-json/include
B64_LIB_Darwin:=-L/usr/local/opt/libb64/lib -lb64
B64_INC_Darwin:=-I/usr/local/opt/libb64/include
else # Macports installation
BOOST_LIB_DIR_Darwin=-L/opt/local/lib
BOOST_INC_Darwin=-I/opt/local/include
CRYPTOPP_LIB_Darwin:=-L/opt/local/lib -lcryptopp
CRYPTOPP_INC_Darwin:=-I/opt/local/include
B64_LIB_Darwin:=-L/opt/local/lib -lb64
B64_INC_Darwin:=-I/opt/local/include
endif
endif

BOOST_CORO_LIB_Darwin:=$(BOOST_LIB_DIR_Darwin) -lboost_coroutine-mt -lboost_context-mt
BOOST_FILESYSTEM_LIB_Darwin:=$(BOOST_LIB_DIR_Darwin) -lboost_system-mt -lboost_filesystem-mt
BOOST_LOG_LIB_Darwin:=$(BOOST_LIB_DIR_Darwin) -lboost_log-mt -lboost_log_setup-mt -lboost_thread-mt
BOOST_PROCESS_LIB_Darwin:=-lpthread
GRPC_INC_Darwin=-I$(BUILDDIR)/include
GRPC_LIB_Darwin=-L$(BUILDDIR)/lib -lgrpc++ -lgrpc -lgpr -lprotobuf -lpthread -labsl_synchronization
PROTOBUF_LIB_Darwin:=-L$(BUILDDIR)/lib -lprotobuf -lpthread
LIBCARTESI_Darwin=libcartesi.dylib
LIBCARTESI_LDFLAGS_Darwin=-dynamiclib -undefined dynamic_lookup -install_name '@rpath/$(LIBCARTESI_Darwin)'
LIBCARTESI_TESTS_LDFLAGS_Darwin=-Wl,-rpath,$(BUILDDIR)/lib -Wl,-rpath,$(CURDIR)
LIBCARTESI_PROTOBUF_Darwin=libcartesi_protobuf.dylib
LIBCARTESI_PROTOBUF_LDFLAGS_Darwin=-dynamiclib -undefined dynamic_lookup -install_name '@rpath/$(LIBCARTESI_PROTOBUF_Darwin)'
LIBCARTESI_GRPC_Darwin=libcartesi_grpc.dylib
LIBCARTESI_GRPC_LDFLAGS_Darwin=-dynamiclib -undefined dynamic_lookup -install_name '@rpath/$(LIBCARTESI_GRPC_Darwin)'
LIBCARTESI_GRPC_TESTS_LDFLAGS_Darwin=-Wl,-rpath,$(BUILDDIR)/lib -Wl,-rpath,$(CURDIR)
LIBCARTESI_JSONRPC_Darwin=libcartesi_jsonrpc.dylib
LIBCARTESI_JSONRPC_LDFLAGS_Darwin=-dynamiclib -undefined dynamic_lookup -install_name '@rpath/$(LIBCARTESI_JSONRPC_Darwin)'
LIBCARTESI_JSONRPC_TESTS_LDFLAGS_Darwin=-Wl,-rpath,$(BUILDDIR)/lib -Wl,-rpath,$(CURDIR)
CARTESI_EXECUTABLE_LDFLAGS_Darwin=-Wl,-rpath,$(BUILDDIR)/lib -Wl,-rpath,$(CURDIR)
PROFILE_DATA_Darwin=default.profdata

# Linux specific setup
SOLDFLAGS_Linux:=-shared -fPIC -pthread
CC_Linux=gcc
CXX_Linux=g++
INCS_Linux=
FS_LIB_Linux=-lstdc++fs
BOOST_CORO_LIB_Linux:=-lboost_coroutine -lboost_context
BOOST_FILESYSTEM_LIB_Linux:=-lboost_system -lboost_filesystem
BOOST_LOG_LIB_Linux:=-lboost_log -lboost_log_setup -lboost_thread
BOOST_PROCESS_LIB_Linux:=-lpthread
BOOST_INC_Linux=
B64_LIB_Linux:=-lb64
B64_INC_Linux:=
CRYPTOPP_LIB_Linux:=-lcryptopp
CRYPTOPP_INC_Linux:=
GRPC_INC_Linux:=-I$(BUILDDIR)/include
GRPC_LIB_Linux:=-L$(BUILDDIR)/lib -lgrpc++ -lgrpc -lgpr -lprotobuf -lpthread -labsl_synchronization
PROTOBUF_LIB_Linux:=-L$(BUILDDIR)/lib -lprotobuf -lpthread
LIBCARTESI_Linux=libcartesi.so
LIBCARTESI_LDFLAGS_Linux:=$(SOLDFLAGS_Linux)
LIBCARTESI_TESTS_LDFLAGS_Linux=-Wl,-rpath,'$$ORIGIN/..'
LIBCARTESI_PROTOBUF_Linux=libcartesi_protobuf.so
LIBCARTESI_PROTOBUF_LDFLAGS_Linux:=$(SOLDFLAGS_Linux)
LIBCARTESI_GRPC_Linux=libcartesi_grpc.so
LIBCARTESI_GRPC_LDFLAGS_Linux:=$(SOLDFLAGS_Linux)
LIBCARTESI_GRPC_TESTS_LDFLAGS_Linux=-Wl,-rpath,'$$ORIGIN/..'
LIBCARTESI_JSONRPC_Linux=libcartesi_jsonrpc.so
LIBCARTESI_JSONRPC_LDFLAGS_Linux:=$(SOLDFLAGS_Linux)
LIBCARTESI_JSONRPC_TESTS_LDFLAGS_Linux=-Wl,-rpath,'$$ORIGIN/..'
CARTESI_EXECUTABLE_LDFLAGS_Linux=-Wl,-rpath,'$$ORIGIN/'
PROFILE_DATA_Linux=

CC=$(CC_$(UNAME))
CXX=$(CXX_$(UNAME))
SOLDFLAGS:=$(SOLDFLAGS_$(UNAME)) $(GCLDFLAGS)
BOOST_CORO_LIB=$(BOOST_CORO_LIB_$(UNAME))
BOOST_PROCESS_LIB=$(BOOST_PROCESS_LIB_$(UNAME))
BOOST_FILESYSTEM_LIB=$(BOOST_FILESYSTEM_LIB_$(UNAME))
BOOST_LOG_LIB=$(BOOST_LOG_LIB_$(UNAME))
BOOST_INC=$(BOOST_INC_$(UNAME))
B64_LIB=$(B64_LIB_$(UNAME))
B64_INC=$(B64_INC_$(UNAME))
CRYPTOPP_LIB=$(CRYPTOPP_LIB_$(UNAME))
CRYPTOPP_INC=$(CRYPTOPP_INC_$(UNAME))
NLOHMANN_JSON_INC=$(NLOHMANN_JSON_INC_$(UNAME))
GRPC_LIB=$(GRPC_LIB_$(UNAME))
GRPC_INC=$(GRPC_INC_$(UNAME))
PROTOBUF_LIB=$(PROTOBUF_LIB_$(UNAME))
LIBCARTESI=$(LIBCARTESI_$(UNAME))
LIBCARTESI_LDFLAGS=$(LIBCARTESI_LDFLAGS_$(UNAME))
LIBCARTESI_TESTS_LDFLAGS=$(LIBCARTESI_TESTS_LDFLAGS_$(UNAME))
LIBCARTESI_LIB=-L. -lcartesi
LIBCARTESI_PROTOBUF=$(LIBCARTESI_PROTOBUF_$(UNAME))
LIBCARTESI_PROTOBUF_LDFLAGS=$(LIBCARTESI_PROTOBUF_LDFLAGS_$(UNAME))
LIBCARTESI_PROTOBUF_LIB=-L. -lcartesi_protobuf
CARTESI_EXECUTABLE_LDFLAGS=$(CARTESI_EXECUTABLE_LDFLAGS_$(UNAME))

LIBCARTESI_GRPC=$(LIBCARTESI_GRPC_$(UNAME))
LIBCARTESI_GRPC_LDFLAGS=$(LIBCARTESI_GRPC_LDFLAGS_$(UNAME))
LIBCARTESI_GRPC_TESTS_LDFLAGS=$(LIBCARTESI_GRPC_TESTS_LDFLAGS_$(UNAME))
LIBCARTESI_GRPC_LIB=-L. -lcartesi_grpc

CARTESI_LIBS:=$(CRYPTOPP_LIB) $(LIBCARTESI_PROTOBUF_LIB)
CARTESI_PROTOBUF_LIBS:=$(PROTOBUF_LIB)
CARTESI_GRPC_LIBS:=$(CRYPTOPP_LIB) $(GRPC_LIB)
LUACARTESI_LIBS:=$(CRYPTOPP_LIB) $(LIBCARTESI_LIB)
LUACARTESI_GRPC_LIBS:=$(CRYPTOPP_LIB) $(LIBCARTESI_GRPC_LIB) $(GRPC_LIB)  $(LIBCARTESI_LIB)
LUACARTESI_JSONRPC_LIBS:=$(CRYPTOPP_LIB) $(LIBCARTESI_JSONRPC_LIB) $(LIBCARTESI_LIB) $(B64_LIB)
REMOTE_CARTESI_MACHINE_LIBS:=$(CRYPTOPP_LIB) $(GRPC_LIB) $(BOOST_LOG_LIB) $(LIBCARTESI_PROTOBUF_LIB)
JSONRPC_REMOTE_CARTESI_MACHINE_LIBS:=$(CRYPTOPP_LIB) $(BOOST_LOG_LIB) $(LIBCARTESI_PROTOBUF_LIB) $(PROTOBUF_LIB) $(B64_LIB)
REMOTE_CARTESI_MACHINE_PROXY_LIBS:=$(CRYPTOPP_LIB) $(GRPC_LIB) $(BOOST_CORO_LIB) $(LIBCARTESI_PROTOBUF_LIB) -ldl
TEST_MACHINE_C_API_LIBS:=$(LIBCARTESI_LIB) $(CRYPTOPP_LIB) $(LIBCARTESI_PROTOBUF_LIB) $(LIBCARTESI_GRPC_LIB) $(BOOST_PROCESS_LIB) $(BOOST_FILESYSTEM_LIB) $(PROTOBUF_LIB)
HASH_LIBS:=$(CRYPTOPP_LIB)

#DEFS+= -DMT_ALL_DIRTY

WARNS=-W -Wall -pedantic

# Place our include directories before the system's
INCS=-I../lib/machine-emulator-defines -I../third-party/llvm-flang-uint128 \
	$(LUA_INC) $(CRYPTOPP_INC) $(NLOHMANN_JSON_INC) $(B64_INC) $(BOOST_INC) $(GRPC_INC) $(INCS_$(UNAME))

ifeq ($(dump),yes)
#DEFS+=-DDUMP_ILLEGAL_INSN_EXCEPTIONS
#DEFS+=-DDUMP_EXCEPTIONS
#DEFS+=-DDUMP_INTERRUPTS
DEFS+=-DDUMP_HIST
#DEFS+=-DDUMP_MMU_EXCEPTIONS
#DEFS+=-DDUMP_INVALID_MEM_ACCESS
#DEFS+=-DDUMP_INVALID_CSR
#DEFS+=-DDUMP_INSN
#DEFS+=-DDUMP_REGS
#DEFS+=-DDUMP_COUNTERS
endif

ifeq ($(relwithdebinfo),yes)
OPTFLAGS+=-O2 -g
else ifeq ($(release),yes)
OPTFLAGS+=-O2
else ifeq ($(debug),yes)
OPTFLAGS+=-Og -g
else ifeq ($(sanitize),yes)
OPTFLAGS+=-Og -g
else
OPTFLAGS+=-O2 -g
endif

# Git commit hash (for releases)
ifneq ($(git_commit),)
DEFS+=-DGIT_COMMIT='"$(git_commit)"'
endif

# Optimization flags
ifneq (,$(filter yes,$(relwithdebinfo) $(release)))
DEFS+=-DNDEBUG
# Disable jump tables, because it degrades the instruction decoding performance in the interpret loop,
# since it generates a memory indirection that has a high cost in opcode switches.
OPTFLAGS+=-fno-jump-tables
ifneq (,$(filter gcc,$(CC)))
# The following flag helps GCC to eliminate more redundant computations in the interpret loop,
# saving some host instructions and improving performance.
# This flag is usually enabled by default at -O3,
# but we don't use -O3 because it enables some other flags that are not worth for the interpreter.
OPTFLAGS+=-fgcse-after-reload -fpredictive-commoning -fsplit-paths -ftree-partial-pre
endif
endif

# Link time optimizations
ifeq ($(lto),yes)
OPTFLAGS+=-flto=auto
endif

ifeq ($(sanitize),yes)
# Enable address and undefined sanitizers
UBFLAGS+=-fsanitize=address,undefined -fno-sanitize-recover=all
LIBASAN_SO=$(shell realpath `$(CC) -print-file-name=libasan.so`)
LUA=LD_PRELOAD=$(LIBASAN_SO) $(LUA_BIN)
else
# Flags to minimize undefined behavior
UBFLAGS+=-fno-strict-aliasing
UBFLAGS+=-fno-strict-overflow
UBFLAGS+=-fno-delete-null-pointer-checks
LUA=$(LUA_BIN)
endif

# Compile for current system processor
ifeq ($(native),yes)
CC_MARCH=-march=native
else
CC_MARCH=
endif

# Workload to use in PGO
PGO_WORKLOAD=\
	tar c -C / bin | gzip > a.tar.gz && gzip -c a.tar.gz | sha256sum; \
	dhrystone 500000; \
	whetstone 2500

# We ignore xkcp-keccak-256-hasher.cpp because it is missing a header file.
# The file is not being compiled but we want to keep it for reference.
LINTER_IGNORE_SOURCES=xkcp-keccak-256-hasher.cpp
LINTER_IGNORE_HEADERS=%.pb.h xkcp-keccak-256-hasher.h
LINTER_SOURCES=$(filter-out $(LINTER_IGNORE_SOURCES),$(strip $(wildcard *.cpp) $(wildcard *.c)))
LINTER_HEADERS=$(filter-out $(LINTER_IGNORE_HEADERS),$(strip $(wildcard *.hpp) $(wildcard *.h)))

CLANG_TIDY=clang-tidy
CLANG_TIDY_TARGETS=$(patsubst %.cpp,%.clang-tidy,$(patsubst %.c,%.clang-tidy,$(LINTER_SOURCES)))

CLANG_FORMAT=clang-format
CLANG_FORMAT_UARCH_FILES:=$(wildcard ../uarch/*.cpp)
CLANG_FORMAT_UARCH_FILES:=$(filter-out %uarch-printf%,$(strip $(CLANG_FORMAT_UARCH_FILES)))
CLANG_FORMAT_FILES:=$(wildcard *.cpp) $(wildcard *.c) $(wildcard *.h) $(wildcard *.hpp) $(CLANG_FORMAT_UARCH_FILES)
CLANG_FORMAT_FILES:=$(filter-out %.pb.h,$(strip $(CLANG_FORMAT_FILES)))

EMPTY:=
SPACE:=$(EMPTY) $(EMPTY)
CLANG_TIDY_HEADER_FILTER=$(PWD)/($(subst $(SPACE),|,$(LINTER_HEADERS)))

CXXFLAGS+=$(OPTFLAGS) -std=c++17 -fvisibility=hidden -fPIC -MMD $(CC_MARCH) $(INCS) $(GCFLAGS) $(UBFLAGS) $(DEFS) $(WARNS) $(MYCFLAGS)
CFLAGS+=$(OPTFLAGS) -std=c99 -fvisibility=hidden -fPIC -MMD $(CC_MARCH) $(INCS) $(GCFLAGS) $(UBFLAGS) $(DEFS) $(WARNS) $(MYCFLAGS)
LDFLAGS+=$(UBFLAGS) $(MYLDFLAGS)

COVERAGE_WORKLOAD=\
	dhrystone 100; \
	whetstone 100
COVERAGE_CPP_SOURCES=$(wildcard *.cpp)
COVERAGE_SOURCES=$(filter-out %.pb.h, $(wildcard *.h) $(wildcard *.cpp))
ifeq ($(coverage-toolchain),gcc)
CC=gcc
CXX=g++
CXXFLAGS+=-g -Og -fno-dce -fno-inline -DCODE_COVERAGE --coverage
LDFLAGS+=--coverage
LCOV_CONFIG=--rc lcov_branch_coverage=1
else ifeq ($(coverage-toolchain),clang)
CC=clang
CXX=clang++
CXXFLAGS+=-g -O0 -DCODE_COVERAGE -fprofile-instr-generate -fcoverage-mapping
LDFLAGS+=-fprofile-instr-generate -fcoverage-mapping
export LLVM_PROFILE_FILE=coverage-%p.profraw
else ifneq ($(coverage-toolchain),)
$(error invalid value for coverage-toolchain: $(coverage-toolchain))
endif

all: luacartesi grpc hash c-api jsonrpc-remote-cartesi-machine

.PHONY: all generate use clean test lint format check-format luacartesi grpc hash docker c-api compile_flags.txt

CARTESI_OBJS:= \
	pma-driver.o \
	clint.o \
	clint-factory.o \
	rom.o \
	tty.o \
	htif.o \
	htif-factory.o \
	shadow-state.o \
	shadow-state-factory.o \
	shadow-pmas.o \
	shadow-pmas-factory.o \
	shadow-tlb.o \
	shadow-tlb-factory.o \
	machine-merkle-tree.o \
	pristine-merkle-tree.o \
	pma.o \
	machine.o \
	machine-config.o \
	interpret.o \
	virtual-machine.o \
	protobuf-util.o \
	machine-c-api.o \
	uarch-machine.o \
	uarch-interpret.o

LUACARTESI_OBJS:= \
	clua-cartesi.o \
	clua-i-virtual-machine.o \
	clua-machine.o \
	clua-htif.o \
	clua-machine-util.o \
	clua.o

CARTESI_PROTOBUF_GEN_OBJS:= \
	versioning.pb.o \
	cartesi-machine.pb.o \
	cartesi-machine-checkin.pb.o

CARTESI_GRPC_GEN_OBJS:= \
	cartesi-machine.grpc.pb.o \
	cartesi-machine-checkin.grpc.pb.o

CARTESI_GRPC_OBJS:= \
	machine-merkle-tree.o \
	pristine-merkle-tree.o \
	$(CARTESI_GRPC_GEN_OBJS) \
	grpc-virtual-machine.o \
	grpc-machine-c-api.o

LUACARTESI_GRPC_OBJS:= \
	clua-i-virtual-machine.o \
	clua-htif.o \
	clua-machine-util.o \
	clua-cartesi-grpc.o \
	clua-grpc-machine.o \
	clua.o

LUACARTESI_JSONRPC_OBJS:= \
	clua-i-virtual-machine.o \
	clua-htif.o \
	clua-machine-util.o \
	clua-cartesi-jsonrpc.o \
	clua-jsonrpc-machine.o \
	json-util.o \
	base64.o \
	$(BUILDDIR)/lib/mongoose.o \
	jsonrpc-machine-c-api.o \
	jsonrpc-virtual-machine.o \
	clua.o

ifeq ($(gperf),yes)
DEFS+=-DGPERF
CARTESI_LIBS+=-lprofiler
CARTESI_PROTOBUF_LIBS+=-lprofiler
CARTESI_GRPC_LIBS+=-lprofiler
LUACARTESI_LIBS+=-lprofiler
endif

$(LIBCARTESI): $(CARTESI_OBJS) $(LIBCARTESI_PROTOBUF)
	$(CXX) $(LDFLAGS) $(LIBCARTESI_LDFLAGS) -o $@ $(CARTESI_OBJS) $(CARTESI_LIBS)

# $(LIBCARTESI_PROTOBUF) is a dynamic library that exports all symbols
$(CARTESI_PROTOBUF_GEN_OBJS): CXXFLAGS += -fvisibility=default

$(LIBCARTESI_PROTOBUF): $(CARTESI_PROTOBUF_GEN_OBJS)
	$(CXX) $(LDFLAGS) $(LIBCARTESI_PROTOBUF_LDFLAGS) -o $@ $^ $(CARTESI_PROTOBUF_LIBS)

$(LIBCARTESI_GRPC): $(CARTESI_GRPC_OBJS) $(LIBCARTESI)
	$(CXX) $(LDFLAGS) $(LIBCARTESI_GRPC_LDFLAGS) -o $@ $(CARTESI_GRPC_OBJS) $(CARTESI_GRPC_LIBS) $(LIBCARTESI_LIB)

cartesi.so: $(LUACARTESI_OBJS) $(LIBCARTESI)
	$(CXX) $(LDFLAGS) $(SOLDFLAGS) -o $@ $(LUACARTESI_OBJS) $(LUACARTESI_LIBS)

cartesi/grpc.so: $(LUACARTESI_GRPC_OBJS) $(LIBCARTESI_GRPC) $(LIBCARTESI)
	$(CXX) $(LDFLAGS) $(SOLDFLAGS) -o $@ $(LUACARTESI_GRPC_OBJS) $(LUACARTESI_GRPC_LIBS)

cartesi/jsonrpc.so: $(LUACARTESI_JSONRPC_OBJS) $(LIBCARTESI_JSONRPC) $(LIBCARTESI)
	$(CXX) $(LDFLAGS) $(SOLDFLAGS) -o $@ $(LUACARTESI_JSONRPC_OBJS) $(LUACARTESI_JSONRPC_LIBS)

test: luacartesi
	$(LUA) cartesi-machine-tests.lua --test-path="$(TEST_PATH)" --test=".*" run

test-uarch-for-coverage:
	$(MAKE) -C ../third-party/riscv-arch-tests LUA="$(LUA)"
	$(MAKE) -C ../uarch
	$(LUA) cartesi-machine-tests.lua --test-path="$(TEST_PATH)" --test=".*csr.*" --uarch-ram-length=0x20000 --uarch-ram-image=../uarch/uarch-ram.bin run_uarch

test-hash: hash
	./tests/test-merkle-tree-hash --log2-root-size=30 --log2-leaf-size=12 --input=tests/test-merkle-tree-hash

# In order to run tests environment variable TEST_PATH with directory path
# to asset files for cartesi machine tests must be set
test-grpc: luacartesi grpc
	./tests/test-grpc-server.sh ./remote-cartesi-machine "$(LUA) ./cartesi-machine.lua" "$(LUA) ./cartesi-machine-tests.lua" "$(TEST_PATH)" "$(LUA)"

# In order to run tests environment variable TEST_PATH with directory path
# to asset files for cartesi machine tests must be set
test-jsonrpc: luacartesi jsonrpc
	./tests/test-jsonrpc-server.sh ./jsonrpc-remote-cartesi-machine "$(LUA) ./cartesi-machine.lua" "$(LUA) ./cartesi-machine-tests.lua" "$(TEST_PATH)" "$(LUA)"

test-scripts: luacartesi
	for x in `cat tests/autorun` ; do \
		echo -n 'CTSICTSI' | $(LUA) tests/$$x local || exit 1; \
	done

test-c-api: c-api remote-cartesi-machine
	./tests/test-machine-c-api

test-linux-workload: luacartesi
	$(LUA) ./cartesi-machine.lua -- "$(COVERAGE_WORKLOAD)"
	# Test interactive mode (to cover mcycle overwriting)
	echo uname | $(LUA) ./cartesi-machine.lua -i sh
	# Test max mcycle (to cover max mcycle branch)
	$(LUA) ./cartesi-machine.lua --max-mcycle=1

test-all: test test-hash test-scripts test-grpc test-jsonrpc test-c-api test-uarch-for-coverage test-linux-workload

lint: $(CLANG_TIDY_TARGETS)

format:
	@$(CLANG_FORMAT) -i $(CLANG_FORMAT_FILES)

check-format:
	@$(CLANG_FORMAT) -Werror --dry-run $(CLANG_FORMAT_FILES)

fs.ext2: fs/*
	genext2fs -f -i 512 -b 8192 -d fs fs.ext2
	truncate -s %4096 fs.ext2

generate: CXXFLAGS += -fprofile-generate
generate: LDFLAGS += -fprofile-generate
generate: luacartesi

ifeq ($(coverage-toolchain),gcc)
coverage-report:
	rm -f *.pb.cc.gcda *.pb.h.gcda
	gcov --demangled-names --relative-only --branch-probabilities *.gcda
	lcov --capture --directory . --no-external --exclude '*.pb.*' -o coverage.info $(LCOV_CONFIG) --quiet
	genhtml --quiet coverage.info --output-directory coverage-gcc $(LCOV_CONFIG)
	lcov --list coverage.info $(LCOV_CONFIG) | tee coverage.txt
	lcov --summary coverage.info $(LCOV_CONFIG) | tee -a coverage.txt
else ifeq ($(coverage-toolchain),clang)
coverage-report:
	llvm-profdata merge -sparse $(shell find . -name 'coverage*.profraw') -o coverage.profdata
	llvm-cov show -format=html -output-dir=coverage-clang -instr-profile=coverage.profdata \
		./cartesi.so \
		-object ./cartesi/grpc.so \
		-object ./cartesi/jsonrpc.so \
		-object ./libcartesi.so \
		-object ./libcartesi_grpc.so \
		-object ./tests/test-merkle-tree-hash \
		-object ./tests/test-machine-c-api \
		-object ./remote-cartesi-machine \
		-object ./jsonrpc-remote-cartesi-machine \
		$(COVERAGE_SOURCES)
endif

ifneq ($(coverage-toolchain),)
coverage: test-all coverage-report
endif

valgrind: luacartesi
	valgrind --leak-check=full --tool=memcheck --track-origins=yes $(LUA_BIN) cartesi-machine-tests.lua --test-path="$(TEST_PATH)" --test=".*" run
	valgrind --leak-check=full --tool=memcheck --track-origins=yes $(LUA_BIN) cartesi-machine.lua --initial-hash --final-hash -- /bin/true

$(PROFILE_DATA_Darwin):
	llvm-profdata merge -output=default.profdata default*.profraw

use: CXXFLAGS += -fprofile-use -Wno-missing-profile
use: LDFLAGS += -fprofile-use
use: $(PROFILE_DATA_$(UNAME)) luacartesi

compile_flags.txt:
	@echo "$(CXXFLAGS)" "-xc++" | sed -e $$'s/ \{1,\}/\\\n/g' | grep -v "MMD" > $@

luacartesi: cartesi.so cartesi/grpc.so cartesi/jsonrpc.so

luacartesi-pgo:
	$(MAKE) --no-print-directory generate
	./cartesi-machine.lua -- "$(PGO_WORKLOAD)"
	$(MAKE) clean-libcartesi
	$(MAKE) --no-print-directory use
	$(MAKE) clean-profile

grpc: cartesi/jsonrpc.so cartesi/grpc.so remote-cartesi-machine remote-cartesi-machine-proxy

jsonrpc: cartesi/jsonrpc.so jsonrpc-remote-cartesi-machine

hash: merkle-tree-hash tests/test-merkle-tree-hash

c-api: $(LIBCARTESI) $(LIBCARTESI_GRPC) tests/test-machine-c-api

MERKLE_TREE_HASH_OBJS:= \
	back-merkle-tree.o \
	pristine-merkle-tree.o \
	merkle-tree-hash.o

TEST_MERKLE_TREE_HASH_OBJS:= \
	back-merkle-tree.o \
	pristine-merkle-tree.o \
	complete-merkle-tree.o \
	full-merkle-tree.o \
	test-merkle-tree-hash.o

TEST_MACHINE_C_API_OBJS:= \
    test-machine-c-api.o

PROTO_OBJS:= \
	$(CARTESI_PROTOBUF_GEN_OBJS) \
	$(CARTESI_GRPC_GEN_OBJS)

$(PROTO_OBJS): CXXFLAGS +=  -Wno-zero-length-array -Wno-unused-parameter -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-type-limits

PROTO_SOURCES:=$(PROTO_OBJS:.o=.cc)

REMOTE_CARTESI_MACHINE_PROXY_OBJS:= \
	$(CARTESI_GRPC_GEN_OBJS) \
	remote-machine-proxy.o

REMOTE_CARTESI_MACHINE_OBJS:= \
	$(CARTESI_GRPC_GEN_OBJS) \
	remote-machine.o \
	protobuf-util.o \
	pma-driver.o \
	clint.o \
	clint-factory.o \
	rom.o \
	tty.o \
	htif.o \
	htif-factory.o \
	shadow-state.o \
	shadow-state-factory.o \
	shadow-pmas.o \
	shadow-pmas-factory.o \
	shadow-tlb.o \
	shadow-tlb-factory.o \
	machine-merkle-tree.o \
	pristine-merkle-tree.o \
	pma.o \
	machine.o \
	machine-config.o \
	interpret.o \
	uarch-machine.o \
	uarch-interpret.o

JSONRPC_REMOTE_CARTESI_MACHINE_OBJS:= \
	jsonrpc-remote-machine.o \
	jsonrpc-discover.o \
	$(BUILDDIR)/lib/mongoose.o \
	protobuf-util.o \
	pma-driver.o \
	clint.o \
	clint-factory.o \
	rom.o \
	tty.o \
	htif.o \
	htif-factory.o \
	json-util.o \
	base64.o \
	shadow-state.o \
	shadow-state-factory.o \
	shadow-pmas.o \
	shadow-pmas-factory.o \
	shadow-tlb.o \
	shadow-tlb-factory.o \
	machine-merkle-tree.o \
	pristine-merkle-tree.o \
	pma.o \
	machine.o \
	machine-config.o \
	interpret.o \
	uarch-machine.o \
	uarch-interpret.o

CYCLE_PERIOD ?= 13
HASH_DIR = hashes/$(CYCLE_PERIOD)
LOG_DIR = logs/$(CYCLE_PERIOD)
GROUND_TRUTH_DIR ?= ground-truth
CONCURRENCY_MERKLE_TREE ?= 0

ifneq ($(wildcard $(TEST_PATH)),)
TESTS=$(shell find $(TEST_PATH) -name '*.bin' -type f -exec basename {} \;)
endif
TEST_HASHES=$(addprefix $(HASH_DIR)/,$(TESTS:.bin=.hashes))
TEST_LOGS=$(addprefix $(LOG_DIR)/,$(TESTS:.bin=.json.br))
TRUTH_HASHES=$(addprefix $(GROUND_TRUTH_DIR)/,$(TEST_HASHES))
TRUTH_LOGS=$(addprefix $(GROUND_TRUTH_DIR)/,$(TEST_LOGS))

HASH_EACH_CYCLE_DIR=
TEST_EACH_CYCLE_HASHES=
ifneq ($(strip $(CYCLE_PERIOD)),1)
HASH_EACH_CYCLE_DIR = hashes/1
TEST_EACH_CYCLE_HASHES=$(addprefix $(HASH_EACH_CYCLE_DIR)/,$(TESTS:.bin=.hashes))
endif

$(HASH_DIR) $(LOG_DIR) $(GROUND_TRUTH_DIR)/$(HASH_DIR) $(GROUND_TRUTH_DIR)/$(LOG_DIR) $(HASH_EACH_CYCLE_DIR):
	mkdir -p $@

test-hashes: $(TEST_HASHES)

test-logs: $(TEST_LOGS)

truth-hashes: $(TRUTH_HASHES)

truth-logs: $(TRUTH_LOGS)

compare-hashes: | $(TEST_EACH_CYCLE_HASHES) $(TEST_HASHES)
	@for t in $(TESTS) ; do \
		result=$$(tail -n +2 -q $(HASH_DIR)/$${t%.bin}.hashes $(HASH_EACH_CYCLE_DIR)/$${t%.bin}.hashes | sort -s -n | uniq | awk -v step=-1 '{pline=line; line=$$0; pstep=step; step=$$1; if (pstep >= 0 && pstep == step) {print(pline); print(line); exit 1;}}'); \
		if [ "$$result" != "" ]; then \
			echo $$t ;\
			echo "$$result"; \
			exit 1; \
		fi \
	done

$(HASH_EACH_CYCLE_DIR)/%.hashes: $(TEST_PATH)/%.bin | $(HASH_EACH_CYCLE_DIR)
	$(LUA) cartesi-machine-tests.lua --test-path=$(TEST_PATH) --test="$(notdir $<)" --periodic-action=1 --output=$@ --concurrency=update_merkle_tree:$(CONCURRENCY_MERKLE_TREE) hash

$(HASH_DIR)/%.hashes: $(TEST_PATH)/%.bin | $(HASH_DIR)
	$(LUA) cartesi-machine-tests.lua --test-path=$(TEST_PATH) --test="$(notdir $<)" --periodic-action=$(CYCLE_PERIOD) --output=$@ --concurrency=update_merkle_tree:$(CONCURRENCY_MERKLE_TREE) hash
	diff $@ $(GROUND_TRUTH_DIR)/$@

$(LOG_DIR)/%.json.br: $(TEST_PATH)/%.bin $(LOG_DIR)
	$(LUA) cartesi-machine-tests.lua --test-path=$(TEST_PATH) --test="$(notdir $<)" --periodic-action=$(CYCLE_PERIOD) --output=$(basename $@) step
	brotli -j -f $(basename $@)
	diff $@ $(GROUND_TRUTH_DIR)/$@

$(GROUND_TRUTH_DIR)/$(HASH_DIR)/%.hashes: $(TEST_PATH)/%.bin | $(GROUND_TRUTH_DIR)/$(HASH_DIR)
	$(LUA) cartesi-machine-tests.lua --test-path=$(TEST_PATH) --test="$(notdir $<)" --periodic-action=$(CYCLE_PERIOD) --output=$@ --concurrency=update_merkle_tree:$(CONCURRENCY_MERKLE_TREE) hash

$(GROUND_TRUTH_DIR)/$(LOG_DIR)/%.json.br: $(TEST_PATH)/%.bin $(GROUND_TRUTH_DIR)/$(LOG_DIR)
	$(LUA) cartesi-machine-tests.lua --test-path=$(TEST_PATH) --test="$(notdir $<)" --periodic-action=$(CYCLE_PERIOD) --output=$(basename $@) step
	brotli -j -f $(basename $@)

merkle-tree-hash: $(MERKLE_TREE_HASH_OBJS)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $^ $(HASH_LIBS)

tests/test-merkle-tree-hash: $(TEST_MERKLE_TREE_HASH_OBJS)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $^ $(HASH_LIBS)

grpc-interfaces: $(PROTO_SOURCES)

remote-cartesi-machine: $(REMOTE_CARTESI_MACHINE_OBJS) $(LIBCARTESI_PROTOBUF)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $(REMOTE_CARTESI_MACHINE_OBJS) $(REMOTE_CARTESI_MACHINE_LIBS)

$(BUILDDIR)/lib/mongoose.o: CFLAGS := $(patsubst %-std=c99,%,$(CFLAGS))

jsonrpc-remote-cartesi-machine: $(JSONRPC_REMOTE_CARTESI_MACHINE_OBJS) $(LIBCARTESI_PROTOBUF)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $(JSONRPC_REMOTE_CARTESI_MACHINE_OBJS) $(JSONRPC_REMOTE_CARTESI_MACHINE_LIBS)

remote-cartesi-machine-proxy: $(REMOTE_CARTESI_MACHINE_PROXY_OBJS) $(LIBCARTESI_PROTOBUF)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $(REMOTE_CARTESI_MACHINE_PROXY_OBJS) $(REMOTE_CARTESI_MACHINE_PROXY_LIBS)

remote-cartesi-machine remote-cartesi-machine-proxy: CXXFLAGS := $(GRPC_INC) $(CXXFLAGS)

tests/test-machine-c-api: $(TEST_MACHINE_C_API_OBJS) $(LIBCARTESI) $(LIBCARTESI_GRPC)
	$(CXX) $(LDFLAGS) $(CARTESI_EXECUTABLE_LDFLAGS) -o $@ $(TEST_MACHINE_C_API_OBJS) $(TEST_MACHINE_C_API_LIBS)

.PRECIOUS: %.grpc.pb.cc %.grpc.pb.h %.pb.cc %.pb.h

%.grpc.pb.cc: $(GRPC_DIR)/%.proto
	$(PROTOC) -I$(<D) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN) $<

%.pb.cc: $(GRPC_DIR)/%.proto
	$(PROTOC) -I$(GRPC_DIR) --cpp_out=. $<

machine-config.o protobuf-util.o: $(CARTESI_PROTOBUF_GEN_OBJS)

grpc-virtual-machine.o grpc-machine-c-api.o remote-machine.o proxy.o: $(CARTESI_GRPC_GEN_OBJS) $(CARTESI_PROTOBUF_GEN_OBJS)

machine-c-version.h: ../tools/template/machine-c-version.h.template
	sed "s|EMULATOR_MARCHID|$(EMULATOR_MARCHID)|g;s|EMULATOR_VERSION_MAJOR|$(EMULATOR_VERSION_MAJOR)|g;s|EMULATOR_VERSION_MINOR|$(EMULATOR_VERSION_MINOR)|g;s|EMULATOR_VERSION_PATCH|$(EMULATOR_VERSION_PATCH)|g;s|EMULATOR_VERSION_LABEL|$(EMULATOR_VERSION_LABEL)|g" $< > $@

jsonrpc-discover.cpp: jsonrpc-discover.json
	echo '// This file is auto-generated and should not be modified' > jsonrpc-discover.cpp
	echo 'namespace cartesi {' >> jsonrpc-discover.cpp
	echo 'extern char const * const jsonrpc_discover_json = R"json(' >> jsonrpc-discover.cpp
	cat jsonrpc-discover.json >> jsonrpc-discover.cpp
	echo ')json";' >> jsonrpc-discover.cpp
	echo '} // namespace cartesi' >> jsonrpc-discover.cpp

%.clang-tidy: %.cpp $(PROTO_SOURCES) machine-c-version.h
	@$(CLANG_TIDY) --header-filter='$(CLANG_TIDY_HEADER_FILTER)' $< -- $(CXXFLAGS) 2>/dev/null
	@$(CXX) $(CXXFLAGS) $< -MM -MT $@ -MF $@.d > /dev/null 2>&1
	@touch $@

%.clang-tidy: %.c $(PROTO_SOURCES)
	@$(CLANG_TIDY) --header-filter='$(CLANG_TIDY_HEADER_FILTER)' $< -- $(CFLAGS) 2>/dev/null
	@$(CC) $(CFLAGS) $< -MM -MT $@ -MF $@.d > /dev/null 2>&1
	@touch $@

%.o: %.cpp machine-c-version.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cc $(PROTO_SOURCES)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: clean-auto-generated clean-coverage clean-profile clean-proto-sources clean-tidy clean-libcartesi clean-executables clean-tests

clean-auto-generated:
	@rm -f jsonrpc-discover.cpp machine-c-version.h

clean-proto-sources:
	@rm -f *.pb.cc *.pb.h

clean-tidy:
	@rm -f *.clang-tidy

clean-objs:
	@rm -f *.o *.d

clean-libcartesi: clean-objs
	@rm -f $(LIBCARTESI) $(LIBCARTESI_PROTOBUF) $(LIBCARTESI_GRPC) cartesi.so cartesi/grpc.so cartesi/jsonrpc.so

clean-executables:
	@rm -f jsonrpc-remote-cartesi-machine remote-cartesi-machine remote-cartesi-machine-proxy merkle-tree-hash

clean-tests:
	@rm -f tests/test-merkle-tree-hash tests/test-machine-c-api

clean-coverage:
	@rm -f *.profdata *.profraw tests/*.profraw *.gcda *.gcov coverage.info coverage.txt
	@rm -rf coverage-gcc coverage-clang

clean-profile:
	@rm -f *.profdata *.profraw tests/*.profraw *.gcda *.gcno

-include $(wildcard *.d)
