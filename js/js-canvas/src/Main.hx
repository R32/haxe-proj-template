package;

import Macros.evalint;
import Macros.eval;

@:nullSafety
class Main {

	var elapsed : Float;
	var then : Float;
	var g2d : CanvasRenderingContext2D;
	var balls : Array<Ball>;

	var scaleX : Float;
	var scaleY : Float;

	public function new() {
		var canvas : CanvasElement = cast document.querySelector("canvas");
		if (canvas == null) {
			canvas = cast document.createElement("canvas");
			document.body.appendChild(canvas);
		}
		canvas.setAttribute("width", "" + APP_WIDTH);
		canvas.setAttribute("height", "" + APP_HEIGHT);
		// init
		this.then = 0;
		this.elapsed = 0;
		this.scaleX = 1.0;
		this.scaleY = 1.0;
		this.balls = [];
		this.g2d = canvas.getContext2d();
		g2d.fillStyle = "black";
		g2d.strokeStyle = "rgba(255, 255, 255)";
		g2d.font = "8pt consolas normal";
		g2d.lineWidth = 1;
		g2d.fillRect(0, 0, APP_WIDTH, APP_HEIGHT);
		canvas.onclick = function( e : MouseEvent ) {
			if (e.ctrlKey) {
				balls.resize(0);
				return;
			}
			balls.push(new Ball(e.offsetX * scaleX , e.offsetY * scaleY));
		}
		window.onresize = function() {
			var canvas = this.g2d.canvas;
			var rect = canvas.getBoundingClientRect();
			this.scaleX = APP_WIDTH / rect.width;
			this.scaleY = APP_HEIGHT / rect.height;
		};
		window.onresize();
	}

	function run( now : Float ) {
		window.requestAnimationFrame(run);
		this.elapsed = now - then;
		if (this.elapsed < FPS_INTERVAL)
			return;
		var tmod = this.elapsed / FPS_INTERVAL;
		this.then = now;
		var g2d = this.g2d;
		g2d.clearRect(0, 0, APP_WIDTH, APP_HEIGHT);
		g2d.fillRect(0, 0, APP_WIDTH, APP_HEIGHT);
		g2d.strokeText("FPS: " + toFixed(1000 / elapsed, 2) , 2, 12);
		// g2d.strokeText("TF  : " + tmod, 2, 26);
		for (b in balls) {
			b.update(g2d, tmod);
		}
	}

	inline function start() {
		then = performance.now();
		window.requestAnimationFrame(run);
	}

	static function main() {
		var main = new Main();
		main.start();
	}
}

class Ball {
	public var x(default, null) : Float;
	public var y(default, null) : Float;
	var dirY : Float;
	public function new( x, y ) {
		this.x = x;
		this.y = y;
		dirY = 1.0;
	}

	public function draw( g2d : CanvasRenderingContext2D ) {
		g2d.beginPath();
		g2d.arc(this.x, this.y, CR , 0, 2 * Math.PI);
		g2d.closePath();
		g2d.stroke();
	}

	public function update( g2d : CanvasRenderingContext2D, tmod : Float ) {
		inline draw(g2d);
		if (this.y >= (APP_HEIGHT - CR)) {
			this.y = (APP_HEIGHT - CR);
			dirY = -1.0;
		} else if (this.y <= CR) {
			this.y = CR;
			dirY = 1.0;
		}
		this.y += dirY * tmod * ( PIXELS_PER_METER * GRAVITY_FORCE / FPS_WANTED );
	}

	static inline var CR = 20;
}
