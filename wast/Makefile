
WAST2WASM := "$(shell cygpath 'E:/Program Files/wabt/wast2wasm.exe')"

all: bin/mem.wasm bin/app.js

bin/app.js: src/Main.hx build.hxml
	haxe build.hxml

bin/mem.wasm: src/mem.wast
	$(WAST2WASM) $< -o $@

clean:
	rm -rf ./bin/*.js ./bin/*.wasm

.PHONY: clean