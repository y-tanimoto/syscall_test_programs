#include <stdio.h>
#include <unistd.h>

struct user_simple_proc_struct {
  int pid;
  int ppid;
  char cmd[20];
};

int main() {
  int pid, pid2, ppid, a;
  char *cmd, *cmd2;
  struct user_simple_proc_struct *usp;

  ppid = getpid();

  pid = fork();
  if (pid == 0) {
    sleep(10);
    return 0;
  }

  cmd = "emacs Hello";
  a = syscall(463, pid, ppid, cmd);               // set_simple_proc
  printf("ppid=%d, pid=%d: set_simple_proc returned %d.\n", ppid, pid, a);
  a =  (struct simple_proc_struct*)syscall(465, pid, usp);                          // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid, a);
  if (usp != NULL)
    printf("child proc 1 profile:\npid=%d\nppid=%d\ncmd=%s\n", usp->pid, usp->ppid, usp->cmd);

  pid2 = fork();
  if (pid2 == 0) {
    sleep(10);
    return 0;
  }

  cmd2 = "ls";
  a = syscall(463, pid2, ppid, cmd2);             // set_simple_proc
  printf("ppid=%d, pid=%d: set_simple_proc returned %d.\n", ppid, pid2, a);
  a =  (struct simple_proc_struct*)syscall(465, pid2, usp);                         // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid2, a);
  if (usp != NULL)
    printf("child proc 1 profile:\npid=%d\nppid=%d\ncmd=%s\n", usp->pid, usp->ppid, usp->cmd);
  
  a = syscall(464, pid);                          // delete_simple_proc
  printf("ppid=%d, pid=%d: delete_simple_proc returned %d.\n", ppid, pid, a); 
  a = (struct simple_proc_struct*)syscall(465, pid, usp);                          // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid, a);
  
  a = syscall(464, pid2);                         // delete_simple_proc
  printf("ppid=%d, pid=%d: delete_simple_proc returned %d.\n", ppid, pid2, a);
  a =  (struct simple_proc_struct*)syscall(465, pid2, usp);                         // search_simple_proc
  printf("ppid=%d, pid=%d: search_simple_proc returned %d.\n", ppid, pid2, a); 

  return 0;
}
    
