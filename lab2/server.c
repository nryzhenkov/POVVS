#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	int a = 228;


	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (sockid < 0)
	{
		printf("Ошибка создания сокета\n");
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
	
	read(chan, &a, sizeof(a));
	printf("%d\n", a);
	close(sockid);

	return 0;
}
