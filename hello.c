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
  _SPM unsigned int *sc_top, *sc_top_tmp;
  _UNCACHED unsigned int *m_top;
  int i, n, n_spill;
  unsigned spilled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  n_spill = m_top - sc_top - _spm_ext_diff + n * 4 - SWSC_SIZE; 
  n_spill = n_spill /4;
  sc_top_tmp = sc_top;

  for (i = 0; i < n_spill; i++){
    m_top -= 0x04;
    sc_top -= 0x04;
    spilled_word = *sc_top;
    *m_top = spilled_word;
  }

  sc_top = sc_top_tmp - n * 4;

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(sc_top), "r"(m_top) /* input regs */
      : "$r27", "$r28" /* clobbered */
      );
}
#endif

#if 0
void _sc_ensure() __attribute__((naked,used));
void _sc_ensure()
{
    _SPM unsigned int *sc_top, *sc_top_tmp;
  _UNCACHED unsigned int *m_top;
  int i, n, n_fill;
  unsigned filled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  
  n_fill =  sc_top + _spm_ext_diff + n * 4 - m_top; 
  n_fill = n_fill /4;
  sc_top_tmp = sc_top;

  for (i = 0; i < n_fill; i++){
    m_top += 0x04;
    sc_top += 0x04;
    filled_word = *sc_top;
    *m_top = filled_word;
  }

  sc_top = sc_top_tmp;

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(sc_top), "r"(m_top) /* input regs */
      : "$r27", "$r28" /* clobbered */
      );

}
#endif


#if 1
void _sc_free() __attribute__((naked,used));
void _sc_free()
{

  _SPM unsigned int *sc_top;
  _UNCACHED unsigned int *m_top;
   int n, r1, r2;

  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  sc_top += n * 4;
  void* sc_top_tmp;
  void * m_top_tmp;
  sc_top_tmp = (void*)sc_top;
  m_top_tmp = (void*)m_top;
   
  if ((sc_top_tmp + _spm_ext_diff) > m_top_tmp) {
	m_top_tmp = sc_top_tmp + _spm_ext_diff;
  }

  m_top = (_UNCACHED unsigned int*)(m_top_tmp);

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      "mov $r1 = %2;" // restore r1
      "mov $r2 = %3;" // restore r2
      : /* no output regs */
      : "r"(sc_top), "r"(m_top), "r"(r1), "r"(r2) /* input regs */
      : "$r1", "$r2", "$r27", "$r28" /* clobbered */
      );
}
#endif



int main(int argc, char **argv) {
  puts("Hello world.\n");
 // printf("0x%x\n", n); 
  return 0;
}
