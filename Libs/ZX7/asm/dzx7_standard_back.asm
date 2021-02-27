; -----------------------------------------------------------------------------
; ZX7 decoder by Einar Saukas, Antonio Villena & Metalbrain
; "Standard" version (69 bytes only) - BACKWARDS VARIANT
; -----------------------------------------------------------------------------
; Parameters:
;   HL: last source address (compressed data)
;   DE: last destination address (decompressing)
; -----------------------------------------------------------------------------

dzx7_standard_back:
        ld      a, $80
dzx7s_copy_byte_loop_b:
        ldd                             ; copy literal byte
dzx7s_main_loop_b:
        call    dzx7s_next_bit_b
        jr      nc, dzx7s_copy_byte_loop_b ; next bit indicates either literal or sequence

; determine number of bits used for length (Elias gamma coding)
        push    de
        ld      bc, 0
        ld      d, b
dzx7s_len_size_loop_b:
        inc     d
        call    dzx7s_next_bit_b
        jr      nc, dzx7s_len_size_loop_b

; determine length
dzx7s_len_value_loop_b:
        call    nc, dzx7s_next_bit_b
        rl      c
        rl      b
        jr      c, dzx7s_exit_b         ; check end marker
        dec     d
        jr      nz, dzx7s_len_value_loop_b
        inc     bc                      ; adjust length

; determine offset
        ld      e, (hl)                 ; load offset flag (1 bit) + offset value (7 bits)
        dec     hl
        defb    $cb, $33                ; opcode for undocumented instruction "SLL E" aka "SLS E"
        jr      nc, dzx7s_offset_end_b  ; if offset flag is set, load 4 extra bits
        ld      d, $10                  ; bit marker to load 4 bits
dzx7s_rld_next_bit_b:
        call    dzx7s_next_bit_b
        rl      d                       ; insert next bit into D
        jr      nc, dzx7s_rld_next_bit_b ; repeat 4 times, until bit marker is out
        inc     d                       ; add 128 to DE
        srl	d			; retrieve fourth bit from D
dzx7s_offset_end_b:
        rr      e                       ; insert fourth bit into E

; copy previous sequence
        ex      (sp), hl                ; store source, restore destination
        push    hl                      ; store destination
        adc     hl, de                  ; HL = destination + offset + 1
        pop     de                      ; DE = destination
        lddr
dzx7s_exit_b:
        pop     hl                      ; restore source address (compressed data)
        jr      nc, dzx7s_main_loop_b
dzx7s_next_bit_b:
        add     a, a                    ; check next bit
        ret     nz                      ; no more bits left?
        ld      a, (hl)                 ; load another group of 8 bits
        dec     hl
        rla
        ret

; -----------------------------------------------------------------------------
