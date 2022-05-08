#include <stdio.h>
#include <unistd.h>

int main() {
  int pid = getpid();
  int uid = getuid();

  printf("pid = %d uid = %d\n", pid, uid);
  
  int my_pid = syscall(456);  // my_getpid
  int my_uid = syscall(457);  // my_getuid

  printf("my_pid = %d my_uid = %d\n", my_pid, my_uid);

  return 0;
}
