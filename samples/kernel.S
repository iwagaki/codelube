.intel_syntax   noprefix
.code16
.section        .kernel
.extern         main_func
.org            512
    
start_main_func:
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         ss, ax          # sp should be set immediately after ss
    mov         sp, 0xfff0

    call        main_func
    jmp         $
