package;


#if js_bg
@:expose("bg") @:keep
#else
#if !macro @:build(Mt.build()) #end
@:native("bg") extern
#end
class Background {	
	static public function main():Void{
		trace("background");
	}
}