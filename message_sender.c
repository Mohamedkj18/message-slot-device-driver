#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    unsigned int channel_id;

    if (argc != 4)
    {
        perror("Usage: %s <device_file> <channel_id> <message>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
        perror("Error opening file\n");
        eit(1);
    }

    channel_id = atoi(argv[2]);

    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0)
    {
        perror("Error setting channel ID\n");
        exit(1);
    }
    if (write(fd, argv[3], strlen(argv[3])) < 0)
    {
        perror("Error writing to file\n");
        exit(1);
    }

    close(fd);
    exit(0);
}