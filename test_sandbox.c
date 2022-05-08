#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int pid = fork();

  if (pid == 0) {
    sleep(10);
    return 0;
  }
  else {
    printf("pid=%d\n", pid);
    
    int a;
    a = syscall(459, pid);
    printf("register: %d\n", a);

    a = syscall(460, pid, 1, 0);
    printf("set syscall-1 to forbidden: %d\n", a);
    a = syscall(460, pid, 2, 0);
    printf("set syscall-2 to forbidden: %d\n", a);
    a = syscall(460, pid, 2, 1);
    printf("set syscall-2 to allow: %d\n", a);

    a = syscall(462, pid);
    printf("exists: %d\n", a);

    a = syscall(461, pid);
    printf("delete profile: %d\n", a);

    wait(NULL);
  }

  return 0;
}
