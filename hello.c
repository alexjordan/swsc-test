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

// define to poison SPM when spilling word
#define POISON
// define to disable filling (check if poisoned value comes back)
// undefine to test whether spill/fill works
#define NOENSURE


#if 1
void _sc_reserve() __attribute__((naked,used));
void _sc_reserve()
{

  unsigned sc_top_tmp, m_top, sc_top1, n, n_spill;
  int   i;
  int spilled_word;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top1), "=r"(m_top) 
      ::
      );

  _SPM char *spm = (_SPM char *) ((m_top-1) & MASK);
  _UNCACHED char *ext_mem = (_UNCACHED char *) (m_top-1);

  n_spill = m_top - sc_top1 - _spm_ext_diff + n * 4 - SWSC_SPM_SIZE; 
  n_spill = n_spill / 4; // convert to words
  sc_top_tmp = sc_top1;
  sc_top_tmp -= n * 4;
  sc_top1 = sc_top_tmp;
  
 

  for (i = 0; i < n_spill; i++){
    m_top -= 4;
#ifdef POISON
 spilled_word = *spm;
 *spm-- = -1;
#else
    spilled_word = *spm;
#endif
    *ext_mem = spilled_word;
    ext_mem--; 
    spm--;
  }


  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : 
      : "r"(sc_top1), "r"(m_top) 
      : "$r27", "$r28" 
      );


}
#endif

#if 1
void _sc_ensure() __attribute__((naked,used));
void _sc_ensure()
{

  unsigned m_top, sc_top1, n, n_fill, filled_word;
  int i;
  int r1, r2;
  asm volatile(
      "mov %0 = $r1;" // save r1
      "mov %1 = $r2;" // save r2
      "mov %2 = $r8;" // copy argument to n
      "mov %3 = $r27;" // copy st to sc_top
      "mov %4 = $r28;" // copy ss to m_top
      : "=r" (r1), "=r" (r2), "=r" (n), "=r"(sc_top1), "=r"(m_top) 
      ::
      );

  _SPM char *spm = (_SPM char *) ((m_top - 1) & MASK);
  _UNCACHED char *ext_mem = (_UNCACHED char *) (m_top -1);

  n_fill = n * 4 - (m_top - sc_top1 - _spm_ext_diff); 
  n_fill = n_fill / 4; // convert to words

  for (i = 0; i < n_fill; i++){
    m_top += 4;
    filled_word = *ext_mem;
#ifndef NOENSURE
    *spm = filled_word;
#endif
    ext_mem++;
    spm++;
  }


  asm volatile(
      "mov $r27 = %0;" // sc_top
      "mov $r28 = %1;" // m_top
      "mov $r1 = %2;" // restore r1
      "mov $r2 = %3;" // restore r2
      : 
      : "r"(sc_top1), "r"(m_top), "r"(r1), "r"(r2) 
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


void call1() {
  
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


void recursion(int i) {
 if (i > 0)
 recursion(--i);
 puts("rec");
}

int main(int argc, char **argv) {
  
 
  int sc_top1;
  int st_val1 = 10;
//  int st_val;

 // puts("Hello world.\n"); 

//  volatile _SPM unsigned *spm = (_SPM unsigned *) sc_top1;
//  *spm = st_val1;

 // printf("0x%x\n", *spm);
 // call1();
 // call2();
 // st_val = *spm;
 // printf("0x%x\n", *spm);

// call recursive function to deplete stack cache and cause spilling
 recursion(10);

  return 0;
}

