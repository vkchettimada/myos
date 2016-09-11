/* app_radio.c */
#include <stdlib.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>
#include <driver/media.h>

static struct i2c_adaptor_t *adap;

static int app_radio_freq_handler(char *s)
{
	int freq;

	freq = atoi(s) * 10000;
	if (freq) {
		console_printf("radio_freq_handler: s=%s freq=%d\n", s, freq);
		mt2050_set_radio_freq(adap, freq);
	}
	return(0);
}

void app_radio_task(void)
{
	adap = bt878a_get_adaptor();
	if (adap)
		console_create("radio", CONSOLE_FLAG_ECHO | CONSOLE_FLAG_STRING, app_radio_freq_handler);
	
	while(1) {
		myOsSleep(1000);
	}
}

