#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern int _addr_base_spm;
extern int _addr_base_ext;
extern int _spm_ext_diff;
extern int SWSC_EXT_SIZE;
extern int SWSC_SPM_SIZE;

#define MASK (SWSC_SPM_SIZE - 1)

#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{

  int sc_top_tmp, m_top, sc_top1;
  int   i, n, n_spill;
  int spilled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top1), "=r"(m_top) /* output regs */
      ::
      );

  _SPM int *spm = (_SPM int *) (sc_top1 & MASK);
  _UNCACHED int *ext_mem = (_UNCACHED int *) m_top;

  n_spill = m_top - sc_top1 - _spm_ext_diff + n - SWSC_SPM_SIZE; 
  n_spill = n_spill / 4; // convert to words
  sc_top_tmp = sc_top1;
  sc_top_tmp -= n ;
  sc_top1 = sc_top_tmp;
  
  spilled_word = *ext_mem--; // m_top --

  for (i = 0; i < n_spill; i++){
    m_top --;
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

  int m_top, sc_top1;
  int n, i, n_fill, filled_word;;
  int r1, r2;
  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top1), "=r"(m_top) /* output regs */
      ::
      );

  _SPM int *spm = (_SPM int *) (sc_top1 & MASK);
  _UNCACHED int *ext_mem = (_UNCACHED int *) m_top;

  n_fill = n - (m_top - sc_top1 - _spm_ext_diff); 
  n_fill = n_fill / 4; // convert to words

  for (i = 0; i < n_fill; i++){
    m_top++;
    filled_word = *ext_mem++;
    *spm++ = filled_word;
  }


  asm volatile(
      "mov $r27 = %0;" // sc_top
      "mov $r28 = %1;" // m_top
      "mov $r1 = %2;" // restore r1
      "mov $r2 = %3;" // restore r2
      : /* no output regs */
      : "r"(sc_top1), "r"(m_top), "r"(r1), "r"(r2) /* input regs */
      : "$r1", "$r2", "$r27", "$r28" /* clobbered */
      );
}
#endif


#if 1
void _sc_free() __attribute__((naked,used));
void _sc_free()
{

  int sc_top, r1, r2, m_top, n;


  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

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

#if 0
void _sc_load() __attribute__((naked,used));
void _sc_load()
{

  


}
#endif


#if 0
void _sc_store() __attribute__((naked,used));
void _sc_store()
{


}
#endif


int main(int argc, char **argv) {
  
 puts("Hello world.\n"); 
 int st_val = 10;
  int sc_top1;

  asm volatile(
      "mov %0 = $r27;" // copy st to sc_top
      : "=r"(sc_top1) 
      ::
      );

  _SPM int *spm = (_SPM int *) sc_top1;

  *spm = st_val;
  puts("Hello world.\n");


  asm volatile(
      "mov %0 = $r27;" // copy st to sc_top
      : "=r"(sc_top1)
      ::
      );

  st_val =  *spm;

  printf("0x%x\n", st_val);


  return 0;
}

