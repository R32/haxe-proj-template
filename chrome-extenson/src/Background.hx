package;


#if js_bg
@:expose("bg") @:keep
#else
@:native("bg") @:dce extern
#end
class Background {
	static public function main():Void{
		trace("background");
	}

#if !js_bg
	static inline function init(context: js.html.Window):Void {
		untyped window.bg = context.bg;
	}
#end
}