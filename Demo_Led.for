# Demo_LED.for
# Send LED commands to ESP32 via wifi

:esp32Url "http://10.0.0.50/" ;

:esp32Url_ON esp32Url "led_1" $append ;
:esp32Url_OFF esp32Url "led_0" $append ;
:time_ON 0.05 ;
:time_OFF 0.2 ;

:send_on esp32Url_ON http.get . ." " ;
:send_off esp32Url_OFF http.get . ." " ;

(blink the LED so many times)
:blink_LED ([count] --) 1 do send_on time_ON sleep send_off time_OFF sleep loop CR ;

1 blink_LED ;
10 blink_LED ;

==== LED in GPIO 15 ====

esp32Url "gpio?pin=15&mode=out" $+ http.get . ; (set GPIO pin to output)
esp32Url "gpio?pin=15&val=0" $+ http.get . ; (set GPIO pin to low)
esp32Url "gpio?pin=15&val=1" $+ http.get . ; (set GPIO pin to high)

:gpled_ON esp32Url "gpio?pin=15&val=0" $+ http.get . time_ON sleep ;
:gpled_OFF esp32Url "gpio?pin=15&val=1" $+ http.get . time_OFF sleep ;
:gpblink ([count] --) 1 do gpled_ON ." > " gpled_OFF CR loop ;

10 gpblink ;
