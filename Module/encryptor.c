/**************************************************************
* Class:  CSC-415-02 Fall 2022
* Name: Anudeep Katukojwala
* Student ID: 922404701
* GitHub UserID: anudeepkatukojwala
* Project: Assignment 6 – Device Driver
*
* File: encryptor.c
*
* Description: This program is to build a loadable device driver 
*              which can be loaded and unloaded into and from 
*              the kernel with some functionalities such as 
*              open, release, read, write, and IOCTL. To test
*              the device driver functionality, we are required
*              write a test program to test every functionality
*              of our device driver.
*
**************************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                
#include <linux/uaccess.h>



#define MAJOR_NUM 302
#define MINOR_NUM 0
#define DEVICE_NAME "encryptor"
#define BUFFER_SIZE 500

static int major; //major number assigned to our driver

static int device_open = 0;	//To check if the device is opened or not

static char message[BUFFER_SIZE]; //Kernal space string is stored here

dev_t dev; //Holds the value returned by MKDEV

static struct cdev my_cdev; //struct cdev is the kernel’s internal structure that represents char devices

static int i; //Counter variable for for-loop

static int encryptionKey; //key used for encryption; 




//Open encryptor device driver file if no one else has already opened
static int encryptor_open(struct inode *inode, struct file *file) {

    //check to see if the device is opened to prevent multiple accesses to driver
    if(device_open) {
        return -EBUSY; 
    }

    device_open++; //set the device as opened
    
    printk(KERN_INFO "Encryptor character device opened successfully\n");
    
    return 0;
}



//close the encryptor device driver file and set it to available state
static int encryptor_release(struct inode *inode, struct file *file) {
    
    device_open--;  //set the driver as available to open

    printk(KERN_INFO "Encryptor device driver file is released!\n");

    return 0; 
}



//read from kernel to user space and return the 
//total number of characters read
//It is called when test program reads from device file
static ssize_t encryptor_read(struct file *file, char *buffer, size_t size, loff_t *offset) {
    
    int returnValue = 0;
    int temp; //To temporarily hold read characters value 
    
    //reads from kernel to buffer
    //returns zero if successful or
    //returns number of bytes that could not be copied
    returnValue = copy_to_user(buffer, message, strlen(message));

    //verify if the read is successfull or not
    if (returnValue == 0){ 
        printk(KERN_INFO "Total number of characters read to user space: %ld\n", strlen(message));
        temp = strlen(message);
        return temp; // returns the characters copied
    } else {
        
        printk(KERN_INFO "Failed to read %d characters to user space\n", returnValue);
        return -EFAULT; //To indiccate an unsuccessful read    
    }
}


//It is called when test program writes into device file
static ssize_t encryptor_write(struct file *file,const char *buf, size_t size, loff_t *offset) {
    
    memset(message, 0, BUFFER_SIZE); //clear contents of message
    
    //writes from the users space to kernel space
    if(copy_from_user(message, buf, size)){
        return -EFAULT;
    }
    
    //print statements for tracking and verification purposes
    printk("Successfully written to kernel: %s", message);
    printk(KERN_INFO "Characters written to kernel are: %lu\n", size);
    return size;
} 

//Encrypts and Decrypts based on user input
static long encryptor_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    
    encryptionKey = arg;
    
    switch(cmd) {
        case 0: 
            //encryptionKey value is added to characters of the string
            for(i = 0; (i < BUFFER_SIZE && message[i] != '\0'); i++) {
                message[i] += encryptionKey;
            }
            printk("String after encryption: %s\n",message);
            break;
        case 1:
            //encryptionKey value is subtracted from characters of the string
            for(i = 0; (i < BUFFER_SIZE && message[i] != '\0'); i++) {
                message[i] = message[i] - encryptionKey;
            }
            printk("Decrypted String: %s\n",message);
            break;
        
    }
    
    return 0;
}



//Modern way for File Operations Struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = encryptor_open,
	.release = encryptor_release,
	.read = encryptor_read,
	.write = encryptor_write,
    .unlocked_ioctl = encryptor_ioctl
	
};



//register our character device driver encryptor
int init_encryptor(void) {
    int returnValue = 0;
    
    dev = MKDEV(MAJOR_NUM, MINOR_NUM);

    //register the device    
    returnValue = register_chrdev_region(dev, 1, DEVICE_NAME);
    //check of the registration is successful
    if(returnValue) {
        printk(KERN_INFO "Registration Failed \n");
        return -EINVAL;
    }   


    cdev_init(&my_cdev, &fops);
    // my_cdev = cdev_alloc();   //obtain a standalone cdev structure at runtime            
    // my_cdev->owner = THIS_MODULE;   //set the owner of this char device         
    // my_cdev->ops = &fops;  //set the file operations structure for this char device

        
    returnValue = cdev_add(&my_cdev,dev,1);  //To add a character device to the system

    //check if the addition of character device is successful
    if(returnValue) {               
        printk("Error while adding character device to system\n"); 
        unregister_chrdev_region(dev, 1);                 
        return -EINVAL;
    }  

    major = MAJOR(dev);

    printk("Character Device registered with Major number: %d\n",major);
    return 0;
} 

//Remove the character device encryptor
void remove_encryptor(void){
    dev = MKDEV(MAJOR_NUM, MINOR_NUM);
    //cdev_del(my_cdev);  //remove a char device from the system
    unregister_chrdev(dev, DEVICE_NAME); //unregister 1 char device
    printk(KERN_INFO "Removed encryptor from the system\n");
}

module_init(init_encryptor);
module_exit(remove_encryptor);
MODULE_AUTHOR("Anudeep Katukojwala");
MODULE_LICENSE("Class Project");