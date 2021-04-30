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
		var lib = new Instance(mod, {});
		js.Lib.global.lib = lib.exports;
		trace(lib.exports.square(101));
	}
}
