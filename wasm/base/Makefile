#
WASM_ROOT ?= /cygdrive/e/SDKS/wasm
CC        := $(WASM_ROOT)/clang
CXX       := $(WASM_ROOT)/clang -x c++
LD        := $(WASM_ROOT)/wasm-ld
OPT       := $(WASM_ROOT)/wasm-opt

EXPORTS   := square
BUILD     ?= RELEASE
#########################
ifeq ($(BUILD),RELEASE)
  LDFLAGS   := --strip-all --gc-sections
  OPTFLAGS  := -O3
else
  LDFLAGS   :=
  OPTFLAGS  := -g -O0
endif
#########################
OUTDIR    := bin
OBJDIR    := obj
INCLUDES  := 
SOURCES   := main.c
OBJS      := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
WASM      := $(OUTDIR)/bin.wasm
JS        := $(OUTDIR)/app.js

# Flags for wasm-ld. 
LDFLAGS   += --no-entry --lto-O3
# LDFLAGS   += --import-memory
# LDFLAGS   += --export-all --allow-undefined
LDFLAGS   += $(addprefix --export=, $(EXPORTS))

all: $(OBJDIR) $(JS)

# wasm
$(WASM): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	$(OPT) $(OPTFLAGS) -o $@ $@

# js
$(JS): $(WASM) build.hxml $(wildcard src/*.hx src/**/*.hx)
	haxe build.hxml

# css
# $(CSS) : $(wildcard hss/*.hss hss/**/*.hss)
# 	hss -o $(OUTDIR) hss/style.hss
#

$(OBJDIR):
	@mkdir -p $@

clean:
	rm -rf $(OBJS) $(WASM) $(JS)

.PHONY: clean all

# Global compiler flags
# CXXFLAGS    := -Ofast -std=c++11 -fno-rtti -Wno-writable-strings -Wno-unknown-pragmas
CCFLAGS     := -Ofast -std=c99

# Global compiler flags for Wasm targeting
CLANGFLAGS  := -target wasm32 -nostdinc++ -flto
CLANGFLAGS  += -D__EMSCRIPTEN__ -D_LIBCPP_ABI_VERSION=2
CLANGFLAGS  += -fvisibility=hidden -fno-builtin -fno-exceptions -fno-threadsafe-statics

# Standard c libs, clang.exe doesn't knwon cygwin path. so uses WINDOWS PATH here
# Unfortunately we have to compile the c/c++ files under $(WASM_ROOT)/system/
# CSTDLIBS    := $(shell cygpath -w $(WASM_ROOT)/system/include)
# CLANGFLAGS  += -isystem $(CSTDLIBS)
# CLANGFLAGS  += -isystem $(CSTDLIBS)/compat


# lower-case vpath, NOTE: Don't uses vpath to match the generated file.
vpath %.h c
vpath %.c c

$(OBJDIR)/%.o: %.c
	$(CC) $(CCFLAGS) $(CLANGFLAGS) -o $@ -c $<
