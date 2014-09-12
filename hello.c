#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern int _addr_base_spm;
extern int _addr_base_ext;
extern int _spm_ext_diff;
extern int SWSC_SIZE;


int n, test;
int  m_top, sc_top1;

#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{

    int sc_top_tmp, m_top;
  int   i, n, n_spill;
  int spilled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top1), "=r"(m_top) /* output regs */
      ::
      );

  _SPM int *spm = (_SPM int *) sc_top1;
  _UNCACHED int *ext_mem = (_UNCACHED int *) m_top;

  n_spill = m_top - sc_top1 - _spm_ext_diff + n - SWSC_SIZE; 
  n_spill = n_spill / 4; // convert to words
  sc_top_tmp = sc_top1;
  sc_top_tmp -= n ;
  sc_top1 = sc_top_tmp;
  
  // spilled_word = *ext_mem--;

  for (i = 0; i < n_spill; i++){
    spilled_word = *spm--;
    *ext_mem-- = spilled_word;
  }


  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(sc_top1), "r"(m_top) /* input regs */
      : "$r27", "$r28" /* clobbered */
      );
}
#endif

#if 1
void _sc_ensure() __attribute__((naked,used));
void _sc_ensure()
{
 // _SPM unsigned int *sc_top, *sc_top_tmp;
 // _UNCACHED unsigned int *m_top;

  int m_top, sc_top;
  int n;
  int r1, r2;
  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  asm volatile(
      "mov $r27 = %0;" // sc_top
      "mov $r28 = %1;" // m_top
      "mov $r1 = %2;" // restore r1
      "mov $r2 = %3;" // restore r2
      : /* no output regs */
      : "r"(sc_top), "r"(m_top), "r"(r1), "r"(r2) /* input regs */
      : "$r1", "$r2", "$r27", "$r28" /* clobbered */
      );
}
#endif


#if 1
void _sc_free() __attribute__((naked,used));
void _sc_free()
{



  int sc_top, r1, r2;


  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );
test = sc_top;

  sc_top += n;
   
  if ((sc_top + _spm_ext_diff) > m_top) {
	m_top = sc_top + _spm_ext_diff;
  }

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
 /*  int *foo = (int*) _addr_base_ext;
*foo = 42;
_SPM unsigned int *bar = (_SPM unsigned int*) _addr_base_spm;
*bar = 42;
  printf("0x%x\n", bar); 

bar--;*/
 printf("0x%x\n", sc_top1); 
// printf("0x%x\n", sc_top); 
 printf("0x%x\n", test); 

  return 0;
}
