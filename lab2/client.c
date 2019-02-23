#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define block_size 1024

int main(int argc, char* argv[])
{

	char buff[block_size];
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
	inet_aton("127.0.0.1", &addr.sin_addr);	
	
	if (connect(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("Ошибка соединения\n");
		return -1;
	}
	
	while ((count_chars = read(file, buff, block_size)) > 0)
	{
		write(sockid, &buff, count_chars);
	}

	close(sockid);

	return 0;
}
