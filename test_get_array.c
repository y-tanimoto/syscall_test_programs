#include <stdio.h>
#include <unistd.h>

int main() {
  char *str = "Hello";
  
  int a = syscall(455, str);
  printf("get_array returned %d\n", a);
  
  return 0;
}
