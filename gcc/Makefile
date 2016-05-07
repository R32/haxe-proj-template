CC		:= gcc
INCLUDE	:= -Iinclude

ifeq ($(OS),Windows_NT)
# 需要注意的是没法区分是在 bash 还是 dos 下, 如果将 cygwin/bin 添加到路径的话
INCLUDE	+= -I$(shell cygpath -u ${NEKO_INSTPATH})/include
else
INCLUDE	+= -I${NEKO_INSTPATH})/include
endif

# make debug=1
ifdef DEBUG
	CFLAGS := -O0 -DDEBUG
else
	CFLAGS := -O4
endif

SOURCE_DIR	:= src
OBJ_DIR		:= obj
TARGET		:= hello.exe
#FILES		:= $(shell find $(SOURCE_DIR) -type f \( -name "*.c" -or -name "*.cpp" \))
CFILES		:= $(foreach dir,$(SOURCE_DIR), $(wildcard $(dir)/*.c))
#CPPFILES	:= $(foreach dir,$(SOURCE_DIR), $(wildcard $(dir)/*.cpp))
OBJS		:= $(addprefix $(OBJ_DIR)/, $(notdir $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)))


all: $(OBJ_DIR) $(TARGET)
	@echo run...........$(CFLAGS)
	@./$(TARGET)

clean:
	@rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean share share
#--------------------------------------------
#
#--------------------------------------------
$(OBJ_DIR):
	@mkdir -p $@

$(TARGET): $(OBJS)
	@echo link...........
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(TARGET)


$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


DEPS := .depend
$(DEPS): $(CFILES) #$(CPPFILES)
	$(CC) $(CFLAGS) $(INCLUDE) -MM $^ > $(DEPS)
include $(DEPS)