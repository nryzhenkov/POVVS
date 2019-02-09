#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

const int block_size = 64;
int got_sig = 0;

void listen_child(int sig) {
  printf("потомок получил сигнал\n");
  got_sig++;
}

int main(int argc, char* argv[]) {
  int fd[2];
  ssize_t chars;

  if (argc < 3) {
    printf("Использование: %s <fromFile1> <toFile>\n", argv[0]);
    return 0;
  }

  if ( signal(SIGUSR1, listen_child) == SIG_ERR) {
    printf("ошибка в signal\n");
  }

  pipe(fd);
  pid_t pid;
  
  if ( (pid = fork()) == 0 ) {
    printf("создание потомка\n");
    char read_buff[block_size];
    int n_bytes;
    close(fd[1]);
    int file_out;
    
    if ( (file_out = open(argv[2], O_WRONLY | O_CREAT, 0644)) != -1 ) {
      while ( (n_bytes = read(fd[0], read_buff, block_size)) > 0) {
	write(file_out, &read_buff, (ssize_t) n_bytes);
      }
    }
    else
      printf("Невозможно открыть файл %s для записи\n", argv[2]);
    close(fd[0]);
    while (1) {
      if (got_sig > 0)
	break;
    }
    printf("Остановка потомка\n");
    return 0;
  }
  else {
    int file;
    char buff[block_size];

    if ( (file = open(argv[1], O_RDONLY)) == -1 ) {
      fprintf(stderr, "Невозможно прочитать %s", argv[1]);
      return 0;
    }
    close(fd[0]);
    while ((chars = read(file, &buff, block_size)) > 0) {
      write(fd[1], &buff, (ssize_t) chars);
    }
    close(fd[1]);
    kill(pid, SIGUSR1);
    printf("Ждем потомка\n");
    wait(NULL);
  }
}
