;; path = %path%;"E:\Program Files\wabt"
;; chokidar src/app.wast -c "wast2wasm src/app.wast -o bin/app.wasm"
(module
    (import "js" "mem" (memory 1))
    ;;
    (func (export "setByte") (param $i i32) (param $v i32)
        get_local $i
        get_local $v
        i32.store8
    )
    (func (export "setI16") (param $i i32) (param $v i32)
        get_local $i
        get_local $v
        i32.store16
    )
    (func (export "setI32") (param $i i32) (param $v i32)
        get_local $i
        get_local $v
        i32.store
    )
    (func (export "setFloat") (param $i i32) (param $v f32)
        get_local $i
        get_local $v
        f32.store
    )
    (func (export "setDouble") (param $i i32) (param $v f64)
        get_local $i
        get_local $v
        f64.store
    )
    ;; getter
    (func (export "getByte") (param $i i32) (result i32)
        get_local $i
        i32.load8_u
    )
    (func (export "getUI16") (param $i i32) (result i32)
        get_local $i
        i32.load16_u
    )
    (func (export "getI32") (param $i i32) (result i32)
        get_local $i
        i32.load
    )
    (func (export "getFloat") (param $i i32) (result f32)
        get_local $i
        f32.load
    )
    (func (export "getDouble") (param $i i32) (result f64)
        get_local $i
        f64.load
    )
)