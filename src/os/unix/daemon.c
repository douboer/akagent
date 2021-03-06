#include "os.h"

#ifdef __USE_UNIX__

int use_daemon(void)
{
    int  fd;

    switch (fork()) {
        case -1:
            return -EACCES;

        case 0:
            break;

        default:
            exit(0);
    }

    if (setsid() == -1) {
        return -EACCES;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        return -EACCES;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        return -EACCES;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        return -EACCES;
    }

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
            return -EACCES;
        }
    }

    return 0;
}

#endif
