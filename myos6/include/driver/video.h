/* video.h */
#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <driver/i2c.h>

void cls(void);
void vga_init(void);
void bt878a_init(void);
int mt2050_init(struct i2c_adaptor_t *adap);

#endif
