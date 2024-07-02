#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
const int N = 1024;
int main(int argc, char* argv[])
{
    int sock, i;
    char buf[N];
    struct sockaddr_in servAddr;
    struct hostent* hp, * gethostbyname();
    if (argc < 4)
    {
        printf(" Ввести IP_сервера порт сообщение\n");
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(" Сокет не открылся.");
        return 1;
    }
    bzero(&servAddr, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &servAddr.sin_addr, hp->h_length);
    if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        perror(" Не удалось соединиться.\n");
        return 1;
    }
    printf(" Клиент готов.\n");

    for (i = 0; i < 5; i++)
    {
        if (send(sock, (const char*)argv[3], strlen(argv[3]), 0) < 0)
        {
            perror(" Не удалось отправить.\n");
            return 1;
        }
        printf(" Сообщение номер %d отправлено.\n", i);
        sleep(atoi(argv[3]));
    }
    close(sock);
    return 0;
}