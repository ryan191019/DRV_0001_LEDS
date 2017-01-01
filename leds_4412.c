#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
 #include <linux/gpio.h>
#include <linux/device.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/leds.h>
//s3c_gpio_cfgpin(led_gpios[i],S3C_GPIO_OUTPUT);
//gpio_set_value(led_gpios[arg], !cmd);

struct led_desc{
int gpio;
char *name;
};

static struct led_desc led_gpios[] = {
	{EXYNOS4212_GPM4(0),"led1"},
	{EXYNOS4212_GPM4(1),"led2"},
	{EXYNOS4212_GPM4(2),"led3"},
	{EXYNOS4212_GPM4(3),"led4"},
};


struct led_classedv_4412 {
	 struct led_classdev  cdev;
	 int gpio;
};

static  struct led_classedv_4412 *led_devs;


static void	brightness_set_4412(struct led_classdev *led_cdev,
					  enum led_brightness brightness)
{
	 struct led_classedv_4412 *dev = ( struct led_classedv_4412 *)led_cdev;
	 led_cdev->brightness = brightness;
	if(brightness != LED_OFF) {
		gpio_set_value(dev->gpio, 0);		
	} else {
		gpio_set_value(dev->gpio, 1);
	}
	 
}
static int leds_init(void)
{
	int i, ret;
	/* 1 alloc led_classdev */
	led_devs = kzalloc(sizeof(struct led_classedv_4412 ) *4, GFP_KERNEL);
	if(led_devs == NULL) {
		printk("no memory for device\n");
		return -ENOMEM;
	}
	for(i = 0; i<4; i++) {
		s3c_gpio_cfgpin(led_gpios[i].gpio,S3C_GPIO_OUTPUT);
		gpio_set_value(led_gpios[i].gpio, 1);


		/* set */
		led_devs[i].cdev.name = led_gpios[i].name;
		led_devs[i].cdev.max_brightness 	= LED_FULL;
		led_devs[i].cdev.brightness_set 	= brightness_set_4412;
		led_devs[i].cdev.flags                	= LED_CORE_SUSPENDRESUME;

		led_devs[i].cdev.brightness = LED_OFF;
		//led_devs[i].cdev.default_trigger = "timer";

		led_devs[i].gpio = led_gpios[i].gpio;
		/* 3 register */
		ret = led_classdev_register(NULL, &led_devs[i].cdev);
		if(ret)  {
			i--;
			while(i >= 0) {
				led_classdev_unregister(&led_devs[i].cdev);
				i--;
			}
			kfree(led_devs);
			return -EIO;
		}
	}
	return 0;
}

static void leds_exit(void)
{
	int i;
	for (i=0;i<4;i++) {
		led_classdev_unregister(&led_devs[i].cdev);
	}
	kfree(led_devs);
}



module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("GPL");













