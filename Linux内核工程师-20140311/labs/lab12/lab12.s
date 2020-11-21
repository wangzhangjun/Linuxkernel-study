	.file	"lab12.c"
	.section	.rodata
.LC0:
	.string	"i = %d,j = %d\n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp ; 0xfffffff0 & %esp -> esp
	subl	$32, %esp
	movl	$10, 24(%esp)
	movl	$5, 28(%esp)
	movl	$.LC0, %eax
	movl	28(%esp), %edx
	movl	%edx, 8(%esp)
	movl	24(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	24(%esp), %edx
#APP
# 7 "lab12.c" 1
	movl %edx,%eax;movl %eax,%edx;
# 0 "" 2
#NO_APP
	movl	%edx, 28(%esp)
	movl	$.LC0, %eax
	movl	28(%esp), %edx
	movl	%edx, 8(%esp)
	movl	24(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.6 20120305 (Red Hat 4.4.6-4)"
	.section	.note.GNU-stack,"",@progbits
