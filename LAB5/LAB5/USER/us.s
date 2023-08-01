   .global entryPoint, main0, syscall, get_cpsr
	.text
.code 32
// upon entry, bl main0 => r0 contains pointer to the string in ustack

entryPoint:	
	bl main0
	
// if main0() ever retrun: syscall to exit(0)
	
@ user process issues int syscall(a,b,c,d) ==> a,b,c,d are in r0-r3	
syscall:

   stmfd sp!, {lr} // save lr in Ustack
   swi #0
   ldmfd sp!, {pc} // restore lr into PC 

get_cpsr:
   mrs r0, cpsr
   mov pc, lr

	
	
