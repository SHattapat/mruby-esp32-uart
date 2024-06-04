module ESP32UART
  class UART
    def initialize uartnum,rx,tx,baudrate
      # o = LoRa.new NUM_2,25,26,115200
      @uartnum = uartnum
      init_uart uartnum,rx,tx,baudrate
      #data_r = lora_read @uartnum
      #puts data_r
      puts "init_uart completed!"
    end

    def send data
      uart_send @uartnum,data
    end

    def read 
      uart_read @uartnum
    end
    def testu
      while true
      data = "test1234"
      #puts "Send : #{data}"
      uart_send @uartnum,data
      sleep 2
      #d = uart_read @uartnum
      #puts d
      #sleep 1
      end
    end


  end
end
