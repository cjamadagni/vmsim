//#include <config.h>
#include <assert.h>
#include <stdio.h>

#include <util.h>

uint log_2(uint x) {
  int i, log = -1;
  
  for (i=0; i<(8*sizeof(x)); i++) {
    if (x & (1 << i)) {
      /* we only want perfect powers of 2 */
      if (log != -1)
	return -1;
      log = i;
    }
  }
  return log;
}

uint pow_2(uint pow) {
  return 1 << pow;
}

void util_test() {
  printf("Testing log_2\n");
  assert(log_2(1UL) == 0);
  assert(log_2(2UL) == 1);
  assert(log_2(4UL) == 2);
  assert(log_2(1073741824UL) == 30);
  assert(log_2(2147483648UL) == 31);
  assert(log_2(3) == -1);
  assert(log_2(12) == -1);

  printf("Testing pow_2\n");
  assert(pow_2(0) == 1UL);
  assert(pow_2(1) == 2UL);
  assert(pow_2(2) == 4UL);
  assert(pow_2(3) == 8UL);
  assert(pow_2(30) == 1073741824UL);
  assert(pow_2(31) == 2147483648UL);
}
