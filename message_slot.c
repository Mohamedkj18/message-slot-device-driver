#include "message_slot.h"

//================== DEVICE FUNCTIONS ===========================
static int device_open(struct inode *inode,
                       struct file *file)
{
    printk("device opened successfully: major: %d, minor: %d\n", imajor(inode), iminor(inode));
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
    node *channel_node;

    // retreive the channel node address from file private data
    channel_node = (node *)(file->private_data);
    if (!channel_node)
    {
        printk("no channel has been set\n");
        return -EINVAL;
    }

    if (channel_node->buffer == NULL)
    {
        printk("no messages were written in this channel;") return -EWOULDBLOCK;
    }

    if (length < channel_node->length)
    {
        printk("provided buffer length is too small\n");
        return -ENOSPC;
    }

    if (copy_to_user(buffer, &channel_node->buffer, channel_node->length) < 0)
    {
        printk("error reading the message\n");
        return -EFAULT;
    }

    printk("read successful: %zu bytes read from channel ID: %lu\n", length, (unsigned long)file->private_data);
    return length;
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
    node *channel_node;
    char *tmp;

    // retreive the channel node address from file private data
    channel_node = (node *)(file->private_data);
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

    // copy message to a temp to ensure no partial messages
    tmp = char[length];
    if (copy_from_user(tmp, &buffer, lenght) > 0)
    {
        printk("error writing the message\n");
        return -EFAULT;
    }

    channel_node->buffer = char[length];
    channel_node->length = length;

    // copy message to the channel
    for (i = 0; i < length; i++)
    {
        channel_node->buffer[i] = tmp[i];
    }

    printk("write successful: %zu bytes were written to channel ID: %lu\n", length, (unsigned long)file->private_data);
    return i;
}

//---------------------------------------------------------------
// retreiving the channel node by the provided channel id
// updating the private data attribute in file to be a pointer
// to the channel node
static int device_ioctl(struct file *file,
                        uint cmd,
                        unsigned long param)
{
    uint channel_id, minor;
    node *channel_node, prev;

    if (cmd != MSG_SLOT_CHANNEL)
    {
        printk("unsupported command");
        return -EINVAL;
    }
    if (param = 0)
    {
        printk("invalid channel ID\n");
        return -EINVAL;
    }

    channel_id = (unsigned int)param;
    minor = iminor(file->f_inode);
    channel_node = message_slot_devices[minor]->head;
    while (channel_node != NULL)
    {
        if (channel_node->id == channel_id)
        {
            break;
        }
        prev = channel_node;
        channel_node = channel_node->next;
    }
    if (channel_node == NULL)
    {
        if (message_slot_devices[minor]->length == pow(2, 20))
        {
            printk("invalid channel ID\n");
            return -EINVAL;
        }
        prev->next = kalloc(sizeof(*node));
        channel_node = prev->next;
        channel_node->id = channel_id;
        message_slot_devices[minor]->lenght += 1;
    }

    kfree(prev);
    // save the channel ID in file->private_data
    file->private_data = (void *)channel_node;

    printk("Channel ID set to %u, channel node address: %d\n", channel_id, &channel_node);
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
    if (major != 235)
    {
        printk("%s registraion failed for  %d\n", DEVICE_FILE_NAME, major);
        return -EFAULT;
    }

    for (i = 0; i < 256; i++)
    {
        message_slot_devices[i]->head = NULL;
        message_slot_devices[i]->length = 0;
    }

    printk("Registeration is successful. ");
    return 0;
}

//---------------------------------------------------------------
static void __exit cleanup(void)
{
    node *p, *q;
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
