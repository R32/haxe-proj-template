package;

import Math.PI;
import Math.sin;

enum abstract TimbreType(Int) to Int {
	var Sin;
	var Square;
	var Sawtooth;
	var Triangle;
	var Noise;
	var TimbreMAX;
}

class App {

	// R360 / SAMPLERATE
	public static inline var PHASE = 3.141592653589793 * 2 / SAMPLERATE;

	public static inline var SAMPLERATE = 44100;

	public static inline var CHANNEL = 2;

	public static inline var STAGE_WIDTH = 600;

	public static inline var STAGE_HEIGHT = 400;

	/**
	 DO   262   523   1046
	 RE   294   587   1175
	 MI   330   659   1318
	 FA   349   698   1397
	 SO   392   784   1568
	 LA   440   880   1760
	 SI   494   988   1967
	*/
	static var atone = [262., 294., 330., 349., 392., 440., 494., 523., 587., 659.];
	public static inline function GetTone( i ) return atone[i];

	/**
	 x * sin(r)  : amplitude
	 sin(r + PI) : phase
	 sin(r * x)  : frequency
	 // https://www.bilibili.com/video/BV11W411H7Uz
	*/
	public static function GetTimbre( r : Float, type : TimbreType ) : Float {
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

	static var wave : Wave;

	static function main() {
	#if flash
		var stage = flash.Lib.current.stage;
		stage.scaleMode = flash.display.StageScaleMode.NO_SCALE;
		stage.align = flash.display.StageAlign.TOP_LEFT;
		wave = new Wave();
		wave.play();
	#elseif js
		var document = js.Browser.document;
		var canvas : js.html.CanvasElement = cast document.querySelector("canvas");
		if (canvas == null) {
			canvas = cast document.createElement("canvas");
			canvas.setAttribute("height", "" + STAGE_HEIGHT);
			canvas.setAttribute("width", "" + STAGE_WIDTH);
			document.body.append(canvas);
		}
		wave = new Wave();
	#end
	}
}
