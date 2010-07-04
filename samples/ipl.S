.intel_syntax	noprefix	# AT&T 式は好きじゃないので Intel 式に切り替える
.code16
.section 	boot_loader
.extern 	main_func
.global		_start
.org		0
    
_start:
    jmp 	0x07c0:start	# 起動時は CS=0 IP=7c00 これを CS=7c0 IP=0 に置き換える（オフセットを 0 としてプログラムは組まれているため）

start:
    mov		ax, cs		# 各セグメントレジスタを CS に一致させる
    mov		ds, ax
    mov		es, ax
    mov		ss, ax
    mov		sp, 0xfff0	# スタックポインタの設定

    call	main_func	# C++ 言語で作成した main 関数を呼び出す
    jmp		$		# 現在位置への無限ループ
