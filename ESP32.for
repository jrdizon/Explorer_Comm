# ESP32.for
#
\ Ifs & Loops:
\  [boolean] if ... endif
\  [boolean] while ... [boolean] loop
\  [end] [start] do ... loop
\  begin [boolean] if ... loop
\
:start showHttp ;

:showHttp

panel_start

20 panel.addGap
"ESP32 URL" panel.addLabel
"http://10.0.0.50/" "inpHttp" panel.addInput

"selectHttp" "selHttp" panel.addSelector
"http://10.0.0.50/" "selHttp" panel.addOption
"http://10.0.0.51/" "selHttp" panel.addOption

10 panel.addGap
"Servo Delay (Sec)" panel.addLabel
"0.0" "delay" panel.addInput

10 panel.addGap
"Get Info" "getInfo" "btnInfo" panel.addButton
"LED 1" "led_1" "btnLED1" panel.addButton
"LED 0" "led_0" "btnLED0" panel.addButton
"Reset" "reset" "btnReset" panel.addButton

10 panel.addGap
"selectRun" "selRun" panel.addSelector
"eyes" "selRun" panel.addOption

10 panel.addGap
"eyes" "inpFunc3" panel.addInput
"Go" "goFunc3" "btnGF3" panel.addButton

5 panel.addGap
"servo_initialize" "chars2" panel.addInput
"Go" "sendChars2" "btnSC2" panel.addButton

5 panel.addGap
"servo_reset" "chars3" panel.addInput
"Go" "sendChars3" "btnSC3" panel.addButton

5 panel.addGap
"servo_mid" "chars4" panel.addInput
"Go" "sendChars4" "btnSC4" panel.addButton
(
10 panel.addGap
"gpio?pin=10&val=1" "chars5" panel.addInput
"Send" "sendChars5" "btnSC5" panel.addButton
)
panel_end
panel_start2
;

########################################

:showSerial

panel_start

10 panel.addGap
"Serial Port Number" panel.addLabel
"0" "inpSerNum" panel.addInput
"Serial Open" "ser_open" "btnOpen" panel.addButton
"Serial Close" "ser_close" "btnClose" panel.addButton

10 panel.addGap
"getStat" "inpFunc1" panel.addInput
"Go" "goFunc1" "btnGF1" panel.addButton

10 panel.addGap
"func1" "inpFunc2" panel.addInput
"Go" "goFunc2" "btnGF2" panel.addButton

10 panel.addGap
"func2" "inpFunc3" panel.addInput
"Go" "goFunc3" "btnGF3" panel.addButton

panel_end
;
########################################

:panel_start (==== Start panel ================)
panel.clear
0.3 sleep
"HTTP" "showHttp" "btnHttp" panel.addButton
"SERIAL" "showSerial" "btnSerail" panel.addButton
;

:panel_end
20 panel.addGap
"List Words" "list10" "btnList1" panel.addButton
"List Functions" "list20" "btnList2" panel.addButton
5 panel.addGap
"Debug ON" "debug1" "btnDebug1" panel.addButton
"Debug OFF" "debug0" "btnDebug0" panel.addButton
"Stack" "stack" "btnStack" panel.addButton
5 panel.addGap
;

list10: list ;
list20: list2 ;
debug1: debug_on ;
debug0: debug_off ;
stack: .stack ;

selectRun:
  "chars3" panel.getValue "chars4" panel.setValue
  "chars2" panel.getValue "chars3" panel.setValue
  "inpFunc3" panel.getValue "chars2" panel.setValue
  "selRun" panel.getValue dup "inpFunc3" panel.setValue execWord ;

:ser_open
  "inpSerNum" panel.getValue dup "Serial#" .. . " open" .
  $num Serial.open
  ;

:ser_close
  Serial.close
  "Serial Port closed." ..
  ;

selectHttp: "selHttp" panel.getValue "inpHttp" panel.setValue ;


.errorColor: ([string] --) dup "Error" $contains dup if dup2 200 0 0 .color endif not if 0 120 0 .color endif ;

esp32.send: ([query] --)
"inpHttp" panel.getValue swap $+ http.get
  .errorColor CR
;

:getInfo "info" esp32.send ;
:led_1 "led_1" esp32.send ;
:led_0 "led_0" esp32.send ;
:led_flash "led_flash?n=3" esp32.send ;
:reset "reset" esp32.send ;

goFunc1:
"inpFunc1" goFuncX ;

goFunc2:
"inpFunc2" goFuncX ;

goFunc3:
"inpFunc3" goFuncX ;

goFuncX: panel.getValue dup .. CR execWord ;


sendChars:
"chars" sendCharsX ;

sendChars2:
"chars2" panel.getValue execWord ;

sendChars3:
"chars3" panel.getValue execWord ;

sendChars4:
"chars4" panel.getValue execWord ;

sendChars5:
"chars5"  sendCharsX ;

sendCharsX:
  panel.getValue
  dup .. "  " .
  esp32.send
 ;

sendStopRun:
"Q" Serial.send "Run stopped." .. stop ;

ssend: (serial send: [string] --)
Serial.send 5 Serial.getResponse drop
;


 "G100" Serial.send
 0.8 sleep
 getStat
;

getStat:
 "S" Serial.send 0.3 sleep
 1 Serial.getResponse dup .. "[|]" $split " size=" . dup $array.size .
 dup 0 $array.get " 0=[" .. . "]" .
 dup 1 $array.get " 1=[" . . "]" .
 dup 2 $array.get " 2=[" . . "]" .
 dup 3 $array.get " 3=[" . . "]" .
 dup 4 $array.get " 4=[" . . "]" .
 drop CR ;


 1 Serial.getResponse dup .. "[|]" $split $array.size ..

sleep1: 0.5 sleep ;

loop1:
 "L2" Serial.send sleep1
;

loop2:
 "L2" Serial.send sleep1
 "L8" Serial.send sleep1
;

waitResp:
0.02 sleep
# 5 Serial.getResponse drop
 5 Serial.getResponse drop ;

func2:
1 while
"VA0" Serial.send waitResp
 1 sleep
 "VA90" Serial.send waitResp
 0.5 sleep
 "VA45" Serial.send waitResp
 1 sleep
 "VA140" Serial.send waitResp
 1 sleep
 1 loop ;


func3:
func1:
"print?line=4&text=qwerty" esp32.send ;

"ultrasonic" esp32.send ;

"gpio" esp32.send ;


"gpio?pin=10"
  dup "&mode=out" $+ esp32.send
  esp32.send ;

"gpio?pin=10"
  "&val=0" $+ esp32.send ;

"gpio?pin=10"
  esp32.send ;

"Library.for" file.include
TFT_VIOLET .. ;

"inpHttp" panel.getValue .

"info" esp32.send ;

"led_on" esp32.send
"gpio?pin=2"
  esp32.send ;

"led_off" esp32.send
"gpio?pin=2" esp32.send ;

"led_flash?n=10" esp32.send
"gpio?pin=2" esp32.send ;

# test include
  "Library.for" file.include
  "Initialized." . CR
  TFT_BROWN . " " .
  TFT_PINK dup . " > " . $num .
  ;

"reset" esp32.send ;

:TestRect ( Testing Rectangle )
"draw?x=0&y=10&h=30&w=100&color=0&fill=1&rect=1"
  esp32.send
"draw?x=20&y=10&h=10&w=20&color=1&fill=1&rect=1&x=80&y=10&h=20&w=20&color=1&r=20&fill=1&rect=1"
  esp32.send
"draw?x=85&y=19&h=10&w=10&color=0&r=20&fill=1&rect=1&x=20&y=25&h=10&w=20&color=1&fill=1&rect=1"
  esp32.send
"draw?x=00&y=20&h=10&w=10&color=1&fill=0&rect=1&x=60&y=10&h=20&w=10&color=1&r=10&rect=1"
  esp32.send
;

:eyes (draw 2 eyes)
"draw?x=0&y=10&h=28&w=127&color=0&fill=1&r=0&rect=1" esp32.send
60 (x pos of whites)
dup
dup "draw?h=20&w=20&color=1&fill=1&r=20&y=10&x=:X&rect=0" ":X" $replace esp32.send
25 (x gap of whites)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send

5 (x pos of blacks relative to whites)
+ dup
18 (y pos of blacks)
"draw?h=10&w=10&color=0&fill=1&r=20&y=:Y&x=:X&rect=0" ":Y" $replace ":X" $replace esp32.send
25 (x gap of blacks)
+ "draw?x=:X&rect=1" ":X" $replace esp32.send
;


(Test placer)
null 44 "[q]" 33 ":x:" 1 ":z:" ":x:qwe-[q]-rty:z:" placer " = " . . .stack ;

:placer ( [null] [value] [token] [main string] -- [main string])
 begin swap dup ?null not if $replace loop drop ;

(==== Servo ================)

panel_start2: (add functions to the Run selector )
"servo_initialize" "selRun" panel.addOption
"pen_up" "selRun" panel.addOption
"pen_down" "selRun" panel.addOption
"servo_reset" "selRun" panel.addOption
"servo_mid" "selRun" panel.addOption
"servo_far" "selRun" panel.addOption
"draw_sample" "selRun" panel.addOption
"draw_sample1" "selRun" panel.addOption
"draw_sample2" "selRun" panel.addOption
;


"servo?steps=1&delay=50" esp32.send ;

:PL 5 ; (left pin 5)
:PR 4 ; (right pin 4)
:PC 32 ; (pin 32)
:c "7" ; (center pin 32)
:l "3" ; (left pin 5)
:r "2" ; (right pin 4)

:lMax 85 ; (left max)
:lMin 30 ; (left min )
:rMax 150 ; (right max)
:rMin 95 ; (right min)
:l_reset lMin ; (left reset) 15
:r_reset rMax ; (right reset) 155
:c_reset 90 ; (up)
:c_down 35 ;
:c_mid 60 ;

(test speed)([VL] [VR] --)
10 20
l r "servo?steps:L=:VL&steps:R=:VR&delay=2" ":R" $replace ":L" $replace ":VR" $replace ":VL" $replace 
dup .. esp32.send ;

:servo.send (send to servo)
  "servo?" swap $+
  dup . " > " .
  esp32.send
  "delay" panel.getValue $num 0 > if "delay" panel.getValue $num sleep endif
  ;

:servo_initialize (initialize)
  PL l PR r PC c "pin:L=:PL&pin:R=:PR&pin:C=:PC"
  ":C" $replace ":PC" $replace
  ":R" $replace ":PR" $replace
  ":L" $replace ":PL" $replace
  servo.send
  l lMin lMax servoMinMax
  r rMin rMax servoMinMax
  set_speen
  set_delay
  servo_reset
  "Servos initialized.\n" . ;

:servoMinMax ([servo#] [min] [max])
  "min:S=:MI&max:S=:MA" ":MA" $replace ":MI" $replace ":S" $replace
  servo.send ;

:set_speed 2 (1|2|..)
  null swap ":V" l ":L" r ":R" "steps:L=:V&steps:R=:V" placer servo.send ;

:set_delay 10
  "delay=:V" ":V" $replace servo.send ;

:pen_up
  c_reset c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

:pen_down
  c_down c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

:pen_mid
  c_mid c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

  50 c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

:serveX ([# angle] [$ servo number] --)
  "deg#=@" "#" $replace "@" $replace
  servo.send ;

:servoLR ([# angle left] [# angle right] --)
  l r "deg:L=:VL&deg:R=:VR" ":R" $replace ":L" $replace ":VR" $replace ":VL" $replace
  servo.send ;

:addLR
  l r "add:L=:VL&add:R=:VR" ":R" $replace ":L" $replace ":VR" $replace ":VL" $replace
  servo.send ;


lMax l serveX
lMin l serveX
rMin r serveX
rMax r serveX
lMax rMin servoLR
lMin rMax servoLR ;

:servo_far (min)
  90 90 servoLR ;

(reset)
:servo_reset
  c "steps:C=3" ":C" $replace servo.send
  c_reset c serveX
  l_reset r_reset servoLR
  "Servos reset.\n" . ;

(mid1)
45 125 servoLR ;
50 120 servoLR ;
55 115 servoLR ;


:draw_sample (sample draw)
servo_reset
\pen_down
\30 140
\35 l serveX 135 r serveX
40 130 servoLR
pen_down
45 l serveX 125 r serveX
50 l serveX 120 r serveX
55 l serveX 115 r serveX
pen_up
servo_reset
;

lMax l serveX ;
lMin l serveX ;
rMax r serveX ;
rMin r serveX ;


:draw_sample1 (sample draw)
  servo_reset
  40 130 servoLR
  pen_down
  45 125 servoLR
  10 0 do
    5 3 addLR
  loop

  10 0 do
    -3 -5 addLR
  loop
  servo_reset
;
50 l serveX 120 r serveX
55 l serveX 115 r serveX
pen_up
servo_reset
;

:draw_sample2 (sample draw)
servo_reset
40 130 servoLR
pen_down
45 140 servoLR
45 l serveX 125 r serveX
50 l serveX 120 r serveX
55 l serveX 115 r serveX
pen_up
servo_reset
;

:draw_sample2 (sample draw)
servo_reset
pen_down
60 120 servoLR
pen_up
servo_reset
pen_down
80 100 servoLR
pen_up
servo_reset
;


(mid)
:servo_mid
90 90 servoLR ;

