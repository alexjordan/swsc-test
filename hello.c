#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>

extern int _addr_base_spm;
extern int _addr_base_ext;


#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{
#if 0
  // some counter
  int i, nspill;
  //_SPM unsigned int *spm = (_SPM unsigned int*) 0x1234;
  _SPM unsigned int *spm1, *spm2;

  asm("mov %0 = $r1;" // copy argument to i
      "mov %1 = $r27;"
      "mov %2 = $r28;"
      : "=r" (i), "=r" (spm1), "=r" (spm2));


  // iterate something
  while(1) {
    if (!i)
      break;
    *spm2 = i;
    *spm1 = 42;
    i--;
  }
#endif
}
#endif

int main(int argc, char **argv) {

  //_SPM unsigned int *spm1 = (_SPM void *) 0x1ff0;
  //*spm1 = 42;
  //void *buf = malloc(1024);
#if 0
  printf("0x%x\n", _swsc_mem_base);
  printf("0x%x\n", _swsc_spm_base);
  //free(buf);
  unsigned int *ext = (void *) _swsc_mem_base;
  //_SPM unsigned int *ext = (_SPM void *) _swsc_mem_base;
  ext -=1;
  *ext = 42;

  _SPM unsigned int *spm = (_SPM void *) _swsc_spm_base;
  spm -=1;
  *spm = 42;
#endif

  printf("0x%x\n", _addr_base_spm);
  printf("0x%x\n", _addr_base_ext);

  // test if we can write to these
  int *foo = (int*) _addr_base_ext;
  *foo = 42;
  _SPM int *bar = (_SPM int*) _addr_base_spm;
  *bar = 42;

  puts("Hello world.\n");

  return 0;
}
