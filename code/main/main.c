// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"


#include "driver/i2s.h"


//buffers
int rxbuf[256], txbuf[256];
float l_in[128], r_in[128];
float l_out[128], r_out[128];
	
	








void app_main(void)
{

    i2s_config_t i2s_config = {

        .mode = I2S_MODE_MASTER | I2S_MODE_TX  | I2S_MODE_RX,  
        .sample_rate = 44100,
        .bits_per_sample = 32,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .intr_alloc_flags = 0,                                                  
        .tx_desc_auto_clear = true                                              
    };


    i2s_driver_install(0, &i2s_config, 0, NULL);

    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = 19                                                       
    };

    i2s_set_pin(0, &pin_config);
	
	//enable MCLK on GPIO0
	REG_WRITE(PIN_CTRL, 0xFF0); 
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
	


	
	size_t readsize = 0;
	
	
	
	
    while (1) {
    	
    	  //read 256 samples (128 stereo samples)
    	  esp_err_t rxfb = i2s_read(0,&rxbuf[0],256*4, &readsize, 1000);
    	  if (rxfb == ESP_OK && readsize==256*4) {
    	  
    	  	//extract stereo samples to mono buffers
    	  	int y=0;
    	  	for (int i=0; i<256; i=i+2) {
    	  		l_in[y] = (float) rxbuf[i];
    	  		r_in[y] = (float) rxbuf[i+1];
    	  		y++;
    	  	}
    	  	
    	  	
    	  	//do something with your left + right channel samples here in the buffers l_in/r_in and ouput result to l_out and r_out (e.g. build mono sum and apply -6dB gain (*0.5)
    	  	
    	  	for (int i=0; i<128; i++) {
    	  	
    	  		l_out[i] = 0.5f * (l_in[i] + r_in[i]);
    	  		r_out[i] = l_out[i];
    	  		
    	  	
    	  	}
    	  	
    	  	
    	  	//merge two l and r buffers into a mixed buffer and write back to HW
    	  	y=0;
    	  	for (int i=0;i<128;i++) {
    	  	txbuf[y] = (int) l_out[i];
    	  	txbuf[y+1] = (int) r_out[i];
    	  	y=y+2;
    	  	}
    	  
    	  	i2s_write(0, &txbuf[0],256*4, &readsize, 1000);
    	  	
    	  	
    	  	
    	  }
    
    
    }




}



