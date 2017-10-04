package;

import es.WebAssembly;
import js.Lib;

class Main {
	static function main() {
		var buf = haxe.Resource.getBytes("mem").getData();
		var mem = new WasmMemory({initial: 1});
		var mod = new WasmModule(buf);
		var lib = new WasmInstance(mod, {
			js: {
				mem: mem
			}
		});
		Lib.global.mem = mem;
		Lib.global.lib = lib.exports;
	}
}
