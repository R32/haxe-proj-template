package;

class Req{
	
	static function main() {
		trace("-----------------" + platform() +"------------------");
		
		for(i in 0...3){
			haxe.Timer.measure(GetInner.runA);
			haxe.Timer.measure(GetInner.runB);
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
	#else
		"PYTHON";
	#end
	}
}