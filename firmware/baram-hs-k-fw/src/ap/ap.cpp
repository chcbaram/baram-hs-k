#include "ap.h"



extern uint32_t data_in_rate;
extern USBD_HandleTypeDef USBD_Device;

void apInit(void)
{  
  cliOpen(HW_UART_CH_SWD, 115200);
  logBoot(false);
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      logPrintf("rate %d\n", data_in_rate);

      // uint8_t hid_buf[4] = {0, };

      // USBD_HID_SendReport(&USBD_Device, hid_buf, 4);        
    }

    cliMain();
  }
}

