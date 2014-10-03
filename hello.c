#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern unsigned _addr_base_spm;
extern unsigned _addr_base_ext;
extern unsigned _spm_ext_diff;
extern unsigned SWSC_EXT_SIZE;
extern unsigned SWSC_SPM_SIZE;

#define MASK (SWSC_SPM_SIZE - 1)

// define to poison SPM when spilling word
#define POISON
// define to disable filling (check if poisoned value comes back)
// undefine to test whether spill/fill works
#define NOENSURE

#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{

  unsigned n, spilled_word, m_top, sc_top;
  int   i;
  int  n_spill;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );


  _SPM char *spm = (_SPM char *) ((m_top - 1) & MASK);
  _UNCACHED char *ext_mem = (_UNCACHED char *) (m_top - 1);

  n_spill = (int) (m_top - sc_top - _spm_ext_diff + n * 4 - SWSC_SPM_SIZE); 
  sc_top -= n * 4;


  for (i = 0; i < n_spill; i++){
    #ifdef POISON
      spilled_word = *spm;
      *spm = -1;
    #else
      spilled_word = *spm;
    #endif
    *ext_mem = spilled_word;
    m_top--;

  }

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : 
      : "r"(sc_top), "r"(m_top) 
      : "$r27", "$r28" 
      );


}
#endif

#if 1
void _sc_ensure() __attribute__((naked,used));
void _sc_ensure()
{

  unsigned  n, m_top, sc_top; 
  char filled_word;
  int i, n_fill;
  int r1, r2;
  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );

  _SPM char *spm = (_SPM char *) ((m_top - 1) & MASK);
  _UNCACHED char *ext_mem = (_UNCACHED char *) (m_top -1);

  n_fill = (int)(n * 4 - (m_top - sc_top - _spm_ext_diff)); 

  for (i = 0; i < n_fill; i++){
    filled_word = *ext_mem;
    #ifndef NOENSURE
      *spm = filled_word;
    #endif
    m_top++;
  }


  asm volatile(
      "mov $r27 = %0;" // sc_top
      "mov $r28 = %1;" // m_top
      "mov $r1 = %2;" // restore r1
      "mov $r2 = %3;" // restore r2
      : 
      : "r"(sc_top), "r"(m_top), "r"(r1), "r"(r2) 
      : "$r1", "$r2", "$r27", "$r28"
      );


}
#endif


#if 1
void _sc_free() __attribute__((naked,used));
void _sc_free()
{

  int r1, r2;
  unsigned sc_top, m_top, n;

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


/*void call1() {
  
  int sc_top1 = 12;
  int st_val1 = 10;
  int st_val;
  _SPM int *spm = (_SPM int *) sc_top1;
  *spm = st_val1;
  st_val = *spm;
  printf("0x%x\n", st_val);


}

void call2() {
  
  int sc_top1 = 12;
  int st_val1 = 10;
  int st_val;
  _SPM int *spm = (_SPM int *) sc_top1;
  *spm = st_val1;
  st_val = *spm;
  printf("0x%x\n", st_val);


}
*/

void recursion(int i) {
 if (i > 0)
 recursion(--i);
 puts("rec");
}

int main(int argc, char **argv) {


// call recursive function to deplete stack cache and cause spilling
 recursion(10);

  return 0;
}

