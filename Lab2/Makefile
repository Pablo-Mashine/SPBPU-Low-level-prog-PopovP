PROG_NAME 	= test
RISCV64_GCC = riscv64-unknown-elf-gcc
GCC_FLAGS	= -march=rv64g -mabi=lp64 -static -mcmodel=medany \
			  -fvisibility=hidden -fno-builtin \
			  -nostdlib -nostartfiles -g

LD_FLAGS=-T linker.ld

.PHONY: all clean qemu
all: $(PROG_NAME)

start.o: 
	$(RISCV64_GCC) $(GCC_FLAGS) -c start.S

test.o: test.c
	$(RISCV64_GCC) $(GCC_FLAGS) -c $(PROG_NAME).c

$(PROG_NAME): start.o $(PROG_NAME).o
	$(RISCV64_GCC) $(GCC_FLAGS) -o $@ start.o $(PROG_NAME).o $(LD_FLAGS)

clean:
	rm *.o *.dump $(PROG_NAME)

run:
	qemu-system-riscv64 -machine sifive_u -nographic -bios none -kernel $(PROG_NAME)