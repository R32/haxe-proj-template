package;

import Math.PI;
import Math.sin;
import flash.Lib;
import flash.utils.ByteArray;
import flash.media.SoundMixer;
import flash.events.SampleDataEvent;
import flash.events.KeyboardEvent;
import flash.events.Event;

class Wave {

	var snd : flash.media.Sound;

	var sndChannel : flash.media.SoundChannel;

	var g2d : flash.display.Graphics;

	var halfHeight : Int;

	var bases : Array<Float>;

	var baseIndex = 0;

	var timbreIndex : Int = Sin;

	public function new() {
		bases = generate(262.);

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
		var base = bases[baseIndex] * PHASE;
		for (c in 0...(2048 * 2)) { // max is 8192(2048 * 4)
			var r = (c + p) * base;
			var f = buildTimbre(r, cast timbreIndex) * .25;
			event.data.writeFloat(f);
			event.data.writeFloat(f);
		}
	}

	/**
	 x * sin(r)  : amplitude
	 sin(r + PI) : phase
	 sin(r * x)  : frequency
	 // https://www.bilibili.com/video/BV11W411H7Uz
	*/
	function buildTimbre( r : Float, type : TimbreType ) : Float {
		var f = switch(type) {
		case Sin:
			sin(r);
		case Square:
			(4 / PI) * (
			 (1 / 1.) * sin(r * 1.) +
			 (1 / 3.) * sin(r * 3.) +
			 (1 / 5.) * sin(r * 5.) +
			 (1 / 7.) * sin(r * 7.) +
			 (1 / 9.) * sin(r * 9.) +
			 (1 / 11.) * sin(r * 11.) +
			 (1 / 13.) * sin(r * 13.) +
			 (1 / 15.) * sin(r * 15.) +
			 (1 / 17.) * sin(r * 17.) +
			 (1 / 19.) * sin(r * 19.) +
			 (1 / 21.) * sin(r * 21.) +
			 (1 / 23.) * sin(r * 23.) +
			 0.
			);
		case Sawtooth:
			0.5 - (1 / PI) * (
			 (1 / 1.) * sin(r * 1.) +
			 (1 / 2.) * sin(r * 2.) +
			 (1 / 3.) * sin(r * 3.) +
			 (1 / 4.) * sin(r * 4.) +
			 (1 / 5.) * sin(r * 5.) +
			 (1 / 6.) * sin(r * 6.) +
			 (1 / 7.) * sin(r * 7.) +
			 (1 / 8.) * sin(r * 8.) +
			 (1 / 9.) * sin(r * 9.) +
			 (1 / 10.) * sin(r * 10.) +
			 (1 / 11.) * sin(r * 11.) +
			 (1 / 12.) * sin(r * 12.) +
			 (1 / 13.) * sin(r * 13.) +
			 (1 / 14.) * sin(r * 14.) +
			 (1 / 15.) * sin(r * 15.) +
			 (1 / 16.) * sin(r * 16.) +
			 0
			);
		case Triangle:
			(8 / (PI * PI)) * (
			 (sin(r * 1.) * (Math.pow(-1., (1. - 1.) / 2.) / (1. * 1.))) +
			 (sin(r * 3.) * (Math.pow(-1., (3. - 1.) / 2.) / (3. * 3.))) +
			 (sin(r * 5.) * (Math.pow(-1., (5. - 1.) / 2.) / (5. * 5.))) +
			 (sin(r * 7.) * (Math.pow(-1., (7. - 1.) / 2.) / (7. * 7.))) +
			 (sin(r * 9.) * (Math.pow(-1., (9. - 1.) / 2.) / (9. * 9.))) +
			 0.
			);
		case Noise:
			Math.random() * 2. - 1.;
		default:
			0.;
		}
		return f;
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

	static inline var CHANNEL_LENGTH = 256;
	static inline var PHASE = 3.141592653589793 * 2 / 44100;

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

	/**
	 DO   262   523   1046
	 RE   294   587   1175
	 MI   330   659   1318
	 FA   349   698   1397
	 SO   392   784   1568
	 LA   440   880   1760
	 SI   494   988   1967
	*/
	function generate( base : Float ) {
		var ret = [base];
		for (i in 1...13) {
			ret.push(base * Math.pow(2, i / 12));
		}
		return ret;
	}

	static function main() {
		var stage = Lib.current.stage;
		stage.scaleMode = flash.display.StageScaleMode.NO_SCALE;
		stage.align = flash.display.StageAlign.TOP_LEFT;
		var wave = new Wave();
		wave.play();
	}
}

enum abstract TimbreType(Int) to Int {
	var Sin;
	var Square;
	var Sawtooth;
	var Triangle;
	var Noise;
	var TimbreMAX;
}
