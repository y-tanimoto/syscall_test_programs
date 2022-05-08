#include <stdio.h>
#include <unistd.h>

int main() {
  int x = syscall(452);
  printf("output_log returned %d.\n", x);
  return 0;
}
