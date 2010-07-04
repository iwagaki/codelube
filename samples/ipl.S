.intel_syntax	noprefix	# AT&T 式は好きじゃないので Intel 式に切り替える
.code16
.text
.extern 	main
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

    mov		ax, 0xb800
    mov		es, ax
    mov		di, 0

    mov		ax, 0xec41	# 背景 yellow & blink, 文字色 hi-red, 文字コード = 0x41 = 'A'
    mov		cx, 80*25

.fill_screen:
    mov		es:word [di], ax
    add 	di, 2
    dec		cx
    jnz		.fill_screen

    jmp		$		# 現在位置への無限ループ

boot_signature:
.org		510		# 有効な MBR として認識されるには 510 番地に 0xaa55 のマーカーが必要
.word 		0xaa55
