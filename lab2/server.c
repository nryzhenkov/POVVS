#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define block_size 1024

int main(int argc, char* argv[])
{

	char buff[block_size];
	int count_chars;
	struct sockaddr *client;
	
	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (sockid < 0)
	{
		printf("Ошибка создания сокета\n");
		return -1;
	}

	int file;

	if ((file = open("prg.out", O_WRONLY | O_CREAT, 0775)) == -1)
	{
		printf("Ошибка создания файл\n");
		return -1;
	}

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(1337);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	if (bind(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("Ошибка связывания сокета с адресом\n");
		return -1;
	}

	printf("Ожидание подключение клиента\n");

	listen(sockid, 1);	

	int chan = accept(sockid, NULL, NULL);
	
	printf("Клиент подключен\n");	

	while ((count_chars = read(chan, buff, block_size)) > 0)
	{
		write(file, buff, count_chars);
	}

	close(file);
	close(chan);
	int pid;
	
	if((pid = fork()) == 0)
	{

		FILE *mf = freopen("lol", "w", stdout);
		execl("prg.out", "./prg.out", NULL);
		fclose(mf);
		return 0;
	}

	printf("Завершение выполнения клиентской программы\n");
	
	int ppid = 1337;
	ppid = waitpid(pid, NULL, 0);
	
	if ((file = open("lol", O_RDONLY)) == -1)
	{
		printf("Ошибка открытия файла с результатом выполнения программы\n");
		close(sockid);
		return -1;
	}
	chan = accept(sockid, NULL, NULL);
	printf("Отправление результат клиенту\n");

	while ((count_chars = read(file, buff, block_size)) > 0)
	{
		write(chan, buff, count_chars);
	}
   
	printf("Результат отправлен \n");

	close(chan);
	close(sockid); 
	close(file);
	remove("lol");
	remove("prg.out");

	printf("Успешное завершение: %s\n", argv[0]);	
	
	return 0;
	
}
