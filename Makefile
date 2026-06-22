GPPARAMS = -m32 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -O2
ASPARAMS = --32
LDPARAMS = -melf_i386 -nostdlib

objects = loader.o kernel.o memory.o gdt.o idt.o idt_asm.o keyboard.o shell.o vga.o paging.o syscall.o syscall_asm.o fs.o timer.o process.o switch.o elf.o usermode.o mmap.o string.o stdio.o stdlib.o math.o script.o net.o ethernet.o arp.o ip.o icmp.o udp.o vbe.o mouse.o wm.o desktop.o

%.o: %.cpp
	g++ $(GPPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

clean:
	rm -f *.o mykernel.bin

run: mykernel.bin
	cp mykernel.bin iso/boot/
	grub-mkrescue -o myos.iso iso
	qemu-system-i386 -cdrom myos.iso -net nic,model=rtl8139 -net user -vga std
