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
//#define NOENSURE

#ifdef LOCAL_SC_FUNCS
void _sc_reserve() __attribute__((naked,used,patmos_preserve_tmp));
void _sc_reserve()
{

  int n, m_top, sc_top;
  unsigned spilled_word;
  int   i;
  int  n_spill;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );



  sc_top -= n * 4;
  n_spill = (m_top - sc_top - (int) SWSC_SPM_SIZE) / 4;


  for (i = 0; i < n_spill; i++){
    m_top -= 4;
    _SPM unsigned *spm = (_SPM unsigned *) (m_top & MASK);
    _UNCACHED unsigned *ext_mem = (_UNCACHED unsigned *) (m_top);
    #ifdef POISON
      spilled_word = *spm;
      *spm = -1;
    #else
      spilled_word = *spm;
    #endif
    *ext_mem = spilled_word;
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

#ifdef LOCAL_SC_FUNCS
void _sc_ensure() __attribute__((naked,used,patmos_preserve_tmp,patmos_preserve_ret));
void _sc_ensure()
{
#if 1
  int  n, m_top, sc_top;
  unsigned filled_word;
  int i, n_fill;
  asm volatile(
      "mov %0 = $r8;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );

  n_fill = (n*4 - (m_top - sc_top)) / 4;

  for (i = 0; i < n_fill; i++){
    _SPM unsigned *spm = (_SPM unsigned *) (m_top & MASK);
    _UNCACHED unsigned *ext_mem = (_UNCACHED unsigned *) (m_top);

    filled_word = *ext_mem;
    #ifndef NOENSURE
      *spm = filled_word;
    #endif
    m_top += 4;
  }


  asm volatile(
      "mov $r27 = %0;" // sc_top
      "mov $r28 = %1;" // m_top
      :
      : "r"(sc_top), "r"(m_top)
      : "$r27", "$r28"
      );


#endif
}
#endif


#ifdef LOCAL_SC_FUNCS
void _sc_free() __attribute__((naked,used,patmos_preserve_tmp,patmos_preserve_ret));
void _sc_free()
{
#if 1
  int sc_top, m_top, n;

  asm volatile(
      "mov %0 = $r8;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  sc_top += n*4;
   
  if (sc_top > m_top) {
  m_top = sc_top ;
  }

  asm volatile(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(sc_top), "r"(m_top) /* input regs */
      : "$r27", "$r28" /* clobbered */
      );
#endif
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

void victim() __attribute__((noinline));
void victim() {
  recursion(7);
  printf("yelp\n");
  //printf("ULL 4x1: %lld %lld %lld %lld\n", u, u, u, u);

  float f = 2.0f;
  double d = 6.0;
  printf("float 2.0, double 6.0: %f, %f\n", f, d);
}

int main(int argc, char **argv) {

  int  sc_top;

  asm volatile("mov %0 = $r19;" // copy st to sc_top
      : "=r"(sc_top));

  //printf("0x%x\n", sc_top);
  _SPM char *spm = (_SPM char *) ((sc_top) & MASK);
  //puts("foo");

  *spm = 42;

  if (*spm == 42)
      puts("ok");

  //puts("bar");
  // printf("0x%x\n", test);

  // call recursive function to deplete stack cache and cause spilling
  recursion(10);

  if (*spm == 42)
      puts("ok");
  else
      puts("notok");

  victim();
  return 0;
}

