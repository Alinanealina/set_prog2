#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
const int N = 1024;
int rec_msg(int sock)
{
    char buf[N];
    int n;
    bzero(buf, N);
    if ((n = recv(sock, buf, N, 0)) < 0)
    {
        perror(" Плохое получение дочерним процессом.");
        return 0;
    }
    buf[n] = '\0';
    //printf(" Socket для клиента: %d\n", sock);
    //printf(" Длина сообщения: %d\n", n);
    printf(" ID работающего процесса-потомка: %i\n", (int)getpid());
    printf(" Сообщение: %s\n\n", buf);
    return 1;
}

void reaper(int sig)
{
    int status;
    while (wait3(&status, WNOHANG, (struct rusage*)0) >= 0);
}

int main()
{
    int sock_s, sock_c, len, child, i, start, stop;
    char* end;
    struct sockaddr_in servAddr;
    if ((sock_s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(" Сокет не открылся.");
        return 1;
    }
    bzero(&servAddr, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;
    if (bind(sock_s, (const struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        perror(" Сервер не удалось связать.");
        return 1;
    }

    len = sizeof(servAddr);
    if (getsockname(sock_s, (struct sockaddr*)&servAddr, &len))
    {
        perror(" Проблема с getsockname.");
        return 1;
    }
    printf(" Номер порта сервера: %d\n", ntohs(servAddr.sin_port));
    listen(sock_s, 5);

    signal(SIGCHLD, reaper);
    while (1)
    {
        if ((sock_c = accept(sock_s, 0, 0)) < 0)
        {
            perror(" Проблема с сокетом клиента.");
            return 1;
        }

        child = fork();
        if (child < 0)
        {
            perror(" Ошибка при порождении процесса");
            return 1;
        }
        else if (child == 0)
        {
            close(sock_s);
            for (i = 0; i < 5; i++)
            {
                if (!rec_msg(sock_c))
                    return 1;
            }
            close(sock_c);
            return 0;
        }
        else
        {
            close(sock_c);
            printf(" ID процесса-родителя: %i\n\n", (int)getpid());
        }
    }
    return 0;
}