(*
* 1 + 2 * (3 + 4)
*)
open Globals

type op =
	| Add
	| Sub
	| Mul
	| Div

type token =
	| Eof
	| Int of int
	| Op of op
	| PrOpen
	| PrClose

type value_def =
	| VInt of int
	| VOp of op * value * value
	| VParentheses of value
and value = value_def * pos

let s_op = function
	| Add -> " + "
	| Sub -> " - "
	| Mul -> " * "
	| Div -> " / "

let s_token = function
	| Eof -> "<end of file>"
	| Int i -> string_of_int i
	| Op o -> s_op o
	| PrOpen ->  "("
	| PrClose -> ")"

let rec s_value v =
	match fst v with
	| VInt i -> string_of_int i
	| VOp (op, v1, v2) -> (s_value v1) ^ (s_op op) ^ (s_value v2)
	| VParentheses v -> "(" ^ (s_value v) ^ ")"

let punion p p2 =
	{
		pmin = min p.pmin p2.pmin;
		pmax = max p.pmax p2.pmax;
	}

let pos v = snd v
