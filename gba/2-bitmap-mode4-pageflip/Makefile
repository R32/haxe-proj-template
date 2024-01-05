# env config
PATH       := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)


##
TARGET     := bitmap-mode4-pageflip

# Tools
PREFIX     := arm-none-eabi-
CC         := $(PREFIX)gcc
LD         := $(PREFIX)gcc
OBJCOPY    := $(PREFIX)objcopy

# Options
ARCH       := -mthumb-interwork -mthumb
CFLAGS     := $(ARCH) -O2 -Wall -fno-strict-aliasing
LDFLAGS    := $(ARCH) -specs=gba.specs
LIBS       := -L$(DEVKITPRO)/libtonc/lib -ltonc
INCLUDES   := -I$(DEVKITPRO)/libtonc/include
GCCFLAGS   :=

# files
SOURCES    := main.c page_pic.c

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

$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) $(GCCFLAGS) $(INCLUDES) -o $@ -c $<
	@echo Compile $< TO $@

$(OBJDIR)/main.o: main.c
$(OBJDIR)/page_pic.o: page_pic.c page_pic.h
