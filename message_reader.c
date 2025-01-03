#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MSG_SLOT_CHANNEL _IOW(235, 0, unsigned int)

int main(int argc, char const *argv[])
{
    int fd;
    unsigned int channel_id;
    ssize_t len;
    char message[128];

    if (argc != 3)
    {
        perror("Usage: <device_file> <channel_id>");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
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

    len = read(fd, message, 128);
    if (len < 0)
    {
        perror("Error reading from file");
        exit(1);
    }

    close(fd);
    if (write(1, message, len) != len)
    {
        perror("Error writing to stdout");
        exit(1);
    }

    exit(0);
}