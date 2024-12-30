
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");

#define SUCCESS 0
#define DEVICE_RANGE_NAME "message_slot"
#define BUF_LEN 128
#define DEVICE_FILE_NAME "dev_file"

struct node
{
    unsigned int id;
    char *buffer;
    int length;
    node *next;

} typedef node;

struct channels
{
    node head;
    int length;

} typedef channels;

// data structure for maintaing all device files
static struct channels message_slot_devices[256];