/* app_tone.c */
#include <stdlib.h>
#include <math.h>
#include <driver/audio.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <myos/myos_sem.h>
#include <apps/console/console.h>

static int sampling_freq=44100;
static int total_time;
static double tone_freq;
static short sample_buf[(1<<14)];

static int app_tone_console_handler(char *s)
{
	switch(*s)
	{
		case 's':
			{
				double d;

				d = atoi(s+1);
				console_printf(0,"%d\n"
						,(int)(sin(M_PI/180.0*d)*1000));
			}
			break;
		case 't':
			{
				total_time = atoi(s+1);
			}
			break;
		case 'p':
			{
				tone_freq = (double) atoi(s+1);
				if (tone_freq!=0.0)
					myOsSemSignal(&sem_tone);
			}
			break;
		default:
			{
				console_printf(0, "s<n>    - sine\n");
				console_printf(0, "t<n>    - duration\n");
				console_printf(0, "p<freq> - Play tone\n");
			}
			break;
	}
	return(0);
}

void app_tone_task(void)
{
	console_create("tone", CONSOLE_FLAG_ECHO | CONSOLE_FLAG_STRING
			, app_tone_console_handler);
	
	while(1) {
		int total_samples, sample, sample_index;
		
		myOsSemWait(&sem_tone);
		console_printf(1, "playing tone\n");
		total_samples = total_time * sampling_freq;
		sample_index = 0;
		for (sample = 0; sample < total_samples; sample++) {
			double val, sample_time;
			
			sample_time = sample / (double) sampling_freq;
			val = sin(2*M_PI*tone_freq*sample_time);
			sample_buf[sample_index++] = (short) val;
			sample_buf[sample_index++] = (short) val;
			console_printf(1,"s= %d, val= %d\n"
					, (int)sample_time*1000
					, (int)(val*1000));
			
			if(sample_index>((1<<13)-1)) {
				int bytes = sample_index*2;
				audio_play(sample_buf, &bytes);
				console_printf(1,"audio bytes= %d\n", bytes);
				sample_index=0;
				myOsSleep(10);
			}
		}
	}
}

