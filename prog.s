.data
	printf_three_strings:
		.string "%s\n%s\n%s\n"
	scanf_three_strings:
		.string "%s%s%s"
	.set sys_exit_num, 60
	.set STDOUTFD, 1
	.set sys_write_num, 1
.bss
	str1:
		.space 255
	str2:
		.space 255
	str3:
		.space 255
.text
	.global main
	main:
	#scanf
		movq $scanf_three_strings, %rdi
		movq $str1, %rsi
		movq $str2, %rdx
		movq $str3, %rcx
		movb $0, %al
		call scanf
	#print forwards: printf(printf_three_string, str3, str2, str1)
		movq $printf_three_strings, %rdi
		movq $str3, %rsi
		movq $str2, %rdx
		movq $str1, %rcx
		movb $0, %al
		call printf
	#exit(0)
		movq $60, %rax
		movq $0, %rdi
		syscall
