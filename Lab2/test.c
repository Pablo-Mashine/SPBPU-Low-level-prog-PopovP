typedef unsigned long uintptr_t;

#define UART_REG_TXFIFO 0x0

#define UART_BASE_ADDR 0x10010000
#define CLINT_BASE_ADDR 0x2000000
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000 // offset for hart #0

void end(void) asm("end");
void timer_setup(unsigned long timecmp_inc) asm("timer_setup");
void timer_init(void) asm("timer_init");

static volatile int *mtime_addr = (int *)(void *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);
static volatile long *mtimecmp_addr = (long *)(void *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);

static volatile int *uart = (int *)(void *)UART_BASE_ADDR;

void wfi(void)
{
    __asm__ volatile("wfi" ::: "memory");
}

static int syscall(unsigned long arg0, unsigned long arg1,
                   unsigned long arg2, unsigned long arg3, unsigned long arg4,
                   unsigned long arg5, unsigned long arg6, unsigned long arg7)
{
    // despite you don't need all these registers it is better to set them all
    // remember that in trap_vector in start.S you rewrite a0, a1 and a2 so you better to use another registers
    // for sending info to handle_trap

    register uintptr_t a0 asm("a0") = (uintptr_t)(arg0); // connect variable to register
    register uintptr_t a1 asm("a1") = (uintptr_t)(arg1); // connect variable to register
    register uintptr_t a2 asm("a2") = (uintptr_t)(arg2); // connect variable to register
    register uintptr_t a3 asm("a3") = (uintptr_t)(arg3); // connect variable to register
    register uintptr_t a4 asm("a4") = (uintptr_t)(arg4); // connect variable to register
    register uintptr_t a5 asm("a5") = (uintptr_t)(arg5); // connect variable to register
    register uintptr_t a6 asm("a6") = (uintptr_t)(arg6); // connect variable to register
    register uintptr_t a7 asm("a7") = (uintptr_t)(arg7); // connect variable to register

    asm volatile("ecall"
                 : "=r"(a0), "=r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");

    return (int)a0;
}

static int syscall_timer_init()
{
    return syscall(2, 0, 0, 0, 0, 0, 0, 0);
}

static int syscall_timer_set(unsigned long timecmp_inc)
{
    return syscall(3, timecmp_inc, 0, 0, 0, 0, 0, 0);
}

static int putchar(char ch)
{
    while (uart[UART_REG_TXFIFO] < 0);
    return uart[UART_REG_TXFIFO] = ch & 0xFF;
}

static void print(char *str)
{
    while (*str)
    {
        putchar(*str);
        str++;
    }
}

static int syscall_print(char* str) {
    return syscall(1, (unsigned long)(void *)str, 0, 0, 0, 0, 0, 0);
}

void main(void)
{
    char* str = "Hello, i`m Popov Pavel\n";
    syscall_print(str);
    syscall_timer_init();
    syscall_timer_set(1000);
    wfi();
    end();
}

void timer_kill(){
    *(mtimecmp_addr) = ~0;
}

long handle_trap(long cause, long epc, long regs[32])
{
    if (cause < 0) // Interrupt
    {
        unsigned long interrupt_cause = cause & ~(1UL << 63);

        if (interrupt_cause == 7){ 
            print("interrupt start\n");
            timer_kill(0);
            print("interrupt end\n");
        }
        else {print("unknown interrupt\n");}

    }
    else // Exception
    {
        unsigned long exception_cause = cause;

        switch (exception_cause)
        {
        case 8: 
            if (regs[10] == 1) 
            {
		print("exception 1\n");
		print((char *)regs[11]);
            }
            else if (regs[10] == 2) 
            {
                print("exception 2\n");
                timer_init();
            }
            else if (regs[10] == 3) 
            {
                print("exception 3\n");
                timer_setup(regs[11]);
            }
            else
            {
                print("unknown ecall\n");
            }
            break;

        default:
            print("unlnown exception\n");
            break;
        }

        epc += 4;
    }

    return epc;
}
