package;

class Main {

	static function main() {
		var t = new MyThing();
		t.a = 2;
		t.b = 3;
		trace( lengthSqrt(t) );
	}
	@:hlNative("mod", "len_sqrt") static function lengthSqrt(t: MyThing): Int { return 0; }
}

// This could be built automatically by haxe macro
abstract MyThing(hl.Bytes) {
	public var a(get, set) : Int ;
	public var b(get, set) : Int ;

	public inline function new() {
		this = new hl.Bytes(8);
	}

	inline function get_a() return this.getI32(0);

	inline function get_b() return this.getI32(4);

	inline function set_a(v:Int) {
		this.setI32(0, v);
		return v;
	}

	inline function set_b(v:Int) {
		this.setI32(4, v);
		return v;
	}
}

