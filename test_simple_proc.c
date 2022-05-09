#include <stdio.h>
#include <unistd.h>

void parent(int pid) {
  
}

int main() {
  int pid, pid2, ppid, a;
  char *cmd, *cmd2;

  ppid = getpid();

  pid = fork();
  if (pid == 0) {
    sleep(10);
    return 0;
  }

  cmd = "emacs Hello";
  a = syscall(463, pid, ppid, cmd);               // set_simple_proc
  printf("ppid=%d, pid=%d: set_simple_proc returned %d.\n", ppid, pid, a);
  a = syscall(465, pid);                          // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid, a); 

  pid2 = fork();
  if (pid2 == 0) {
    sleep(10);
    return 0;
  }

  cmd2 = "ls";
  a = syscall(463, pid2, ppid, cmd2);             // set_simple_proc
  printf("ppid=%d, pid=%d: set_simple_proc returned %d.\n", ppid, pid, a);
  a = syscall(465, pid2);                         // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid, a); 

  a = syscall(464, pid);                          // delete_simple_proc
  printf("ppid=%d, pid=%d: delete_simple_proc returned %d.\n", ppid, pid, a); 
  a = syscall(465, pid);                          // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid, a);
  
  a = syscall(464, pid2);                         // delete_simple_proc
  printf("ppid=%d, pid=%d: delete_simple_proc returned %d.\n", ppid, pid2, a);
  a = syscall(465, pid2);                         // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid2, a); 

  return 0;
}
    
