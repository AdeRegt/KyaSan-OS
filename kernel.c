void kernel_main();

//ports
unsigned char inportb(unsigned short port);
unsigned short inportw(unsigned short port);
void outportb(unsigned short port,unsigned char data);
void outportw(unsigned short port,unsigned short data);

void outportb(unsigned short port,unsigned char data){
	__asm__ __volatile__("outb %1, %0" : : "dN" (port), "a" (data));
}

void outportw(unsigned short port,unsigned short data){
	__asm__ __volatile__("outw %1, %0" : : "dN" (port), "a" (data));
}


unsigned char inportb(unsigned short port){
	unsigned char rv;
	__asm__ __volatile__(" inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

unsigned short inportw(unsigned short port){
	unsigned short rv;
	__asm__ __volatile__("inw %1, %0" : "=a" (rv) : "dN"(port));
	return rv;
}


void kernel_main(){}
