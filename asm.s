
section .data                    	      	

section .text                    	
	global add_bignum	           							
	global sub_bignum
	global mult_bignum
	global div_bignum
	global div2_bignum



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_ADD;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


carry_add:				;add 1 to the next num and dec by 10
	sub rax, 10
	lea r13, [r11+8]
	inc qword [r13+8]	
	jmp back_add
	

add_bignum:                        	    
	push    rbp              	
        mov     rbp, rsp 
	mov r11, 0	
	mov r11, rdi			;get 1st arg
	mov r12, rsi   			;get 2nd arg
	mov rcx, rdx    		;get 3rd arg

add_loop:
	mov rax, [r11+8]
	mov rbx, [r12+8]
	add rax, rbx			;add command
	cmp rax, 10			;carry?
	jge carry_add

back_add:
 	mov [r11+8] , rax		
	lea r11, [r11+8]		;get the next num to add
	lea r12, [r12+8]		;get the next num to add
	loop add_loop, rcx

	mov     rsp, rbp
        pop     rbp
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_SUB;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

carry_sub:				;loan 10 from the next num and sub 1
	lea r13, [r11+8]
	dec qword [r13+8]
	add rax, 10	
	jmp back_sub



sub_bignum:
	push    rbp              	
        mov     rbp, rsp 
	mov r11, rdi			;get 1st arg
	mov r12, rsi    		;get 2nd arg
	mov rcx, rdx    		;get 3rd arg
	
sub_loop:
	mov rax, [r11+8]
	mov rbx, [r12+8]
	cmp rax, rbx			;carry?
	jl carry_sub			;jmp for carry calc

back_sub:
	sub rax, rbx			;sub command
 	mov [r11+8] , rax
	lea r11, [r11+8]		;get the next num to sub
	lea r12, [r12+8]		;get the next num to sub
	loop sub_loop, rcx

	mov     rsp, rbp
        pop     rbp
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_MULT;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


mult_bignum:
	push    rbp              	
        mov     rbp, rsp
	mov r14, rcx			;get 4th arg
	mov r15, r14
	call mult_helper

	mov     rsp, rbp
        pop     rbp
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_MULT_HELPER;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


is_one:

	mov r11, rdi
	mov r14, r15
	mov rax, 0	
	push rdi
	push rsi
	mov rdi,r14
	mov rsi, r11
	call add_bignum
	pop rsi
	pop rdi
	mov     rsp, rbp
        pop     rbp
	ret

is_odd2:
	mov r11, rdi
	mov r14, r15
	mov r12, rsi
	mov rax, 0	
	mov ax, [r12+8]		
	test ax, 1
 	jz continue2
	push rdi
	push rsi
	mov rdi,r14
	mov rsi, r11
	call add_bignum
	pop rsi
	pop rdi
	jmp continue2

mult_helper:

	push    rbp              	
        mov     rbp, rsp
	mov rax,0
	mov r11,0
	mov r12,0
	mov r14,0 
	mov r11, rdi			;get 1st arg
	mov r12, rsi			;get 2nd arg
	mov r14, r15
	mov rcx, rdx    		;get 3rd arg

	;;;;;;;;;check_one;;;;;;;;
	cmp qword [r12+8], 1
	jne check_even2
	lea r12, [r12+rcx*8]
	dec rcx
	check_loop:
		cmp qword [r12], 0
		jne check_even2
		lea r12, [r12-8]
		loop check_loop, rcx
	jmp is_one

check_even2:	
	;;;;;;;;;check_even;;;;;;;;
	mov r12, rsi
	mov rcx, rdx
	jmp is_odd2

	

continue2:
	mov r12, rsi
	push rdi
	mov rdi, rsi
	call div2_bignum
	pop rdi

	mov r11,rdi
	mov rcx, rdx
	lea r11, [r11+rcx*8]
	lea r8, [r8+rcx*8]
	loop_copy3:
		mov rax,0
		mov qword [r8],0
		mov rax, [r11]
		mov [r8], rax
		lea r8, [r8-8]
		lea r11, [r11-8]
		loop loop_copy3, rcx
  
	mov rcx, rdx
	push rsi
	mov rsi,r8
	call add_bignum
	pop rsi
	mov rcx, rdx
	mov r11, rdi			
	mov r12, rsi
	lea r8, [r8+rcx*8]
	loop_initial2:
		mov qword [r8],0
		lea r8, [r8-8]
		loop loop_initial2, rcx
	mov rcx, rdx
	call mult_helper
	mov r14, r15
	mov r11,rdi
	mov rcx,rdx
	loop_copy4:
		mov rax,0
		mov rax, [r14+8]
		mov [r11+8], rax
		lea r14, [r14+8]
		lea r11, [r11+8]
		loop loop_copy4, rcx
	mov rcx, rdx
	mov     rsp, rbp
        pop     rbp
	ret




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_DIV;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

div_bignum:
	push    rbp              	
        mov     rbp, rsp
	mov rax,0
	mov r11,0
	mov r12,0
	mov r14,0 
	mov r11, rdi			;get 1st arg
	mov r12, rsi			;get 2nd arg
	mov r14, rcx			;get 4th arg
	mov rcx, rdx    		;get 3rd arg
	mov r10, [rbp+16]
	call div_helper


	;;;;;;;;;check_larger;;;;;;;;
	mov r11, rdi			
	mov r12, rsi
	mov rcx, rdx
	lea r11, [r11+rcx*8]
	lea r12, [r12+rcx*8]
	loop_check:
		mov rax, [r11]
		cmp rax,[r12]
		jl back_end
		jg back_larger
		lea r11, [r11-8]
		lea r12, [r12-8]
		loop loop_check, rcx

	back_larger:  		
		call sub_bignum
		push rdi
		push rsi
		mov rdi,r14
		mov rsi,r8
		call add_bignum
		pop rsi
		pop rdi

back_end:
	mov r11,rdi
	mov rcx,rdx
	loop_copy2:
		mov rax,0
		mov rax, [r14+8]
		mov [r11+8], rax
		lea r14, [r14+8]
		lea r11, [r11+8]
		loop loop_copy2, rcx
	mov rcx, rdx
	mov     rsp, rbp
        pop     rbp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_DIV_HELPER;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

less_than:
	call div2_bignum
	push rdi
	mov rdi, rsi
	call div2_bignum
	pop rdi
	mov     rsp, rbp
        pop     rbp
	ret 


div_helper:
	push    rbp              	
        mov     rbp, rsp
	mov r11,0
	mov r12,0 
	mov rax,0
	mov r11, rdi			;get 1st arg
	mov r12, rsi			;get 2nd arg
	mov rcx, rdx    		;get 3rd arg
	

	;;;;;;;;;check_larger;;;;;;;;
	lea r11, [r11+rcx*8]
	lea r12, [r12+rcx*8]
	loop_check2:
		mov rax, [r11]
		cmp rax,[r12]
		jl less_than
		jg back_larger2
		lea r11, [r11-8]
		lea r12, [r12-8]
		loop loop_check2, rcx
	
back_larger2:
	mov rcx, rdx
	mov r11, rdi			
	mov r12, rsi
	lea r12, [r12+rcx*8]
	lea r8, [r8+rcx*8]
	lea r9, [r9+rcx*8]
	lea r10, [r10+rcx*8]
	loop_copy:
		mov rax,0
		mov rbx,0
		mov qword [r9],0
		mov qword [r10],0
		mov rax, [r12]
		mov rbx, [r8]
		mov [r9], rax
		mov [r10], rbx
		lea r9, [r9-8]
		lea r10, [r10-8]
		lea r12, [r12-8]
		lea r8, [r8-8]
		loop loop_copy, rcx

	mov rcx, rdx
	push rdi
	push rsi
	mov rdi,rsi
	mov rsi,r9
	call add_bignum
	mov rdi,r8
	mov rsi,r10
	call add_bignum
	pop rsi
	pop rdi
	mov r11, rdi			
	mov r12, rsi
	mov rcx, rdx
	lea r9, [r9+rcx*8]
	lea r10, [r10+rcx*8]
	loop_initial:
		mov qword [r9],0
		mov qword [r10],0
		lea r9, [r9-8]
		lea r10, [r10-8]
		loop loop_initial, rcx
	mov rcx, rdx
	
	call div_helper
	
	;;;;;;;;;check_larger;;;;;;;;
	mov r11, rdi			
	mov r12, rsi
	mov rcx, rdx
	lea r11, [r11+rcx*8]
	lea r12, [r12+rcx*8]
	loop_check3:
		mov rax, [r11]
		cmp rax,[r12]
		jl back_end2
		jg back_larger3
		lea r11, [r11-8]
		lea r12, [r12-8]
		loop loop_check3, rcx
	
back_larger3:
	call sub_bignum
	push rdi
	push rsi
	mov r11, rdi			
	mov r12, rsi
	mov rdi,r14
	mov rsi,r8
	call add_bignum
	pop rsi
	pop rdi



back_end2:
	push rdi
	mov rdi,r8
	call div2_bignum
	mov rdi, rsi
	call div2_bignum
	pop rdi
	mov     rsp, rbp
        pop     rbp
	ret 




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;START_DIV2;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


check_even:

	test ax, 1
	jz continue
	jmp odd

div2_bignum:

	push    rbp              	
        mov     rbp, rsp 
	mov r13,0
	mov r11,0
	mov rax,0
	mov rcx,0
	mov r11, rdi			;get 1st arg
	mov rcx, rdx    		;get 3rd arg
	lea r11, [r11+rcx*8]

div_loop:
	mov rax, 0	
	mov ax, [r11]
	jmp check_even			;even?
	odd:
	lea r13, [r11-8]
	add byte [r13], 10

continue:
	shr qword [r11], 1		
	lea r11, [r11-8]		;get the next num to div		
	loop div_loop, rcx




	mov     rsp, rbp
        pop     rbp
	ret






