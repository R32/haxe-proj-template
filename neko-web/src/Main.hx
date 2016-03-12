package;




class Main {
	
	static function main() {
		neko.Web.cacheModule(run);
		run();
	}
	
	static function run(){
		Sys.println("It workds!");
	}
}

