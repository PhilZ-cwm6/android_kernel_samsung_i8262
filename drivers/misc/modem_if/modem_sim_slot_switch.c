#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/err.h>

#if !defined (CONFIG_MACH_ARUBA_CTC) && !defined(CONFIG_MACH_INFINITE_DUOS_CTC) && !defined(CONFIG_MACH_DELOS_DUOS_CTC) && !defined(CONFIG_MACH_HENNESSY_DUOS_CTC)
#include <plat/gpio-cfg.h>
#endif
#include <mach/gpio.h>

extern struct class *sec_class;
struct device *slot_switch_dev;

#if defined (CONFIG_MACH_ARUBA_CTC) || defined(CONFIG_MACH_INFINITE_DUOS_CTC) || defined(CONFIG_MACH_DELOS_DUOS_CTC) || defined(CONFIG_MACH_HENNESSY_DUOS_CTC)
#define GPIO_UIM_SIM_SEL		5
#endif

static ssize_t get_slot_switch(struct device *dev, struct device_attribute *attr, char *buf)
{
	int value;

	//return '0' slot path is '||', return '1' slot path is 'X'
	value = gpio_get_value(GPIO_UIM_SIM_SEL);
	printk("Current Slot is %x\n", value);

	return sprintf(buf, "%d\n", value);
}

static ssize_t set_slot_switch(struct device *dev, struct device_attribute *attr,   const char *buf, size_t size)
{
	int value;

	sscanf(buf, "%d", &value);

	switch(value) {
		case 0:
			gpio_set_value(GPIO_UIM_SIM_SEL, 0);
			printk("set slot switch to %x\n", gpio_get_value(GPIO_UIM_SIM_SEL));
			break;
		case 1:
			gpio_set_value(GPIO_UIM_SIM_SEL, 1);
			printk("set slot switch to %x\n", gpio_get_value(GPIO_UIM_SIM_SEL));
			break;
		default:
			printk("Enter 0 or 1!!\n");
	}

	return size;
}

static DEVICE_ATTR(slot_sel, S_IRUGO | S_IWUSR | S_IWGRP, get_slot_switch, set_slot_switch); 

static int __init slot_switch_manager_init(void)
{
	int ret = 0;
	int err = 0;

	printk("slot_switch_manager_init\n");

    //initailize uim_sim_switch gpio
	err = gpio_request(GPIO_UIM_SIM_SEL, "PDA_ACTIVE");
	if (err) {
		pr_err("fail to request gpio %s, gpio %d, errno %d\n",
					"PDA_ACTIVE", GPIO_UIM_SIM_SEL, err);
	} else {
#if defined (CONFIG_MACH_ARUBA_CTC) || defined(CONFIG_MACH_INFINITE_DUOS_CTC) || defined(CONFIG_MACH_DELOS_DUOS_CTC) || defined(CONFIG_MACH_HENNESSY_DUOS_CTC)
		gpio_tlmm_config(GPIO_CFG(GPIO_UIM_SIM_SEL,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
#else
		gpio_direction_output(GPIO_UIM_SIM_SEL, 1);
		s3c_gpio_setpull(GPIO_UIM_SIM_SEL, S3C_GPIO_PULL_NONE);
#endif

		gpio_set_value(GPIO_UIM_SIM_SEL, 0);
	}

	//initailize slot switch device
	slot_switch_dev = device_create(sec_class,
                                    NULL, 0, NULL, "slot_switch");
	if (IS_ERR(slot_switch_dev))
		pr_err("Failed to create device(switch)!\n");

	if (device_create_file(slot_switch_dev, &dev_attr_slot_sel) < 0)
		pr_err("Failed to create device file(%s)!\n",
					dev_attr_slot_sel.attr.name);

	return ret;
}

static void __exit slot_switch_manager_exit(void)
{
}

module_init(slot_switch_manager_init);
module_exit(slot_switch_manager_exit);

//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("SAMSUNG ELECTRONICS CO., LTD");
//MODULE_DESCRIPTION("Slot Switch");
