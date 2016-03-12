package;

import Data;
import h2d.Console;
import hxd.Res;
import hxd.res.FontBuilder;

class Game extends hxd.App{
	
	var cl:Console;
	
	override function init() {
		s2d.setFixedSize(720>>1, 568>>1);
		//cl = new Console(FontBuilder.getFont("Nokia Cellphone FC", 8, { antiAliasing: false } ), s2d);
		cl = new Console(Res.nokiafc22.build(8, { antiAliasing: false } ), s2d);
		cl.log("hello heaps!", 0xff0000);
	}
	
	override function update(dt:Float) {
	}
	
	static public var inst(default, null):Game;
	
	public static function main() {
		Res.initEmbed( { compressSounds:false } );
		
		Data.load(Res.data.entry.getText());
		Texts.load(Res.texts.entry.getText());
		
		inst = new Game();		
	}
}