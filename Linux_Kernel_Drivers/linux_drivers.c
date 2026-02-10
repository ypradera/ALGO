/*
 * Linux Kernel & Driver Development - Complete Guide for Embedded Engineers
 *
 * ============================================================================
 *                    LINUX KERNEL ARCHITECTURE OVERVIEW
 * ============================================================================
 *
 * The Linux kernel is a monolithic kernel with modular capabilities.
 *
 * KERNEL SPACE vs USER SPACE:
 * +-----------------------------------------------------------------+
 * |                      USER SPACE                                  |
 * |  +------------+  +------------+  +------------+  +------------+ |
 * |  |   App 1    |  |   App 2    |  |   Shell    |  |  Daemon    | |
 * |  +------------+  +------------+  +------------+  +------------+ |
 * |                           |                                      |
 * |                    System Calls (syscalls)                       |
 * +-----------------------------------------------------------------+
 * |                      KERNEL SPACE                                |
 * |  +----------------------------------------------------------+   |
 * |  |              Virtual File System (VFS)                    |   |
 * |  +----------------------------------------------------------+   |
 * |  +-------------+  +-------------+  +-------------+              |
 * |  | Char Drivers|  |Block Drivers|  | Net Drivers |              |
 * |  +-------------+  +-------------+  +-------------+              |
 * |  +----------------------------------------------------------+   |
 * |  |           Hardware Abstraction Layer (HAL)                |   |
 * |  +----------------------------------------------------------+   |
 * +-----------------------------------------------------------------+
 * |                        HARDWARE                                  |
 * |    CPU    Memory    GPIO    I2C    SPI    UART    Ethernet      |
 * +-----------------------------------------------------------------+
 *
 * KEY CONCEPTS:
 * 1. Kernel runs in privileged mode (Ring 0 on x86)
 * 2. User apps run in unprivileged mode (Ring 3)
 * 3. Communication via syscalls, ioctl, sysfs, procfs
 * 4. Drivers are kernel modules (.ko files)
 *
 * ============================================================================
 *                         DRIVER TYPES
 * ============================================================================
 *
 * 1. CHARACTER DEVICES (char drivers)
 *    - Accessed as stream of bytes (like a file)
 *    - Examples: serial ports, keyboards, sensors
 *    - Device files: /dev/ttyS0, /dev/input/event0
 *
 * 2. BLOCK DEVICES
 *    - Accessed in fixed-size blocks
 *    - Support random access and buffering
 *    - Examples: hard drives, SSDs, SD cards
 *    - Device files: /dev/sda, /dev/mmcblk0
 *
 * 3. NETWORK DEVICES
 *    - No device file in /dev
 *    - Accessed via socket API
 *    - Examples: eth0, wlan0, can0
 *
 * ============================================================================
 *                    DEVELOPMENT ENVIRONMENT SETUP
 * ============================================================================
 *
 * PREREQUISITES:
 *
 * # Install kernel headers and build tools (Ubuntu/Debian)
 * sudo apt-get update
 * sudo apt-get install build-essential linux-headers-$(uname -r)
 * sudo apt-get install libncurses-dev flex bison libssl-dev
 *
 * # For cross-compilation (ARM embedded)
 * sudo apt-get install gcc-arm-linux-gnueabihf
 * sudo apt-get install gcc-aarch64-linux-gnu
 *
 * KERNEL SOURCE:
 *
 * # Download kernel source
 * git clone https://github.com/torvalds/linux.git
 * cd linux
 * git checkout v6.1  # or your target version
 *
 * # Configure kernel
 * make menuconfig    # Interactive menu
 * make defconfig     # Default config for your arch
 *
 * # Build kernel (native)
 * make -j$(nproc)
 *
 * # Cross-compile for ARM
 * make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
 *
 * ============================================================================
 */

#include <linux/module.h>       /* Required for all modules */
#include <linux/kernel.h>       /* printk(), KERN_INFO */
#include <linux/init.h>         /* __init, __exit macros */
#include <linux/fs.h>           /* file_operations, register_chrdev */
#include <linux/cdev.h>         /* cdev_init, cdev_add */
#include <linux/device.h>       /* device_create, class_create */
#include <linux/uaccess.h>      /* copy_to_user, copy_from_user */
#include <linux/slab.h>         /* kmalloc, kfree */
#include <linux/mutex.h>        /* mutex_lock, mutex_unlock */
#include <linux/spinlock.h>     /* spin_lock, spin_unlock */
#include <linux/interrupt.h>    /* request_irq, free_irq */
#include <linux/gpio.h>         /* gpio_request, gpio_direction_input */
#include <linux/platform_device.h>  /* platform_driver */
#include <linux/of.h>           /* Device Tree support */
#include <linux/of_device.h>    /* of_device_id */
#include <linux/i2c.h>          /* I2C subsystem */
#include <linux/spi/spi.h>      /* SPI subsystem */
#include <linux/ioctl.h>        /* ioctl commands */
#include <linux/wait.h>         /* wait_queue */
#include <linux/poll.h>         /* poll support */
#include <linux/workqueue.h>    /* work_struct */
#include <linux/kthread.h>      /* kernel threads */
#include <linux/delay.h>        /* msleep, usleep_range */
#include <linux/firmware.h>     /* request_firmware */
#include <linux/dma-mapping.h>  /* DMA support */

/*
 * ============================================================================
 *              EXAMPLE 1: MINIMAL "HELLO WORLD" MODULE
 * ============================================================================
 *
 * This is the simplest possible kernel module.
 *
 * BUILDING:
 * ---------
 * Create a Makefile:
 *
 * obj-m += hello.o
 *
 * all:
 *     make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 *
 * clean:
 *     make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
 *
 * LOADING/UNLOADING:
 * ------------------
 * sudo insmod hello.ko      # Load module
 * sudo rmmod hello          # Unload module
 * dmesg | tail              # View kernel messages
 * lsmod | grep hello        # Check if loaded
 *
 * MODULE INFO:
 * ------------
 * modinfo hello.ko          # Show module information
 */

/* Module metadata - ALWAYS include these */
MODULE_LICENSE("GPL");              /* License: GPL, MIT, Proprietary */
MODULE_AUTHOR("Your Name");         /* Author information */
MODULE_DESCRIPTION("A simple Linux driver example");
MODULE_VERSION("1.0");

/*
 * __init - This function runs once when module is loaded
 *
 * The __init macro tells kernel this code can be discarded after init
 * to free memory. This is important for embedded systems!
 */
static int __init hello_init(void)
{
    /*
     * printk() is the kernel's printf()
     *
     * Log levels (from most to least severe):
     * KERN_EMERG   - System is unusable
     * KERN_ALERT   - Action must be taken immediately
     * KERN_CRIT    - Critical conditions
     * KERN_ERR     - Error conditions
     * KERN_WARNING - Warning conditions
     * KERN_NOTICE  - Normal but significant
     * KERN_INFO    - Informational
     * KERN_DEBUG   - Debug messages
     *
     * Modern alternative: pr_info(), pr_err(), pr_debug(), etc.
     */
    pr_info("Hello, Linux Kernel! Module loaded.\n");

    /* Return 0 on success, negative error code on failure */
    return 0;
}

/*
 * __exit - This function runs when module is unloaded
 *
 * The __exit macro marks this code as unloadable.
 * For built-in drivers (not modules), this is never called.
 */
static void __exit hello_exit(void)
{
    pr_info("Goodbye, Linux Kernel! Module unloaded.\n");
}

/* Register init and exit functions */
module_init(hello_init);
module_exit(hello_exit);


/*
 * ============================================================================
 *              EXAMPLE 2: CHARACTER DEVICE DRIVER
 * ============================================================================
 *
 * Character devices are the most common driver type.
 * They provide file-like access to hardware.
 *
 * USER SPACE INTERACTION:
 * -----------------------
 * # After loading the module:
 * echo "test" > /dev/mychardev    # Write to device
 * cat /dev/mychardev              # Read from device
 *
 * C program example:
 * int fd = open("/dev/mychardev", O_RDWR);
 * write(fd, "hello", 5);
 * read(fd, buffer, sizeof(buffer));
 * close(fd);
 */

#define DEVICE_NAME "mychardev"
#define CLASS_NAME  "mychar"
#define BUFFER_SIZE 1024

/* Device state structure - encapsulate all driver data */
struct mychar_device {
    dev_t dev_num;              /* Device number (major:minor) */
    struct cdev cdev;           /* Character device structure */
    struct class *class;        /* Device class for /sys/class */
    struct device *device;      /* Device structure */
    struct mutex lock;          /* Mutex for synchronization */
    char buffer[BUFFER_SIZE];   /* Data buffer */
    size_t buffer_size;         /* Current data size */
    int open_count;             /* Track number of opens */
};

static struct mychar_device *mydev;

/*
 * OPEN - Called when user opens the device file
 *
 * @inode: Information about the file on disk
 * @file:  File structure representing the open file
 *
 * Common tasks:
 * - Allocate resources
 * - Initialize hardware
 * - Check permissions
 * - Store private data in file->private_data
 */
static int mychar_open(struct inode *inode, struct file *file)
{
    /* Get device structure from cdev */
    struct mychar_device *dev = container_of(inode->i_cdev,
                                             struct mychar_device, cdev);

    /* Store in private_data for other operations */
    file->private_data = dev;

    /* Lock to safely modify open_count */
    mutex_lock(&dev->lock);
    dev->open_count++;
    pr_info("%s: Device opened %d time(s)\n", DEVICE_NAME, dev->open_count);
    mutex_unlock(&dev->lock);

    /*
     * Increment module reference count
     * Prevents module unload while device is open
     */
    try_module_get(THIS_MODULE);

    return 0;  /* Success */
}

/*
 * RELEASE - Called when user closes the device file
 *
 * Common tasks:
 * - Free resources allocated in open
 * - Flush buffers
 * - Shutdown hardware if last close
 */
static int mychar_release(struct inode *inode, struct file *file)
{
    struct mychar_device *dev = file->private_data;

    mutex_lock(&dev->lock);
    dev->open_count--;
    pr_info("%s: Device closed. %d opener(s) remaining\n",
            DEVICE_NAME, dev->open_count);
    mutex_unlock(&dev->lock);

    /* Decrement module reference count */
    module_put(THIS_MODULE);

    return 0;
}

/*
 * READ - Called when user reads from the device
 *
 * @file:   File structure
 * @buf:    User-space buffer to write data to
 * @count:  Number of bytes requested
 * @offset: Current position in file
 *
 * IMPORTANT: Must use copy_to_user() for user-space memory access!
 * Direct pointer access would cause a kernel panic.
 *
 * Returns: Number of bytes read, or negative error code
 */
static ssize_t mychar_read(struct file *file, char __user *buf,
                           size_t count, loff_t *offset)
{
    struct mychar_device *dev = file->private_data;
    ssize_t bytes_read = 0;

    mutex_lock(&dev->lock);

    /* Check if offset is beyond buffer */
    if (*offset >= dev->buffer_size) {
        mutex_unlock(&dev->lock);
        return 0;  /* EOF */
    }

    /* Calculate bytes to read */
    bytes_read = min(count, dev->buffer_size - (size_t)*offset);

    /*
     * copy_to_user(to, from, size)
     * Returns: Number of bytes that COULD NOT be copied (0 = success)
     *
     * WHY NOT memcpy()?
     * - User memory might be paged out (swapped to disk)
     * - User might pass invalid pointer (security risk)
     * - Kernel must validate access before copying
     */
    if (copy_to_user(buf, dev->buffer + *offset, bytes_read)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;  /* Bad address */
    }

    *offset += bytes_read;

    pr_info("%s: Read %zd bytes from offset %lld\n",
            DEVICE_NAME, bytes_read, *offset - bytes_read);

    mutex_unlock(&dev->lock);

    return bytes_read;
}

/*
 * WRITE - Called when user writes to the device
 *
 * IMPORTANT: Must use copy_from_user() for user-space memory access!
 *
 * Returns: Number of bytes written, or negative error code
 */
static ssize_t mychar_write(struct file *file, const char __user *buf,
                            size_t count, loff_t *offset)
{
    struct mychar_device *dev = file->private_data;
    ssize_t bytes_written = 0;

    mutex_lock(&dev->lock);

    /* Limit write to buffer size */
    bytes_written = min(count, (size_t)(BUFFER_SIZE - 1));

    /*
     * copy_from_user(to, from, size)
     * Returns: Number of bytes that COULD NOT be copied
     */
    if (copy_from_user(dev->buffer, buf, bytes_written)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;
    }

    dev->buffer[bytes_written] = '\0';  /* Null terminate */
    dev->buffer_size = bytes_written;
    *offset = bytes_written;

    pr_info("%s: Wrote %zd bytes: '%s'\n",
            DEVICE_NAME, bytes_written, dev->buffer);

    mutex_unlock(&dev->lock);

    return bytes_written;
}

/*
 * FILE OPERATIONS STRUCTURE
 *
 * Maps system calls to driver functions.
 * Only implement what you need - others default to error or no-op.
 *
 * Common operations:
 * .owner          - THIS_MODULE (for reference counting)
 * .open           - Open device
 * .release        - Close device
 * .read           - Read from device
 * .write          - Write to device
 * .unlocked_ioctl - Device control commands
 * .mmap           - Memory map device
 * .poll           - Poll/select support
 * .llseek         - Seek in device
 * .fsync          - Flush data
 */
static const struct file_operations mychar_fops = {
    .owner   = THIS_MODULE,
    .open    = mychar_open,
    .release = mychar_release,
    .read    = mychar_read,
    .write   = mychar_write,
    /* .unlocked_ioctl = mychar_ioctl,  -- See Example 3 */
};

/*
 * MODULE INIT - Register the character device
 */
static int __init mychar_init(void)
{
    int ret;

    /* Allocate device structure */
    mydev = kzalloc(sizeof(*mydev), GFP_KERNEL);
    if (!mydev)
        return -ENOMEM;

    /* Initialize mutex */
    mutex_init(&mydev->lock);

    /*
     * STEP 1: Allocate device number
     *
     * alloc_chrdev_region() - Dynamically allocate major number
     *   - Preferred method (avoids conflicts)
     *   - Major number assigned by kernel
     *
     * Alternative: register_chrdev_region() - Request specific major
     *   - Used when major number must be fixed
     *   - Risk of conflict with existing drivers
     *
     * Device number format: MAJOR:MINOR
     *   - MAJOR identifies the driver
     *   - MINOR identifies specific device instance
     */
    ret = alloc_chrdev_region(&mydev->dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        goto err_alloc;
    }

    pr_info("%s: Registered with major=%d, minor=%d\n",
            DEVICE_NAME, MAJOR(mydev->dev_num), MINOR(mydev->dev_num));

    /*
     * STEP 2: Initialize and add cdev
     *
     * cdev represents the character device in the kernel.
     * Links file_operations to device number.
     */
    cdev_init(&mydev->cdev, &mychar_fops);
    mydev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&mydev->cdev, mydev->dev_num, 1);
    if (ret < 0) {
        pr_err("Failed to add cdev\n");
        goto err_cdev;
    }

    /*
     * STEP 3: Create device class
     *
     * Classes organize devices in /sys/class/
     * Enables automatic device file creation via udev
     */
    mydev->class = class_create(CLASS_NAME);
    if (IS_ERR(mydev->class)) {
        pr_err("Failed to create device class\n");
        ret = PTR_ERR(mydev->class);
        goto err_class;
    }

    /*
     * STEP 4: Create device file
     *
     * Creates /dev/mychardev automatically via udev
     * Before this: mknod /dev/mychardev c MAJOR MINOR
     * After this:  automatic!
     */
    mydev->device = device_create(mydev->class, NULL, mydev->dev_num,
                                  NULL, DEVICE_NAME);
    if (IS_ERR(mydev->device)) {
        pr_err("Failed to create device\n");
        ret = PTR_ERR(mydev->device);
        goto err_device;
    }

    pr_info("%s: Driver initialized successfully\n", DEVICE_NAME);
    return 0;

/* Error handling - clean up in reverse order */
err_device:
    class_destroy(mydev->class);
err_class:
    cdev_del(&mydev->cdev);
err_cdev:
    unregister_chrdev_region(mydev->dev_num, 1);
err_alloc:
    kfree(mydev);
    return ret;
}

/*
 * MODULE EXIT - Unregister and clean up
 *
 * IMPORTANT: Clean up in REVERSE order of init!
 */
static void __exit mychar_exit(void)
{
    device_destroy(mydev->class, mydev->dev_num);
    class_destroy(mydev->class);
    cdev_del(&mydev->cdev);
    unregister_chrdev_region(mydev->dev_num, 1);
    mutex_destroy(&mydev->lock);
    kfree(mydev);

    pr_info("%s: Driver unloaded\n", DEVICE_NAME);
}


/*
 * ============================================================================
 *              EXAMPLE 3: IOCTL - DEVICE CONTROL COMMANDS
 * ============================================================================
 *
 * IOCTL (Input/Output Control) allows user-space to send commands
 * and configure the driver beyond simple read/write.
 *
 * USE CASES:
 * - Get/set device configuration
 * - Trigger special operations
 * - Query device status
 * - Reset device
 *
 * USER SPACE USAGE:
 * -----------------
 * #include <sys/ioctl.h>
 *
 * int fd = open("/dev/mychardev", O_RDWR);
 *
 * // Get buffer size
 * int size;
 * ioctl(fd, MYCHAR_IOCTL_GET_SIZE, &size);
 *
 * // Clear buffer
 * ioctl(fd, MYCHAR_IOCTL_CLEAR, NULL);
 *
 * // Set configuration
 * struct mychar_config cfg = { .timeout = 100 };
 * ioctl(fd, MYCHAR_IOCTL_SET_CONFIG, &cfg);
 */

/*
 * IOCTL Command Definition
 *
 * Format: _IO, _IOR, _IOW, _IOWR macros
 *
 * _IO(type, nr)           - No data transfer
 * _IOR(type, nr, datatype) - Read data from driver
 * _IOW(type, nr, datatype) - Write data to driver
 * _IOWR(type, nr, datatype) - Read and write
 *
 * type: Magic number (unique to your driver)
 * nr:   Command number
 */
#define MYCHAR_MAGIC 'M'  /* Unique magic number */

#define MYCHAR_IOCTL_CLEAR      _IO(MYCHAR_MAGIC, 0)
#define MYCHAR_IOCTL_GET_SIZE   _IOR(MYCHAR_MAGIC, 1, int)
#define MYCHAR_IOCTL_SET_SIZE   _IOW(MYCHAR_MAGIC, 2, int)
#define MYCHAR_IOCTL_GET_CONFIG _IOR(MYCHAR_MAGIC, 3, struct mychar_config)
#define MYCHAR_IOCTL_SET_CONFIG _IOW(MYCHAR_MAGIC, 4, struct mychar_config)

/* Configuration structure for ioctl */
struct mychar_config {
    int timeout_ms;
    int mode;
    char name[32];
};

/*
 * IOCTL Handler
 *
 * unlocked_ioctl - Modern ioctl handler (no BKL - Big Kernel Lock)
 *
 * @file: File structure
 * @cmd:  Command code (created with _IO macros)
 * @arg:  Command argument (user-space pointer or value)
 */
static long mychar_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mychar_device *dev = file->private_data;
    struct mychar_config config;
    int size;
    long ret = 0;

    /*
     * Verify command belongs to this driver
     * _IOC_TYPE extracts the magic number
     */
    if (_IOC_TYPE(cmd) != MYCHAR_MAGIC) {
        pr_warn("Invalid ioctl magic number\n");
        return -ENOTTY;  /* Inappropriate ioctl for device */
    }

    /*
     * Check user-space pointer validity
     * access_ok() verifies the pointer is in user space
     */
    if (_IOC_DIR(cmd) & _IOC_READ) {
        if (!access_ok((void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;
    }
    if (_IOC_DIR(cmd) & _IOC_WRITE) {
        if (!access_ok((void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;
    }

    mutex_lock(&dev->lock);

    switch (cmd) {
    case MYCHAR_IOCTL_CLEAR:
        /* Clear the buffer */
        memset(dev->buffer, 0, BUFFER_SIZE);
        dev->buffer_size = 0;
        pr_info("%s: Buffer cleared via ioctl\n", DEVICE_NAME);
        break;

    case MYCHAR_IOCTL_GET_SIZE:
        /* Return current buffer size to user */
        size = dev->buffer_size;
        if (copy_to_user((int __user *)arg, &size, sizeof(size))) {
            ret = -EFAULT;
            break;
        }
        pr_info("%s: Returned size %d via ioctl\n", DEVICE_NAME, size);
        break;

    case MYCHAR_IOCTL_SET_SIZE:
        /* Get new size from user */
        if (copy_from_user(&size, (int __user *)arg, sizeof(size))) {
            ret = -EFAULT;
            break;
        }
        if (size < 0 || size >= BUFFER_SIZE) {
            ret = -EINVAL;
            break;
        }
        dev->buffer_size = size;
        pr_info("%s: Set size to %d via ioctl\n", DEVICE_NAME, size);
        break;

    case MYCHAR_IOCTL_GET_CONFIG:
        /* Build config structure */
        config.timeout_ms = 100;
        config.mode = 1;
        strscpy(config.name, "mychardev", sizeof(config.name));

        if (copy_to_user((struct mychar_config __user *)arg,
                        &config, sizeof(config))) {
            ret = -EFAULT;
        }
        break;

    case MYCHAR_IOCTL_SET_CONFIG:
        if (copy_from_user(&config,
                          (struct mychar_config __user *)arg,
                          sizeof(config))) {
            ret = -EFAULT;
            break;
        }
        pr_info("%s: Config set: timeout=%d, mode=%d, name=%s\n",
                DEVICE_NAME, config.timeout_ms, config.mode, config.name);
        break;

    default:
        ret = -ENOTTY;  /* Unknown command */
    }

    mutex_unlock(&dev->lock);
    return ret;
}


/*
 * ============================================================================
 *              EXAMPLE 4: PLATFORM DRIVER WITH DEVICE TREE
 * ============================================================================
 *
 * Platform drivers are for devices that cannot be automatically detected
 * (no enumeration protocol like PCI or USB).
 *
 * Common in embedded: GPIO controllers, I2C/SPI peripherals, memory-mapped
 * hardware, SoC internal peripherals.
 *
 * DEVICE TREE:
 * ------------
 * Device Tree (DT) describes hardware configuration.
 * Located in: arch/arm/boot/dts/ or arch/arm64/boot/dts/
 *
 * Example DT entry (mydevice.dtsi):
 *
 * mydevice@10000000 {
 *     compatible = "mycompany,mydevice";
 *     reg = <0x10000000 0x1000>;      // Base address, size
 *     interrupts = <GIC_SPI 50 IRQ_TYPE_LEVEL_HIGH>;
 *     clocks = <&clk_peripheral>;
 *     clock-names = "mydevice_clk";
 *     mycompany,custom-property = <42>;
 *     status = "okay";
 * };
 *
 * BINDING:
 * --------
 * Driver matches DT node via "compatible" string.
 * When match found, probe() is called.
 */

/* Device-specific data structure */
struct myplatform_device {
    void __iomem *base;         /* Memory-mapped I/O base address */
    int irq;                    /* Interrupt number */
    struct clk *clk;            /* Clock reference */
    struct device *dev;         /* Parent device */
    spinlock_t lock;            /* Spinlock for ISR safety */
    u32 custom_value;           /* Custom property from DT */
};

/*
 * PROBE - Called when device is detected
 *
 * This is where you:
 * 1. Parse Device Tree properties
 * 2. Map I/O memory
 * 3. Request interrupts
 * 4. Enable clocks
 * 5. Initialize hardware
 * 6. Register with subsystems
 */
static int myplatform_probe(struct platform_device *pdev)
{
    struct myplatform_device *mydev;
    struct resource *res;
    struct device *dev = &pdev->dev;
    int ret;

    pr_info("myplatform: Probe called\n");

    /* Allocate device structure */
    mydev = devm_kzalloc(dev, sizeof(*mydev), GFP_KERNEL);
    if (!mydev)
        return -ENOMEM;

    mydev->dev = dev;
    spin_lock_init(&mydev->lock);

    /*
     * Parse Device Tree properties
     *
     * of_property_read_* functions extract values from DT
     * Returns 0 on success, negative on error
     */
    ret = of_property_read_u32(dev->of_node, "mycompany,custom-property",
                               &mydev->custom_value);
    if (ret) {
        mydev->custom_value = 0;  /* Default value */
        dev_warn(dev, "custom-property not specified, using default\n");
    }

    /*
     * Get memory resource from DT "reg" property
     *
     * platform_get_resource() returns NULL if not found
     * IORESOURCE_MEM = memory-mapped I/O
     * IORESOURCE_IRQ = interrupt
     */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(dev, "No memory resource found\n");
        return -ENODEV;
    }

    /*
     * Map physical address to kernel virtual address
     *
     * devm_ioremap_resource() does:
     * 1. Request memory region (claim ownership)
     * 2. Map to kernel address space
     * 3. Auto-cleanup on driver removal (devm_*)
     *
     * WHY NOT direct pointer access?
     * - Physical addresses aren't valid in kernel space
     * - Need MMU mapping
     * - ioremap creates the mapping
     */
    mydev->base = devm_ioremap_resource(dev, res);
    if (IS_ERR(mydev->base)) {
        dev_err(dev, "Failed to map memory\n");
        return PTR_ERR(mydev->base);
    }

    dev_info(dev, "Mapped registers at %p\n", mydev->base);

    /*
     * Get interrupt number from DT "interrupts" property
     *
     * platform_get_irq() handles DT interrupt parsing
     */
    mydev->irq = platform_get_irq(pdev, 0);
    if (mydev->irq < 0) {
        dev_warn(dev, "No IRQ specified, polling mode\n");
    }

    /*
     * Enable clock (if specified in DT)
     *
     * devm_clk_get() gets clock reference
     * clk_prepare_enable() turns it on
     */
    mydev->clk = devm_clk_get(dev, NULL);  /* NULL = first/only clock */
    if (!IS_ERR(mydev->clk)) {
        ret = clk_prepare_enable(mydev->clk);
        if (ret) {
            dev_err(dev, "Failed to enable clock\n");
            return ret;
        }
    }

    /* Store private data for later retrieval */
    platform_set_drvdata(pdev, mydev);

    /*
     * Initialize hardware
     * Use readl/writel for memory-mapped I/O
     */
    /* Example: writel(0x1, mydev->base + REG_CONTROL); */

    dev_info(dev, "Platform device probed successfully\n");
    return 0;
}

/*
 * REMOVE - Called when device is removed
 *
 * Clean up in reverse order of probe
 */
static int myplatform_remove(struct platform_device *pdev)
{
    struct myplatform_device *mydev = platform_get_drvdata(pdev);

    /* Disable clock */
    if (!IS_ERR_OR_NULL(mydev->clk))
        clk_disable_unprepare(mydev->clk);

    /* Note: devm_* resources auto-cleanup */

    dev_info(&pdev->dev, "Platform device removed\n");
    return 0;
}

/*
 * Device Tree match table
 *
 * Connects DT "compatible" strings to this driver
 * Can have multiple entries for different hardware versions
 */
static const struct of_device_id myplatform_of_match[] = {
    { .compatible = "mycompany,mydevice", },
    { .compatible = "mycompany,mydevice-v2", .data = (void *)2 },
    { /* sentinel - marks end of array */ }
};
MODULE_DEVICE_TABLE(of, myplatform_of_match);

/*
 * Platform driver structure
 */
static struct platform_driver myplatform_driver = {
    .probe  = myplatform_probe,
    .remove = myplatform_remove,
    .driver = {
        .name = "myplatform",
        .of_match_table = myplatform_of_match,
        /* .pm = &myplatform_pm_ops,  -- Power management ops */
    },
};

/* Convenience macro: creates init/exit functions */
module_platform_driver(myplatform_driver);


/*
 * ============================================================================
 *              EXAMPLE 5: INTERRUPT HANDLING
 * ============================================================================
 *
 * TYPES OF INTERRUPTS:
 * 1. Hardware IRQ - From hardware devices
 * 2. Software IRQ (softirq) - Deferred processing
 * 3. Tasklets - Simplified softirq
 * 4. Workqueues - Process context deferred work
 *
 * INTERRUPT CONTEXT RULES:
 * - Cannot sleep (no mutex_lock, kmalloc with GFP_KERNEL, etc.)
 * - Must be fast (< 100us ideally)
 * - Cannot access user space
 * - Use spinlock, not mutex
 * - Use GFP_ATOMIC for allocation
 *
 * TOP HALF vs BOTTOM HALF:
 * - Top half: ISR itself, runs immediately, minimal work
 * - Bottom half: Deferred work (tasklet, workqueue)
 */

#define MY_IRQ_NUM 42

static irqreturn_t mydevice_isr(int irq, void *dev_id)
{
    struct myplatform_device *dev = dev_id;
    u32 status;
    unsigned long flags;

    /*
     * TOP HALF - Runs in interrupt context
     * Keep this FAST!
     */

    /* Read interrupt status register */
    /* status = readl(dev->base + REG_IRQ_STATUS); */
    status = 0x1;  /* Placeholder */

    if (!(status & 0x1)) {
        /* Not our interrupt */
        return IRQ_NONE;
    }

    /* Acknowledge/clear interrupt in hardware */
    /* writel(status, dev->base + REG_IRQ_CLEAR); */

    /*
     * Spinlock in ISR context
     * spin_lock_irqsave disables interrupts on local CPU
     * MUST use if data shared with non-ISR code
     */
    spin_lock_irqsave(&dev->lock, flags);
    /* ... critical section ... */
    spin_unlock_irqrestore(&dev->lock, flags);

    /*
     * Schedule bottom half for heavy processing
     * Options:
     * 1. tasklet_schedule(&my_tasklet);
     * 2. schedule_work(&my_work);
     * 3. queue_work(my_workqueue, &my_work);
     */

    return IRQ_HANDLED;
}

/*
 * Request interrupt in probe()
 */
static int setup_irq_example(struct platform_device *pdev,
                             struct myplatform_device *mydev)
{
    int ret;

    /*
     * devm_request_irq() - Request IRQ with auto-cleanup
     *
     * Parameters:
     * - dev: Device structure
     * - irq: Interrupt number
     * - handler: ISR function
     * - flags: IRQF_* flags
     * - name: Interrupt name (shows in /proc/interrupts)
     * - dev_id: Passed to ISR (use for driver data)
     *
     * Common flags:
     * - IRQF_SHARED: Allow sharing with other devices
     * - IRQF_TRIGGER_RISING: Trigger on rising edge
     * - IRQF_TRIGGER_FALLING: Trigger on falling edge
     * - IRQF_TRIGGER_HIGH: Trigger on high level
     * - IRQF_ONESHOT: Don't re-enable until handler finishes
     */
    ret = devm_request_irq(&pdev->dev, mydev->irq, mydevice_isr,
                          IRQF_SHARED | IRQF_TRIGGER_RISING,
                          "mydevice", mydev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ %d\n", mydev->irq);
        return ret;
    }

    return 0;
}

/*
 * WORKQUEUE - Bottom half in process context
 *
 * Advantages:
 * - Can sleep
 * - Can use mutex
 * - Can access user space
 */
static struct work_struct my_work;

static void my_work_handler(struct work_struct *work)
{
    /* Process context - can sleep */
    pr_info("Workqueue handler running\n");

    /* Can use msleep, mutex_lock, etc. */
    msleep(10);
}

static void setup_workqueue_example(void)
{
    INIT_WORK(&my_work, my_work_handler);

    /* Schedule from ISR or other context */
    /* schedule_work(&my_work); */
}


/*
 * ============================================================================
 *              EXAMPLE 6: I2C DEVICE DRIVER
 * ============================================================================
 *
 * I2C (Inter-Integrated Circuit) is common for:
 * - Sensors (temperature, accelerometer, gyroscope)
 * - EEPROMs
 * - Real-time clocks
 * - Touch controllers
 *
 * DEVICE TREE EXAMPLE:
 *
 * &i2c1 {
 *     status = "okay";
 *
 *     mysensor@48 {
 *         compatible = "mycompany,mysensor";
 *         reg = <0x48>;           // I2C address
 *         interrupt-parent = <&gpio1>;
 *         interrupts = <5 IRQ_TYPE_EDGE_FALLING>;
 *     };
 * };
 */

struct mysensor_data {
    struct i2c_client *client;
    struct mutex lock;
    u16 calibration;
};

/*
 * I2C Read/Write helpers
 *
 * i2c_smbus_* - SMBus compatible functions (preferred)
 * i2c_transfer - Raw I2C transfers (for complex protocols)
 */
static int mysensor_read_reg(struct mysensor_data *data, u8 reg, u8 *val)
{
    int ret;

    /*
     * i2c_smbus_read_byte_data - Read single byte from register
     * Returns: byte value on success, negative on error
     */
    ret = i2c_smbus_read_byte_data(data->client, reg);
    if (ret < 0) {
        dev_err(&data->client->dev, "Failed to read reg 0x%02x\n", reg);
        return ret;
    }

    *val = ret;
    return 0;
}

static int mysensor_write_reg(struct mysensor_data *data, u8 reg, u8 val)
{
    int ret;

    /*
     * i2c_smbus_write_byte_data - Write single byte to register
     * Returns: 0 on success, negative on error
     */
    ret = i2c_smbus_write_byte_data(data->client, reg, val);
    if (ret < 0) {
        dev_err(&data->client->dev, "Failed to write reg 0x%02x\n", reg);
        return ret;
    }

    return 0;
}

/* Read multiple bytes using i2c_transfer */
static int mysensor_read_block(struct mysensor_data *data, u8 reg,
                               u8 *buf, int len)
{
    struct i2c_msg msgs[2];
    int ret;

    /*
     * I2C transaction: Write register address, then read data
     *
     * Message 1: Write register address
     * Message 2: Read data bytes
     */
    msgs[0].addr = data->client->addr;
    msgs[0].flags = 0;          /* Write */
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = data->client->addr;
    msgs[1].flags = I2C_M_RD;   /* Read */
    msgs[1].len = len;
    msgs[1].buf = buf;

    ret = i2c_transfer(data->client->adapter, msgs, 2);
    if (ret != 2) {
        dev_err(&data->client->dev, "I2C transfer failed\n");
        return ret < 0 ? ret : -EIO;
    }

    return 0;
}

/*
 * I2C Driver Probe
 */
static int mysensor_probe(struct i2c_client *client)
{
    struct mysensor_data *data;
    struct device *dev = &client->dev;
    u8 chip_id;
    int ret;

    /* Allocate driver data */
    data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    mutex_init(&data->lock);

    /* Store in i2c_client for later retrieval */
    i2c_set_clientdata(client, data);

    /* Verify chip ID */
    ret = mysensor_read_reg(data, 0x00, &chip_id);  /* Reg 0x00 = ID */
    if (ret)
        return ret;

    if (chip_id != 0x42) {  /* Expected chip ID */
        dev_err(dev, "Unknown chip ID: 0x%02x\n", chip_id);
        return -ENODEV;
    }

    dev_info(dev, "Found sensor with chip ID 0x%02x\n", chip_id);

    /* Initialize hardware */
    /* mysensor_write_reg(data, REG_CONFIG, CONFIG_DEFAULT); */

    return 0;
}

static void mysensor_remove(struct i2c_client *client)
{
    /* Cleanup - devm resources auto-freed */
    dev_info(&client->dev, "Sensor removed\n");
}

static const struct i2c_device_id mysensor_id[] = {
    { "mysensor", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mysensor_id);

static const struct of_device_id mysensor_of_match[] = {
    { .compatible = "mycompany,mysensor" },
    { }
};
MODULE_DEVICE_TABLE(of, mysensor_of_match);

static struct i2c_driver mysensor_driver = {
    .driver = {
        .name = "mysensor",
        .of_match_table = mysensor_of_match,
    },
    .probe = mysensor_probe,
    .remove = mysensor_remove,
    .id_table = mysensor_id,
};

module_i2c_driver(mysensor_driver);


/*
 * ============================================================================
 *              EXAMPLE 7: SPI DEVICE DRIVER
 * ============================================================================
 *
 * SPI (Serial Peripheral Interface) is faster than I2C, used for:
 * - Flash memory
 * - Display controllers
 * - ADCs/DACs
 * - Radio transceivers
 *
 * DEVICE TREE EXAMPLE:
 *
 * &spi0 {
 *     status = "okay";
 *
 *     myflash@0 {
 *         compatible = "mycompany,myflash";
 *         reg = <0>;              // Chip select 0
 *         spi-max-frequency = <10000000>;  // 10 MHz
 *         spi-cpol;               // Clock polarity
 *         spi-cpha;               // Clock phase
 *     };
 * };
 */

struct myflash_data {
    struct spi_device *spi;
    struct mutex lock;
    u8 *tx_buf;
    u8 *rx_buf;
};

#define MYFLASH_CMD_READ_ID     0x9F
#define MYFLASH_CMD_READ_DATA   0x03
#define MYFLASH_CMD_WRITE_EN    0x06
#define MYFLASH_CMD_PAGE_PROG   0x02

/*
 * SPI Transfer - Full duplex
 */
static int myflash_read_id(struct myflash_data *data, u8 *id, int len)
{
    struct spi_transfer xfer[2];
    struct spi_message msg;
    u8 cmd = MYFLASH_CMD_READ_ID;
    int ret;

    memset(xfer, 0, sizeof(xfer));

    /*
     * Transfer 1: Send command
     */
    xfer[0].tx_buf = &cmd;
    xfer[0].len = 1;

    /*
     * Transfer 2: Receive ID bytes
     */
    xfer[1].rx_buf = id;
    xfer[1].len = len;

    /* Build message from transfers */
    spi_message_init(&msg);
    spi_message_add_tail(&xfer[0], &msg);
    spi_message_add_tail(&xfer[1], &msg);

    /* Execute synchronously */
    ret = spi_sync(data->spi, &msg);
    if (ret) {
        dev_err(&data->spi->dev, "SPI transfer failed\n");
        return ret;
    }

    return 0;
}

/* Simpler API for small transfers */
static int myflash_write_enable(struct myflash_data *data)
{
    u8 cmd = MYFLASH_CMD_WRITE_EN;

    /*
     * spi_write - Simple write-only transfer
     * spi_read - Simple read-only transfer
     * spi_write_then_read - Write then read (common pattern)
     */
    return spi_write(data->spi, &cmd, 1);
}

static int myflash_probe(struct spi_device *spi)
{
    struct myflash_data *data;
    struct device *dev = &spi->dev;
    u8 id[3];
    int ret;

    /* Configure SPI mode */
    spi->mode = SPI_MODE_0;     /* CPOL=0, CPHA=0 */
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret)
        return ret;

    data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->spi = spi;
    mutex_init(&data->lock);

    spi_set_drvdata(spi, data);

    /* Read chip ID */
    ret = myflash_read_id(data, id, sizeof(id));
    if (ret)
        return ret;

    dev_info(dev, "Flash ID: %02x %02x %02x\n", id[0], id[1], id[2]);

    return 0;
}

static void myflash_remove(struct spi_device *spi)
{
    dev_info(&spi->dev, "Flash removed\n");
}

static const struct spi_device_id myflash_spi_id[] = {
    { "myflash", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, myflash_spi_id);

static const struct of_device_id myflash_of_match[] = {
    { .compatible = "mycompany,myflash" },
    { }
};
MODULE_DEVICE_TABLE(of, myflash_of_match);

static struct spi_driver myflash_driver = {
    .driver = {
        .name = "myflash",
        .of_match_table = myflash_of_match,
    },
    .probe = myflash_probe,
    .remove = myflash_remove,
    .id_table = myflash_spi_id,
};

module_spi_driver(myflash_driver);


/*
 * ============================================================================
 *              EXAMPLE 8: GPIO DRIVER
 * ============================================================================
 *
 * GPIO (General Purpose Input/Output) is fundamental for:
 * - LED control
 * - Button input
 * - Chip select signals
 * - Reset signals
 *
 * DEVICE TREE EXAMPLE:
 *
 * myled {
 *     compatible = "mycompany,myled";
 *     gpios = <&gpio1 5 GPIO_ACTIVE_HIGH>;
 *     default-state = "off";
 * };
 */

struct myled_data {
    struct gpio_desc *gpio;
    bool state;
};

static int myled_probe(struct platform_device *pdev)
{
    struct myled_data *data;
    struct device *dev = &pdev->dev;
    enum gpiod_flags flags;
    const char *default_state;
    int ret;

    data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    /* Read default state from DT */
    ret = of_property_read_string(dev->of_node, "default-state",
                                  &default_state);
    if (ret || strcmp(default_state, "on") != 0) {
        flags = GPIOD_OUT_LOW;
        data->state = false;
    } else {
        flags = GPIOD_OUT_HIGH;
        data->state = true;
    }

    /*
     * Get GPIO descriptor from DT "gpios" property
     *
     * devm_gpiod_get - Get GPIO with auto-cleanup
     * Parameters:
     * - dev: Device
     * - con_id: Connection ID (NULL for first/only)
     * - flags: Initial configuration
     *
     * Flags:
     * - GPIOD_IN: Configure as input
     * - GPIOD_OUT_LOW: Output, initially low
     * - GPIOD_OUT_HIGH: Output, initially high
     * - GPIOD_OUT_LOW_OPEN_DRAIN: Open-drain output
     */
    data->gpio = devm_gpiod_get(dev, NULL, flags);
    if (IS_ERR(data->gpio)) {
        dev_err(dev, "Failed to get GPIO\n");
        return PTR_ERR(data->gpio);
    }

    platform_set_drvdata(pdev, data);

    dev_info(dev, "LED initialized, state=%s\n",
             data->state ? "on" : "off");

    return 0;
}

/* GPIO operations */
static void gpio_examples(struct myled_data *data)
{
    int value;

    /* Set output value */
    gpiod_set_value(data->gpio, 1);     /* Set high */
    gpiod_set_value(data->gpio, 0);     /* Set low */

    /* Read input value */
    /* gpiod_direction_input(data->gpio); */
    value = gpiod_get_value(data->gpio);

    /* Toggle */
    gpiod_set_value(data->gpio, !gpiod_get_value(data->gpio));
}


/*
 * ============================================================================
 *              EXAMPLE 9: MEMORY MANAGEMENT
 * ============================================================================
 *
 * KERNEL MEMORY ALLOCATION:
 *
 * 1. kmalloc/kfree - General purpose allocation
 *    - Fast, physically contiguous
 *    - Limited size (typically < 128KB)
 *    - Use for small allocations
 *
 * 2. vmalloc/vfree - Virtual contiguous allocation
 *    - Can allocate larger buffers
 *    - Not physically contiguous
 *    - Slower due to page table setup
 *
 * 3. devm_* - Device-managed allocation
 *    - Auto-freed when device removed
 *    - Preferred for driver allocations
 *
 * 4. DMA allocation - For hardware DMA transfers
 *    - Physically contiguous
 *    - Cache coherent or streaming
 */

static void memory_examples(struct device *dev)
{
    void *ptr;
    dma_addr_t dma_handle;

    /*
     * kmalloc flags:
     * - GFP_KERNEL: Normal allocation, can sleep
     * - GFP_ATOMIC: Cannot sleep (use in ISR)
     * - GFP_DMA: DMA-capable memory
     * - __GFP_ZERO: Zero the memory
     *
     * kzalloc = kmalloc + memset(0)
     */
    ptr = kmalloc(1024, GFP_KERNEL);
    if (ptr) {
        /* Use memory */
        kfree(ptr);
    }

    /* Device-managed (auto-freed on remove) */
    ptr = devm_kzalloc(dev, 1024, GFP_KERNEL);
    /* No need to free - handled automatically */

    /*
     * DMA Coherent allocation
     * - CPU and device see same data
     * - No cache flush needed
     * - Use for frequently accessed DMA buffers
     */
    ptr = dma_alloc_coherent(dev, 4096, &dma_handle, GFP_KERNEL);
    if (ptr) {
        /* dma_handle is physical address for hardware */
        /* ptr is virtual address for CPU */

        dma_free_coherent(dev, 4096, ptr, dma_handle);
    }

    /*
     * DMA Streaming - For one-shot transfers
     * More efficient but requires explicit sync
     *
     * dma_map_single() / dma_unmap_single()
     * dma_sync_single_for_cpu() / dma_sync_single_for_device()
     */
}


/*
 * ============================================================================
 *              EXAMPLE 10: SYSFS INTERFACE
 * ============================================================================
 *
 * Sysfs provides user-space access to driver attributes via files.
 * Located at /sys/class/<class>/<device>/
 *
 * USER SPACE USAGE:
 * -----------------
 * cat /sys/class/myclass/mydevice/status
 * echo 1 > /sys/class/myclass/mydevice/enable
 */

/* Device attribute: show function (read) */
static ssize_t status_show(struct device *dev, struct device_attribute *attr,
                           char *buf)
{
    struct mychar_device *mydev = dev_get_drvdata(dev);

    /*
     * sysfs_emit - Safe way to write to sysfs buffer
     * Replaces sprintf/snprintf for sysfs
     * Returns: number of bytes written
     */
    return sysfs_emit(buf, "%d\n", mydev->open_count);
}

/* Device attribute: store function (write) */
static ssize_t enable_store(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count)
{
    /* struct mychar_device *mydev = dev_get_drvdata(dev); */
    int val;
    int ret;

    /* Parse integer from user input */
    ret = kstrtoint(buf, 10, &val);
    if (ret)
        return ret;

    /* Validate */
    if (val != 0 && val != 1)
        return -EINVAL;

    /* Apply setting */
    pr_info("Enable set to %d\n", val);

    return count;
}

/* Define attributes */
static DEVICE_ATTR_RO(status);  /* Read-only */
static DEVICE_ATTR_WO(enable);  /* Write-only */
/* DEVICE_ATTR_RW(name) for read-write */

/* Attribute group */
static struct attribute *mychar_attrs[] = {
    &dev_attr_status.attr,
    &dev_attr_enable.attr,
    NULL
};

ATTRIBUTE_GROUPS(mychar);

/* Add to device_create():
 * device_create_with_groups(class, parent, devt, data, groups, name);
 */


/*
 * ============================================================================
 *              EXAMPLE 11: FIRMWARE LOADING
 * ============================================================================
 *
 * Load firmware files from /lib/firmware/
 *
 * SETUP:
 * ------
 * 1. Place firmware file in /lib/firmware/mydevice.fw
 * 2. Request firmware in driver
 * 3. Write to hardware
 * 4. Release firmware memory
 */

static int load_firmware_example(struct device *dev)
{
    const struct firmware *fw;
    int ret;

    /*
     * request_firmware - Load firmware from filesystem
     *
     * Blocks until firmware loaded or timeout
     * Firmware data in fw->data, size in fw->size
     *
     * Alternative: request_firmware_nowait() for async loading
     */
    ret = request_firmware(&fw, "mydevice.fw", dev);
    if (ret) {
        dev_err(dev, "Failed to load firmware: %d\n", ret);
        return ret;
    }

    dev_info(dev, "Loaded firmware: %zu bytes\n", fw->size);

    /* Write firmware to hardware */
    /* ... */

    /* Free firmware memory */
    release_firmware(fw);

    return 0;
}


/*
 * ============================================================================
 *              EXAMPLE 12: KERNEL DEBUGGING
 * ============================================================================
 *
 * DEBUGGING TECHNIQUES:
 *
 * 1. printk / pr_* / dev_* - Kernel logging
 *    dmesg - View kernel messages
 *    /var/log/kern.log - Persistent log
 *
 * 2. Dynamic Debug
 *    echo 'module mydriver +p' > /sys/kernel/debug/dynamic_debug/control
 *    Enables pr_debug() messages for mydriver
 *
 * 3. ftrace - Function tracing
 *    cd /sys/kernel/debug/tracing
 *    echo function > current_tracer
 *    echo mydriver_* > set_ftrace_filter
 *
 * 4. KGDB - Kernel debugger
 *    GDB over serial connection
 *
 * 5. /proc and /sys inspection
 *    cat /proc/interrupts
 *    cat /proc/iomem
 *    ls /sys/class/
 */

/* Debug macros with varying levels */
#define MYDRV_DEBUG 1

#if MYDRV_DEBUG
#define mydrv_dbg(dev, fmt, ...) \
    dev_dbg(dev, "%s: " fmt, __func__, ##__VA_ARGS__)
#else
#define mydrv_dbg(dev, fmt, ...) do { } while (0)
#endif

/* WARN/BUG macros for serious issues */
static void debug_examples(void)
{
    int error_condition = 0;

    /* Warning - continues execution */
    if (error_condition)
        WARN(1, "Unexpected condition occurred\n");

    /* WARN_ON - warning if condition true */
    WARN_ON(error_condition);

    /* BUG - stops kernel (use sparingly!) */
    /* if (unrecoverable_error) BUG(); */

    /* BUG_ON - BUG if condition true */
    /* BUG_ON(unrecoverable_error); */

    /* Dump stack trace */
    /* dump_stack(); */
}


/*
 * ============================================================================
 *                         MAKEFILE TEMPLATE
 * ============================================================================
 *
 * Save as "Makefile" (no extension):
 *
 * # Module name (without .ko)
 * obj-m += mydriver.o
 *
 * # For multi-file modules:
 * # obj-m += mydriver.o
 * # mydriver-objs := main.o utils.o hardware.o
 *
 * # Kernel build directory
 * KDIR := /lib/modules/$(shell uname -r)/build
 *
 * # Current directory
 * PWD := $(shell pwd)
 *
 * # Default target
 * all:
 *     $(MAKE) -C $(KDIR) M=$(PWD) modules
 *
 * # Clean target
 * clean:
 *     $(MAKE) -C $(KDIR) M=$(PWD) clean
 *
 * # Install module
 * install:
 *     $(MAKE) -C $(KDIR) M=$(PWD) modules_install
 *
 * # Cross-compile for ARM
 * arm:
 *     $(MAKE) -C $(KDIR) M=$(PWD) \
 *         ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
 *
 *
 * ============================================================================
 *                    ADDING DRIVER TO KERNEL SOURCE
 * ============================================================================
 *
 * To add your driver to the kernel source tree:
 *
 * 1. Create directory: drivers/misc/mydriver/
 *
 * 2. Add source files: mydriver.c
 *
 * 3. Create Kconfig:
 *    config MYDRIVER
 *        tristate "My custom driver"
 *        depends on I2C
 *        help
 *          This is my custom driver for XYZ hardware.
 *          Say Y or M if you have this hardware.
 *
 * 4. Create Makefile:
 *    obj-$(CONFIG_MYDRIVER) += mydriver.o
 *
 * 5. Add to parent Kconfig (drivers/misc/Kconfig):
 *    source "drivers/misc/mydriver/Kconfig"
 *
 * 6. Add to parent Makefile (drivers/misc/Makefile):
 *    obj-y += mydriver/
 *
 * 7. Configure kernel:
 *    make menuconfig
 *    Navigate to: Device Drivers -> Misc -> My custom driver
 *    Set to [*] built-in or [M] module
 *
 * 8. Build kernel:
 *    make -j$(nproc)
 *
 *
 * ============================================================================
 *                    COMMON INTERVIEW QUESTIONS
 * ============================================================================
 *
 * Q1: What's the difference between spin_lock and mutex?
 * A: Spinlock busy-waits (for short critical sections, usable in ISR).
 *    Mutex sleeps (for longer sections, only in process context).
 *
 * Q2: When to use devm_* functions?
 * A: Always prefer devm_* for driver allocations. They auto-cleanup
 *    on driver removal, preventing resource leaks.
 *
 * Q3: What's the difference between platform_driver and i2c_driver?
 * A: Platform drivers are for non-discoverable devices (memory-mapped).
 *    I2C drivers are for devices on I2C bus with enumeration.
 *
 * Q4: How does Device Tree binding work?
 * A: DT describes hardware. Driver's of_match_table lists compatible
 *    strings. Kernel matches them and calls probe().
 *
 * Q5: What's the top-half vs bottom-half split?
 * A: Top-half is the ISR - must be fast, can't sleep.
 *    Bottom-half (tasklet/workqueue) handles deferred work.
 *
 * Q6: How to safely share data between ISR and process context?
 * A: Use spinlock with spin_lock_irqsave/spin_unlock_irqrestore.
 *
 * Q7: What does copy_to_user/copy_from_user do?
 * A: Safely copies data between kernel and user space, validating
 *    the user pointer and handling page faults.
 *
 * Q8: What are the main kernel memory allocators?
 * A: kmalloc (small, contiguous), vmalloc (large, virtual),
 *    dma_alloc_coherent (DMA buffers).
 *
 * Q9: How to debug kernel crashes?
 * A: Check dmesg for oops/panic message, decode with addr2line,
 *    use dynamic debug, ftrace, or KGDB.
 *
 * Q10: What's the difference between ioctl and sysfs?
 * A: ioctl is for complex operations/binary data via device file.
 *     sysfs is for simple attributes as text files.
 *
 *
 * ============================================================================
 *                         USEFUL COMMANDS
 * ============================================================================
 *
 * # Module operations
 * sudo insmod mydriver.ko           # Load module
 * sudo insmod mydriver.ko param=5   # Load with parameter
 * sudo rmmod mydriver               # Unload module
 * sudo modprobe mydriver            # Load with dependencies
 * lsmod                             # List loaded modules
 * modinfo mydriver.ko               # Show module info
 *
 * # Kernel logs
 * dmesg                             # Show kernel messages
 * dmesg -w                          # Follow new messages
 * dmesg -T                          # With timestamps
 * journalctl -k                     # systemd kernel log
 *
 * # Device information
 * ls -la /dev/                      # List device files
 * cat /proc/devices                 # Show device numbers
 * cat /proc/interrupts              # Show IRQ usage
 * cat /proc/iomem                   # Show memory mappings
 * lspci -v                          # PCI devices
 * lsusb -v                          # USB devices
 * ls /sys/class/                    # Device classes
 *
 * # Device Tree
 * dtc -I fs /sys/firmware/devicetree/base  # Dump live DT
 * ls /sys/firmware/devicetree/base/        # Browse DT
 *
 * # Debugging
 * cat /sys/kernel/debug/gpio        # GPIO status
 * cat /sys/kernel/debug/clk/clk_summary  # Clock tree
 * echo 8 > /proc/sys/kernel/printk  # Enable all printk levels
 */

/*
 * ============================================================================
 *                              END OF GUIDE
 * ============================================================================
 */
