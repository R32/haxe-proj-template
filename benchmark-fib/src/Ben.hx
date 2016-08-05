package;

class Ben{
	static function fib(x:Int):Int{
		if (x <= 1) return 1;
		return fib(x - 1) + fib(x - 2);
	}

	static function main() {
		trace("---------------- " + platform() + " -----------------");
		var count:Int = 0;
		var t0:Float;
		var t1:Float;
		for (i in 0...3){
			t0 = haxe.Timer.stamp();
			count += fib(21);
			t1 = (haxe.Timer.stamp() - t0);
			trace('count: $count, time: $t1');
		}
	}


	static inline function platform(){
		return
	#if js
		"JAVASCRIT";
	#elseif neko
		"NEKO";
	#elseif cpp
		"CPP";
	#elseif flash
		"FLASH";
	#elseif cs
		"C#";
	#elseif java
		"JAVA";
	#elseif php
		"PHP";
	#elseif lua
		"LUA";
	#elseif hl
		"HL";
	#elseif python
		"PYTHON";
	#else
		"Unknown";
	#end
	}
}