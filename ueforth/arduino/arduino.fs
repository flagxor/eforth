( Set up Basic I/O )
: arduino-bye   0 terminate ;
' arduino-bye is bye
: arduino-type ( a n -- ) Serial.write drop ;
' arduino-type is type
: key? ( -- n ) Serial.available ;
: arduino-key ( -- n )
   begin Serial.available until 0 >r rp@ 1 Serial.readBytes drop r> ;
' arduino-key is key

( Map Arduino / ESP32 things to shorter names. )
: pin ( n n -- ) swap digitalWrite ;
: adc ( n -- n ) analogRead ;
: duty ( n n -- ) 255 min 8191 255 */ ledcWrite ;
: freq ( n n -- ) 1000 * 13 ledcSetup drop ;
: tone ( n n -- ) 1000 * ledcWriteTone drop ;

( Startup Setup )
-1 echo !
115200 Serial.begin
100 ms
-1 z" /" 10 SPIFFS.begin drop
