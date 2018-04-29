######################################################################
# Neko Ref: http://nekovm.org/specs/values
# Neko Std: http://nekovm.org/doc/libs

# IDE: TODO
################################################
OUT  := hi
MAIN := Main
SRC := src
TMP := obj
FILES := $(foreach dir,$(SRC), $(wildcard $(dir)/*.neko))


run: $(TMP)/$(OUT).n
	@echo
	neko $<

test: $(TMP)/$(OUT).n
	@echo
	neko -interp -stats $<

release: $(OUT).n
	@echo $@...
	nekotools boot $<

clean:
	@echo $@...
	@rm -rf *.n $(OUT).exe $(OUT).n $(TMP)/*.n

##########################
$(TMP)/$(OUT).n: $(FILES)
	@mkdir -p $(TMP)
	nekoc -o $(TMP) $^
	cd $(TMP) && nekoc -link $(OUT).n $(MAIN)

$(OUT).n: $(TMP)/$(OUT).n
	nekoc -o ./ -z $<

.PHONY: clean