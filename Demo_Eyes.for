# Demo_LED.for
# Send LED commands to ESP32 via wifi

:esp32Url "http://10.0.0.50/" ;

\==== eyes ==== Using I2C Display Adafruit SSD1306

:esp32.send esp32Url swap $+ http.get drop ;

:eyes (draw 2 eyes)
  "draw?x=0&y=8&h=24&w=127&color=0&fill=1&r=0&rect=1" esp32.send
  timer.stop
  "eyes00" 3 timer.start ;

:eyes00
30 (x pos of whites)
dup
dup "draw?h=20&w=20&color=1&fill=1&r=20&y=12&x=:X&rect=0" ":X" $replace esp32.send
25 (x gap of whites)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send

12 random (x pos of blacks relative to whites)
+ dup
8 random 10 + (y pos of blacks)
"draw?h=10&w=10&color=0&fill=1&r=20&y=:Y&x=:X&rect=0" ":Y" $replace ":X" $replace esp32.send
25 (x gap of blacks)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send
;
