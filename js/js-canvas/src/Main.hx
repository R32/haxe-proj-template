package;

class Main {

	var timer : Timer;
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
		this.scaleX = 1.0;
		this.scaleY = 1.0;
		this.balls = [];
		this.g2d = canvas.getContext2d();
		this.timer = new Timer(running);
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
			var rect = this.g2d.canvas.getBoundingClientRect();
			this.scaleX = APP_WIDTH / rect.width;
			this.scaleY = APP_HEIGHT / rect.height;
		};
		window.onresize();
	}

	function running( tmod : Float ) {
		var g2d = this.g2d;
		g2d.clearRect(0, 0, APP_WIDTH, APP_HEIGHT);
		g2d.fillRect(0, 0, APP_WIDTH, APP_HEIGHT);
		g2d.strokeText("FPS: " + toFixed(tmod * FPS_WANTED, 2) , 2, 12);
		for (b in balls) {
			b.update(g2d, tmod);
		}
	}

	static function main() {
		var main = new Main();
		main.timer.start();
	}
}

class Ball {
	public var x(default, null) : Float;
	public var y(default, null) : Float;
	var radian : Float;
	var dirX : Float;
	var dirY : Float;
	public function new( x, y ) {
		this.x = x;
		this.y = y;
		radian = 0.;
		dirX = 1.;
		dirY = 1.;
	}

	public function draw( g2d : CanvasRenderingContext2D ) {
		g2d.beginPath();
		g2d.arc(this.x, this.y,  1 , radian, 2 * Math.PI + radian);
		g2d.arc(this.x, this.y, CR , radian, 2 * Math.PI + radian);
		g2d.closePath();
		g2d.stroke();
	}

	public function update( g2d : CanvasRenderingContext2D, tmod : Float ) {
		inline draw(g2d);
		// 45ANG/SEC
		this.radian += tmod * ((PI / 180 * 45 * FPS_INTERVAL));
		if (this.y >= (APP_HEIGHT - CR)) {
			this.y = (APP_HEIGHT - CR);
			return;
			dirY = -1.0;
		}
		// 9.81M/SEC
		this.y += dirY * tmod * ( PIXELS_PER_METER * GRAVITY_FORCE * FPS_INTERVAL);
	}

	static inline var CR = 20;
}
