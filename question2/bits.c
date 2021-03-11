#include <stdint.h>
#include <stdio.h>

int main(){
  uint16_t N = 21;
  N |= N >> 1;
  printf("%d\n", N);
  N |= N >> 0;
  N |= N >> 0;
  N |= N >> 0;
  printf("%d\n", N);
  printf("%d\n", (N + 1) >> 1);

  return 0;
}

