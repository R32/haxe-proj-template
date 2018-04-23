(*
* camlp4o -impl parser.mly
*)
open Globals
open Ast

type error_msg =
	| Unexpected of token
	| Message of string

exception Error of error_msg * pos

let error_msg = function
	| Unexpected t -> "Unexpected " ^ (s_token t)
	| Message s -> s

let rec eval v =
	match fst v with
	| VInt i -> i
	| VOp (op, v1, v2) ->
		let i: int = eval v1 in
		let j: int = eval v2 in
		(match op with
		| Add -> i + j
		| Sub -> i - j
		| Mul -> i * j
		| Div -> i / j
		)
	| VParentheses v -> eval v

let rec value = parser
	| [< '(Int i, p); stream >] -> value_next (VInt i, p) stream
	| [< '(PrOpen, p); v = value; '(PrClose, p2); stream >] -> value_next (VParentheses v, punion p p2) stream
	| [< '(t,p) >] -> raise (Error(Unexpected t, p))

and value_next v = parser
	| [< '(Op o, _); v2 = value; stream >] ->
		let rec loop o v v2 =
			let p = punion (pos v) (pos v2) in
			(match fst v2 with
			| VOp(o2, v1, v2) when (o2 == Add || o2 == Sub || o == Mul || o == Div) ->
				value_next (VOp(o2, (loop o v v1), v2), p) stream
			| _ ->
				value_next (VOp(o, v, v2), p) stream
			)
		in value_next (loop o v v2) stream
	| [< >] -> v

