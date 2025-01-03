#include "message_slot.h"

//================== DEVICE FUNCTIONS ===========================
static int device_open(struct inode *inode,
                       struct file *file)
{
    file->private_data = NULL;
    printk("device opened successfully: major: 235, minor: %d\n", iminor(inode));
    return SUCCESS;
}

//---------------------------------------------------------------
// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read(struct file *file,
                           char __user *buffer,
                           size_t length,
                           loff_t *offset)
{
    struct node *channel_node;
    int i;
    // retreive the channel node address from file private data
    channel_node = (struct node *)(file->private_data);
    if (channel_node == NULL)
    {
        printk("no channel has been set\n");
        return -EINVAL;
    }

    if (channel_node->buffer == NULL)
    {
        printk("no messages were written in this channel;");
        return -EWOULDBLOCK;
    }

    if (length < channel_node->length)
    {
        printk("provided buffer length is too small\n");
        return -ENOSPC;
    }

    for (i = 0; i < channel_node->length; i++)
    {
        if (put_user(channel_node->buffer[i], &buffer[i]) != 0)
        {
            printk("error in put_user after %d bytes\n", i);
            return -EFAULT;
        }
    }

    printk("read successful: %d bytes read from channel ID: %lu\n", channel_node->length, (unsigned long)file->private_data);
    return channel_node->length;
}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write(struct file *file,
                            const char __user *buffer,
                            size_t length,
                            loff_t *offset)
{
    ssize_t i;
    struct node *channel_node;
    char *tmp;

    // retreive the channel node address from file private data
    channel_node = (struct node *)(file->private_data);
    if (!channel_node)
    {
        printk("no channel has been set\n");
        return -EINVAL;
    }
    if (length <= 0 || length > BUF_LEN)
    {
        printk("unsuported message length\n");
        return -EMSGSIZE;
    }
    if (!buffer)
    {
        printk("buffer pointer is NULL\n");
        return -EINVAL;
    }

    // copy message to a temporary buffer to ensure no partial messages is written
    tmp = (char *)kmalloc(length * sizeof(char), GFP_KERNEL);
    if (!tmp)
    {
        printk("memory allocation failed\n");
        return -ENOMEM;
    }
    for (i = 0; i < length; i++)
    {
        if (get_user(tmp[i], &buffer[i]) != 0)
        {
            printk("get_user failed after %ld bytes\n", i);
            kfree(tmp);
            return -EFAULT;
        }
    }
    channel_node->buffer = (char *)kmalloc(length * sizeof(char), GFP_KERNEL);
    if (!channel_node->buffer)
    {
        printk("buffer allocation failed\n");
        kfree(tmp);
        return -ENOMEM;
    }

    channel_node->length = length;

    // copy message to the channel
    for (i = 0; i < length; i++)
    {
        channel_node->buffer[i] = tmp[i];
    }
    kfree(tmp);
    printk("write successful: %zu bytes were written to channel ID: %lu\n", length, (unsigned long)file->private_data);
    return i;
}

//---------------------------------------------------------------
// retreiving the channel node by the provided channel id
// updating the private data attribute in file to be a pointer
// to the channel node
static long device_ioctl(struct file *file,
                         uint cmd,
                         unsigned long param)
{
    int minor, channel_id;
    struct node *channel_node, *prev;

    if (cmd != MSG_SLOT_CHANNEL)
    {
        printk("unsupported command\n");
        return -EINVAL;
    }
    if (param == 0)
    {
        printk("invalid channel ID\n");
        return -EINVAL;
    }

    channel_id = (int)param;
    minor = iminor(file->f_inode);
    channel_node = message_slot_devices[minor].head;
    prev = NULL;
    while (channel_node != NULL)
    {
        if (channel_node->id == channel_id)
        {
            // the channel already exists
            file->private_data = channel_node;
            printk("Channel ID set to %u\n", channel_id);
            return SUCCESS;
        }
        prev = channel_node;
        channel_node = channel_node->next;
    }

    if (channel_node == NULL)
    {
        if (message_slot_devices[minor].length == 1048576)
        {
            // the message slot device file is at maximum capacity 2^20
            printk("invalid channel ID\n");
            return -EINVAL;
        }
        channel_node = (struct node *)kmalloc(sizeof(struct node), GFP_KERNEL);
        if (channel_node == NULL)
        {
            printk("memory allocation failed\n");
            return -EFAULT;
        }
        if (prev == NULL)
        {
            message_slot_devices[minor].head = channel_node;
        }
        else
        {
            prev->next = channel_node;
        }

        channel_node->id = param;
        ;
        channel_node->next = NULL;
        channel_node->buffer = NULL;
        channel_node->length = 0;
        message_slot_devices[minor].length += 1;
    }

    // save the channel node in file->private_data
    file->private_data = channel_node;

    printk("Channel ID set to %u\n", channel_id);
    return SUCCESS;
}
//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .unlocked_ioctl = device_ioctl};

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init init(void)
{
    int major, i;

    // Register driver capabilities. Obtain major num
    major = register_chrdev(235, DEVICE_RANGE_NAME, &Fops);

    // Negative values signify an error
    if (major < 0)
    {
        printk("%s registraion failed for  %d\n", DEVICE_FILE_NAME, major);
        return -EFAULT;
    }

    for (i = 0; i < 256; i++)
    {
        message_slot_devices[i].head = NULL;
        message_slot_devices[i].length = 0;
    }

    printk("Registeration is successful.\n");
    return 0;
}

//---------------------------------------------------------------
static void __exit cleanup(void)
{
    struct node *p, *q;
    int i;

    for (i = 0; i < 256; i++)
    {
        p = message_slot_devices[i].head;
        while (p != NULL)
        {
            q = p;
            p = p->next;
            kfree(q);
        }
    }
    unregister_chrdev(235, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(init);
module_exit(cleanup);

//========================= END OF FILE =========================
