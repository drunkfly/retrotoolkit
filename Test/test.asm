
#section main

PROG = 23635
CHAN_OPEN equ 5633
PRINT equ 8252
CHARS equ 23606

INK = 16
PAPER = 17
BRIGHT = 19
AT = 22

entry:          ld      hl, (PROG)
                ld      de, 5
                add     hl, de          ; HL = *PROG + 5
                ld      de, baseof(main); 0x8000
                ld      bc, sizeof(main)
                ldir                    ; копирует BC байт из HL в DE
                jp      start

start:          ld      sp, 0x8000
                ld      a, 2            ; номер канала для экрана
                call    CHAN_OPEN
                ld      ix, delta
loop:           ; рисуем X
                ld      l, 'X'
                ld      a, 0x07
                ld      bc, (x)         ; C = x, B = y
                call    DrawChar
                ; задержка
                halt
                halt
                ; стираем предыдущий символ
                ld      l, ' '
                ld      a, 0x38
                ld      bc, (x)         ; C = x, B = y
                call    DrawChar
                ; двигаем координату X
                ld      a, (x)
                add     a, (ix+dx)
                ld      (x), a
                cp      31              ; a-31 => flag
                jr      nz, @@1
                ld      (ix+dx), -1
@@1:            cp      0               ; лучше использовать `or a` == `or a,a`
                jr      nz, @@2
                ld      (ix+dx), 1
@@2:            ; двигаем координату Y
                ld      a, (y)
                add     a, (ix+dy)      ; dy
                ld      (y), a
                cp      192-8
                jr      nz, @@3
                ld      (ix+dy), -8
@@3:            cp      0               ; лучше использовать `or a` == `or a,a`
                jr      nz, @@4
                ld      (ix+dy), 8
@@4:            jp      loop

x               db      2
y               db      8

                ; Input:
                ;   L = symbol
                ;   A = attribute
                ;   C = X
                ;   B = Y (в пикселях, должно быть выровнено на знакоместо)

DrawChar:       ; Сохраняем А
                ex      af, af'
                ; Расчитываем адрес назначения
                call    CalcScreenAddr
                ; Расчитываем адрес символа
                ld      h, 0
                add     hl, hl          ; HL+HL = HL*2
                add     hl, hl          ; (HL*2)+(HL*2) = HL*4
                add     hl, hl          ; HL*8
                ld      bc, (CHARS)
                add     hl, bc          ; HL => адрес пикселей символа
                ; Рисуем
                ld      b, 8
@@1:            ld      a, (hl)
                ld      (de), a
                inc     d
                inc     hl
                djnz    @@1
                ; Расчитываем адрес в области атрибутов
                dec     d
                ld      a, d
                rra
                rra
                rra
                and     0x03
                or      0x58
                ld      d, a
                ; Восстанавливаем A
                ex      af, af'
                ; Записываем атрибут
                ld      (de), a
                ret

                ; Input:
                ;   C = X
                ;   B = Y (пиксели)
                ; Output:
                ;   DE => screen addr

CalcScreenAddr: ld      a, b
                rla                                   ; A = ? |Y5|Y4|Y3| ?| ?| ?| ?
                rla                                   ; A = Y5|Y4|Y3| ?| ?| ?| ?| ?
                and     0xe0            ; 1110 0000   ; A = Y5|Y4|Y3| 0| 0| 0| 0| 0
                or      c               ;               A = Y5|Y4|Y3|X4|X3|X2|X1|X0
                ld      e, a
                ld      a, b
                rra                           
                rra
                rra                                   ; A =  ?| ?| ?|Y7|Y6| ?| ?| ?
                and     0x18                          ; A =  0| 0| 0|Y7|Y6| 0| 0| 0
                ld      d, a
                ld      a, b
                and     0x07                          ; A =  0| 0| 0| 0| 0|Y2|Y1|Y0
                or      d                             ; A =  0| 0| 0|Y7|Y6|Y2|Y1|Y0
                or      0x40                          ; A =  0| 1| 0|Y7|Y6|Y2|Y1|Y0
                ld      d, a
                ret

;data            db      AT,{@here y=9},{@here x=2},16,4,"X"
data            db      AT
                db      16,0
symbol          db      "X"

dx = delta@@dx - delta
dy = delta@@dy - delta

delta:
@@dx            db      1
@@dy            db      8
