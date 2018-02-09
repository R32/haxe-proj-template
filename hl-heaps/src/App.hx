package;

import hxd.Res;

class App extends hxd.App {

	override function init() {
		var font = hxd.res.DefaultFont.get();
		var tf = new h2d.Text(font, s2d);
		tf.textColor = 0xff0000;
		tf.text = "hello world!";
	}

	static function main(){
		Res.initLocal();
		new App();
	}
}