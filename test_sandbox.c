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

int child(int sem_id) {
  // セマフォのアンロックを待機
  sb_semop(sem_id, LOCK);
  printf("pid %d: child process start.\n", getpid());
    
  // syscall451: 実効不可
  int a = 10, b;
  b = syscall(451);
  printf("pid %d: syscall-451 returned %d\n", getpid(), b);

  // syscall452: 実行可能
  b = syscall(452);
  printf("pid %d: syscall-452 returned %d\n", getpid(), b);

  // syscall453: 実効可能
  b = syscall(453, &a);
  printf("pid %d: syscall-452 returned %d\n", getpid(), b);
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
    child(sem_id);

    // セマフォを用意
    int sem_id2 = semget(IPC_PRIVATE, 1, 0600);
    if (sem_id2 < 0) {
      perror("semget");
      return 1;
    }
    if (sb_sem_init(sem_id2) < 0) {
      perror("sb_sem_init");
      return 1;
    }

    // 子プロセスを生成
    int pid2 = fork();
    if (pid2 == 0) {
      child(sem_id2);
      
      sb_semop(sem_id2, UNLOCK);

      // 親プロセスが452を禁止するまで待つ
      sb_semop(sem_id2, LOCK);

      // syscall452: 実行不可
      int b;
      b = syscall(452);
      printf("pid %d: syscall-452 returned %d\n", getpid(), b);
  
      return 0;
    }

    printf("pid %d: child pid=%d\n", getpid(), pid2);
    // セマフォをアンロック（子プロセスの処理が始まる）
    sb_semop(sem_id2, UNLOCK);

    // 子プロセスの処理（sb_semop）を待つ
    sb_semop(sem_id2, LOCK);

    // 子プロセス2に対し452を禁止
    int a;
    a = syscall(460, pid2, 452, 0);
    printf("pid %d: set syscall-452 to forbidden: %d\n", getpid(), a);

    // 子プロセス2を再開
    sb_semop(sem_id2, UNLOCK);

    wait(NULL);

    // セマフォを解放
    if (semctl(sem_id2, 0, IPC_RMID, ctl_arg) < 0) {
      perror("semctl");
      return 1;
    }
    
    return 0;
  }

  // 親プロセス側の処理
  printf("pid %d: child pid=%d\n", getpid(), pid);
  int a;

  // 子スレッドをサンドボックスに入れる
  a = syscall(459, pid);
  printf("pid %d: register: %d\n", getpid(), a);

  // Syscall1と2を禁止
  a = syscall(460, pid, 451, 0);
  printf("pid %d: set syscall-451 to forbidden: %d\n", getpid(), a);
  a = syscall(460, pid, 452, 0);
  printf("pid %d: set syscall-452 to forbidden: %d\n", getpid(), a);

  // サンドボックスに入っているかチェック＆禁止リストをログに表示
  a = syscall(462, pid);
  printf("pid %d: exists: %d\n", getpid(), a);

  // Syscall2を許可
  a = syscall(460, pid, 452, 1);
  printf("pid %d: set syscall-452 to allow: %d\n", getpid(), a);

  a = syscall(460, pid, 453, 1);
  printf("pid %d: set syscall-453 to allow: %d\n", getpid(), a);

  // 禁止リストをログに表示
  a = syscall(462, pid);
  printf("pid %d: exists: %d\n", getpid(), a);

  // セマフォをアンロック（子プロセスの処理が始まる）
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
  printf("pid %d: delete profile: %d\n", getpid(), a);

  // サンドボックスに入っているかかチェック
  a = syscall(462, pid);
  printf("pid %d: exists: %d\n", getpid(), a);

  return 0;
}
