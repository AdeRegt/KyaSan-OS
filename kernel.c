void kernel_main();
void prints(char* a);
void printc(char a);
unsigned char getc();
void keyboard_wait();
void keyboard_init();
void setInterrupt(int i, unsigned long base);
void lidt();
void cdrRead(long lba,char count,char* location);

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

void prints(char* a){
    int c = 0;
    char b;
    while((b = a[c++])!=0x00){
        printc(b);
    }
}

char *vidpnt = (char*) 0xb8000;
char *location = (char*) 0x2000;
int vidpn = 0;

void printc(char a){
    vidpnt[vidpn++] = a;
    vidpnt[vidpn++] = 0x07;
}


char read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
short* read_xmd = (short*) &read_cmd;

void kernel_main(){
    
    prints("32 bit kernel created by Kyaw and Sander");
    keyboard_init();
    getc();
    lidt();
    cdrRead(1,1,location);
    int i;
    for(i = 0 ; i < 100 ; i++){printc(location[i]);}
    prints("DONE");
}

void cdrRead(long lba,char count,char* locationx){
    #define ATAPI_SECTOR_SIZE 512
    outportb(0x1f6,0xE0);
    outportb(0x1f1,0x00);
    outportb(0x1f4,ATAPI_SECTOR_SIZE & 0xff);
    outportb(0x1f5,ATAPI_SECTOR_SIZE >> 8);
    outportb(0x1f7,0xA0);
    char status;
    while ((status = inportb (0x1f7)) & 0x80)     /* BUSY */
    asm volatile ("pause");
    while (!((status = inportb (0x1f7)) & 0x8) && !(status & 0x1))
    asm volatile ("pause");
    if(status & 0x1){
        prints(" FAILURE ");
    }
    vidpnt[0]=0x00;
    read_cmd[9] = count;
    read_cmd[2] = (lba >> 0x18) & 0xFF;   /* most sig. byte of LBA */
    read_cmd[3] = (lba >> 0x10) & 0xFF;
    read_cmd[4] = (lba >> 0x08) & 0xFF;
    read_cmd[5] = (lba >> 0x00) & 0xFF; 
    int f = 0;
    for(f = 0 ; f < 6 ; f++){
        outportw(0x1f0,read_cmd[f]);
    }
    int tyt = 0;
    while(1){
        tyt++;
        char gz =  vidpnt[0];
        if(gz=='X'){
            break;
        }else{
            vidpnt[2] = vidpnt[0];
        }
        if(tyt==100000){
            prints("TIMEOUT");
            return;
        }
    }
    vidpnt[0]=0x00;
    short size = (((int) inportb(0x1f5)) << 8) | (int) (inportb(0x1f4));
    int i ;
    int z = 0;
    for (i = 0; i < (size / 2); i++) {
        short d = inportw(0x1F0);
        char A = d;
        char B = (unsigned char)(d >> 8);
        locationx[z++] = A;
        locationx[z++] = B;
        printc(A);printc(B);
    }
}

// =========================
// KEYBOARD
// =========================

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    2,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    1,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0xbe,	/* F12 Key */
    0,	/* All other keys are undefined */
};

void keyboard_init(){
    outportb(0x60,0xff);
    keyboard_wait();
    outportb(0x60,0xF6);
    keyboard_wait();
    outportb(0x60,0xF4);
    keyboard_wait();
}

void keyboard_wait(){
    while(1){
        unsigned char deze = inportb(0x60);
        if(deze==0xAA){break;}
        if(deze==0xFA){break;}
    }
}

	char lastone = 0x00;
	
unsigned char getc(){
	while(1){
		unsigned char in = inportb(0x60);
		if(lastone!=in){
			lastone = inportb(0x60);
			if(lastone>0){
				keyboard_init();
				return (unsigned char)kbdus[(int)lastone];
			}
		}
	}
}

// =================================================
// IRQ section
// =================================================
extern void irq_defaulte();
extern void irq_error();
extern void irq_hdd();
void hdd(){
    vidpnt[0] = 'X';
    vidpnt[1] = 0x75;
}
#define IDT_SIZE 256
struct idt_entry {
    unsigned short base_lo;
    unsigned short sel; /* Our kernel segment goes here! */
    unsigned char always0; /* This will ALWAYS be set to 0! */
    unsigned char flags; /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;
/**
 * Let the CPU know where the idt is
 */
void lidt() {
    // setup PIC http://www.osdever.net/bkerndev/Docs/whatsleft.htm
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
    // set all ints
    int i = 0;
    for (i = 0; i < IDT_SIZE; i++) {
        setInterrupt(i, (unsigned long) &irq_defaulte);
    }
    for(i = 0 ; i < 10 ; i++){
        setInterrupt(i, (unsigned long) &irq_error);
    }
    //for(i = 30 ; i < 33 ; i++){//35
    //setInterrupt(32, (unsigned long) &irq_timer);
    //}
    //setInterrupt(32, (unsigned long) &startToYield);
    setInterrupt(32+14, (unsigned long) &irq_hdd);
    idtp.limit = (sizeof (struct idt_entry) * IDT_SIZE) - 1;
    idtp.base = (unsigned int) &idt;
    asm volatile("lidt idtp\nsti");
    //idt_load();
}

/**
 * The default int shows a dot on the screen
 */
void defaulte() {
    
}

/**
 * Errormessage
 */
void error(char* msg) {
    prints("Unhandeld Exception: %s");prints(msg);
    asm volatile("cli\nhlt");
}

/**
 * Set interrupt
 * @param i interrupt number
 * @param base the function to go to
 */
void setInterrupt(int i, unsigned long base) {
    idt[i].base_lo = (base & 0xFFFF);
    idt[i].base_hi = (base >> 16) & 0xFFFF;
    idt[i].sel = 0x10;
    idt[i].always0 = 0;
    idt[i].flags = 0x8E;
}
// </editor-fold>
