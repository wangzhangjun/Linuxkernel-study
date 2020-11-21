# display a string "Hello, world."
.section .rodata
msg:   
	.ascii "Hello, world.\n" 
.section .text
.globl _start
_start:
# 	fork()  call

	movl  $2,%eax
	int   $0x80

#	write(int fd, const void *buf, size_t count)

	movl  $4,    %eax    
	movl  $1,    %ebx    
	movl  $msg,  %ecx    
	movl  $14,    %edx    
	int   $0x80

#     exit(0)	
	movl  $1,    %eax    
	movl  $0,    %ebx    
	int   $0x80
