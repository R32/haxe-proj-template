OUTPUT := bin

CSS := $(OUTPUT)/style.css
JS  := $(OUTPUT)/main.js

#start#
all: $(CSS) $(JS)
	@echo done!

clean:
	@echo $@...
	@rm -f $(CSS) $(JS)

.PHONY: all clean

##
$(CSS): hss/style.hss $(wildcard hss/subs/*.hss)
	hss -output $(OUTPUT)/ -D ie8 $<

$(JS): build.hxml $(wildcard src/*.hx)
	haxe $<
