/*
 * Device Tree Overlays & L4T Kernel Driver Loading - Complete Guide
 *
 * ============================================================================
 *                    WHAT IS DEVICE TREE?
 * ============================================================================
 *
 * Device Tree (DT) is a data structure that describes hardware to the kernel.
 * Instead of hardcoding hardware info in drivers, DT provides a flexible way
 * to describe the system's hardware topology.
 *
 * WHY DEVICE TREE?
 * ----------------
 * Before DT: Each board needed custom kernel with hardcoded hardware info
 * After DT:  One kernel binary works on many boards, DT describes differences
 *
 * KEY FILES:
 * ----------
 * .dts  - Device Tree Source (human-readable)
 * .dtsi - Device Tree Source Include (reusable fragments)
 * .dtb  - Device Tree Blob (compiled binary, loaded by bootloader)
 * .dtbo - Device Tree Blob Overlay (compiled overlay)
 *
 * LOCATION IN KERNEL SOURCE:
 * --------------------------
 * arch/arm/boot/dts/           - 32-bit ARM boards
 * arch/arm64/boot/dts/         - 64-bit ARM boards (including Jetson)
 * arch/arm64/boot/dts/nvidia/  - NVIDIA Jetson specific
 *
 *
 * ============================================================================
 *                    DEVICE TREE SYNTAX
 * ============================================================================
 *
 * BASIC STRUCTURE:
 *
 * /dts-v1/;                    // DTS version declaration
 *
 * / {                          // Root node
 *     compatible = "nvidia,jetson-nano";  // Board compatibility
 *     model = "NVIDIA Jetson Nano";
 *
 *     // Child nodes represent hardware
 *     memory@80000000 {
 *         device_type = "memory";
 *         reg = <0x80000000 0x40000000>;  // base address, size
 *     };
 *
 *     cpus {
 *         cpu@0 {
 *             compatible = "arm,cortex-a57";
 *         };
 *     };
 * };
 *
 *
 * NODE FORMAT:
 * ------------
 * node-name@unit-address {
 *     property = value;
 *     child-node {
 *         ...
 *     };
 * };
 *
 * - node-name: Descriptive name (i2c, spi, gpio, etc.)
 * - unit-address: Base address (must match 'reg' property)
 *
 *
 * COMMON PROPERTIES:
 * ------------------
 *
 * compatible = "vendor,device";
 *   - Most important property
 *   - Driver matches via this string
 *   - Can have multiple entries (fallback): "vendor,device-v2", "vendor,device"
 *
 * reg = <address size>;
 *   - Register address and size
 *   - For memory-mapped peripherals
 *
 * interrupts = <type number flags>;
 *   - Interrupt specification
 *   - Type: 0=SPI (Shared), 1=PPI (Private)
 *   - Flags: trigger type (edge/level, high/low)
 *
 * status = "okay" | "disabled";
 *   - Enable or disable the node
 *   - Overlays often change this
 *
 * clocks = <&clock_controller clock_id>;
 *   - Reference to clock source
 *
 * pinctrl-0 = <&pinmux_state>;
 *   - Pin multiplexing configuration
 *
 *
 * PHANDLE REFERENCES:
 * -------------------
 * Phandles allow nodes to reference other nodes:
 *
 * clock_controller: clock@1000 {
 *     #clock-cells = <1>;       // How many cells in clock specifier
 * };
 *
 * my_device: device@2000 {
 *     clocks = <&clock_controller 5>;  // Reference clock_controller, clock ID 5
 * };
 *
 *
 * ============================================================================
 *                    DEVICE TREE OVERLAYS
 * ============================================================================
 *
 * WHAT ARE OVERLAYS?
 * ------------------
 * Overlays modify the base Device Tree at runtime without recompiling.
 * Perfect for:
 * - Add-on boards (HATs, capes, shields)
 * - Enabling optional peripherals
 * - Changing pin configurations
 * - Development and testing
 *
 * OVERLAY SYNTAX:
 * ---------------
 */

/*
 * Example 1: Basic Overlay to Enable I2C Device
 * File: my_i2c_sensor.dts
 */
#if 0  /* DTS syntax - not C code */

/dts-v1/;
/plugin/;           /* <-- This marks it as an overlay */

/*
 * Fragment targeting existing node
 * __overlay__ contains modifications
 */
/ {
    fragment@0 {
        target = <&i2c1>;           /* Target the i2c1 bus */
        __overlay__ {
            #address-cells = <1>;
            #size-cells = <0>;
            status = "okay";        /* Enable the bus */

            /* Add new device node */
            temp_sensor: tmp102@48 {
                compatible = "ti,tmp102";
                reg = <0x48>;       /* I2C address */
                status = "okay";
            };
        };
    };
};

/*
 * Alternative syntax using target-path (simpler):
 */
/dts-v1/;
/plugin/;

&i2c1 {                             /* Direct reference to node */
    #address-cells = <1>;
    #size-cells = <0>;
    status = "okay";

    my_sensor@48 {
        compatible = "mycompany,mysensor";
        reg = <0x48>;
    };
};

#endif /* End DTS example */


/*
 * ============================================================================
 *                    HOW OVERLAYS CONNECT TO DRIVERS
 * ============================================================================
 *
 * DRIVER MATCHING PROCESS:
 * ------------------------
 *
 *   1. Bootloader loads DTB (base device tree)
 *   2. Bootloader/kernel loads DTBO (overlays)
 *   3. Overlays merged into device tree
 *   4. Kernel parses merged tree
 *   5. For each node, kernel finds matching driver via "compatible" string
 *   6. Driver's probe() function called with device info
 *
 *
 *   +------------------+
 *   |   Base DTB       |     +------------------+
 *   | (hardware desc)  | --> |   Merged DT      |
 *   +------------------+     +------------------+
 *            +                        |
 *   +------------------+              v
 *   | Overlay DTBO     |     +------------------+
 *   | (modifications)  |     |   Kernel         |
 *   +------------------+     | Device Matching  |
 *                            +------------------+
 *                                     |
 *                                     v
 *                            +------------------+
 *                            |   Driver probe() |
 *                            |   called         |
 *                            +------------------+
 *
 *
 * MATCHING EXAMPLE:
 * -----------------
 *
 * Device Tree Node:
 *   my_sensor@48 {
 *       compatible = "mycompany,mysensor", "generic,i2c-sensor";
 *       reg = <0x48>;
 *   };
 *
 * Driver Code:
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_device.h>

/*
 * of_device_id table - MUST match "compatible" strings in DT
 * This is how the kernel knows which driver to use
 */
static const struct of_device_id mysensor_of_match[] = {
    { .compatible = "mycompany,mysensor", .data = (void *)1 },
    { .compatible = "generic,i2c-sensor", .data = (void *)0 },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mysensor_of_match);

/*
 * probe() is called when:
 * 1. Device Tree node has matching "compatible" string
 * 2. Device status = "okay" (not "disabled")
 */
static int mysensor_probe(struct i2c_client *client)
{
    const struct of_device_id *match;
    struct device *dev = &client->dev;

    /* Get matched entry (useful if driver supports multiple devices) */
    match = of_match_device(mysensor_of_match, dev);
    if (!match) {
        dev_err(dev, "No matching device found\n");
        return -ENODEV;
    }

    dev_info(dev, "Probed! compatible=%s, data=%ld\n",
             match->compatible, (long)match->data);

    /*
     * Read custom properties from Device Tree
     */
    {
        u32 custom_value;
        const char *label;

        /* Read integer property */
        if (of_property_read_u32(dev->of_node, "mycompany,custom-prop",
                                 &custom_value) == 0) {
            dev_info(dev, "Custom property: %u\n", custom_value);
        }

        /* Read string property */
        if (of_property_read_string(dev->of_node, "label", &label) == 0) {
            dev_info(dev, "Label: %s\n", label);
        }

        /* Check if property exists (boolean) */
        if (of_property_read_bool(dev->of_node, "mycompany,feature-enabled")) {
            dev_info(dev, "Feature is enabled\n");
        }
    }

    return 0;
}

static void mysensor_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "Removed\n");
}

static struct i2c_driver mysensor_driver = {
    .driver = {
        .name = "mysensor",
        .of_match_table = mysensor_of_match,
    },
    .probe = mysensor_probe,
    .remove = mysensor_remove,
};
module_i2c_driver(mysensor_driver);


/*
 * ============================================================================
 *                    COMPILING DEVICE TREE OVERLAYS
 * ============================================================================
 *
 * INSTALL DTC (Device Tree Compiler):
 * -----------------------------------
 * # Ubuntu/Debian
 * sudo apt-get install device-tree-compiler
 *
 * # From kernel source (has latest features)
 * make scripts/dtc
 *
 *
 * COMPILE OVERLAY:
 * ----------------
 * # Basic compilation
 * dtc -@ -I dts -O dtb -o my_overlay.dtbo my_overlay.dts
 *
 * # -@  : Generate symbols (required for overlays)
 * # -I  : Input format (dts = source)
 * # -O  : Output format (dtb = binary)
 * # -o  : Output file
 *
 * # With warnings
 * dtc -@ -W no-unit_address_vs_reg -I dts -O dtb -o my_overlay.dtbo my_overlay.dts
 *
 * # Include search path
 * dtc -@ -I dts -O dtb -i include_dir/ -o my_overlay.dtbo my_overlay.dts
 *
 *
 * DECOMPILE (for debugging):
 * --------------------------
 * # Convert DTB back to DTS
 * dtc -I dtb -O dts -o output.dts input.dtb
 *
 * # Dump live device tree from running system
 * dtc -I fs /sys/firmware/devicetree/base -O dts -o live_dt.dts
 *
 *
 * ============================================================================
 *                    LOADING OVERLAYS - GENERIC LINUX
 * ============================================================================
 *
 * METHOD 1: ConfigFS (Runtime loading)
 * ------------------------------------
 * # Check if configfs overlay support is enabled
 * ls /sys/kernel/config/device-tree/overlays/
 *
 * # Create overlay directory
 * sudo mkdir /sys/kernel/config/device-tree/overlays/my_overlay
 *
 * # Load overlay
 * sudo cp my_overlay.dtbo /sys/kernel/config/device-tree/overlays/my_overlay/dtbo
 * # OR
 * sudo cat my_overlay.dtbo > /sys/kernel/config/device-tree/overlays/my_overlay/dtbo
 *
 * # Check status
 * cat /sys/kernel/config/device-tree/overlays/my_overlay/status
 * # Output: "applied" or error message
 *
 * # Remove overlay
 * sudo rmdir /sys/kernel/config/device-tree/overlays/my_overlay
 *
 *
 * METHOD 2: U-Boot (At boot time)
 * -------------------------------
 * # In U-Boot prompt:
 * load mmc 0:1 ${fdt_addr} tegra210-p3448-0000-p3449-0000-a02.dtb
 * load mmc 0:1 ${fdto_addr} my_overlay.dtbo
 * fdt addr ${fdt_addr}
 * fdt resize 8192
 * fdt apply ${fdto_addr}
 * # Then boot
 *
 *
 * METHOD 3: Bootloader config file
 * --------------------------------
 * # Raspberry Pi (config.txt):
 * dtoverlay=my_overlay
 * dtparam=i2c_arm=on
 *
 * # BeagleBone (uEnv.txt):
 * dtb_overlay=/lib/firmware/my_overlay.dtbo
 *
 *
 * ============================================================================
 *                    L4T (LINUX FOR TEGRA) - NVIDIA JETSON
 * ============================================================================
 *
 * L4T is NVIDIA's Linux distribution for Jetson platforms:
 * - Jetson Nano
 * - Jetson TX1/TX2
 * - Jetson Xavier NX/AGX
 * - Jetson Orin
 *
 * L4T KERNEL SOURCE:
 * ------------------
 * # Download L4T source
 * # Go to: https://developer.nvidia.com/embedded/linux-tegra
 * # Or use:
 * git clone https://github.com/OE4T/linux-tegra.git
 * cd linux-tegra
 * git checkout l4t/l4t-r35.4.1  # Example: JetPack 5.1.2
 *
 *
 * L4T DEVICE TREE LOCATIONS:
 * --------------------------
 * # Hardware folder (Jetson-specific)
 * nvidia/platform/t210/                    # Nano, TX1
 * nvidia/platform/t186/                    # TX2
 * nvidia/platform/t194/                    # Xavier
 * nvidia/platform/t234/                    # Orin
 *
 * # Main DTB files
 * tegra210-p3448-0000-p3449-0000-a02.dtb   # Jetson Nano Dev Kit
 * tegra186-quill-p3310-1000-c03-00-base.dtb # TX2 Dev Kit
 * tegra194-p2888-0001-p2822-0000.dtb       # Xavier Dev Kit
 *
 *
 * ============================================================================
 *                    L4T OVERLAY LOADING METHODS
 * ============================================================================
 *
 * METHOD 1: extlinux.conf (Recommended for Development)
 * -----------------------------------------------------
 * Edit: /boot/extlinux/extlinux.conf
 *
 * TIMEOUT 30
 * DEFAULT primary
 *
 * MENU TITLE L4T Bootloader
 *
 * LABEL primary
 *     MENU LABEL primary kernel
 *     LINUX /boot/Image
 *     INITRD /boot/initrd
 *     FDT /boot/dtb/kernel_tegra210-p3448-0000-p3449-0000-a02.dtb
 *     # Add overlays:
 *     FDTOVERLAYS /boot/my_overlay.dtbo /boot/another_overlay.dtbo
 *     APPEND ${cbootargs} root=/dev/mmcblk0p1 rw rootwait
 *
 * # After editing, reboot to apply
 *
 *
 * METHOD 2: Jetson-IO Tool (For Pin Configuration)
 * ------------------------------------------------
 * # Interactive tool for configuring Jetson expansion headers
 * sudo /opt/nvidia/jetson-io/jetson-io.py
 *
 * # This generates and applies overlays for:
 * # - SPI, I2C, UART on expansion header
 * # - PWM pins
 * # - GPIO configurations
 *
 *
 * METHOD 3: Modify Base DTB (For Production)
 * ------------------------------------------
 * # Decompile existing DTB
 * dtc -I dtb -O dts -o base.dts /boot/dtb/kernel_tegra210-p3448-0000-p3449-0000-a02.dtb
 *
 * # Edit base.dts with your changes
 * # Recompile
 * dtc -I dts -O dtb -o modified.dtb base.dts
 *
 * # Replace (backup original first!)
 * sudo cp modified.dtb /boot/dtb/kernel_tegra210-p3448-0000-p3449-0000-a02.dtb
 *
 *
 * METHOD 4: CBoot Configuration (Advanced)
 * ----------------------------------------
 * # CBoot is Jetson's bootloader
 * # Can apply overlays before kernel boot
 *
 * # Flash with modified DTB
 * cd Linux_for_Tegra
 * sudo ./flash.sh -r -k DTB jetson-nano-devkit mmcblk0p1
 *
 *
 * ============================================================================
 *                    L4T DRIVER LOADING
 * ============================================================================
 *
 * STEP 1: Build Driver as Module
 * ------------------------------
 */

/* Example: Makefile for L4T kernel module */
#if 0  /* Makefile content */

# Makefile for L4T kernel module

# Module name
obj-m += mydriver.o

# For multi-file modules
# mydriver-objs := main.o hardware.o utils.o

# Kernel source directory
# Option 1: Use installed headers (if available)
KDIR ?= /lib/modules/$(shell uname -r)/build

# Option 2: Use downloaded L4T kernel source
# KDIR ?= /path/to/Linux_for_Tegra/source/public/kernel/kernel-5.10

# Cross-compile settings (if building on x86 for ARM)
# ARCH ?= arm64
# CROSS_COMPILE ?= aarch64-linux-gnu-

# Current directory
PWD := $(shell pwd)

# Targets
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Cross-compile target
cross:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

# Install to Jetson (requires SSH access)
deploy:
	scp mydriver.ko nvidia@jetson:/home/nvidia/
	ssh nvidia@jetson "sudo insmod /home/nvidia/mydriver.ko"

#endif /* End Makefile */


/*
 * STEP 2: Cross-Compile Setup (Building on x86 Host)
 * ---------------------------------------------------
 *
 * # Install cross-compiler
 * sudo apt-get install gcc-aarch64-linux-gnu
 *
 * # Download L4T kernel source
 * # From: https://developer.nvidia.com/embedded/linux-tegra
 * tar xf public_sources.tbz2
 * cd Linux_for_Tegra/source/public
 * tar xf kernel_src.tbz2
 * cd kernel/kernel-5.10  # or kernel-4.9 for older L4T
 *
 * # Configure kernel
 * make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- tegra_defconfig
 *
 * # Build kernel (needed for modules)
 * make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)
 *
 * # Now build your module against this kernel
 * cd /path/to/your/driver
 * make KDIR=/path/to/kernel/kernel-5.10 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
 *
 *
 * STEP 3: Deploy and Load Driver
 * ------------------------------
 *
 * # Copy to Jetson
 * scp mydriver.ko nvidia@<jetson-ip>:/home/nvidia/
 *
 * # On Jetson:
 * # Load module
 * sudo insmod mydriver.ko
 *
 * # Or with parameters
 * sudo insmod mydriver.ko debug_level=2 buffer_size=4096
 *
 * # Check if loaded
 * lsmod | grep mydriver
 *
 * # View kernel messages
 * dmesg | tail -20
 *
 * # Unload module
 * sudo rmmod mydriver
 *
 *
 * STEP 4: Auto-load at Boot
 * -------------------------
 *
 * # Copy module to standard location
 * sudo cp mydriver.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
 *
 * # Update module database
 * sudo depmod -a
 *
 * # Load at boot (create config file)
 * echo "mydriver" | sudo tee /etc/modules-load.d/mydriver.conf
 *
 * # Or add to /etc/modules
 * echo "mydriver" | sudo tee -a /etc/modules
 *
 *
 * ============================================================================
 *                    COMPLETE L4T OVERLAY EXAMPLE
 * ============================================================================
 *
 * SCENARIO: Add custom I2C sensor to Jetson Nano expansion header
 *
 * Step 1: Create Overlay
 */

#if 0  /* DTS file: jetson_nano_i2c_sensor.dts */

/dts-v1/;
/plugin/;

/*
 * Jetson Nano has I2C bus 1 on expansion header pins:
 * - Pin 3: I2C1_SDA (GPIO_PJ.03)
 * - Pin 5: I2C1_SCL (GPIO_PJ.02)
 */

/ {
    overlay-name = "Jetson Nano Custom I2C Sensor";
    jetson-header-name = "Jetson 40pin Header";
    compatible = "nvidia,p3449-0000-b00+p3448-0002-b00";

    /* Enable I2C1 and add our sensor */
    fragment@0 {
        target-path = "/i2c@7000c400";  /* I2C1 on Tegra210 */
        __overlay__ {
            status = "okay";
            #address-cells = <1>;
            #size-cells = <0>;

            /* Custom sensor at address 0x48 */
            custom_sensor: mysensor@48 {
                compatible = "mycompany,custom-sensor";
                reg = <0x48>;
                status = "okay";

                /* Custom properties */
                mycompany,sample-rate = <100>;  /* Hz */
                mycompany,resolution = <12>;    /* bits */
                label = "temperature-sensor-1";

                /* Interrupt (optional) */
                /* interrupt-parent = <&gpio>; */
                /* interrupts = <TEGRA_GPIO(C, 7) IRQ_TYPE_EDGE_FALLING>; */
            };
        };
    };

    /* Pin muxing (if needed) */
    fragment@1 {
        target-path = "/pinmux@700008d4";
        __overlay__ {
            i2c1_pins: i2c1-pins {
                nvidia,pins = "gen2_i2c_sda_pj3", "gen2_i2c_scl_pj2";
                nvidia,function = "i2c2";
                nvidia,pull = <TEGRA_PIN_PULL_UP>;
                nvidia,tristate = <TEGRA_PIN_DISABLE>;
                nvidia,enable-input = <TEGRA_PIN_ENABLE>;
            };
        };
    };
};

#endif /* End DTS file */


/*
 * Step 2: Compile Overlay
 *
 * # On Jetson or cross-compile host:
 * dtc -@ -I dts -O dtb -o jetson_nano_i2c_sensor.dtbo jetson_nano_i2c_sensor.dts
 *
 * Step 3: Install Overlay
 *
 * sudo cp jetson_nano_i2c_sensor.dtbo /boot/
 *
 * Step 4: Edit extlinux.conf
 *
 * sudo nano /boot/extlinux/extlinux.conf
 * # Add: FDTOVERLAYS /boot/jetson_nano_i2c_sensor.dtbo
 *
 * Step 5: Reboot and Verify
 *
 * sudo reboot
 *
 * # After reboot, check device tree:
 * ls /proc/device-tree/i2c@7000c400/mysensor@48/
 * cat /proc/device-tree/i2c@7000c400/mysensor@48/compatible
 *
 * # Should see your sensor node
 */


/*
 * ============================================================================
 *                    COMPLETE DRIVER FOR L4T
 * ============================================================================
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#define DRIVER_NAME "custom_sensor"

/* Module parameters (configurable at load time) */
static int debug_level = 0;
module_param(debug_level, int, 0644);
MODULE_PARM_DESC(debug_level, "Debug level (0=off, 1=info, 2=verbose)");

/* Device data structure */
struct custom_sensor_data {
    struct i2c_client *client;
    struct mutex lock;
    u32 sample_rate;
    u32 resolution;
    char label[32];
};

/* Read properties from Device Tree */
static int custom_sensor_parse_dt(struct custom_sensor_data *data)
{
    struct device *dev = &data->client->dev;
    struct device_node *np = dev->of_node;
    int ret;

    /* Read sample rate */
    ret = of_property_read_u32(np, "mycompany,sample-rate", &data->sample_rate);
    if (ret) {
        data->sample_rate = 10;  /* Default */
        dev_warn(dev, "Using default sample rate: %u Hz\n", data->sample_rate);
    }

    /* Read resolution */
    ret = of_property_read_u32(np, "mycompany,resolution", &data->resolution);
    if (ret) {
        data->resolution = 10;  /* Default */
    }

    /* Read label */
    ret = of_property_read_string(np, "label", (const char **)&data->label);
    if (ret) {
        strscpy(data->label, "unknown", sizeof(data->label));
    }

    if (debug_level >= 1) {
        dev_info(dev, "DT config: rate=%u Hz, res=%u bits, label=%s\n",
                 data->sample_rate, data->resolution, data->label);
    }

    return 0;
}

/* I2C read helper */
static int sensor_read_reg(struct custom_sensor_data *data, u8 reg, u8 *val)
{
    int ret;

    ret = i2c_smbus_read_byte_data(data->client, reg);
    if (ret < 0) {
        dev_err(&data->client->dev, "Failed to read reg 0x%02x: %d\n", reg, ret);
        return ret;
    }

    *val = ret;
    return 0;
}

/* Sysfs attribute: show sensor value */
static ssize_t value_show(struct device *dev, struct device_attribute *attr,
                          char *buf)
{
    struct custom_sensor_data *data = dev_get_drvdata(dev);
    u8 val;
    int ret;

    mutex_lock(&data->lock);
    ret = sensor_read_reg(data, 0x00, &val);  /* Example: read temp register */
    mutex_unlock(&data->lock);

    if (ret)
        return ret;

    return sysfs_emit(buf, "%u\n", val);
}
static DEVICE_ATTR_RO(value);

/* Sysfs attribute: show label */
static ssize_t label_show(struct device *dev, struct device_attribute *attr,
                          char *buf)
{
    struct custom_sensor_data *data = dev_get_drvdata(dev);
    return sysfs_emit(buf, "%s\n", data->label);
}
static DEVICE_ATTR_RO(label);

static struct attribute *custom_sensor_attrs[] = {
    &dev_attr_value.attr,
    &dev_attr_label.attr,
    NULL
};
ATTRIBUTE_GROUPS(custom_sensor);

/* Probe function - called when DT node matches */
static int custom_sensor_probe(struct i2c_client *client)
{
    struct custom_sensor_data *data;
    struct device *dev = &client->dev;
    int ret;

    dev_info(dev, "Probing custom sensor at address 0x%02x\n", client->addr);

    /* Allocate device data */
    data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    mutex_init(&data->lock);

    /* Parse Device Tree */
    ret = custom_sensor_parse_dt(data);
    if (ret)
        return ret;

    /* Verify device communication */
    {
        u8 id;
        ret = sensor_read_reg(data, 0x00, &id);  /* Read chip ID register */
        if (ret) {
            dev_err(dev, "Failed to communicate with sensor\n");
            /* Comment out for testing without real hardware */
            /* return ret; */
        } else {
            dev_info(dev, "Sensor ID: 0x%02x\n", id);
        }
    }

    /* Save private data */
    i2c_set_clientdata(client, data);

    dev_info(dev, "Custom sensor initialized: %s\n", data->label);
    return 0;
}

static void custom_sensor_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "Custom sensor removed\n");
}

/* Device Tree match table */
static const struct of_device_id custom_sensor_of_match[] = {
    { .compatible = "mycompany,custom-sensor" },
    { }
};
MODULE_DEVICE_TABLE(of, custom_sensor_of_match);

/* I2C device ID table (for non-DT systems) */
static const struct i2c_device_id custom_sensor_id[] = {
    { "custom-sensor", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, custom_sensor_id);

/* I2C driver structure */
static struct i2c_driver custom_sensor_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = custom_sensor_of_match,
        .dev_groups = custom_sensor_groups,  /* Sysfs attributes */
    },
    .probe = custom_sensor_probe,
    .remove = custom_sensor_remove,
    .id_table = custom_sensor_id,
};

module_i2c_driver(custom_sensor_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom I2C Sensor Driver for Jetson");
MODULE_VERSION("1.0");


/*
 * ============================================================================
 *                    DEBUGGING DEVICE TREE ISSUES
 * ============================================================================
 *
 * CHECK IF OVERLAY APPLIED:
 * -------------------------
 * # List loaded overlays
 * ls /sys/kernel/config/device-tree/overlays/
 *
 * # View live device tree
 * ls /proc/device-tree/
 *
 * # Find your node
 * find /proc/device-tree -name "mysensor*"
 *
 * # Read properties
 * cat /proc/device-tree/i2c@7000c400/mysensor@48/compatible
 * xxd /proc/device-tree/i2c@7000c400/mysensor@48/reg
 *
 * # Dump full device tree
 * dtc -I fs /sys/firmware/devicetree/base -O dts -o live.dts
 * grep -A10 "mysensor" live.dts
 *
 *
 * CHECK DRIVER BINDING:
 * ---------------------
 * # See if driver bound to device
 * ls -la /sys/bus/i2c/devices/
 * ls -la /sys/bus/i2c/drivers/custom_sensor/
 *
 * # Check kernel messages for probe
 * dmesg | grep -i "custom_sensor\|mysensor"
 *
 * # Check driver info
 * modinfo custom_sensor.ko
 *
 *
 * COMMON ISSUES:
 * --------------
 *
 * 1. "compatible" mismatch
 *    - DT: compatible = "mycompany,custom-sensor"
 *    - Driver: { .compatible = "mycompany,custom_sensor" }  // Wrong!
 *    - Ensure EXACT match including hyphens vs underscores
 *
 * 2. Status not "okay"
 *    - Check: cat /proc/device-tree/.../status
 *    - Overlay should set status = "okay"
 *
 * 3. Parent node disabled
 *    - If I2C bus node is disabled, child nodes won't probe
 *    - Enable parent: status = "okay" on i2c@... node
 *
 * 4. Overlay not loading
 *    - Check extlinux.conf syntax
 *    - Verify .dtbo file exists at specified path
 *    - Check dmesg for overlay errors
 *
 * 5. Module not loading
 *    - Check: dmesg | tail
 *    - Verify kernel version matches (uname -r)
 *    - Check dependencies: modprobe --show-depends custom_sensor
 *
 *
 * ============================================================================
 *                    L4T SPECIFIC TIPS
 * ============================================================================
 *
 * JETSON-IO FOR QUICK PIN CONFIG:
 * -------------------------------
 * sudo /opt/nvidia/jetson-io/jetson-io.py
 * # Select header, enable interfaces (SPI, I2C, UART)
 * # Automatically creates and applies overlays
 *
 *
 * CHECK HARDWARE CONNECTIONS:
 * ---------------------------
 * # I2C bus scan
 * sudo i2cdetect -y -r 1  # Bus 1
 *
 * # Should show your device address (e.g., 48)
 *
 *
 * GPIO TESTING:
 * -------------
 * # Find GPIO chip
 * ls /sys/class/gpio/
 *
 * # Export and test GPIO
 * echo 79 > /sys/class/gpio/export  # Example GPIO number
 * echo out > /sys/class/gpio/gpio79/direction
 * echo 1 > /sys/class/gpio/gpio79/value
 *
 *
 * KERNEL CONFIG CHECK:
 * --------------------
 * # Check if feature is enabled
 * zcat /proc/config.gz | grep CONFIG_I2C
 * zcat /proc/config.gz | grep CONFIG_OF_OVERLAY
 *
 *
 * ============================================================================
 *                    INTERVIEW QUESTIONS - DEVICE TREE
 * ============================================================================
 *
 * Q1: What is Device Tree and why is it used?
 * A: Device Tree describes hardware configuration in a data structure,
 *    allowing one kernel to boot on multiple boards. It separates
 *    hardware description from driver code.
 *
 * Q2: How does a driver know which device to bind to?
 * A: Through the "compatible" property. Driver's of_match_table
 *    contains compatible strings that must match DT node's compatible.
 *
 * Q3: What's the difference between DTB and DTBO?
 * A: DTB is the compiled base Device Tree. DTBO is an overlay that
 *    modifies the base tree at runtime without recompiling.
 *
 * Q4: When is probe() called?
 * A: When kernel finds a DT node whose compatible string matches
 *    a loaded driver's of_match_table and status is "okay".
 *
 * Q5: How do you debug DT binding issues?
 * A: Check /proc/device-tree for node presence, verify compatible
 *    string matches exactly, check status property, review dmesg.
 *
 * Q6: What's a phandle?
 * A: A phandle is a unique reference to a node, allowing other nodes
 *    to reference it. Used for clocks, GPIOs, interrupts, etc.
 *
 * Q7: How do overlays work?
 * A: Overlays use fragment nodes with target references. At runtime,
 *    the __overlay__ contents are merged into the target node.
 *
 * Q8: What's the reg property?
 * A: reg specifies the address and size of a device's resources.
 *    For I2C, it's the device address. For memory-mapped, it's
 *    the physical address and region size.
 *
 *
 * ============================================================================
 *                              END OF GUIDE
 * ============================================================================
 */
