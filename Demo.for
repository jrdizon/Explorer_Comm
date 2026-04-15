# ESP32.for
#
# Ifs & Loops:
#  [boolean] if ... endif
#  [boolean] while ... [boolean] loop
#  [end] [start] do ... loop
#  begin [boolean] if ... loop
#
:start showHttp ;

:showHttp

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

:showSerial

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

:panel.start
panel.clear
0.1 sleep
"List" "list10" "btnList1" panel.addButton
"List Functions" "list20" "btnList2" panel.addButton
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

:ser_open
  "inpSerNum" panel.getValue dup "Serial#" .. . " open" .
  $num Serial.open
  ;

:ser_close
  Serial.close
  "Serial Port closed." ..
  ;

selectHttp: "selHttp" panel.getValue "inpHttp" panel.setValue ;

//--

.errorColor: ([string] --) dup "Error" $contains dup if dup2 200 0 0 .color endif not if 0 120 0 .color endif ;
//--
esp32.send: ([query] --)
"inpHttp" panel.getValue swap $+ http.get
  .errorColor CR
  "delay" panel.getValue $num 0 > if "delay" panel.getValue $num sleep endif
;

:getInfo "info" esp32.send ;
:led_1 "led_1" esp32.send ;
:led_0 "led_0" esp32.send ;
:led_flash "led_flash?n=3" esp32.send ;


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
"chars2" sendCharsX ;

sendChars3:
"chars3" sendCharsX ;

sendChars4:
"chars4"  sendCharsX ;

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

//--

Serial.close
//--

//--
 "G100" Serial.send
 0.8 sleep
 getStat

//--
getStat
//--
getStat:
 "S" Serial.send 0.3 sleep
 1 Serial.getResponse dup .. "[|]" $split " size=" . dup $array.size .
 dup 0 $array.get " 0=[" .. . "]" .
 dup 1 $array.get " 1=[" . . "]" .
 dup 2 $array.get " 2=[" . . "]" .
 dup 3 $array.get " 3=[" . . "]" .
 dup 4 $array.get " 4=[" . . "]" .
 drop CR ;
//--

 1 Serial.getResponse dup .. "[|]" $split $array.size ..

//--

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

//--

func3:
1 while
 "G400" Serial.send waitResp
 "G-500" Serial.send waitResp
 "G300" Serial.send waitResp
 "G-20" Serial.send waitResp
"VA0" Serial.send waitResp
 "G20" Serial.send waitResp
 "G20" Serial.send waitResp
 "G-20" Serial.send waitResp
 "VA90" Serial.send waitResp
 1 sleep
 1 loop ;

func1:
"servo?init=10" esp32.send
1 while
"" ..
"servo?deg=0" esp32.send
0.6 sleep
"servo?deg=45" esp32.send
"servo?deg=125" esp32.send
"servo?deg=180" esp32.send
"servo?deg=90" esp32.send
0.6 sleep
true
loop

//--

"servo?deg=0" esp32.send ;
"servo?deg=180" esp32.send ;
//--


"servo?deg=180" esp32.send

//--

"print?line=4&text=qwerty" esp32.send
//--

"ultrasonic" esp32.send
//--

servo30:
  dup $num 20 < if
    dup $num 10 * to$
    "servo?deg=" swap $+
    dup ..
    "http://10.0.0.50/" swap $+ http.get drop
  } drop ;

//--

1 while
"ultrasonic" "http://10.0.0.50/" swap $+ http.get
dup
..

dup
"print?line=3&size=2&text=" swap $+ "http://10.0.0.50/" swap $+ http.get drop
#"print?size=1&text=+cm" "http://10.0.0.50/" swap $+ http.get drop

dup servo30

dup
$num 2 / to$
#"rect?x=0&y=0&h=8&color=0&w=128" "http://10.0.0.50/" swap $+ http.get ..
"rect?x=0&y=0&h=8&color=1&w=" swap $+ "http://10.0.0.50/" swap $+ http.get drop
dup
"rect?y=0&h=8&color=0&w=128&x=" swap $+ "http://10.0.0.50/" swap $+ http.get drop

drop
0.02 sleep
 1
 loop

//--

"print?line=2&text=Hello+&white=5&clear=1&size=2" "http://10.0.0.50/" swap $+ http.get ..

//--

#"rect?x=10&y=1&h=32&w=20&color=10" "http://10.0.0.50/" swap $+ http.get ..
"print?text=Hello+&white=0&clear=1&size=1" "http://10.0.0.50/" swap $+ http.get ..
"print?line=3&text=Hello+&white=0&clear=0&size=2" "http://10.0.0.50/" swap $+ http.get ..

//--

"cursor?x=10&y=4" esp32.send
"print?text=Hello" esp32.send



"servo?init=10" esp32.send ;
"servo?deg=120" esp32.send ;

"ssswifi-off" esp32.send ;

"inline" esp32.send ;

"gpio" esp32.send ;


"gpio?pin=10"
  dup "&mode=out" $+ esp32.send
  esp32.send ;

"gpio?pin=10"
  "&val=0" $+ esp32.send ;

"gpio?pin=10"
  esp32.send ;
//--

"10" $num 5 / .. ;

list ;
list2 ;


"Library.for" file.include
TFT_VIOLET .. ;

//--

"inpHttp" panel.getValue .

"info" esp32.send ;

"led_on" esp32.send
"gpio?pin=2"
  esp32.send ;

"led_off" esp32.send
"gpio?pin=2" esp32.send ;

"led_flash?n=10" esp32.send
"gpio?pin=2" esp32.send ;


"servo?init=10" esp32.send ;
"servo?deg=90" esp32.send ;

"adc?init=36" esp32.send ;
:adc36
1 while "adc?pin=36" esp32.send CR 1 loop ;


"adc?pin=36" esp32.send ;

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



"reset" esp32.send ;

:TestLine ( Testing Line )
"print?clear=1&text=123" esp32.send
"line?x1=0&y1=10&x2=80&y2=10&color=1&wait=1" esp32.send
"line?x1=0&y1=12&x2=100&y2=12&wait=1" esp32.send
"line?x1=0&y1=15&x2=130&y2=15&wait=1" esp32.send
"line?x1=1&y1=20&x2=100&y2=10&wait=1" esp32.send
"line?x1=1&y1=22&x2=100&y2=12" esp32.send
;

:TestRect ( Testing Rectangle )
"print?clear=1" esp32.send
"rect?x=0&y=10&h=20&w=100&color=0&fill=1&wait=1" esp32.send
"rect?x=00&y=20&h=10&w=10&color=1&fill=0&wait=1" esp32.send
"rect?x=20&y=10&h=10&w=20color=1&fill=1&wait=1" esp32.send
"rect?x=60&y=10&h=20&w=10&color=1&fill=0&r=10&wait=1" esp32.send
"rect?x=80&y=10&h=20&w=20color=1&fill=1&r=20&wait=1" esp32.send
"rect?x=85&y=19&h=10&w=10&color=0&fill=1&r=20&wait=1" esp32.send
"rect?x=20&y=25&h=1&w=20color=1&fill=1" esp32.send
;

:eyes (draw 2 eyes)
"rect?x=0&y=10&h=20&w=100&color=0&fill=1" esp32.send
10
dup eye
25 + eye
"print" esp32.send
;

:eyes2 (draw 2 eyes)
"rect?x=0&y=10&h=20&w=100&color=0&fill=1" esp32.send
10 eye
35 eye
"print" esp32.send
;

:eye ([x] --)
  dup
  to$ "rect?h=20&w=20&color=1&fill=1&r=20&y=10&wait=1&wait=1&x=" swap $+ esp32.send
  5 +
  to$ "rect?h=10&w=10&color=0&fill=1&r=20&y=19&wait=1&wait=1&x=" swap $+ esp32.send
;

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

begin i 20 <= if " i=" . i .red 2 loop+ .stack ;

true while " i=" . i .red i 20 < loop .stack ;

true while " i=" . i .red i 20 < 2 loop+ .stack ;

20 0 do " i=" . i .red loop .stack ;

20 0 do " i=" . i .red 2 loop+ .stack ;

