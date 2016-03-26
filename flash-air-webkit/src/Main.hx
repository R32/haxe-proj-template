package;

import flash.events.KeyboardEvent;
import flash.display.Stage;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.html.HTMLLoader;
import flash.net.URLRequest;
import flash.filesystem.File;
import flash.display.NativeWindow;
import haxe.Log;
import flash.Lib;

/**
set CC=E:\SDKS\AIR_210\bin\adl.exe
%CC% app.xml .
*/
class Main {

	static var stage:Stage;
	static var html:HTMLLoader;

	static function main() {
		stage = Lib.current.stage;
		stage.scaleMode = StageScaleMode.NO_SCALE;
		stage.align = StageAlign.TOP_LEFT;
		html = new HTMLLoader();
		html.width = stage.stageWidth;
		html.height = stage.stageHeight;
		html.addEventListener(Event.COMPLETE, _onComplete);
		html.load(new URLRequest("index.html"));
		Lib.current.addChild(html);
	}

	static function _resize(evt:Event):Void{
		html.width = stage.stageWidth;
		html.height = stage.stageHeight;
	}

	static function _onKeyUp(e:KeyboardEvent):Void{
		if (e.keyCode == 116){
			html.reload();
			Log.clear();
		}
	}

	static function _onComplete(evt:Event):Void {
		stage.addEventListener(Event.RESIZE, _resize);
		stage.addEventListener(KeyboardEvent.KEY_UP,_onKeyUp);
		Log.setColor(0xffffff);
		html.window.console = { log: haxe.Log.trace };
		stage.nativeWindow.visible = true;
	}
}


