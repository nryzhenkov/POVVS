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

	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (sockid < 0)
	{
		printf("Ошибка создания сокета\n");
		return -1;
	}

	int file;

	if ((file = open("prg.out", O_WRONLY | O_CREAT, 0744)) == -1)
	{
		printf("Ошибка создания файл\n");
		return -1;
	}

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(1337);
	inet_aton("127.0.0.1", &addr.sin_addr);	
	
	if (bind(sockid, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("Ошибка связывания сокета с адресом\n");
		return -1;
	}

	listen(sockid, 1);	

	int chan = accept(sockid, NULL, NULL);

	printf("Client подключен\n");	
	
	while ((count_chars = read(chan, buff, block_size)) > 0)
	{
		write(file, buff, count_chars);
	}
	
	printf("End\n");
	close(sockid);

	return 0;
}
