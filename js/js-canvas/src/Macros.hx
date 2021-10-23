package;

#if macro
import haxe.macro.Context;
import haxe.macro.Compiler;
import haxe.macro.Expr;
 using haxe.macro.Tools;
#end

class Macros {

	macro static public function text(node) return macro ($node: js.html.DOMElement).innerText;

	macro static public function evalint(e) {
		var f = evalInner(e);
		return macro @:pos(e.pos) $v{ Std.int(f) }
	}

	macro static public function eval(e) {
		return macro @:pos(e.pos) $v{ evalInner(e) }
	}

#if macro
	static function evalInner(e) : Dynamic {
		if (Context.defined("display"))
			return 0;
		function loop(e) : Dynamic {
			return switch(e.expr) {
			case EBinop(op, e1, e2):
				switch(op) {
				case OpAdd:  loop(e1) + loop(e2);
				case OpSub:  loop(e1) - loop(e2);
				case OpMult: loop(e1) * loop(e2);
				case OpDiv:  loop(e1) / loop(e2);
				default:     Context.fatalError("UnSupported: " + Std.string(op), e.pos);
				}
			case EConst(CFloat(f)):
				Std.parseFloat(f);
			case EConst(CInt(i)):
				Std.parseInt(i);
			case EParenthesis(e):
				loop(e);
			case ECall(macro Math.$name, args):
				var func = Reflect.field(Math, name);
				if (func == null)
					Context.fatalError("Math has no field: " + name, e.pos);
				Reflect.callMethod(Math, func, args.map( loop ));
			default:
				try {
					// try to extract inline variable, It will fails if "display" mode
					switch(Context.typeExpr(e).expr) {
					case TConst(TInt(i)):   i;
					case TConst(TFloat(f)): Std.parseFloat(f);
					default: throw "error";
					}
				} catch (_) {
					Context.fatalError("UnSupported: " + e.toString(), e.pos);
				}
			}
		}
		return loop(e);
	}
#end
}