/* media.h */
#ifndef _MEDIA_H_
#define _MEDIA_H_

#include <driver/i2c.h>

struct i2c_adaptor_t *bt878a_get_adaptor(void);
void bt878a_init(void);

int mt2050_set_radio_freq(struct i2c_adaptor_t *adap, unsigned int freq);
int mt2050_init(struct i2c_adaptor_t *adap);
int mt2050_deinit(struct i2c_adaptor_t *adap);

#define TDA9887T_RADIO_STD_FM  0x00000001
#define TDA9887T_TV_STD_PAL_BG 0x00000002

int tda9887t_status(struct i2c_adaptor_t *adap, unsigned char *status);
int tda9887t_configure(struct i2c_adaptor_t *adap, int std);

int msp34x0g_reset(struct i2c_adaptor_t *adap);
int msp34x0g_detect(struct i2c_adaptor_t *adap);
#endif

