/*
 * Linux Kernel Driver Debugging - Complete Guide
 *
 * ============================================================================
 *                    DEBUGGING OVERVIEW
 * ============================================================================
 *
 * Debugging kernel code is different from user-space:
 * - No printf (use printk/pr_* instead)
 * - Crashes affect entire system
 * - Limited debugging tools compared to user-space
 * - Must understand kernel memory layout
 *
 * DEBUGGING TOOLS HIERARCHY:
 * --------------------------
 * 1. printk / pr_* / dev_* - Basic logging
 * 2. Dynamic Debug - Runtime enable/disable debug messages
 * 3. /proc and /sys - Runtime inspection
 * 4. ftrace - Function tracing
 * 5. kprobes - Dynamic instrumentation
 * 6. KGDB - Kernel debugger (GDB over serial)
 * 7. kdump/crash - Post-mortem analysis
 *
 *
 * ============================================================================
 *                    1. PRINTK - KERNEL LOGGING
 * ============================================================================
 *
 * printk() is the kernel's printf(). Messages go to:
 * - Kernel ring buffer (dmesg)
 * - Console (if level high enough)
 * - /var/log/kern.log or journalctl
 *
 * LOG LEVELS (from highest to lowest priority):
 * ---------------------------------------------
 * KERN_EMERG   "0" - System is unusable
 * KERN_ALERT   "1" - Action must be taken immediately
 * KERN_CRIT    "2" - Critical conditions
 * KERN_ERR     "3" - Error conditions
 * KERN_WARNING "4" - Warning conditions
 * KERN_NOTICE  "5" - Normal but significant
 * KERN_INFO    "6" - Informational
 * KERN_DEBUG   "7" - Debug messages (not shown by default)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Debug Guide");
MODULE_DESCRIPTION("Kernel Driver Debugging Examples");

/*
 * PRINTK EXAMPLES:
 */
static void printk_examples(void)
{
    /* Classic printk with explicit log level */
    printk(KERN_ERR "This is an error message\n");
    printk(KERN_WARNING "This is a warning\n");
    printk(KERN_INFO "This is informational\n");
    printk(KERN_DEBUG "This is a debug message\n");

    /*
     * Modern pr_* macros (preferred)
     * Automatically add module name prefix
     * Defined in <linux/printk.h>
     */
    pr_emerg("Emergency: system unstable!\n");
    pr_alert("Alert: immediate action needed!\n");
    pr_crit("Critical error occurred\n");
    pr_err("Error: operation failed\n");
    pr_warn("Warning: potential issue\n");
    pr_notice("Notice: significant event\n");
    pr_info("Info: module loaded\n");
    pr_debug("Debug: detailed trace\n");  /* Only shown if DEBUG defined */

    /*
     * Rate-limited printing - prevents log flooding
     * Only prints once per specified interval
     */
    if (printk_ratelimit())
        pr_info("This message is rate-limited\n");

    /* Or use _ratelimited variants */
    pr_info_ratelimited("Rate-limited info message\n");
    pr_err_ratelimited("Rate-limited error message\n");

    /* Print once (useful in loops) */
    pr_info_once("This only prints once ever\n");

    /*
     * Formatted output
     */
    {
        int value = 42;
        void *ptr = (void *)0xDEADBEEF;
        const char *str = "test";

        pr_info("Integer: %d, Hex: 0x%x\n", value, value);
        pr_info("Pointer: %p, %px (actual address)\n", ptr, ptr);
        pr_info("String: %s\n", str);
        pr_info("Size: %zu, Signed size: %zd\n", sizeof(int), (ssize_t)-1);
    }
}


/*
 * DEVICE-SPECIFIC LOGGING (dev_* macros)
 * --------------------------------------
 * These include device name in output - PREFERRED for drivers
 */
static void device_logging_example(struct device *dev)
{
    dev_emerg(dev, "Emergency message\n");
    dev_alert(dev, "Alert message\n");
    dev_crit(dev, "Critical message\n");
    dev_err(dev, "Error message\n");
    dev_warn(dev, "Warning message\n");
    dev_notice(dev, "Notice message\n");
    dev_info(dev, "Info message\n");
    dev_dbg(dev, "Debug message\n");

    /*
     * Output format:
     * [ 1234.567890] mydriver i2c1: Error message
     *                ^^^^^^^  ^^^^  ^^^^^^^^^^^^^
     *                driver   bus   your message
     */
}


/*
 * ============================================================================
 *                    VIEWING KERNEL MESSAGES
 * ============================================================================
 *
 * DMESG COMMANDS:
 * ---------------
 *
 * # View all kernel messages
 * dmesg
 *
 * # Follow new messages in real-time
 * dmesg -w
 * # OR
 * dmesg --follow
 *
 * # Show human-readable timestamps
 * dmesg -T
 *
 * # Show messages since last boot
 * dmesg -b
 *
 * # Filter by log level (show errors and above)
 * dmesg --level=err,crit,alert,emerg
 * # OR
 * dmesg -l err
 *
 * # Search for specific module
 * dmesg | grep mydriver
 * dmesg | grep -i "error\|fail"
 *
 * # Clear ring buffer (requires root)
 * sudo dmesg -c
 *
 * # Show colored output
 * dmesg --color=always | less -R
 *
 *
 * JOURNALCTL (systemd):
 * ---------------------
 *
 * # Kernel messages
 * journalctl -k
 *
 * # Follow in real-time
 * journalctl -kf
 *
 * # Since last boot
 * journalctl -k -b
 *
 * # Filter by priority
 * journalctl -k -p err
 *
 *
 * ADJUST LOG LEVEL:
 * -----------------
 *
 * # Check current console log level
 * cat /proc/sys/kernel/printk
 * # Output: 4    4    1    7
 * #         ^    ^    ^    ^
 * #         |    |    |    +-- default level for printk
 * #         |    |    +------- minimum console level
 * #         |    +------------ default console level
 * #         +----------------- current console level
 *
 * # Show all messages (including debug)
 * echo 8 | sudo tee /proc/sys/kernel/printk
 *
 * # Quiet mode (errors only)
 * echo 3 | sudo tee /proc/sys/kernel/printk
 *
 * # Permanent change in /etc/sysctl.conf:
 * # kernel.printk = 8 4 1 7
 *
 *
 * ============================================================================
 *                    2. DYNAMIC DEBUG
 * ============================================================================
 *
 * Dynamic Debug allows enabling/disabling pr_debug() and dev_dbg()
 * messages at runtime WITHOUT recompiling.
 *
 * REQUIREMENTS:
 * - Kernel config: CONFIG_DYNAMIC_DEBUG=y
 * - Debug messages compiled in (even if not displayed)
 *
 * CONTROL FILE:
 * /sys/kernel/debug/dynamic_debug/control (requires debugfs mounted)
 */

/* Example with debug messages */
static void dynamic_debug_example(void)
{
    pr_debug("This is a dynamic debug message\n");
    pr_debug("Processing item %d\n", 42);
    /* dev_dbg(dev, "Device debug message\n"); */
}

/*
 * DYNAMIC DEBUG COMMANDS:
 * -----------------------
 *
 * # Mount debugfs if not mounted
 * sudo mount -t debugfs none /sys/kernel/debug
 *
 * # View available debug statements
 * cat /sys/kernel/debug/dynamic_debug/control
 *
 * # Enable debug for specific module
 * echo 'module mydriver +p' | sudo tee /sys/kernel/debug/dynamic_debug/control
 *
 * # Disable debug for module
 * echo 'module mydriver -p' | sudo tee /sys/kernel/debug/dynamic_debug/control
 *
 * # Enable debug for specific file
 * echo 'file mydriver.c +p' | sudo tee /sys/kernel/debug/dynamic_debug/control
 *
 * # Enable debug for specific function
 * echo 'func my_probe +p' | sudo tee /sys/kernel/debug/dynamic_debug/control
 *
 * # Enable debug for specific line
 * echo 'file mydriver.c line 150 +p' | sudo tee /sys/kernel/debug/dynamic_debug/control
 *
 * # Enable with additional info
 * echo 'module mydriver +pflmt' | sudo tee /sys/kernel/debug/dynamic_debug/control
 * # +p = print message
 * # +f = include function name
 * # +l = include line number
 * # +m = include module name
 * # +t = include thread ID
 *
 * # Enable at boot (kernel command line)
 * # Add to GRUB: dyndbg="module mydriver +p"
 *
 *
 * ============================================================================
 *                    3. DEBUGFS - DEBUG FILESYSTEM
 * ============================================================================
 *
 * DebugFS provides a simple way to expose debug information to user-space.
 * Located at /sys/kernel/debug/ (requires root access)
 */

static struct dentry *debug_dir;
static u32 debug_counter = 0;
static char debug_buffer[256] = "Hello Debug";

/* Simple integer value */
/* debugfs_create_u32("counter", 0644, debug_dir, &debug_counter); */

/* Custom file operations for complex data */
static int debug_info_show(struct seq_file *s, void *v)
{
    seq_printf(s, "Counter: %u\n", debug_counter);
    seq_printf(s, "Buffer: %s\n", debug_buffer);
    seq_printf(s, "Jiffies: %lu\n", jiffies);
    return 0;
}

static int debug_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, debug_info_show, inode->i_private);
}

static const struct file_operations debug_info_fops = {
    .owner = THIS_MODULE,
    .open = debug_info_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static void setup_debugfs(void)
{
    /* Create directory /sys/kernel/debug/mydriver/ */
    debug_dir = debugfs_create_dir("mydriver", NULL);
    if (IS_ERR_OR_NULL(debug_dir)) {
        pr_err("Failed to create debugfs directory\n");
        return;
    }

    /* Create files */
    debugfs_create_u32("counter", 0644, debug_dir, &debug_counter);
    debugfs_create_file("info", 0444, debug_dir, NULL, &debug_info_fops);
    debugfs_create_bool("enabled", 0644, debug_dir, NULL);

    pr_info("DebugFS entries created at /sys/kernel/debug/mydriver/\n");
}

static void cleanup_debugfs(void)
{
    debugfs_remove_recursive(debug_dir);
}

/*
 * DEBUGFS USAGE:
 * --------------
 *
 * # Mount debugfs
 * sudo mount -t debugfs none /sys/kernel/debug
 *
 * # Read values
 * cat /sys/kernel/debug/mydriver/counter
 * cat /sys/kernel/debug/mydriver/info
 *
 * # Write values (if writable)
 * echo 100 | sudo tee /sys/kernel/debug/mydriver/counter
 *
 *
 * ============================================================================
 *                    4. FTRACE - FUNCTION TRACING
 * ============================================================================
 *
 * Ftrace traces function calls, latency, and events in the kernel.
 * Extremely powerful for understanding code flow.
 *
 * CONTROL: /sys/kernel/debug/tracing/
 *
 * BASIC USAGE:
 * ------------
 *
 * # Go to tracing directory
 * cd /sys/kernel/debug/tracing
 *
 * # View available tracers
 * cat available_tracers
 * # Output: function_graph function nop
 *
 * # Enable function tracer
 * echo function > current_tracer
 *
 * # Filter to specific functions
 * echo mydriver_* > set_ftrace_filter
 * # OR specific function
 * echo my_probe > set_ftrace_filter
 *
 * # Start tracing
 * echo 1 > tracing_on
 *
 * # ... do something to trigger the code ...
 *
 * # Stop tracing
 * echo 0 > tracing_on
 *
 * # View trace
 * cat trace
 *
 * # Clear trace buffer
 * echo > trace
 *
 *
 * FUNCTION GRAPH TRACER (shows call hierarchy):
 * ---------------------------------------------
 *
 * echo function_graph > current_tracer
 * echo mydriver_* > set_graph_function
 * echo 1 > tracing_on
 *
 * # Output shows call graph:
 * # my_probe() {
 * #   my_init_hw();
 * #   my_register_device() {
 * #     device_create();
 * #   }
 * # }
 *
 *
 * TRACING SPECIFIC MODULE:
 * ------------------------
 *
 * # Find functions in module
 * cat available_filter_functions | grep mydriver
 *
 * # Set filter
 * echo ':mod:mydriver' > set_ftrace_filter
 *
 *
 * EVENTS TRACING:
 * ---------------
 *
 * # List available events
 * cat available_events
 *
 * # Enable I2C events
 * echo 1 > events/i2c/enable
 *
 * # Enable specific event
 * echo 1 > events/i2c/i2c_read/enable
 *
 * # View trace
 * cat trace
 *
 *
 * TRACE-CMD (easier interface):
 * -----------------------------
 *
 * # Install
 * sudo apt install trace-cmd
 *
 * # Record trace
 * sudo trace-cmd record -p function -l 'mydriver_*'
 *
 * # View trace
 * trace-cmd report
 *
 * # Function graph
 * sudo trace-cmd record -p function_graph -g my_probe
 *
 *
 * ============================================================================
 *                    5. PROC AND SYS INSPECTION
 * ============================================================================
 *
 * USEFUL /PROC FILES:
 * -------------------
 *
 * # Loaded modules
 * cat /proc/modules
 * lsmod
 *
 * # Interrupt statistics
 * cat /proc/interrupts
 *
 * # I/O memory mappings
 * cat /proc/iomem
 *
 * # I/O ports
 * cat /proc/ioports
 *
 * # DMA information
 * cat /proc/dma
 *
 * # Kernel command line
 * cat /proc/cmdline
 *
 * # Memory info
 * cat /proc/meminfo
 *
 * # Device tree (if available)
 * ls /proc/device-tree/
 *
 *
 * USEFUL /SYS FILES:
 * ------------------
 *
 * # List device classes
 * ls /sys/class/
 *
 * # List devices on specific bus
 * ls /sys/bus/i2c/devices/
 * ls /sys/bus/platform/devices/
 *
 * # Driver information
 * ls /sys/bus/i2c/drivers/mydriver/
 *
 * # Device attributes
 * ls /sys/class/myclass/mydevice/
 * cat /sys/class/myclass/mydevice/status
 *
 * # Module parameters
 * ls /sys/module/mydriver/parameters/
 * cat /sys/module/mydriver/parameters/debug_level
 *
 * # Bind/unbind drivers manually
 * echo "1-0048" > /sys/bus/i2c/drivers/mydriver/unbind
 * echo "1-0048" > /sys/bus/i2c/drivers/mydriver/bind
 *
 *
 * ============================================================================
 *                    6. KERNEL DEBUGGING - OOPS AND PANIC
 * ============================================================================
 *
 * When the kernel crashes, it prints an "oops" (non-fatal) or "panic" (fatal).
 *
 * OOPS MESSAGE ANATOMY:
 * ---------------------
 *
 * [  123.456789] BUG: unable to handle kernel NULL pointer dereference at 0000000000000000
 * [  123.456790] IP: [<ffffffffa0001234>] my_function+0x42/0x100 [mydriver]
 * [  123.456791] PGD 0
 * [  123.456792] Oops: 0002 [#1] SMP
 * [  123.456793] CPU: 0 PID: 1234 Comm: my_process Tainted: P           O 5.10.0
 * [  123.456794] Hardware name: NVIDIA Jetson Nano
 * [  123.456795] task: ffff888012345678 task.stack: ffff888087654321
 * [  123.456796] RIP: 0010:my_function+0x42/0x100 [mydriver]
 * [  123.456797] RSP: 0018:ffff888087654000 EFLAGS: 00010246
 * [  123.456798] RAX: 0000000000000000 RBX: ffff888011111111 RCX: 0000000000000000
 * [  123.456799] ...
 * [  123.456800] Call Trace:
 * [  123.456801]  another_function+0x23/0x50 [mydriver]
 * [  123.456802]  i2c_device_probe+0x150/0x200
 * [  123.456803]  driver_probe_device+0x31/0x90
 * [  123.456804]  ...
 *
 * KEY INFORMATION:
 * - IP (Instruction Pointer): Where crash occurred
 * - Call Trace: Function call stack leading to crash
 * - Register values: CPU state at crash
 * - Tainted flags: P=proprietary module, O=out-of-tree module
 *
 *
 * DECODING CRASH ADDRESS:
 * -----------------------
 *
 * Method 1: Using addr2line
 *
 * # Get module .ko file with debug symbols
 * # Build with: CFLAGS += -g
 *
 * # Decode address
 * addr2line -e mydriver.ko 0x42
 * # Output: /path/to/mydriver.c:123
 *
 * # Or use objdump
 * objdump -dS mydriver.ko | less
 * # Search for the offset (0x42 in my_function)
 *
 *
 * Method 2: Using faddr2line (kernel script)
 *
 * # From kernel source directory
 * ./scripts/faddr2line vmlinux my_function+0x42/0x100
 *
 *
 * Method 3: Using decode_stacktrace.sh
 *
 * # Save oops to file, then:
 * ./scripts/decode_stacktrace.sh vmlinux < oops.txt
 *
 *
 * ============================================================================
 *                    7. KGDB - KERNEL DEBUGGER
 * ============================================================================
 *
 * KGDB allows using GDB to debug the kernel over serial connection.
 *
 * REQUIREMENTS:
 * - CONFIG_KGDB=y
 * - CONFIG_KGDB_SERIAL_CONSOLE=y
 * - Serial connection between target and host
 *
 * SETUP (on target Jetson):
 * -------------------------
 *
 * # Add to kernel command line (extlinux.conf):
 * # kgdboc=ttyS0,115200 kgdbwait
 *
 * # Or enable at runtime:
 * echo ttyS0 > /sys/module/kgdboc/parameters/kgdboc
 *
 * # Trigger breakpoint
 * echo g > /proc/sysrq-trigger
 *
 *
 * CONNECT FROM HOST:
 * ------------------
 *
 * # Start GDB with kernel symbols
 * aarch64-linux-gnu-gdb vmlinux
 *
 * (gdb) target remote /dev/ttyUSB0
 * (gdb) continue
 *
 * # Set breakpoint
 * (gdb) break my_probe
 * (gdb) continue
 *
 * # Examine variables
 * (gdb) print my_variable
 * (gdb) info registers
 *
 *
 * ============================================================================
 *                    8. KPROBES - DYNAMIC INSTRUMENTATION
 * ============================================================================
 *
 * Kprobes allow inserting breakpoints into running kernel code
 * WITHOUT recompiling.
 */

/* Example: Trace every call to kmalloc */
static struct kprobe kp = {
    .symbol_name = "kmalloc",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    /* Called before kmalloc executes */
    /* regs contains CPU registers - can read arguments */
    pr_info("kmalloc called\n");
    return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
                         unsigned long flags)
{
    /* Called after kmalloc returns */
    pr_info("kmalloc returned\n");
}

static void setup_kprobe(void)
{
    int ret;

    kp.pre_handler = handler_pre;
    kp.post_handler = handler_post;

    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("register_kprobe failed: %d\n", ret);
        return;
    }
    pr_info("Kprobe registered at %p\n", kp.addr);
}

static void cleanup_kprobe(void)
{
    unregister_kprobe(&kp);
}

/*
 * KPROBE VIA COMMAND LINE (no code needed):
 * -----------------------------------------
 *
 * # Enable event tracing with kprobe
 * cd /sys/kernel/debug/tracing
 *
 * # Create kprobe
 * echo 'p:myprobe my_function arg1=%x0 arg2=%x1' > kprobe_events
 *
 * # Enable it
 * echo 1 > events/kprobes/myprobe/enable
 *
 * # View trace
 * cat trace
 *
 * # Remove probe
 * echo '-:myprobe' >> kprobe_events
 *
 *
 * ============================================================================
 *                    9. MEMORY DEBUGGING
 * ============================================================================
 *
 * KASAN (Kernel Address Sanitizer):
 * ---------------------------------
 * Detects memory errors: use-after-free, out-of-bounds access
 *
 * # Enable in kernel config
 * CONFIG_KASAN=y
 *
 * # Will print detailed report on memory errors
 *
 *
 * SLUB DEBUG:
 * -----------
 * Debug slab allocator issues
 *
 * # Boot with:
 * slub_debug=FPUZ
 *
 * # F = sanity checks
 * # P = poisoning
 * # U = user tracking
 * # Z = red zoning
 *
 *
 * KMEMLEAK:
 * ---------
 * Detect memory leaks
 *
 * # Enable in kernel config
 * CONFIG_DEBUG_KMEMLEAK=y
 *
 * # Trigger scan
 * echo scan > /sys/kernel/debug/kmemleak
 *
 * # View leaks
 * cat /sys/kernel/debug/kmemleak
 *
 *
 * DUMP MEMORY:
 * ------------
 */

static void dump_memory_example(void *ptr, size_t len)
{
    /* print_hex_dump shows memory contents */
    print_hex_dump(KERN_INFO, "data: ", DUMP_PREFIX_OFFSET,
                   16, 1, ptr, len, true);

    /*
     * Output format:
     * data: 00000000: 48 65 6c 6c 6f 20 57 6f 72 6c 64 00              Hello World.
     */

    /* Or use print_hex_dump_bytes for simple dump */
    print_hex_dump_bytes("bytes: ", DUMP_PREFIX_NONE, ptr, len);
}


/*
 * ============================================================================
 *                    10. COMMON DEBUGGING SCENARIOS
 * ============================================================================
 *
 * SCENARIO 1: Driver doesn't load
 * -------------------------------
 *
 * # Check kernel messages
 * dmesg | tail -50
 *
 * # Check module info
 * modinfo mydriver.ko
 *
 * # Check kernel version match
 * uname -r
 * modinfo mydriver.ko | grep vermagic
 *
 * # Check dependencies
 * modprobe --show-depends mydriver
 *
 * # Check for symbol errors
 * dmesg | grep -i "unknown symbol"
 *
 *
 * SCENARIO 2: Driver loads but device not detected
 * ------------------------------------------------
 *
 * # Check device tree
 * ls /proc/device-tree/
 * find /proc/device-tree -name "*mydevice*"
 *
 * # Check compatible string
 * cat /proc/device-tree/path/to/device/compatible
 *
 * # Check device status
 * cat /proc/device-tree/path/to/device/status
 *
 * # Check bus for device
 * ls /sys/bus/i2c/devices/
 * i2cdetect -y 1
 *
 * # Check driver binding
 * ls /sys/bus/i2c/drivers/mydriver/
 *
 * # Enable dynamic debug for probe
 * echo 'func my_probe +p' > /sys/kernel/debug/dynamic_debug/control
 *
 *
 * SCENARIO 3: Driver crashes during operation
 * -------------------------------------------
 *
 * # Save dmesg before it scrolls
 * dmesg > crash_log.txt
 *
 * # Decode the oops
 * addr2line -e mydriver.ko <offset>
 *
 * # Add debug prints around suspicious code
 * pr_info("Before risky operation\n");
 * risky_operation();
 * pr_info("After risky operation\n");
 *
 * # Use ftrace to see last functions called
 * echo function > /sys/kernel/debug/tracing/current_tracer
 * echo mydriver_* > /sys/kernel/debug/tracing/set_ftrace_filter
 *
 *
 * SCENARIO 4: Data corruption
 * ---------------------------
 *
 * # Enable KASAN if available
 * # Add memory dumps
 * print_hex_dump(KERN_INFO, "data: ", DUMP_PREFIX_OFFSET,
 *                16, 1, buffer, len, true);
 *
 * # Check for race conditions - use lockdep
 * CONFIG_PROVE_LOCKING=y
 *
 *
 * SCENARIO 5: Performance issues
 * ------------------------------
 *
 * # Use ftrace to measure function latency
 * echo function_graph > /sys/kernel/debug/tracing/current_tracer
 * echo funcgraph-duration > /sys/kernel/debug/tracing/trace_options
 *
 * # Use perf for profiling
 * sudo perf top
 * sudo perf record -g -- sleep 10
 * sudo perf report
 *
 *
 * ============================================================================
 *                    DEBUGGING CHEAT SHEET
 * ============================================================================
 *
 * +------------------------------------------+--------------------------------+
 * | Problem                                   | Tool/Command                   |
 * +------------------------------------------+--------------------------------+
 * | Module won't load                         | dmesg, modinfo, depmod -a      |
 * | Device not detected                       | /proc/device-tree, i2cdetect   |
 * | Probe not called                          | dynamic debug on probe func    |
 * | Crash/oops                                | addr2line, objdump             |
 * | See function calls                        | ftrace, trace-cmd              |
 * | Memory corruption                         | KASAN, slub_debug              |
 * | Memory leak                               | kmemleak                       |
 * | Race condition                            | lockdep, KCSAN                 |
 * | Performance                               | perf, ftrace latency           |
 * | Live kernel debugging                     | KGDB                           |
 * | Dynamic instrumentation                   | kprobes                        |
 * | Expose debug info                         | debugfs                        |
 * +------------------------------------------+--------------------------------+
 *
 *
 * ============================================================================
 *                    BUILD WITH DEBUG SYMBOLS
 * ============================================================================
 *
 * Makefile:
 *
 * ccflags-y += -g -DDEBUG
 * # -g     : Include debug symbols
 * # -DDEBUG: Enable pr_debug() messages
 *
 * # Or in driver code:
 * #define DEBUG   // Before includes - enables pr_debug
 * #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt  // Custom prefix
 *
 *
 * ============================================================================
 *                    ESSENTIAL DEBUG MACROS
 * ============================================================================
 */

/* Add these to your driver for debugging */

/* Compile-time debug enable */
#define MYDRV_DEBUG 1

/* Custom debug macro */
#if MYDRV_DEBUG
#define mydrv_dbg(fmt, ...) \
    pr_debug("%s:%d: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define mydrv_dbg(fmt, ...) do { } while (0)
#endif

/* Assert macro */
#define mydrv_assert(cond, fmt, ...) do { \
    if (unlikely(!(cond))) { \
        pr_err("ASSERTION FAILED: %s:%d: " fmt, \
               __func__, __LINE__, ##__VA_ARGS__); \
        dump_stack(); \
    } \
} while (0)

/* Function entry/exit tracing */
#define TRACE_ENTRY() pr_debug(">>> %s\n", __func__)
#define TRACE_EXIT()  pr_debug("<<< %s\n", __func__)

static void example_function(void)
{
    TRACE_ENTRY();

    mydrv_dbg("Processing started\n");

    mydrv_assert(1 == 1, "Math is broken!\n");

    TRACE_EXIT();
}


/*
 * ============================================================================
 *                    INTERVIEW QUESTIONS
 * ============================================================================
 *
 * Q1: What's the difference between printk levels?
 * A: KERN_ERR for errors, KERN_INFO for normal info, KERN_DEBUG for
 *    development debugging (not shown by default).
 *
 * Q2: How do you debug a kernel crash?
 * A: Save dmesg output, identify the crash address from oops,
 *    use addr2line or objdump to find source line, examine call trace.
 *
 * Q3: What is dynamic debug?
 * A: Ability to enable/disable pr_debug() messages at runtime
 *    via /sys/kernel/debug/dynamic_debug/control.
 *
 * Q4: How do you trace function calls in the kernel?
 * A: Use ftrace: set current_tracer to function/function_graph,
 *    filter to specific functions, enable tracing.
 *
 * Q5: What tools detect memory errors?
 * A: KASAN (use-after-free, out-of-bounds), kmemleak (memory leaks),
 *    slub_debug (slab corruption).
 *
 * Q6: How do you debug without modifying source code?
 * A: Use kprobes for dynamic instrumentation, ftrace for tracing,
 *    /sys and /proc for inspection.
 *
 * Q7: What does KGDB do?
 * A: KGDB enables GDB debugging of the kernel over serial connection,
 *    allowing breakpoints, stepping, and variable inspection.
 *
 * Q8: How do you find what went wrong in a crash?
 * A: Look at the IP (instruction pointer) for crash location,
 *    call trace for execution path, register values for context.
 *
 *
 * ============================================================================
 *                              END OF GUIDE
 * ============================================================================
 */

static int __init debug_demo_init(void)
{
    pr_info("Debug Demo Module loaded\n");
    printk_examples();
    setup_debugfs();
    example_function();
    return 0;
}

static void __exit debug_demo_exit(void)
{
    cleanup_debugfs();
    pr_info("Debug Demo Module unloaded\n");
}

module_init(debug_demo_init);
module_exit(debug_demo_exit);
