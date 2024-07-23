#include "xiicps.h"

XIicPs_Config XIicPs_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"cdns,i2c-r1p10", /* compatible */
		0xe0004000, /* reg */
		0x69f6bcb, /* xlnx,clock-freq */
		0x4019, /* interrupts */
		0xf8f01000, /* interrupt-parent */
		0x26 /* clocks */
	},
	{
		"cdns,i2c-r1p10", /* compatible */
		0xe0005000, /* reg */
		0x69f6bcb, /* xlnx,clock-freq */
		0x4030, /* interrupts */
		0xf8f01000, /* interrupt-parent */
		0x27 /* clocks */
	},
	 {
		 NULL
	}
};