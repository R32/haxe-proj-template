package;


import chrome.Extension;

class Popup{
	public static function main() {	
		Background.init(Extension.getBackgroundPage());
	}
}