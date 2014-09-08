
.PHONY: dis run clean

hello.elf: hello.c
	patmos-clang -O1 -mpatmos-disable-stack-cache $< -o $@

dis: hello.elf
	patmos-llvm-objdump -d $< | less

run: hello.elf
	pasim --lsize 64k $<

clean:
	rm -f *.elf

