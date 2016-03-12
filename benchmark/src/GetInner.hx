package;


class GetInner{
	
	static var size = 10000;
	static var name = ["ToUserName", "FromUserName", "CreateTime", "MsgType", "PicUrl", "MediaId", "MsgId"];
	static var len = name.length;
	
	static public function runA(){
		var f:Float = 0;
		for (i in 0...size){
			var x = name[Std.int( Math.random() * len)];
				f += fastInner(XXX, x).length;
		}
		trace((fastInner(XXX, "ToUserName") == "toUser") + " - indexOf: " + f);
	}
	
	static public function runB(){
		var f:Float = 0;
		for(i in 0...size){
			var x = name[Std.int( Math.random() * len)];
			f += firstInerData(Xml.parse(XXX).firstElement(), x).length;
		}
		trace((firstInerData(Xml.parse(XXX).firstElement(), "PicUrl") == "this is a url") +" - Std XML: " + f);
	}
	
	static function fastInner(x:String, name:String):String{
		var pstart = "<" + name;
		var pend = "</" + name;
		var p:Int, p2:Int;

		p = x.indexOf(pstart);
		if (p == -1) return null;
		p += pstart.length;

		p = x.indexOf(">", p);
		if (p == -1) return null;
		p += 1;

		p2 = x.indexOf(pend, p + 1);
		if (p2 == -1) return null;

		var inner = StringTools.trim( x.substring(p, p2) );
		var len = inner.length;
		if (len >= 12 && inner.substr(0, 9) == "<![CDATA["){ 	// "(<![CDATA[" +  "]]>").length == 12
			inner = inner.substr(9, len - 12);
		}
		return inner;
	}
	
	static function firstInerData(x:Xml, name:String):String{
		var n = x.elementsNamed(name).next();
		return n != null ? getInnerData(n) : null;
	}

	/**
	copy from haxe.xml.Fast, 用于获得一个元素的 innerData
	*/
	static function getInnerData(x:Xml):String{
		var it = x.iterator();
		if( !it.hasNext() ) return null;
		var v = it.next();
		var n = it.next();
		if( n != null ) {
			// handle <spaces>CDATA<spaces>
			if( v.nodeType == Xml.PCData && n.nodeType == Xml.CData && StringTools.trim(v.nodeValue) == "" ) {
				var n2 = it.next();
				if( n2 == null || (n2.nodeType == Xml.PCData && StringTools.trim(n2.nodeValue) == "" && it.next() == null) )
					return n.nodeValue;
			}
			return null;
		}
		if ( v.nodeType != Xml.PCData && v.nodeType != Xml.CData )
			return null;
		return v.nodeValue;
	}
	
static var XXX='<xml>
 <ToUserName><![CDATA[toUser]]></ToUserName>
 <FromUserName><![CDATA[fromUser]]></FromUserName>
 <CreateTime>1348831860</CreateTime>
 <MsgType><![CDATA[image]]></MsgType>
 <PicUrl><![CDATA[this is a url]]></PicUrl>
 <MediaId><![CDATA[]]></MediaId>
 <MsgId>1234567890123456</MsgId>
 </xml>';
}