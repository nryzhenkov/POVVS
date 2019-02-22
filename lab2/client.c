#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char* argv[])
{

	if (argc < 2)
	{
		printf("Не указан второй параметр: \"имя программы для выполнения на сервере\"\n");
		return -1;
	}

	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	if (sockid < 0)
	{
		printf("Ошибка создания сокета");
		return -1;
	}

	return 0;
}
