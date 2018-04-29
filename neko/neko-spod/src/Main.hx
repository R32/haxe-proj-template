package;

import db.Group;
import db.User;
import db.UserGroup;
import haxe.io.Bytes;
import neko.Lib;
import sys.db.Connection;
import sys.db.Manager;
import sys.db.Mysql;
import sys.db.Sqlite;
import sys.db.Transaction;



class Main {
	
	static var database:Connection;
	
	static function mainLoop(){
		//setupDatabase();
		//database.commit();
	}
	
	static function cleanup() {
		if (database != null) {
			database.close();
			database = null;
		}
	}
	
	static function errorHandler(e:Dynamic){
		Lib.rethrow(e);
	}
	
	static function main() {
		try{
			database = initDatabase();
		}catch(e:Dynamic){
			return;
		}
		Transaction.main(database, mainLoop, errorHandler);
		database = null;
		cleanup();
	}
	
	
	static function initDatabase(useMysql:Bool = false):Connection{
		return
		if(useMysql){
			Mysql.connect({
				host: "localhost",
				port: 3306,
				database: "MyDatabase",
				user: "root",
				pass: "",
				socket: null
			});
		}else{
			Sqlite.open("hello.db");
		}
	}
	
	// 通过其它的方式布署表格, 如果用于 http 服务器端
	static function setupDatabase(){
	#if dbadmin
		//sys.db.Admin.initializeDatabase();
	#else
		var _db = neko.Lib.getClasses().db;
		for( c in Reflect.fields(_db) ) {
			var m = Reflect.field(_db, c).manager;
			if( m != null) sys.db.TableCreate.create(m);
		}		
	#end
		// other some database init here
	}
}