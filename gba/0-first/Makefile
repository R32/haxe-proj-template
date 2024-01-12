# env config
PATH       := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)


##
TARGET     := first

# Tools
PREFIX     := arm-none-eabi-
CC         := $(PREFIX)gcc
LD         := $(PREFIX)gcc
OBJCOPY    := $(PREFIX)objcopy

# Options
ARCH       := -mthumb-interwork -mthumb
CFLAGS     := $(ARCH) -O2 -Wall -fno-strict-aliasing
LDFLAGS    := $(ARCH) -specs=gba.specs
LIBS       :=
INCLUDES   :=
GCCFLAGS   :=

# files
SOURCES    := main.c

OBJDIR     := obj
OBJS       := $(SOURCES:%.c=$(OBJDIR)/%.o)

# main
build: $(OBJDIR) $(TARGET).gba

# Fix header
release: $(TARGET).gba
	gbafix $<

# Strip
$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@ 

clean:
	rm -rf $(OBJS) $(TARGET).*

.PHONY: build release clean

$(OBJDIR):
	@mkdir -p $@

# lower-case vpath
vpath %.h src
vpath %.c src

%.s: %.c
	@$(CC) $(CFLAGS) -S -o $@ -c $<
	@echo Compile $< TO $@

$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compile $< TO $@

$(OBJDIR)/main.o: main.c
