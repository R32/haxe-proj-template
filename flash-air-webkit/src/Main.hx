package;

import haxe.Log;
import flash.Lib;
import flash.display.Stage;
import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.events.Event;
import flash.html.HTMLLoader;
import flash.net.URLRequest;

/**
cd bin
set CC=E:\SDKS\AIR_180\bin\adl.exe
%CC% app.xml .
*/
class Main {

	static var stage:Stage;
	static var html:HTMLLoader;

	static function main() {
		
		stage = Lib.current.stage;
		stage.scaleMode = StageScaleMode.NO_SCALE;
		stage.align = StageAlign.TOP_LEFT;
		stage.addEventListener(Event.RESIZE, _resize);
		// entry point
		html = new HTMLLoader();
		html.placeLoadStringContentInApplicationSandbox = true;
		html.width = stage.stageWidth;
		html.height = stage.stageHeight;
		html.addEventListener(Event.COMPLETE, _onComplete);
		html.load(new URLRequest("index.html")); 
		Lib.current.addChild(html);
		
		html.window.console = { log: Log.trace };
		
		Log.trace("from flash: " + html.window.navigator.userAgent);
	}
	
	static function _resize(evt:Event):Void{
		html.width = stage.stageWidth;
		html.height = stage.stageHeight;
	}
	
	static function _onComplete(evt:Event):Void {
		Log.setColor(0xff0000);
	}
}

