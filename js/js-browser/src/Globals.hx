package;

class Globals {
	public static inline var CSS_NONE = "none";
	public static inline var CSS_BLOCK = "block";
	public static inline var CSS_EMPTY = "";
}


@:native("window") extern var window : js.html.Window;
@:native("console") extern var console : js.html.ConsoleInstance;
@:native("document") extern var document : js.html.Document;
