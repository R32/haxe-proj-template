package;

#if macro
import haxe.macro.Context;
import haxe.macro.Compiler;
import haxe.macro.Expr;
 using haxe.macro.Tools;
#end

class Macros {

	macro static public function text(node) return macro ($node: js.html.DOMElement).innerText;

#if macro

#end
}