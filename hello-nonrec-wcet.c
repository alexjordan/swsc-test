#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>

volatile int foo = 0;

#define UNROLL_REC(X,Y) \
    void rec_##X(int i) __attribute__((noinline)); \
    void rec_##X(int i) { \
     if (i > 0) \
         rec_##Y(--i); \
     foo++; \
    }

void rec_deadend(int i) {}
UNROLL_REC(10,deadend)
UNROLL_REC(9,10)
UNROLL_REC(8,9)
UNROLL_REC(7,8)
UNROLL_REC(6,7)
UNROLL_REC(5,6)
UNROLL_REC(4,5)
UNROLL_REC(3,4)
UNROLL_REC(2,3)
UNROLL_REC(1,2)
UNROLL_REC(0,1)

#if 0
void victim() __attribute__((noinline));
void victim() {
  recursion(7);
  printf("yelp\n");
  //printf("ULL 4x1: %lld %lld %lld %lld\n", u, u, u, u);

  float f = 2.0f;
  double d = 6.0;
  printf("float 2.0, double 6.0: %f, %f\n", f, d);
}
#endif

int main(int argc, char **argv) {


  // call recursive function to deplete stack cache and cause spilling
  rec_0(10);

  //victim();
  return foo;
}

