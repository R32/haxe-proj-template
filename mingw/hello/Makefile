#
ifeq ($(OS),Windows_NT)
	MINGW64 := x86_64-w64-mingw32-
else
	MINGW64 :=
endif


CC      := $(MINGW64)gcc.exe

# ld.exe is hard to configure
# LD     := $(MINGW64)ld.exe

CFLAGS  := -std=c99
LDFLAGS :=

TARGET  := main.exe
OBJS    := main.o

all: $(TARGET)
	./$<

clean:
	rm -rf $(TARGET) $(OBJS)

.PHONY: all clean

# main
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# dependences
main.o: main.c
