#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int pid = fork();

  if (pid == 0) {
    sleep(3);

    // syscall-451を実行
    int b = -1;
    b = syscall(451);
    printf("syscall-451 returned %d\n", b);
    
    return 0;
  }
  else {
    printf("pid=%d\n", pid);
    int a;

    // 子スレッドをサンドボックスに入れる
    a = syscall(459, pid);
    printf("register: %d\n", a);

    // Syscall1と2を禁止
    a = syscall(460, pid, 451, 0);
    printf("set syscall-1 to forbidden: %d\n", a);
    a = syscall(460, pid, 452, 0);
    printf("set syscall-2 to forbidden: %d\n", a);

    // サンドボックスに入っているかチェック＆禁止リストをログに表示
    a = syscall(462, pid);
    printf("exists: %d\n", a);

    // Syscall2を許可
    /*
    a = syscall(460, pid, 2, 1);
    printf("set syscall-2 to allow: %d\n", a);

    // 禁止リストをログに表示
    a = syscall(462, pid);
    printf("exists: %d\n", a);

    // サンドボックスから取り出す
    a = syscall(461, pid);
    printf("delete profile: %d\n", a);

    // サンドボックスに入っているかかチェック
    a = syscall(462, pid);
    printf("exists: %d\n", a);*/

    wait(NULL);
  }

  return 0;
}
