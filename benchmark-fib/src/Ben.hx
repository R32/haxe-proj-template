package;

class Ben{
	static function fib(x:Int):Int{
		if (x <= 1) return 1;
		return fib(x - 1) + fib(x - 2);
	}

	static function main() {
		log("------------ " + platform() + " -------------");
		var count = 0;
		var t0:Float;
		var t1:Float;
		for (i in 0...3){
			t0 = haxe.Timer.stamp();
			count += fib(36); // prevent opt
			t1 = (haxe.Timer.stamp() - t0);
			log('fib(36) x 3 - time: $t1');
		}
		return count;         // prevent opt
	}

	static inline function log(s) {
	#if sys
		Sys.println(s);
	#else
		trace(s);
	#end
	}


	static inline function platform(){
		return
	#if js
		"Nodejs";
	#elseif neko
		"NEKO";
	#elseif cpp
		"HXCPP";
	#elseif flash
		"FLASH";
	#elseif cs
		"hxCS";
	#elseif java
		"hxJAVA";
	#elseif php
		"PHP";
	#elseif lua
		"hxLUA";
	#elseif (hl && NATIVE)
		"HL/C";
	#elseif hl
		"HL";
	#elseif python
		"PYTHON";
	#else
		"Unknown";
	#end
	}
}