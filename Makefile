PASIM_CONFIG=--lsize 64k
.PHONY: dis run clean

hello.elf: hello.c
	patmos-clang -O1 -mpatmos-disable-stack-cache $< -o $@

dis: hello.elf
	patmos-llvm-objdump -d $< | less

run: hello.elf
	pasim ${PASIM_CONFIG} $<

run-debug: hello.elf
	pasim ${PASIM_CONFIG} --debug --debug-fmt instr $< 2>&1 | less

clean:
	rm -f *.elf

