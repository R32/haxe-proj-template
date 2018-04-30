#
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

TARGET    := hi.$(TARGET_EXT)
OBJ_DIR   := _build
SRC_DIR   := src
LEX_YACC  := $(SRC_DIR)/lexer.ml $(SRC_DIR)/parser.ml
MLFILES   := $(wildcard $(SRC_DIR)/*.ml) $(LEX_YACC)
CAML_LIBS :=
OPAM_LIBS :=
FIND_LIBS := $(CAML_LIBS) $(OPAM_LIBS)
FIND_PACK := $(FIND_LIBS:%=-package %)
CFLAGS    :=
ALL_FLAGS := -bin-annot -safe-string $(FIND_PACK)

MK_MODULES:= Makefile.modules
MK_DEPENDS:= Makefile.dependencies

#
all: $(OBJ_DIR) $(TARGET)
	./$(TARGET) "2*(3+4)+5*1" "1+2*3"

depends: $(MK_MODULES) $(MK_DEPENDS)

clean:
	rm -rf $(TARGET) $(LEX_YACC) $(MK_MODULES) $(MK_DEPENDS) $(OBJ_DIR)/**

.PHONY: clean

$(MK_MODULES): $(MLFILES)
	@printf "MODULES = " > $@
	ocamlfind ocamldep -slash -sort -I $(SRC_DIR) $(MLFILES) | sed -e "s/$(SRC_DIR)\///g" | sed -e "s/\.ml//g" >> $@

$(MK_DEPENDS): $(MLFILES)
	ocamlfind ocamldep -slash -I $(SRC_DIR) $(MLFILES) | sed -e "s/$(SRC_DIR)/$(OBJ_DIR)/g" > $@

ifeq ($(DEPS),1)
# Auto Calc Dependencies if: make DEPS=1
  -include $(MK_MODULES)
  -include $(MK_DEPENDS)
else
  MODULES := globals ast lexer parser main
endif

$(OBJ_DIR):
	@mkdir -p $@

$(TARGET): $(MODULES:%=$(OBJ_DIR)/%.$(MODULE_EXT))
	@$(COMPILER) -linkpkg $(FIND_PACK) -o $@ $^ && echo " link: $@"

$(SRC_DIR)/lexer.ml: $(SRC_DIR)/lexer.mll
	ocamllex -q $<

$(SRC_DIR)/parser.ml: $(SRC_DIR)/parser.mly
	camlp4o -impl $< -o $@

$(OBJ_DIR)/%.$(MODULE_EXT): $(SRC_DIR)/%.ml
	@$(COMPILER) -c $(ALL_FLAGS) -I $(OBJ_DIR) -o $@ $< && echo "build: $<	=> $@"

:FORCE;
