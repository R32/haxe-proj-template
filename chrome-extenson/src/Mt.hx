package;


#if macro
import haxe.macro.Context;
import haxe.macro.Compiler;
import haxe.macro.Expr;
import haxe.macro.Type;
using haxe.macro.TypeTools;
using haxe.macro.ExprTools;
using haxe.macro.ComplexTypeTools;
#end

class Mt{
	#if macro
	/**
	 - ~~remove static main function~~
	 
	 - add static inline init function
	*/
	static function build(){
		var fields = Context.getBuildFields();
		var pos = Context.currentPos();
		var cls = Context.getLocalClass().get();
		
		var expose = cls.name;	// for native
		switch(cls.meta.extract(":native")) {
			case [ { name:_, pos:_, params:[t] } ]:
				expose = t.toString();	// with quotes: e.g: "aaa";
				expose = expose.substr(1, expose.length - 2);
			default:
		}
		
		/*
		for (f in fields){
			if (f.name == "main" && f.access.indexOf(AStatic) != -1 ) {
				fields.remove(f);
				break;
			}
		}*/
		
		fields.push({
			name: "init",
			doc: "e.g: init(chrome.Extension.getBackgroundPage());",
			access: [AStatic, APublic, AInline],
			pos: pos,
			kind: FFun( {
				ret: macro :Void,
				args: [{
					name: "context",
					type: macro :Dynamic
				}],
				expr: macro {
					#if !nodejs
					untyped js.Browser.window[$v{expose}] = context[$v{expose}];
					#end
				}
			})
		});
		
		return fields;
	}
	#end
}