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
"Go" "goFunc3" "btnGF3" panel.addButton

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
"getStat" "inpFunc1" panel.addInput
"Go" "goFunc1" "btnGF1" panel.addButton

10 panel.addGap
"func1" "inpFunc2" panel.addInput
"Go" "goFunc2" "btnGF2" panel.addButton

10 panel.addGap
"func2" "inpFunc3" panel.addInput
"Go" "goFunc3" "btnGF3" panel.addButton

;
######################################## end of start

:panel.start (Start panel)
panel.clear
0.1 sleep
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

.errorColor: ([string] --) dup "Error" $contains dup if dup2 200 0 0 .color endif not if 0 120 0 .color endif ;

goFunc1:
"inpFunc1" goFuncX ;

goFunc2:
"inpFunc2" goFuncX ;

goFunc3:
"inpFunc3" goFuncX ;

goFuncX: panel.getValue dup .. CR execWord ;


sendChars:
"chars" sendCharsX ;

sleep1: 0.5 sleep ;

func2:

func3:

func1:
"10" $num 5 / .. ;

"Library.for" file.include
TFT_VIOLET .. ;

"inpHttp" panel.getValue . ;

"ajggjsd" panel.getValue . ;

list ;

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

"876876" .green ;

".123" $num . ;

debug_on
(debugging)
"correct" false if 0 123 123 123 "qweqe" aksjhd endif ..
3 1 do "123 " . loop "" ..
3 true while "www " . 1 - dup loop drop ;

(Test infinite loop)
begin " i=" . i . loop ;

(Test finite loop)
begin i 10 <= if " i=" .green i .red loop .stack ;

begin i 20 <= if " i=" . i .red loop .stack ;

begin i 20 <= if " i=" . i .red 2 +loop .stack ;

true while " i=" . i .red i 20 < loop .stack ;

true while " i=" . i .red i 20 < 2 +loop .stack ;

20 0 do " i=" . i .red loop .stack ;

20 0 do " i=" . i .red 2.5 +loop .stack ;

(array demo)
12 34 45 3 array.make
"size= " .. dup array.size .
"string= " .. "|" array.to$ .
;
(Demo placer)
null 44 "[q]" 33 ":x:" 1 ":z:" ":x:qwe-[q]-rty:z:" placer " = " . . .stack ;

:placer ( [null] [value 1] [token 1] ... [value n] [token n] [main string] -- [main string])
 begin swap dup ?null not if $replace loop drop ;


