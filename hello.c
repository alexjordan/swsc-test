#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern int _addr_base_spm;
extern int _addr_base_ext;
extern int _spm_ext_diff;
extern int SWSC_SIZE;
 
#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{
  _SPM unsigned int *sc_top;
  _UNCACHED unsigned int *m_top;
  int i, n, n_spill;
  unsigned spilled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  sc_top -= n;
  n_spill = sc_top + _spm_ext_diff - m_top - SWSC_SIZE;

#if 0
  for (i = 0; i < n_spill; i++){
    m_top -= 0x01;
    spilled_word = *sc_top;
    *m_top = spilled_word;
  }
#endif

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(n_spill), "r"(m_top) /* input regs */
      : "$r27", "$r28" /* clobbered */
      );
}
#endif

int main(int argc, char **argv) {
  puts("Hello world.\n");

  return 0;
}
