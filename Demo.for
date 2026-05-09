# ESP32.for
#
# Ifs & Loops:
#  [boolean] if ... endif
#  [boolean] while ... [boolean] loop
#  [end] [start] do ... loop
#  begin [boolean] if ... loop
#
:start (create panel) showHttp ;

:showHttp (Web panel)

panel.start

10 panel.addGap
"eyes" "inpFunc3" panel.addInput
"Exec" "goFunc3" "btnGF3" panel.addButton

20 panel.addGap
"ESP32 URL" panel.addLabel
"http://10.0.0.50/" "inpHttp" panel.addInput

"selectHttp" "selHttp" panel.addSelector
"http://10.0.0.50/" "selHttp" panel.addOption
"http://10.0.0.51/" "selHttp" panel.addOption

10 panel.addGap
"Delay After Send" panel.addLabel
"0.0" "delay" panel.addInput

10 panel.addGap
"Get Info" "getInfo" "btnInfo" panel.addButton
"LED 1" "led_1" "btnLED1" panel.addButton
"LED 0" "led_0" "btnLED0" panel.addButton

10 panel.addGap
"gpio?pin=10&mode=out" "chars2" panel.addInput
"Send" "sendChars2" "btnSC2" panel.addButton

10 panel.addGap
"gpio?pin=10&val=0" "chars3" panel.addInput
"Send" "sendChars3" "btnSC3" panel.addButton

10 panel.addGap
"gpio?pin=10&val=1" "chars4" panel.addInput
"Send" "sendChars4" "btnSC4" panel.addButton
;

######################################## end of start

:showSerial (Serial panel)

panel.start

10 panel.addGap
"Serial Port Number" panel.addLabel
"0" "inpSerNum" panel.addInput
"Serial Open" "ser_open" "btnOpen" panel.addButton
"Serial Close" "ser_close" "btnClose" panel.addButton

10 panel.addGap
"getInfo" "inpFunc1" panel.addInput
"Exec" "goFunc1" "btnGF1" panel.addButton

10 panel.addGap
"func1" "inpFunc2" panel.addInput
"Exec" "goFunc2" "btnGF2" panel.addButton

10 panel.addGap
"func2" "inpFunc3" panel.addInput
"Exec" "goFunc3" "btnGF3" panel.addButton

;
######################################## end of start

:panel.start (Start panel)
panel.clear
0.4 sleep
"List" "words" "btnList1" panel.addButton
"List Functions" "functions" "btnList2" panel.addButton
5 panel.addGap
"Debug ON" "debug1" "btnDebug1" panel.addButton
"Debug OFF" "debug0" "btnDebug0" panel.addButton
"Stack" "stack" "btnStack" panel.addButton
5 panel.addGap
"HTTP" "showHttp" "btnHttp" panel.addButton
"SERIAL" "showSerial" "btnSerail" panel.addButton
;

list10: list ;
list20: list2 ;
debug1: debug_on "Debug ON" .green CR ;
debug0: debug_off "Debug  OFF" .purple CR ;
stack: .stack ;

:.blue ([$] --) 0 0 200 .color ;
:.purple ([$] --) 127 0 127 .color ;
:.green ([$] --) 0 200 0 .color ;

.errorColor: ([string] --) dup "Error" $contains if CR 200 0 0 .color else 0 120 0 .color endif ;

:.green 0 150 0 .color ;

( loop tests )
20 1 while i . " " . dup .green " " . 1 - dup loop ;
20 1 do i . " " . loop ;
1 20 do i drop " " . loop ;

# test include
  "Library.for" file.include
  "Initialized." . CR
  TFT_BROWN . " " .
  TFT_PINK dup . " > " . $num .
  ;

debug_on
(debugging)
"correct" false if 0 123 123 123 "qweqe" aksjhd endif ..
3 1 do "123 " . loop "" ..
3 true while "www " . 1 - dup loop drop ;

(Test infinite loop)
begin " i=" . i . loop ;

(Test finite loops)
begin i 10 <= while " i=" .green i .red loop .stack ;

begin i 20 <= if " i=" . i .red loop .stack ; (if can also be used instead of while)

begin i 20 <= if " i=" . i .red 2 +loop .stack ;

20 0 do " i=" . i .red loop .stack ;

20 0 do " i=" . i .red 2.5 +loop .stack ;

(array demo)
12 34 45 3 array.make
"size= " .. dup array.size .
"string= " .. "|" array.to$ .
;
(Demo replacer)
null 44 "[q]" 33 ":x:" 1 ":z:" ":x:qwe-[q]-rty:z:" replacer ." = " . .stack ;

:replacer ( [null] [value 1] [token 1] ... [value n] [token n] [main string] -- [main string])
 begin swap dup ?null not if $replace loop drop ;

#-----------------------------------

esp32.send: ([query] --)
"inpHttp" panel.getValue swap $+
  dup CR ."Send: " . ." > "
  http.get .errorColor CR
  \.stack
;

:getInfo "info" esp32.send ;
:led_1 "led_1" esp32.send ;
:led_0 "led_0" esp32.send ;
:led_flash "led_flash?n=3" esp32.send ;
:reset "reset" esp32.send ;

sendChars2: "chars2" sendX ;
sendChars3: "chars3" sendX ;
sendChars4: "chars4" sendX ;
sendX: panel.getValue esp32.send ;

goFunc1: "inpFunc1" goFuncX ;
goFunc2: "inpFunc2" goFuncX ;
goFunc3: "inpFunc3" goFuncX ;
goFuncX: panel.getValue dup .. CR execWord ;

#--------------------------------------

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

#----------------------------------

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


(==== Servo ================)

:PL 5 ; (left: gpio pin 5)
:PR 4 ; (right: gpio pin 4)
:PC 32 ; (center: gpio pin 32)
:c "7" ; (center: servo 7)
:l "3" ; (left: servo 3)
:r "2" ; (right: servo 2)

:lMax 85 ; (left max)
:lMin 30 ; (left min )
:rMax 150 ; (right max)
:rMin 95 ; (right min)
:l_reset lMin ; (left reset)
:r_reset rMax ; (right reset)
:c_reset 90 ; (pen up)
:c_down 35 ; (pen down)
:c_mid 60 ;

(test speed)([VL] [VR] --)
10 20
l r "servo?speed:L=:VL&speed:R=:VR&delay=2" ":R" $replace ":L" $replace ":VR" $replace ":VL" $replace 
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
  set_speed
  set_delay
  servo_reset
  "Servos initialized.\n" . ;

:servoMinMax ([servo#] [min] [max])
  "min:S=:MI&max:S=:MA" ":MA" $replace ":MI" $replace ":S" $replace
  servo.send ;

:set_speed 10 (1|2|..)
  null swap ":V" l ":L" r ":R" "speed:L=:V&speed:R=:V" placer servo.send
  1 c "speed:C=:V" ":C" $replace ":V" $replace servo.send
  ;

:set_delay 1
  "delay=:V" ":V" $replace servo.send ;

:pen_up
  c_reset c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

:pen_down
  c_down c "deg:S=:D&go=1" ":S" $replace ":D" $replace
  servo.send ;

:pen_mid
  c_mid c "deg:S=:D&go=0" ":S" $replace ":D" $replace
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
  set_speed
  c "speed:C=4" ":C" $replace servo.send
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




