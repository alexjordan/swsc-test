SWSC_SPM_SIZE=32
PASIM_CONFIG=--lsize 1024
.PHONY: dis run clean

all: hello.elf hello-nonrec-wcet.elf

%.elf: %.c
	patmos-clang -g -O1 -Xllc -mpatmos-enable-soft-sc -mpatmos-swsc-spm-size=${SWSC_SPM_SIZE} $< -o $@

dis: hello.elf
	patmos-llvm-objdump -d -g $< | less

run: hello.elf
	pasim ${PASIM_CONFIG} $<

run-debug: hello.elf
	pasim ${PASIM_CONFIG} --debug --debug-fmt instr $< 2>&1 | less

run-debug-r27: hello.elf
	pasim ${PASIM_CONFIG} --debug --debug-fmt instr $< 2>&1 | grep r27.=

stats: hello.elf
	pasim ${PASIM_CONFIG} --verbose $<

clean:
	rm -f *.elf


