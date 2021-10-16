package;

class Main {
	static function main() {
		var h2 = document.querySelector("h2");
		trace(text(h2));
		text(h2) = "hello world!";
	}
}
