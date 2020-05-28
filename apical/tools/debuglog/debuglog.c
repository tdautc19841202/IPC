#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    char *tty_name = ttyname(STDOUT_FILENO);
    int   tty_fd   = -1;

    if (argc < 2) {
        printf("usage: %s on/off\n", argv[0]);
        return 0;
    }

    printf("tty_name: %s\n", tty_name);

    if (strcmp(argv[1], "on") == 0) {
        tty_fd = open(tty_name, O_RDONLY|O_WRONLY);
    }

    if (strcmp(argv[1], "off") == 0) {
        tty_fd = open("/dev/console", O_RDONLY|O_WRONLY);
    }

    if (tty_fd < 0) {
        printf("failed to open tty fd !\n");
        return 0;
    }

    ioctl(tty_fd, TIOCCONS);
    close(tty_fd);
    return 0;
}

