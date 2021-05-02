// copy from em_macros.h
#define EM_IMPORT(NAME) __attribute__((import_module("env"), import_name(#NAME)))

// if there is no "--allow-undefined" in "wasm-ld",
// you need to explicitly add "EM_IMPORT(NAME)" instead of "extern"
// when you passed a pointer to JS, you could find it in webassembly.Memory
EM_IMPORT(log) void log(char* x, int a, int b, int c, int d, int e, int f);

// The built-in values, uses &__xxxx to access it. e.g: "&__heap_base"
// then the stack size is: "&__heap_base - &__data_end";
extern int __data_end;           // the end offset of the data section
extern int __global_base;        // __data_base ???
extern int __heap_base;          // the start offset of the heap
extern int __memory_base;        // ths start offset of the whole memeory???
extern int __dso_handle;
extern int __table_base;

extern void __wasm_call_ctors(); // no idea what's this

// export by "wasm-ld --export=square" in Makefile
int square(int n) {
	return n * n;
}

#define EM_EXPORT(name) __attribute__((used, export_name(#name)))
// export by macro
EM_EXPORT(test) char* arbitrary_name() {
	char* s1 = "string literal";
	char* s2 = "others";
	log(s1,
		(int)&__data_end,
		(int)&__global_base,
		(int)&__heap_base,
		(int)&__memory_base,
		(int)&__table_base,
		(int)&__dso_handle
	);
	return s2;
}
