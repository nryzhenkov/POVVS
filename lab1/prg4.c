#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>

#define block_size 64


int main(int argc, char* argv[]) 
{

 int snd_id;
 struct send {long mtype; char text[block_size]; int size;} snd;
 int file_ds;
 pid_t pid;
 ssize_t chars;

 if (argc < 3)
 {
  printf("Использование: %s <fromFile1> <toFile>\n", argv[0]);
  return -1;
 }
 
 snd_id = msgget(100, 0600 | IPC_CREAT);

 if (snd_id == -1)
 {
  printf("Ошибка инициализации очереди сообщений %d \n", errno);
  return -1;
 }

 if ((pid = fork()) == 0)
 {
  printf("Создание потомка с pid %d \n", getpid());
  
  if ((file_ds = open(argv[2], O_WRONLY | O_CREAT, 0644)) == -1)
  {
   printf("Ошибка создания файла %d \n", errno);
   return -1;
  }
  
  while (1)
  {
   msgrcv(snd_id, &snd, block_size * sizeof(char) + sizeof(int), 0, 0);
   if (snd.size == 0)
   {
    printf("Уничтожение потомка с pid %d \n", getpid());
    return 0;    
   }
   write(file_ds, snd.text, snd.size);
  }   
 }
 else
 {
  if ((file_ds = open(argv[1], O_RDONLY)) == -1)
  {
   printf("Ошибка открытия файла %d \n", errno);
   return -1;
  }
  
  while((snd.size = read(file_ds, snd.text, block_size)) > 0)
  {
   msgsnd(snd_id, &snd, block_size * sizeof(char) + sizeof(int), 0);
  }

  snd.size = 0;
  msgsnd(snd_id, &snd, block_size * sizeof(char) + sizeof(int), 0);

 }
 
 wait(NULL);
 printf("Успешное выполнение\n"); 

 return 0;
 
}