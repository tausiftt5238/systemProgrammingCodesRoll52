/* Prototype module for second mandatory DM510 assignment */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <linux/cdev.h>

#define MAXSIZE 8

static DECLARE_WAIT_QUEUE_HEAD(wq);

//(1) Create a structure for our fake device
struct fake_device{
	char data[MAXSIZE];
	int count;
	struct semaphore sem;
}virtual_device;

static int head;
static int tail;

//(2) To later register our device we need a cdev object and some other variables
struct cdev *mcdev;	//m stands 'my'
int major_number;	//will store our major number- extracted from dev_t using macro
int ret;		//will be used to hold return values of functions;this is because the kernel stack is so small
			//so declaring variables all over the pass in our module functions eats up the stack very fast

static dev_t dev_num; //will hold major number that kernel gives us
		//name--> appears in /proc/devices

#define DEVICE_NAME "sysprogdevice"

//(7)called on device_file open
//	inode reference to the file on disk
//	and contains information about that file
//	struct file represents an abstract open file
int device_open(struct inode *inode, struct file *filp){
	//only allows one process to pen this device by using a semaphore as a mutual exclusion lock
	printk(KERN_INFO "length = %d head = %d tail = %d string = %s\n",virtual_device.count, head, tail, virtual_device.data);
	if(iminor(inode) == 0){
		printk(KERN_ALERT "opened write end\n");
	}
	else if(iminor(inode) == 1){
		printk(KERN_ALERT "opened read end\n");
	}
	/*if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "sysprogdevice: could not lock device during open");
		return -1;
	}*/
	printk(KERN_INFO "sysprogdevice: opened device\n");
	return 0;

}

//(8) called when user wants to get information from the device
ssize_t device_read(struct file* filp, char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//take data from kernel space(device) to user space (process)
	//copy_to_user (destination, source, sizeTo Taransfer)
	printk(KERN_ALERT "sysprogdevice: Reading from device\n");
	int bytes_read = 0;
	wake_up_interruptible(&wq);
	while(bufCount){
		wait_event_interruptible(wq, virtual_device.count != 0);
		
		put_user(virtual_device.data[head], bufSourceData++);
		printk(KERN_INFO "%c",virtual_device.data[head]);
		head = ( head + 1 )% MAXSIZE;
		bufCount--;
		bytes_read++;
		virtual_device.count--;
		
	}
	put_user(0, bufSourceData);
	//ret = copy_to_user(bufSourceData, virtual_device.data, bufCount);
	
	return bytes_read;
}


//(9) called when user wants to send information to the device
ssize_t device_write(struct file* filep, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//send data from user to kernel
	//copy_from_user (dest, source, count)

	printk(KERN_INFO "sysprogdevice: writing to device - %lu\n", bufCount);
	
	int i;
	
	for(i = 0 ; i < bufCount ; i++){
		if(i == 0){
			wake_up_interruptible(&wq);
		}
		wait_event_interruptible(wq, virtual_device.count != MAXSIZE);
		get_user(virtual_device.data[tail], bufSourceData+ i);
		printk(KERN_INFO "%c",virtual_device.data[tail]);
		tail = (tail + 1) %MAXSIZE;
		virtual_device.count++;
	}
	//ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);
	
	return i;
}

//(10) called upon user close
int device_close(struct inode *inode, struct file *filp){
	//by calling up, which is opposite of down for semaphore, we release the mutex that we obtained on open
//this has the effect of allowing other process to use the device now
	//up(&virtual_device.sem);
	printk(KERN_INFO "sysprogdevice:closed device\n");
	return 0;
}

//(6) Tell the kernel which functions to call when user operates on our device file
struct file_operations fops = {
	.owner = THIS_MODULE,		//prevent unloading of this module when operations are in use
	.open = device_open,		//points to the method to call when opening the device
	.release = device_close,	//points to the method to call when closing the device
	.write = device_write,		//points to the method to call when writing the device
	.read = device_read		//points to the method to call when reading the device
};


static int driver_entry(void){
	//(3) REgister our device with the system: a two step process
	//step(1) use dynamic allocation to assign our device
	//a major number--alloc_chrdev_region(dev_t*,uint fminor, uint count, char* name)
	ret = alloc_chrdev_region(&dev_num,0,2,DEVICE_NAME);
	if(ret < 0){
		printk(KERN_ALERT "sysprogdevice: Failed to allocate a device %d\n", major_number);
		return ret; //propagate error
	}
	major_number = MAJOR(dev_num); //extracts the major number
	printk(KERN_INFO "sysprogdevice: Successfully allocated a device %d\n",major_number);
	printk(KERN_INFO "sysprogdevice: \tuse \"mknod /dev/%s c %d 0\" for device file\n",DEVICE_NAME,major_number); //dmesg
	//step(2)
	mcdev= cdev_alloc(); //create our cdev structure, initialized our cdev
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;
	//now that we created cdev, we have to add it to the kernel
	//int cdev_add(struct cdev* dev, dev_t num, unsigned int count)
	ret = cdev_add(mcdev, dev_num, 2); //because we have 2 devices
	if(ret < 0){	//always check errors
		printk(KERN_ALERT "sysprogdevice: unable to add cdev to kernel");
		return ret;
	}

	//(4) Initialize our semaphore
	sema_init(&virtual_device.sem,1);	//initial value of one
	virtual_device.count = 0;
	head = 0;
	tail = 0;
	return 0;
}

static void driver_exit(void){
	//(5) unregister everything in reverse order
	//(a)
	cdev_del(mcdev);

	//(b)
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "sysprogdevice: unloaded module");
}


module_init(driver_entry);
module_exit(driver_exit);

MODULE_AUTHOR( "Tahrima Hashem" );
MODULE_LICENSE( "GPL" );
