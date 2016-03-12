package;


import neko.vm.Deque;
import neko.vm.Mutex;

class Request {
	
	static var s2 = Sys.time() * 1000  + 2000;
	static var deq = new Deque<Int>();
	static var mut = new Mutex();
	
	static function main() {
		var loop = 10;
		
		for (i in 0...loop) neko.vm.Thread.create(run);
		
		for (i in 0...loop) deq.pop(true);
		
		Sys.println("thread exited.");
	}
	
	static function run() {
		while (true)	{
			var time = Sys.time() * 1000;			
			if (time > s2){
				try{
					Sys.println("time: " + time + ", respond: " + haxe.Http.requestUrl("http://localhost:8080/app.n?random=" + Math.random()));	
				}catch (err:Dynamic)	{
					mut.acquire();
					Sys.println("time: " + time + ", err: " + err);
					mut.release();
				}
				deq.push(1);
				break;
			}
			Sys.sleep(0.01);
		}		
	}
}