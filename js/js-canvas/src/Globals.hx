package;

class Globals {

	public static inline var PI = 3.141592653589793;

	// canvas size
	public static inline var APP_WIDTH = 500;
	public static inline var APP_HEIGHT = 376;

	// fps
	public static inline var FPS_WANTED = #if FPS60 60. #else 30. #end;
	public static inline var FPS_INTERVAL = 1 / FPS_WANTED;
	public static inline var FPS_INTERVAL_HACK = 1 / (FPS_WANTED + 3);

	// gravity
	public static inline var GRAVITY_FORCE = 9.81;
	static inline var PLATFORM_HEIGHT_IN_METERS = 20.;
	public static inline var PIXELS_PER_METER = APP_HEIGHT / PLATFORM_HEIGHT_IN_METERS;

	// css
	public static inline var CSS_NONE = "none";
	public static inline var CSS_BLOCK = "block";
	public static inline var CSS_EMPTY = "";
}

@:native("window") extern var window : js.html.Window;
@:native("console") extern var console : js.html.ConsoleInstance;
@:native("document") extern var document : js.html.Document;
@:native("performance") extern var performance : js.html.Performance;
