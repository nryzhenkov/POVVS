#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#define block_size 64

int main(int argc, char* argv[]) 
{
 int file_ds;
 pid_t pid;
 struct sembuf sb, sb2;
 //char char_buff[block_size];
 ssize_t chars;
 char *char_shm;

 if (argc < 3)
 {
  printf("Использование: %s <fromFile1> <toFile>\n", argv[0]);
  return -1;
 }
 
 int sem_id = semget(106, 2,  0666 | IPC_CREAT);

 if (sem_id == -1)
 {
  printf("Ошибка инициализации семафора %d \n", errno);
  return -1;
 }

 int shm_id = shmget(101, (block_size + 2) * sizeof(int), 0666 | IPC_CREAT);

 if (shm_id == -1)
 {
  printf("Ошибка инициализации общей памяти %d \n", errno);
  return -1;
 }

 if ((pid = fork()) == 0)
 {
  printf("Создание потомка с pid %d \n", getpid());
  
  sb.sem_num = 0;
  sb.sem_op  = -1;
  sb.sem_flg = 0;

  sb2.sem_num = 1;
  sb2.sem_op = 1;
  sb2.sem_flg = 0;

  char_shm = shmat(shm_id, 0, 0);

  if ((file_ds = open(argv[2], O_WRONLY | O_CREAT, 0644)) == -1)
  {
   printf("Ошибка создания файла %d \n", errno);
   return -1;
  }

  semop(sem_id, &sb, 1);
  
  while ((chars = write(file_ds, char_shm, char_shm[block_size + 1])) > 0)
  {

   semop(sem_id, &sb2, 1);
   printf("test \n");
   //semop(sem_id, &sb,  1);
   if (char_shm[block_size] == 0)
   {
    shmdt(char_shm);
    return 0;    
   }
  } 

  
  

  
 }
 else
 {
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = 0;

  sb2.sem_num = 1;
  sb2.sem_op = -1;
  sb2.sem_flg = 0;
  
  char_shm = shmat(shm_id, 0, 0);
 
  if ((file_ds = open(argv[1], O_RDONLY)) == -1)
  {
   printf("Ошибка открытия файла %d \n", errno);
   return -1;
  }
  
  while((chars = read(file_ds, char_shm, block_size)) > 0)
  {
   char_shm[block_size + 1] = chars; 
   char_shm[block_size] = 1;
   semop(sem_id, &sb, 1);
   semop(sem_id, &sb2, 1);
  }

  char_shm[block_size] = 0;
  shmdt(char_shm);

 }
 
 wait(NULL);
 printf("Успешное выполнение\n"); 

 return 0;
 
}