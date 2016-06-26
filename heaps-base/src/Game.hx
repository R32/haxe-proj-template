package;

import Data;
import hxd.Res;

class Game extends hxd.App{

	override function init() {
		s2d.setFixedSize(720 >> 1, 568 >> 1);
	}

	override function update(dt:Float) {
	}

	static public var inst(default, null):Game;

	public static function main() {
		Res.initEmbed( { compressSounds:false } );
		if (!hxd.res.Sound.startWorker()) {
			Data.load(Res.data.entry.getText());
			Texts.load(Res.texts.entry.getText());
			inst = new Game();
		}
	}
}