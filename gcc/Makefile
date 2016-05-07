
CC		:= gcc
INCLUDE	:=

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

.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(TARGET)


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