{(*
 * ocamllex
 *)
open Globals
open Ast
open Lexing

type error_msg =
	| Invalid_character of int

exception Error of error_msg * pos

let error_msg = function
	| Invalid_character c when c > 32 && c < 128 -> Printf.sprintf "Invalid character '%c'" (char_of_int c)
	| Invalid_character c -> Printf.sprintf "Invalid character 0x%.2X" c

let mp lexbuf =
	{
		pmin = lexeme_start lexbuf;
		pmax = lexeme_end lexbuf;
	}

let mk t l =
	(t, mp l)

let mk_int l =
	(Int (int_of_string (lexeme l)), mp l)

(* header end *)
}


let space = [' ' '\t']

rule token = parse
| space { token lexbuf }
| '(' { mk PrOpen lexbuf   }
| ')' { mk PrClose lexbuf  }
| '+' { mk (Op Add) lexbuf }
| '-' { mk (Op Sub) lexbuf }
| '*' { mk (Op Mul) lexbuf }
| '/' { mk (Op Div) lexbuf }
| '0' { mk (Int 0 ) lexbuf}
| '-'?['1'-'9']['0'-'9']* { mk_int lexbuf }
| eof | '\n' | '\r' { mk Eof lexbuf}
| _ as c {
	raise (Error (Invalid_character (int_of_char c), mp lexbuf))
}
