#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SLOT_CHANNEL _IOW(235, 0, unsigned int)

int main(int argc, char *argv[])
{
    int fd;
    unsigned int channel_id;

    if (argc != 4)
    {
        perror("Usage: <device_file> <channel_id> <message>");
        exit(1);
    }

    fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(1);
    }

    channel_id = atoi(argv[2]);

    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0)
    {
        perror("Error setting channel ID");
        exit(1);
    }
    if (write(fd, argv[3], strlen(argv[3])) != strlen(argv[3]))
    {
        perror("Error writing to file");
        exit(1);
    }

    close(fd);
    exit(0);
}