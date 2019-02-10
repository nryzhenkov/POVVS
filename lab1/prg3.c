#include <stdio.h>
#include <sys/sem.h>
#include <errno.h>

int main(int argc, char* argv[]) 
{
 if (argc < 3)
 {
  printf("Использование: %s <fromFile1> <toFile>\n", argv[0]);
  return -1;
 }
 
 int sem_id = semget(102, 1,  707 | IPC_CREAT);

 if (sem_id == -1)
 {
  printf("Ошибка инициализации семафора %d \n", errno);
  return -1;
 }

 printf("Успешное выполнение\n"); 

 return 0;
 
}