#include "soc/soc_ulp.h"
#include "soc/rtc_io_reg.h"

	/* ADC1 channel 6, GPIO34 */
	.set adc_channel, 6
	.set sample_count, 12

	/* Code goes into .text section */
.text

	.global entry
entry:
	// r0 = index
	// r1 = address
	// r2 = scratch
	// r3 = sample
    WRITE_RTC_REG(RTC_IO_TOUCH_PAD0_REG, RTC_IO_TOUCH_PAD0_MUX_SEL_S, 1, 1)
	WRITE_RTC_REG(RTC_GPIO_ENABLE_W1TS_REG, RTC_GPIO_ENABLE_W1TS_S + 10, 1, 1)

	WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG, RTC_GPIO_ENABLE_W1TS_S + 10, 1, 1)

	move r1, mic_buffer 	// r1 now contains buffer base address
	move r0, 0 					// Use r0 as index

sample_loop:
	adc r3, 0, adc_channel+1	// Put the sample in r3
   	st r3, r1, 0                // Store the sample to the current address

   	add r0, r0, 1				// Increment index
	add r1, r1, 1				// Increment address

	wait 1000 					// Wait for 500 cycles

	sub r2, r0, sample_count			// If index is now 8000, we're done. return
	jump done, eq
	; jump entry, eq

	jump sample_loop			// Otherwise, loop again
	
done:

	WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG, RTC_GPIO_OUT_DATA_W1TC_S + 10, 1, 1)
	wake
	halt

.global mic_buffer
mic_buffer:
	.fill sample_count