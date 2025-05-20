	NAME	functest
        include lm8086.mac
X	EQU	6		;OFFSET OF ARGUMENTS
        extrn  _pcomrv:far
        extrn  _pcomkey:far
        extrn  _timeint0:far
I8259M  equ     00h             ;master 8259 address

        pseg

        PUBLIC  _savereg
        IF      LPROG
_savereg PROC    FAR
        ELSE
_savereg PROC    NEAR
        ENDIF
        mov     cs:save_ax,ax
        mov     cs:save_bx,bx
        mov     cs:save_cx,cx
        mov     cs:save_dx,dx
        mov     cs:save_di,di
        mov     cs:save_si,si
        mov     cs:save_es,es
        mov     cs:save_bp,bp
        mov     cs:save_ds,ds
        ret
_savereg endp

        PUBLIC  _int232c
        IF      LPROG
_int232c PROC    FAR
        ELSE
_int232c PROC    NEAR
        ENDIF
        cli
        push    ax
        push    bx
        push    cx
        push    dx
        push    si
        push    di
        push    ds
        push    es
        push    bp
        mov     cs:save_ss,ss
        mov     cs:save_sp,sp
        mov     ax,cs:save_ax
        mov     bx,cs:save_bx
        mov     cx,cs:save_cx
        mov     dx,cs:save_dx
        mov     di,cs:save_di
        mov     si,cs:save_si
        mov     es,cs:save_es
        mov     bp,cs:save_bp
        mov     ds,cs:save_ds
        call    _pcomrv
        mov     ss,cs:save_ss
        mov     sp,cs:save_sp

        mov     al,20h           ;EOI
        out     I8259M,al

        pop     bp
        pop     es
        pop     ds
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        sti
        iret
_int232c endp
;
        PUBLIC  _intkey
        IF      LPROG
_intkey  PROC    FAR
        ELSE
_intkey  PROC    NEAR
        ENDIF
        cli
        push    ax
        push    bx
        push    cx
        push    dx
        push    si
        push    di
        push    ds
        push    es
        push    bp
        mov     cs:save_ss,ss
        mov     cs:save_sp,sp
        mov     ax,cs:save_ax
        mov     bx,cs:save_bx
        mov     cx,cs:save_cx
        mov     dx,cs:save_dx
        mov     di,cs:save_di
        mov     si,cs:save_si
        mov     es,cs:save_es
        mov     bp,cs:save_bp
        mov     ds,cs:save_ds
        call    _pcomkey
        mov     ss,cs:save_ss
        mov     sp,cs:save_sp

        mov     al,20h           ;EOI
        out     I8259M,al

        pop     bp
        pop     es
        pop     ds
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        sti
        iret
_intkey  endp
;
        PUBLIC  _intime0
        IF      LPROG
_intime0 PROC    FAR
        ELSE
_intime0 PROC    NEAR
        ENDIF
        cli
        push    ax
        push    bx
        push    cx
        push    dx
        push    si
        push    di
        push    ds
        push    es
        push    bp
        mov     cs:save_ss,ss
        mov     cs:save_sp,sp
        mov     ax,cs:save_ax
        mov     bx,cs:save_bx
        mov     cx,cs:save_cx
        mov     dx,cs:save_dx
        mov     di,cs:save_di
        mov     si,cs:save_si
        mov     es,cs:save_es
        mov     bp,cs:save_bp
        mov     ds,cs:save_ds
        call    _timeint0
        mov     ss,cs:save_ss
        mov     sp,cs:save_sp

        mov     al,20h           ;EOI
        out     I8259M,al

        pop     bp
        pop     es
        pop     ds
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        sti
        iret
_intime0 endp
;
save_ss dw      1
save_sp dw      1
save_ax dw      1
save_bx dw      1
save_cx dw      1
save_dx dw      1
save_di dw      1
save_si dw      1
save_es dw      1
save_bp dw      1
save_ds dw      1
        endps
        end
