package;

class Timer {

	public var frameCount(default, null) : Int;

	var then : Float;

	var id : Int;

	var running : Float->Void;

	var __update : Float->Void;

	public function new( run ) {
		running = run;
		__update = update; // do $bind only once
	}

	function update( now : Float ) {
		this.id = window.requestAnimationFrame(__update);
		var elapsed = now - then;
	#if !FPS60
		if (elapsed < microsec(Globals.FPS_INTERVAL_HACK))
			return;
	#end
		frameCount++;
		this.then = now;
		var tmod = elapsed / microsec(FPS_INTERVAL);
		running(tmod);
	}

	public function start() {
		frameCount = 0;
		var now = performance.now();
		this.then = now - microsec(FPS_INTERVAL);
		this.update(now);
	}

	public function stop() {
		window.cancelAnimationFrame(this.id);
	}
}
