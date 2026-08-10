;;;;;;;;;;;;;;;;
;; FASTWORD.S ;;
;;;;;;;;;;;;;;;;

;; asm routines for worddell.c

;; the C side loads one packed starting-letter group from ALL5.BIN into
;; groupBuf - this routine searches that RAM buffer for queryWord

;; each word record is exactly 5 bytes -  there are no CR/LF characters

        .export _asm_find_word

        .import _groupBuf
        .import _groupWords
        .import _queryWord

        .include "zeropage.inc"

        .segment "BSS"
fw_left:        .res 2

        .segment "CODE"


;; asm_find_word(void)
;; returns 1 if queryWord is found in groupBuf
;; returns 0 if it is not found

_asm_find_word:
        lda     _groupWords
        sta     fw_left
        lda     _groupWords+1
        sta     fw_left+1

        lda     fw_left
        ora     fw_left+1
        bne     @have_words

        lda     #0
        rts

@have_words:
        lda     #<(_groupBuf)
        sta     ptr1
        lda     #>(_groupBuf)
        sta     ptr1+1

@word_loop:
        ldy     #0
        lda     (ptr1),y
        cmp     _queryWord
        bne     @next_word

        iny
        lda     (ptr1),y
        cmp     _queryWord+1
        bne     @next_word

        iny
        lda     (ptr1),y
        cmp     _queryWord+2
        bne     @next_word

        iny
        lda     (ptr1),y
        cmp     _queryWord+3
        bne     @next_word

        iny
        lda     (ptr1),y
        cmp     _queryWord+4
        bne     @next_word

        lda     #1
        rts

@next_word:
        clc
        lda     ptr1
        adc     #5
        sta     ptr1
        bcc     @dec_count
        inc     ptr1+1

@dec_count:
        lda     fw_left
        bne     @dec_low
        dec     fw_left+1

@dec_low:
        dec     fw_left
        lda     fw_left
        ora     fw_left+1
        bne     @word_loop

        lda     #0
        rts
