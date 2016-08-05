package;

// haxe -cp src -main Run --interp
class Run{
	static function main(){
		var cmd = [
			"neko" => ["bin/ben.n"],	// neko
			"node" => ["bin/ben.js"],	// nodejs
			"luajit" => ["bin/ben.lua"],// luajit
			"java" => ["-jar", "bin/java/ben.jar"],	// hxjava(java)
			"bin/cs/bin/Ben.exe" => [],	// hxcs(c#)
			"bin/cpp/Ben.exe" => [],	// hxcpp(cpp)
		];

		for (k in cmd.keys()) {
			var c = is_windows ? k.split("/").join("\\") : k;
			var t0 = haxe.Timer.stamp();
			var p = new sys.io.Process(c, cmd.get(k));
			if (p.exitCode() == 0) {
				var t1 = haxe.Timer.stamp() - t0;
				Sys.print(p.stdout.readAll().toString());
				Sys.print('*** from start to finish. [TIME: $t1] ***\n\n');
			} else {
				Sys.print(p.stderr.readAll().toString());
			}
		}
	}
	static var is_windows = Sys.systemName() == "Windows";
}