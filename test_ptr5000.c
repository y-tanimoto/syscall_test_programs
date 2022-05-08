#include <stdio.h>
#include <unistd.h>

int main() {
  int a = 10, b;

  b = syscall(453, &a);
  printf("a = %d, b = %d\n", a, b);

  return 0;
}
