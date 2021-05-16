package;

import flash.utils.ByteArray;
import flash.media.SoundMixer;
import flash.events.SampleDataEvent;
import flash.events.KeyboardEvent;
import flash.events.Event;
import flash.Lib;

class Wave {

	static inline var CHANNEL_LENGTH = 256; // for graphics

	var snd : flash.media.Sound;

	var sndChannel : flash.media.SoundChannel;

	var g2d : flash.display.Graphics;

	var halfHeight : Int;

	var baseIndex = 0;

	var timbreIndex : Int = Sin;

	public function new() {
		var shape = new flash.display.Shape();
		g2d = shape.graphics;
		Lib.current.addChild(shape);
		shape.x = (Lib.current.stage.stageWidth - CHANNEL_LENGTH * 2) >> 1;
		halfHeight = Lib.current.stage.stageHeight >> 1;

		snd = new flash.media.Sound();
		snd.addEventListener(SampleDataEvent.SAMPLE_DATA, onSample);
	}

	public function play() {
		if (sndChannel != null)
			return;
		sndChannel = snd.play();
		var stage = Lib.current.stage;
		stage.addEventListener(Event.ENTER_FRAME, onEnterFrame);
		stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
	}

	function onSample ( event : SampleDataEvent ) {
		var p = event.position;
		var tone = App.GetTone(baseIndex) * PHASE;
		for (c in 0...(2048 * 2)) {   // max is 8192(2048 * 4)
			var r = (c + p) * tone;
			var f = App.GetTimbre(r, cast timbreIndex) * .25;
			event.data.writeFloat(f); // Left Channel
			event.data.writeFloat(f); // Right Chahnel
		}
	}

	function onKeyDown( event : KeyboardEvent ) {
		var code : Int = event.keyCode;
		switch(code) {
		case 27: // ESC
			var stage = Lib.current.stage;
			stage.removeEventListener(Event.ENTER_FRAME, onEnterFrame);
			stage.removeEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			sndChannel.stop();
			sndChannel = null;
		case 32: // SPACE
			timbreIndex++;
			if (timbreIndex >= (TimbreMAX : Int)) {
				timbreIndex = Sin;
			}
		case "1".code, "2".code, "3".code, "4".code, "5".code
		,"6".code, "7".code, "8".code, "9".code:
			baseIndex = code - "1".code;
		default:
		}
	}

	function onEnterFrame ( event : Event ) {
		var bytes = new ByteArray();
		bytes.length = (CHANNEL_LENGTH * 2 * 4); // sizeof(float)

		SoundMixer.computeSpectrum(bytes, false, 0);

		g2d.clear();
		var nf : Float = 0.;
		var hf : Float = 1. * halfHeight;

		g2d.lineStyle(0, 0x6600CC);
		g2d.beginFill(0x6600CC);
		g2d.moveTo(0, halfHeight);
		for (i in 0...CHANNEL_LENGTH) {
			nf = bytes.readFloat() * hf;
			g2d.lineTo(i * 2, Std.int(hf - nf));
		}
		g2d.lineTo(CHANNEL_LENGTH * 2, halfHeight);
		g2d.endFill();

	#if CHANNEL_RIGHT
		g2d.lineStyle(0, 0xCC0066);
		g2d.beginFill(0xCC0066, 0.5);
		g2d.moveTo(CHANNEL_LENGTH * 2, halfHeight);
		var i = CHANNEL_LENGTH;
		while(i > 0) {
			nf = bytes.readFloat() * hf;
			g2d.lineTo(i * 2, Std.int(hf - nf));
			i--;
		}
		g2d.lineTo(0, halfHeight);
		g2d.endFill();
	#end
	}
}
