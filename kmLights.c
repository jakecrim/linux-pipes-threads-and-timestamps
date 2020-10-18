#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
#define GPIO_BASE 0x3F200000U

int mydev_id;

static irqreturn_t button_isr(int irq, void *dev_id)
{
	// // In general, you want to disable the interrupt while handling it.
	// disable_irq_nosync(79);

	// unsigned long * gpsel0_ptr = (unsigned long*) ioremap(GPIO_BASE, 4096);
	// unsigned long * gpeds0_ptr = gpsel0_ptr + GPEDS_OFFSET;
	// unsigned long * gpset0_ptr = (gpsel0_ptr + 7);
	// unsigned long * gpclr0_ptr = (gpsel0_ptr + 10);

	// printk("GPESD PTR: %X \n", gpeds0_ptr);



	// // setting leds to output
	// iowrite32(0x00009240 , gpsel0_ptr);

	// unsigned long high_pin = ioread32(gpeds0_ptr);
	// printk("high pins: 0x%X", high_pin);
	// // F8000
	// unsigned long high_pin_mask = high_pin & 0b00000000000011111000000000000000;
	// printk("high pins: 0x%X", high_pin_mask);

	// switch (high_pin_mask)
	// {
	// 	case 0x10000:
	// 		printk("button 1 \n");
	// 		iowrite32(0x00000004U, gpset0_ptr);
	// 		break;
	// 	case 0x20000:
	// 		iowrite32(0x00000008U, gpset0_ptr);
	// 		break;
	// 	case 0x40000:
	// 		iowrite32(0x000000010U, gpset0_ptr);
	// 		break;
	// 	case 0x80000:
	// 		iowrite32(0x000000020U, gpset0_ptr);
	// 		break;
	// 	case 0x0:
	// 		iowrite32(0x0000003CU, gpclr0_ptr);
	// 		printk("button 5 \n");
	// 		break;
	// }

	// iowrite32(0x001F0000, gpeds0_ptr);
	// enable_irq(79);		// re-enable interrupt
	// printk("Interrupt handled\n");	
	
    return IRQ_HANDLED;
}


int init_module()
{
    printk("---Initializing Button ISR--- \n");

    unsigned long * gpsel_ptr = ioremap(GPIO_BASE, 4096);
    printk("sel0 ptr %X \n", gpsel_ptr);

	unsigned long * gpsel1_ptr = (gpsel_ptr + 1);
	printk("sel1 ptr %X \n", gpsel1_ptr);
	unsigned long * gppud_ptr = (gpsel_ptr + 37);
	unsigned long * gppudclk0_ptr = (gpsel_ptr + 38);
	unsigned long * gpafen0_ptr = (gpsel_ptr + 34);


	// set as input
	iowrite32( 0x00000000 , gpsel1_ptr);

	// set as pull down
	iowrite32( 0x00000001, gppud_ptr);

	// delay
	udelay(100);

	// write 1 for bit 16 to gppudclk reg
	iowrite32( 0x00010000, gppudclk0_ptr);

	// delay
	udelay(100);

	iowrite32(0x00000000, gppud_ptr);
	iowrite32(0x00000000, gppudclk0_ptr);

	// set bit 16 for gparen0
	iowrite32(0x00010000 , gpafen0_ptr);


    
    // clearing gpsel1 in case it was already set?
    iowrite32(0x00038000, gpsel1_ptr);

	// bind
	int dummy = 0;
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

    return 0;
}

void cleanup_module()
{
    printk("Shutting Down Module \n");

    free_irq(79, &mydev_id);
}