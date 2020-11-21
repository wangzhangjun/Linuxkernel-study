	.file	"lab31.c"
	.comm	ready,4,4
	.comm	buf,400,32
	.text
.globl test
	.type	test, @function
test:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$50, buf(,%eax,4)
	movl	$1, ready
	popl	%ebp
	ret
	.size	test, .-test
	.section	.rodata
.LC0:
	.string	"before buf[%d] = %d\n"
.LC1:
	.string	"after buf[%d] = %d\n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$10, 28(%esp)
	movl	28(%esp), %eax
	movl	buf(,%eax,4), %edx
	movl	$.LC0, %eax
	movl	%edx, 8(%esp)
	movl	28(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	test
	movl	28(%esp), %eax
	movl	buf(,%eax,4), %edx
	movl	$.LC1, %eax
	movl	%edx, 8(%esp)
	movl	28(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.6 20120305 (Red Hat 4.4.6-4)"
	.section	.note.GNU-stack,"",@progbits
