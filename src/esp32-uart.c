#include <stdlib.h>
#include <string.h>

#include "mruby.h"
#include "mruby/value.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/uart.h"

//#define BUF_SIZE 1024
#define MAX_EXPECTED_DATA_SIZE 256
#define BUF_SIZE (MAX_EXPECTED_DATA_SIZE + 1)  // Add 1 for null-termination

static mrb_value
mrb_esp32_uart_init(mrb_state *mrb, mrb_value self) {
  mrb_value uartnum,rx,tx,baudrate;
  mrb_get_args(mrb, "oooo",&uartnum,&rx,&tx, &baudrate);
  //mrb_fixnum(duty)
  printf("INITIAL :  %d %d %d %d\n",mrb_fixnum(uartnum),mrb_fixnum(rx),mrb_fixnum(tx),mrb_fixnum(baudrate));
  const uart_config_t uart_config = {
        .baud_rate = mrb_fixnum(baudrate),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
  uart_param_config(mrb_fixnum(uartnum), &uart_config);
  uart_set_pin(mrb_fixnum(uartnum),mrb_fixnum(rx),mrb_fixnum(tx), UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  //uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 5, &uart_queue, 0);
  uart_driver_install(mrb_fixnum(uartnum), BUF_SIZE , BUF_SIZE , 5, NULL, 0);
  //xTaskCreate(uart_task, "uart_task", TASK_MEMORY, NULL, 7, NULL);
  

  return mrb_nil_value();
}

static mrb_value
mrb_esp32_uart_read(mrb_state *mrb, mrb_value self) {
    mrb_value uartnum;
    mrb_get_args(mrb, "o", &uartnum);
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

    if (data != NULL) {
      int data_r = 0;
      char* data_s = "";
      //printf("Data S : %s \n",data_s);
   
      int rxBytes = uart_read_bytes(mrb_fixnum(uartnum), data,BUF_SIZE, 500/ portTICK_PERIOD_MS);
      if (rxBytes > 0){
        data[rxBytes] = '\0';
        //printf("data read = %s \n",data);
        data_s = data;
        mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s); 
      }
      else{
        data_s = "Emtry data";
        mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s); 
      }
      //printf("Data S : %s \n",data_s);
      mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s);
      free(data);
      return mruby_str;
  }else{
   mrb_raise(mrb, mrb->eStandardError_class, "Memory allocation failed");
   return mrb_nil_value();
  }
}

static mrb_value
mrb_esp32_uart_send(mrb_state *mrb, mrb_value self) {
  mrb_value uartnum;
  char* data;
  mrb_get_args(mrb,"oz", &uartnum,&data);
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  //vTaskDelay(pdMS_TO_TICKS(500 * 2));
  printf("uart send : %s \n",data);
  return  mrb_nil_value();
}



void
mrb_esp32_uart_gem_init(mrb_state* mrb)
{
  struct RClass *c;
  c = mrb_define_module(mrb, "ESP32UART");

  // UART_NUM 
  mrb_define_const(mrb, c, "NUM_0", mrb_fixnum_value(UART_NUM_0));
  mrb_define_const(mrb, c, "NUM_1", mrb_fixnum_value(UART_NUM_1));
  mrb_define_const(mrb, c, "NUM_2", mrb_fixnum_value(UART_NUM_2));


  // UART fucntion requirment
  mrb_define_module_function(mrb, c, "init_uart",mrb_esp32_uart_init, MRB_ARGS_REQ(4));

  // UART comunication funtion setup
  mrb_define_module_function(mrb, c, "uart_send", mrb_esp32_uart_send, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "uart_read", mrb_esp32_uart_read, MRB_ARGS_REQ(1));

}

void
mrb_esp32_uart_gem_final(mrb_state* mrb)
{
}
