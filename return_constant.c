#include <stdio.h>
#include <unistd.h>

int main() {
  int x = syscall(451);
  printf("%d\n", x);
  return 0;
}

