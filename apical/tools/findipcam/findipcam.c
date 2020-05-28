#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#define FACTORY_TEST_PORT  8313

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr = {};
    int    socklen  = sizeof(struct sockaddr_in);
    int    sock     = -1;
    char   msg[256] = "ipcam?";
    char   ip [16 ] = "192.168.1.1";
    int    broadcast= 0;
    int    timeout  = 0;
    int    opt, ret;
    char  *str;

    if (argc >= 2) {
        strncpy(ip, argv[1], sizeof(ip));
    }
    if (argc >= 3) {
        timeout = atoi(argv[2]);
    }
    if (timeout == 0) {
        timeout = 100;
    }
    if ((str = strstr(ip, ".255")) && str + 4 == ip + strlen(ip)) {
        broadcast = 1;
    }
    printf("ip: %s, broadcast: %d, timeout: %d\n", ip, broadcast, timeout);

    // wsa startup
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa)) {
         printf("init winsock failed !");
         return 0;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("failed to open factory test socket !\n");
        return 0;
    }

    opt = timeout  ; setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO , (char*)&opt, sizeof(int));
    opt = broadcast; setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(int));

    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port        = htons(FACTORY_TEST_PORT);
    do {
        printf("find ipcam on %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port)); fflush(stdout);
        ret = sendto  (sock, (void*)msg, strlen(msg), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        do {
            ret = recvfrom(sock, (void*)msg, sizeof(msg), 0, (struct sockaddr*)&servaddr, &socklen);
            if (ret > 0) {
                msg[ret < sizeof(msg) ? ret : sizeof(msg) - 1] = '\0';
                printf("recv msg from %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
                printf("msg: %s\n", msg);
                fflush(stdout);
            }
        } while (ret > 0);
    } while (1 && servaddr.sin_addr.S_un.S_un_b.s_b4--);
    closesocket(sock);

    // wsa cleanup
    WSACleanup();
    return 0;
}


