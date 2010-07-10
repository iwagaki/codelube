.intel_syntax   noprefix
.code16
.section        .kernel
.extern         main_func
.org            0
    
start_main_func:
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         ss, ax          # 直後に sp を変更すること
    mov         sp, 0xfff0

    call        main_func
    jmp         $
