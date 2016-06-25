package;

import flash.display.StageAlign;
import flash.display.StageScaleMode;
import flash.Lib;

import flash.events.SampleDataEvent;
import flash.utils.ByteArray;
import flash.media.SoundMixer;


class Main {

	static var snd:flash.media.Sound;
	static var cc:flash.media.SoundChannel;

	static var gc:flash.display.Graphics;
	static var stageWidth = Lib.current.stage.stageWidth;
	static var halfHeight = Lib.current.stage.stageHeight >> 1;

	static function main() {
		var stage = Lib.current.stage;
		stage.scaleMode = StageScaleMode.NO_SCALE;
		stage.align = StageAlign.TOP_LEFT;
		// entry point

		var shape = new flash.display.Shape();
		Lib.current.addChild(shape);
		gc = shape.graphics;

		var snd = new flash.media.Sound();
		snd.addEventListener(SampleDataEvent.SAMPLE_DATA, onSample);
		cc = snd.play();

		stage.addEventListener(flash.events.Event.ENTER_FRAME, onEnterFrame);
		stage.addEventListener(flash.events.MouseEvent.CLICK, function(e){
			cc.stop();
		});
	}

	static function onSample(event:SampleDataEvent){
		for (c in 0...8192) {
			var n = Math.sin(((c + event.position) / Math.PI / 4));
			event.data.writeFloat(n * 0.25);
			event.data.writeFloat(n * 0.25);
		}
	}

	static function onEnterFrame(event: flash.events.Event){
		var bytes = new ByteArray();
		bytes.length = 512;

		SoundMixer.computeSpectrum(bytes, false, 0);

		gc.clear();
		gc.lineStyle(0, 0xff4800);
		gc.moveTo(0, halfHeight);

		var n:Float;

		for (i in 0...256) {
			n = bytes.readFloat() * halfHeight;
			gc.lineTo( i * 2, halfHeight - n);
		}
	}
}
