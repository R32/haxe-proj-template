open Printf
open Globals

let value s =
	let lex = Lexing.from_string s in
	let stk = Stream.from (fun i -> Some (Lexer.token lex)) in
	Parser.value stk

let error msg p =
	printf "error pos: %d-%d: %s\n" p.pmin p.pmax msg;
	exit(-1)

let () =
	try
		let usage = "A Simple Lexer(ocamllex)/Parser(camlp4o) Template\n Options:" in
		let output = ref "" in
		let files = ref [] in
		let basic_args_spec = [
			("-o", Arg.Set_string output, "<dir> : specify output directory");
			("--version", Arg.Unit (fun()->print_string "0.1.0\n"), "print version and exit")
		] in
		Arg.parse basic_args_spec (fun s-> files := s :: !files) usage;
		if !Arg.current == 1 then
			Arg.usage basic_args_spec usage
		else
			List.iteri (fun i s ->
				let v = value s in
				printf "arg %d#\t %s = %d\n" i (Ast.s_value v) (Parser.eval v)
			) (List.rev !files)
	with
		| Exit -> ()
		| Lexer.Error (m, p) -> error (Lexer.error_msg m) p
		| Parser.Error (m, p) -> error (Parser.error_msg m) p
		| e -> raise e

