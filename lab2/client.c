#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>
#include <errno.h>

#define block_size 1024

int main(int argc, char* argv[])
{

	char buff[block_size];
	char buff2[block_size];
	int count_chars;

	if (argc < 2)
	{
		printf("Не указан второй параметр: \"имя программы для выполнения на сервере\"\n");
		return -1;
	}

	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (sockid < 0)
	{
		printf("Ошибка создания сокета\n");
		return -1;
	}

	int file;

	if ((file = open(argv[1], O_RDONLY)) == -1)
	{
		printf("Ошибка открытия файла\n");
		return -1;
	}

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(1337);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	
	if (connect(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("Ошибка соединения\n");
		return -1;
	}
	
	while ((count_chars = read(file, buff, block_size)) > 0)
	{
		write(sockid, &buff, count_chars);
	}
	
	printf("Файл отправлен\n");
	close(file);
 	close(sockid);

	if ((file = open("result", O_WRONLY | O_CREAT, 0644)) == -1)
	{
		printf("Ошибка создания файла результата выполнения программы\n");
		close(sockid);
		return -1;
	}
	
	sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("Ошибка соединения\n");
		return -1;
	}
	printf("Ожидание файла с результатом\n");

	
	while ( (count_chars = read(sockid, buff2, block_size)) >0)
	{

			write(file, buff2, count_chars);
	}	
	
	printf("Результат сохранен в фале result\n");

	close(file);
	close(sockid);

	printf("Успешное завершение: %s\n", argv[0]);

	return 0;
}
