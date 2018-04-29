######################################################################
# API Ref: https://github.com/HaxeFoundation/neko/tree/master/src/core
# DOC Ref: http://nekovm.org/doc/nekoml
# Neko Ref: http://nekovm.org/specs/values
# Neko Std: http://nekovm.org/doc/libs

# IDE: TODO
################################################
MAIN	:= Main
TARGET	:= non
FILES	:= $(foreach dir,$(TARGET), $(wildcard $(dir)/*.nml))

all: bin/$(TARGET).n
	@echo $@...

run: bin/$(TARGET).n
	@echo
	neko $<

test: bin/$(TARGET).n
	@echo
	neko -interp -stats $<

release: $(TARGET).n
	@echo $@...
	nekotools boot $<

clean:
	@echo $@...
	@rm -rf *.n $(TARGET).exe $(TARGET)/*.n bin release


##########################
bin/$(TARGET).n: $(FILES)
	nekoml $^
	@mkdir -p bin
	nekoc -link bin/$(TARGET).n $(TARGET)/$(MAIN)
##  nekoc -link bin/hss.n hss/Main

$(TARGET).n: bin/$(TARGET).n
	nekoc -o ./ -z $<

.PHONY: clean