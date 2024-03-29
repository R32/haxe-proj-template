package;


class Utils {

	public static inline function microsec( f : Float ) : Float {
		return f * 1000;
	}

	public static function lerp( start : Int, end : Int, t : Float ) : Float {
		return start + t * (end - start);
	}

	public static inline function toFixed( f : Float, c : Int ) : Float {
		return (f : Dynamic).toFixed(c);
	}
}
