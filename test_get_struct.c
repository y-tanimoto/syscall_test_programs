#include <stdio.h>
#include <unistd.h>

typedef struct {
  int a;
  int b;
} my_struct;

int main() {
  my_struct s;
  s.a = 123;
  s.b = 456;

  int a = syscall(454, &s);
  printf("return_get_struct returned %d\n", a);
  
  return 0;
}
