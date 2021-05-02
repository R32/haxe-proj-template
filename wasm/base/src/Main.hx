package;

import js.lib.WebAssembly;
import js.lib.webassembly.Module;
import js.lib.webassembly.Memory;
import js.lib.webassembly.Global;
import js.lib.webassembly.Instance;
import js.lib.ArrayBuffer;
import js.Browser.window;

class Main {
	static function main() {
		var buffer = haxe.Resource.getBytes("bin.wasm").getData();
		if (!WebAssembly.validate(buffer))
			return;
		var mod = new Module(buffer);
		//var mem = new Memory({initial: 10}); // 64K * 10
		var lib = new Instance(mod, {
			env : {
				log: function(a, b, c, d, e, f, g) {
					js.Browser.console.log(a,
						"data_end: " + b, "global_base: " + c , "heap_base: " + d,
						"memory_base:" + e, "table_base:" + f, "dso_handle:" + g
					);
				},
			}
		});
		var clib = lib.exports;
		js.Lib.global.lib = clib;
		trace(clib.square(101));
		trace(clib.test());
	}
}
