BYTECODE  ?= 1
ifeq ($(BYTECODE),1)
	COMPILER = ocamlfind ocamlc
	MODULE_EXT = cmo
	TARGET_EXT = byte
else
	COMPILER = ocamlfind ocamlopt
	MODULE_EXT = cmx
	TARGET_EXT = exe
endif

## base
TARGET    := hi.$(TARGET_EXT)
OBJ_DIR   := _build
SRC_DIR   := src
## libs
CAML_LIBS :=
OPAM_LIBS :=
FIND_PACK := $(CAML_LIBS:%=-package %) $(OPAM_LIBS:%=-package %)
##
ALL_FLAGS := -bin-annot $(FIND_PACK)

MODULES   := hello

run: $(OBJ_DIR) $(TARGET)
	@./$(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ_DIR)/**

.PHONY: clean

$(OBJ_DIR):
	@mkdir -p $@

$(TARGET): $(MODULES:%=$(OBJ_DIR)/%.$(MODULE_EXT))
	$(COMPILER) -linkpkg $(FIND_PACK) -o $@ $^

$(OBJ_DIR)/%.$(MODULE_EXT): $(SRC_DIR)/%.ml
	$(COMPILER) $(ALL_FLAGS) -I $(OBJ_DIR) -c -o $@ $<

:FORCE;