#include <stdio.h>
#include <string.h>
#include <driver/i2c.h>
#include <driver/media.h>

#define I2C_ADDR_TDA9887T 0x4B /* 0x43 - 0x4C */

/* ---------------------------------------------------------------------- */

//
// TDA defines
//

//// first reg (b)
#define cVideoTrapBypassOFF     0x00    // bit b0
#define cVideoTrapBypassON      0x01    // bit b0

#define cAutoMuteFmInactive     0x00    // bit b1
#define cAutoMuteFmActive       0x02    // bit b1

#define cIntercarrier           0x00    // bit b2
#define cQSS                    0x04    // bit b2

#define cPositiveAmTV           0x00    // bit b3:4
#define cFmRadio                0x08    // bit b3:4
#define cNegativeFmTV           0x10    // bit b3:4


#define cForcedMuteAudioON      0x20    // bit b5
#define cForcedMuteAudioOFF     0x00    // bit b5

#define cOutputPort1Active      0x00    // bit b6
#define cOutputPort1Inactive    0x40    // bit b6

#define cOutputPort2Active      0x00    // bit b7
#define cOutputPort2Inactive    0x80    // bit b7


//// second reg (c)
#define cDeemphasisOFF          0x00    // bit c5
#define cDeemphasisON           0x20    // bit c5

#define cDeemphasis75           0x00    // bit c6
#define cDeemphasis50           0x40    // bit c6

#define cAudioGain0             0x00    // bit c7
#define cAudioGain6             0x80    // bit c7


//// third reg (e)
#define cAudioIF_4_5             0x00    // bit e0:1
#define cAudioIF_5_5             0x01    // bit e0:1
#define cAudioIF_6_0             0x02    // bit e0:1
#define cAudioIF_6_5             0x03    // bit e0:1


#define cVideoIF_58_75           0x00    // bit e2:4
#define cVideoIF_45_75           0x04    // bit e2:4
#define cVideoIF_38_90           0x08    // bit e2:4
#define cVideoIF_38_00           0x0C    // bit e2:4
#define cVideoIF_33_90           0x10    // bit e2:4
#define cVideoIF_33_40           0x14    // bit e2:4
#define cRadioIF_45_75           0x18    // bit e2:4
#define cRadioIF_38_90           0x1C    // bit e2:4


#define cTunerGainNormal         0x00    // bit e5
#define cTunerGainLow            0x20    // bit e5

#define cGating_18               0x00    // bit e6
#define cGating_36               0x40    // bit e6

#define cAgcOutON                0x80    // bit e7
#define cAgcOutOFF               0x00    // bit e7

/* ---------------------------------------------------------------------- */

struct tda9887t_norm_t {
	int std;
	char *name;
	unsigned char b;
	unsigned char c;
	unsigned char e;
};

static struct tda9887t_norm_t tda9887t_norms[] = {
	{
		.std  = TDA9887T_RADIO_STD_FM,
		.name = "radio",
		.b    = (
				cOutputPort2Inactive // tuner control
				| cOutputPort1Inactive
				| cFmRadio
				| cQSS
			),
		.c    = (
				cDeemphasisON
				| cDeemphasis50
			),
		.e    = (
				cAudioIF_5_5
				| cRadioIF_38_90
			),
	},/*{
		.std   = V4L2_STD_PAL_BG,
		.name  = "PAL-BG",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis50  ),
		.e     = ( cAudioIF_5_5   |
			   cVideoIF_38_90 ),
	},{
		.std   = V4L2_STD_PAL_I,
		.name  = "PAL-I",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis50  ),
		.e     = ( cAudioIF_6_0   |
			   cVideoIF_38_90 ),
	},{
		.std   = V4L2_STD_PAL_DK,
		.name  = "PAL-DK",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis50  ),
		.e     = ( cAudioIF_6_5   |
			   cVideoIF_38_00 ),
	},{
		.std   = V4L2_STD_PAL_M | V4L2_STD_PAL_N,
		.name  = "PAL-M/N",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis75  ),
		.e     = ( cAudioIF_4_5   |
			   cVideoIF_45_75 ),
	},{
		.std   = V4L2_STD_SECAM_L,
		.name  = "SECAM-L",
		.b     = ( cPositiveAmTV  |
			   cQSS           ),
		.e     = ( cAudioIF_6_5   |
			   cVideoIF_38_90 ),
	},{
		.std   = V4L2_STD_NTSC_M,
		.name  = "NTSC-M",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis50  ),
		.e     = ( cGating_36     |
			   cAudioIF_4_5   |
			   cVideoIF_45_75 ),
	},{
		.std   = V4L2_STD_NTSC_M_JP,
		.name  = "NTSC-JP",
		.b     = ( cNegativeFmTV  ),
		.c     = ( cDeemphasisON  |
			   cDeemphasis50  ),
		.e     = ( cGating_36     |
			   cAudioIF_4_5   |
			   cVideoIF_58_75 ),
	}*/
};

#define TDA9887T_NORMS_SIZE ( sizeof(tda9887t_norms) \
		/ sizeof(struct tda9887t_norm_t) )

int tda9887t_status(struct i2c_adaptor_t *adap, unsigned char *status)
{
	struct i2c_msg_t msg[1] = {
		{I2C_RD, I2C_ADDR_TDA9887T, 1, status}
	};
	int res=0;

	res = i2c_transfer(adap, msg, 1);
	if (res) {
		printk("tda9887t: read status failed\n");
		return(res);
	}
	printk("tda9887t: status= 0x%x\n", *status);
	return res;
}

int tda9887t_configure(struct i2c_adaptor_t *adap, int std)
{
	struct tda9887t_norm_t *norm = 0;
	unsigned char buf[4], status;
	struct i2c_msg_t msg[1] = {
		{0, I2C_ADDR_TDA9887T, 4, buf}
	};
	int i,res=0;
	
	for (i = 0; i < TDA9887T_NORMS_SIZE; i++) {
		if (tda9887t_norms[i].std == std) {
			norm = tda9887t_norms+i;
			break;
		}
	}
	if (norm == 0) {
		printk("Oops: no tvnorm entry found\n");
		return -1;
	}

	printk("tda877t: configure for: %s\n",norm->name);
	memset(buf,0,sizeof(buf));
	buf[1] = norm->b;
	buf[2] = norm->c;
	buf[3] = norm->e;
	printk("tda9887t: configuring: b=0x%x c=0x%x e=0x%x\n", buf[1], buf[2], buf[3]);
        
	res = i2c_transfer(adap, msg, 1);
	if (res) {
		printk("tda9887t: configure failed\n");
		return(res);
	}
	
	res = tda9887t_status(adap, &status);

	return res;
}

