
# Message Slot Device Driver

This repository contains an implementation of a Linux kernel module for a message slot device driver. The module enables inter-process communication (IPC) by providing a character device that can be configured with different channels to exchange messages between user-space processes.

## Features
- Supports multiple message slots. 
- Allows configuration of channels for each message slot.
- Provides an efficient way to send and receive messages between processes.
- Implements read and write operations using standard file operations.

## File Overview
- **`message_slot.c`**: The main implementation of the message slot device driver. Contains the core functionality, including initialization, cleanup, and the file operations for the device.

## Prerequisites
- Linux kernel development environment
- Kernel headers for your system
- Basic understanding of Linux kernel modules and device drivers

## Compilation and Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/Mohamedkj18/message-slot-device-driver.git
   cd message-slot-device-driver
2. Compile the kernel module:
   ```bash
   make
3. Load the kernel module:
   ```bash
   sudo insmod message_slot.ko
4. Verify that the device is registered:
   ```bash
   lsmod | grep message_slot
## Usage
1. Create a device file:
   ```bash
   sudo mknod /dev/message_slot c 235 0
2. Open the device file in your user-space program or with tools like **`cat`** or **`echo`**.
3. Use ioctl to set the channel:
   ```bash
   ioctl(fd, MSG_SLOT_CHANNEL, channel_id)
4. Write a message to the device:
   ```bash
   write(fd, message, length)
5. Read a message from the device:
   ```bash
   read(fd, buffer, buffer_size)
## Unloading the Module
1. First remove all device files that has been created:
   ```bash
   sudo rm /dev/device_file_name
2. Remove the module from the kernel:
   ```bash
   sudo rmmod message_slot
3. Cleanup any initialized data structures:
   ```bash
   make clean

Its higمy recomended to run this code on a Virtual Mشchine
