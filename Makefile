PASIM_CONFIG=--lsize 1024
.PHONY: dis run clean

hello.elf: hello.c
	patmos-clang -O1 -mpatmos-disable-stack-cache $< -o $@

dis: hello.elf
	patmos-llvm-objdump -d $< | less

run: hello.elf
	pasim ${PASIM_CONFIG} $<

run-debug: hello.elf
	pasim ${PASIM_CONFIG} --debug --debug-fmt instr $< 2>&1 | less

run-debug-r27: hello.elf
	pasim ${PASIM_CONFIG} --debug --debug-fmt instr $< 2>&1 | grep r27.=

clean:
	rm -f *.elf


