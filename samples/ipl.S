.intel_syntax   noprefix         # AT&T 式は好きじゃないので Intel 式に切り替える 
.code16
.section        .boot_loader
.global         _start
.org            0

_start:
    jmp         0x07c0:start    # 起動時は CS=0 IP=7c00 これを CS=7c0 IP=0 に置き換える（オフセットを 0 としてプログラムは組まれているため）

start:
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         ss, ax          # sp は直後に変更すること
    mov         sp, 0xfffc

    mov         dl, 0x80        # dl はドライブ番号が入っている
    jb          error

    mov         ah, 0x41        # bx = 0x55a, dl = drive index
    mov         bx, 0x55aa
    int         0x13
    jc          error
    cmp         bx, 0xaa55
    jne         error
    test        cl, 0x01        # 1 - Device Access using the packet structure
    jz          error

#    mov                ax, 0x1000
#    mov                es, ax
#    movb       es:[0], 0xea
#    movb       es:[1], 0x05
    
     ## mov        ax, 0x4200
     ## mov        si, DAP
     ## int        0x13
     ## jc         error

    mov         ax, 0x1000	
    mov         es, ax
    mov         bx, 0
    mov         ah, 2
    ##  sector
    mov         al, 10
    mov         ch, 0
    mov         cl, 1
    mov         dh, 0
    int         0x13
    jc          error
    
#    mov                ax, 0x1000
#    mov                es, ax
#    cmpb       es:[0], 0xea
#    jne                error
#    cmpb       es:[1], 0x05
#    jne                error

    ## mov                ax, 0x1000
    ## mov                es, ax
    ## mov                bx, 0xa30
    ## jmp                dump	

    #    cmpb   es:[512], 0x8c
#    jne                error
#    cmpb       es:[513], 0xc8
#    jne                error

    jmp         0x1000:0x0200

error:
    mov         ax, 0xb800
    mov         ds, ax
    mov         di, 0
    movw        ds:[di], 0xec41
    jmp         $
    

dump:                           # es:bx
    mov         ax, 0xb800
    mov         ds, ax
    mov         di, 0
    mov         cl, 20
put1bytes:  
    mov         dl, es:[bx]
    mov         dh, dl
    and         dl, 0x0f
    shr         dh, 4
    mov         ax, 0x0430
    cmp         dh, 0x0a
    jb          less_than_0a
    mov         ax, 0x0437
less_than_0a:   
    add         al, dh
    movw        ds:[di], ax
    add         di, 2
    mov         ax, 0x0430
    cmp         dl, 0x0a
    jb          less_than_0a_2
    mov         ax, 0x0437
less_than_0a_2: 
    add         al, dl
    movw        ds:[di], ax
    add         di, 2
    add         di, 2
    add         bx, 1
    dec         cl
    jnz         put1bytes
    jmp         $

DAP:
    .byte       0x10            # size of DAP
    .byte       0x00            # unused
    .word       0x7f7f          # number of sectors to be read
    .word       0x0000
    .word       0x1000
    .long       0x00000000      # start secotr (LBA)
    .long       0x00000000      # start secotr (LBA)
