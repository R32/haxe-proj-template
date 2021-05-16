package;

import js.Browser.window;
import js.Browser.document;
import js.html.MouseEvent;
import js.html.KeyboardEvent;
import js.html.DOMElement;
import js.html.audio.AudioContext;
import js.html.audio.AnalyserNode;
import js.html.audio.ScriptProcessorNode;
import js.html.CanvasElement;
import js.html.CanvasRenderingContext2D;

class Wave {

	static inline var CHANNEL_LENGTH = 256; // for graphics

	var audio : AudioContext;
	var procs : ScriptProcessorNode;
	var analy : AnalyserNode;
	var adata : js.lib.Uint8Array;
	var halfHeight : Int;
	var volume : Float;

	// graphics
	var g2d : CanvasRenderingContext2D;

	var playing : Bool;
	var baseIndex = 0;
	var timbreIndex : Int = Sin;

	public function new() {
		var canvas : CanvasElement = cast document.querySelector("canvas");
		g2d = canvas.getContext2d();
		showIntro();
		halfHeight = STAGE_HEIGHT >> 1;
		canvas.onclick = onClick;
		document.onkeydown = onKeydown;
		volume = 0.25;
	}

	function init() {
		audio = new AudioContext();
		analy = audio.createAnalyser();
		analy.connect(audio.destination);
		analy.fftSize = CHANNEL_LENGTH * 2;
		adata = new js.lib.Uint8Array(analy.frequencyBinCount);
		procs = audio.createScriptProcessor(); // 2048, 2, 1
		procs.onaudioprocess = onProcess;
		procs.connect(analy);
		prevT = window.performance.now();
		mainLoop(prevT + 1000 / 60);
	}

	var pacc = 0;
	function onProcess( e : js.html.audio.AudioProcessingEvent ) {
		var out = e.outputBuffer;
		var ch0 = out.getChannelData(0);
		var ch1 = out.getChannelData(1);
		var tone = App.GetTone(baseIndex) * PHASE;
		var p = pacc;
		for (i in 0...ch0.length) {
			var r = (i + p) * tone;
			var f = App.GetTimbre(r, cast timbreIndex) * volume;
			ch0[i] = f;
			ch1[i] = f;
		}
		pacc += ch0.length;
	}

	function onClick( e : MouseEvent ) {
		if (audio == null)
			init();
		if (playing) {
			procs.disconnect();
			playing = false;
		} else {
			procs.connect(analy);
			playing = true;
		}
	}

	function onKeydown( e: KeyboardEvent ) {
		var code = e.keyCode;
		switch(e.keyCode) {
		case 32: // SPACE
			timbreIndex++;
			if (timbreIndex >= (TimbreMAX : Int)) {
				timbreIndex = Sin;
			}
		case "1".code, "2".code, "3".code, "4".code, "5".code
		,"6".code, "7".code, "8".code, "9".code:
			baseIndex = code - "1".code;
		}
	}

	function showIntro() {
		var msg = "click to run";
		g2d.font = "16px consolas";
		g2d.fillStyle = "#ffffff";
		var width = Std.int(g2d.measureText(msg).width);
		g2d.fillText(msg, (STAGE_WIDTH - width) >> 1, ((STAGE_HEIGHT - 16) >> 1));
		g2d.strokeStyle = "#6600CC";
		g2d.fillStyle = "#6600CC";
		g2d.lineWidth = 1;
	}

	function update( dt : Float ) {
		analy.getByteTimeDomainData(adata);
		var nf : Float = 0.;
		var hf : Float = 1. * halfHeight;
		var size = analy.frequencyBinCount;
		var fw = STAGE_WIDTH / size;
		g2d.clearRect(0, 0, STAGE_WIDTH, STAGE_HEIGHT);
		g2d.beginPath();
		g2d.moveTo(0, hf);
		for (i in 0...size) {
			nf = 0.5 * (adata[i] - 128.) / 128. * hf;
			g2d.lineTo(i * fw, (hf - nf));
		}
		g2d.lineTo(size * fw, hf);
		g2d.closePath();
		g2d.fill();
		g2d.stroke();
	}

	var prevT = 0.;
	static inline var FRAME_T = 1000 / 30;
	var FPS = document.querySelector("#FPS");
	function mainLoop( t : Float ) {
		var dt = t - prevT;
		prevT = t;
		if (playing)
			update(dt);
		FPS.innerHTML = "FPS: " + 1000 / dt;
		window.requestAnimationFrame(mainLoop);
	}
}
