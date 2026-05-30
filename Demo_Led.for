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

\==== eyes ==== Using I2C Display Adafruit SSD1306

:esp32.send esp32Url swap $+ http.get drop ;

:eyes (draw 2 eyes)
  timer.stop
  "eyes00" 3 timer.start ;

:eyes00
"draw?x=0&y=10&h=28&w=127&color=0&fill=1&r=0&rect=1" esp32.send
60 (x pos of whites)
dup
dup "draw?h=20&w=20&color=1&fill=1&r=20&y=10&x=:X&rect=0" ":X" $replace esp32.send
25 (x gap of whites)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send

12 random (x pos of blacks relative to whites)
+ dup
8 random 10 + (y pos of blacks)
"draw?h=10&w=10&color=0&fill=1&r=20&y=:Y&x=:X&rect=0" ":Y" $replace ":X" $replace esp32.send
25 (x gap of blacks)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send
;
