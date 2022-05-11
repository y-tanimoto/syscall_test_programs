#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define LOCK   -1
#define UNLOCK 1
#define WAIT   0

union semun {
  int             val;
  struct semid_ds *buf;
  unsigned short  *array;
} ctl_arg;

int sb_sem_init(int sem_id) {
  ctl_arg.val = 0;
  if (semctl(sem_id, 0, SETVAL, ctl_arg) < 0) {
    return -1;
  }
  return 0;
}

int sb_semop(int sem_id, int op) {
  struct sembuf sops[1];

  sops[0].sem_num = 0;
  sops[0].sem_op = op;
  sops[0].sem_flg = 0;

  if (semop(sem_id, sops, 1) < 0) {
    return -1;
  }

  return 0;
}

int main() {
  int ppid = getpid();

  // セマフォを用意
  int sem_id = semget(IPC_PRIVATE, 1, 0600);
  if (sem_id < 0) {
    perror("semget");
    return 1;
  }

  if (sb_sem_init(sem_id) < 0) {
    perror("sb_sem_init");
    return 1;
  }

  // 子プロセスを生成
  int pid = fork();

  // 子プロセス側の処理
  if (pid == 0) {
    // セマフォのアンロックを待機
    sb_semop(sem_id, LOCK);
    printf("child process start.\n");
    
    // syscall451: 実効不可
    int a = 10, b;
    b = syscall(451);
    printf("syscall-451 returned %d\n", b);

    // syscall452: 実行可能
    b = syscall(452);
    printf("syscall-452 returned %d\n", b);

    // syscall453: 実効可能
    b = syscall(453, &a);
    printf("syscall-452 returned %d\n", b);
    
    return 0;
  }

  // 親プロセス側の処理
  printf("ppid=%d, pid=%d\n", ppid, pid);
  int a;

  // 子スレッドをサンドボックスに入れる
  a = syscall(459, pid);
  printf("register: %d\n", a);

  // Syscall1と2を禁止
  a = syscall(460, pid, 451, 0);
  printf("set syscall-451 to forbidden: %d\n", a);
  a = syscall(460, pid, 452, 0);
  printf("set syscall-452 to forbidden: %d\n", a);

  // サンドボックスに入っているかチェック＆禁止リストをログに表示
  a = syscall(462, pid);
  printf("exists: %d\n", a);

  // Syscall2を許可
  a = syscall(460, pid, 452, 1);
  printf("set syscall-452 to allow: %d\n", a);

  a = syscall(460, pid, 453, 1);
  printf("set syscall-453 to allow: %d\n", a);

  // 禁止リストをログに表示
  a = syscall(462, pid);
  printf("exists: %d\n", a);

  // セマフォをアンロック（子プロセスの処理が始まる）
  printf("parent process finish.\n");
  sb_semop(sem_id, UNLOCK);

  // 子プロセスの終了を待機
  wait(NULL);

  // セマフォを解放
  if (semctl(sem_id, 0, IPC_RMID, ctl_arg) < 0) {
    perror("semctl");
    return 1;
  }

  // サンドボックスから取り出す
  a = syscall(461, pid);
  printf("delete profile: %d\n", a);

  // サンドボックスに入っているかかチェック
  a = syscall(462, pid);
  printf("exists: %d\n", a);

  return 0;
}
