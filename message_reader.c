#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    unsigned int channel_id
        ssize_t len;
    char *message;

    if (argc != 3)
    {
        perror("Usage: %s <device_file> <channel_id>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (file == NULL)
    {
        perror("Error opening file\n");
        exit(1);
    }

    channel_id = atoi(argv[2]);

    if (ioctl(fd, MSG_SLOT_CHANNEL, &channel_id) < 0)
    {
        perror("Error setting channel ID\n");
        exit(1);
    }

    len = read(fd, message, strlen(message));
    if (len < 0)
    {
        perror("Error reading from file\n");
        exit(1);
    }

    close(fd);
    if (write(1, message, len) != len)
    {
        perror("Error writing to stdout\n");
        exit(1);
    }
    exit(0);
}