#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>
#define PIN 6	 // input pin Neopixel is attached to
#define NUMPIXELS      16 // number of neopixels in strip

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// rgb farben
byte r=50; 	// rot
byte g=50; 	// grün
byte b=50;	// blau
// zeit
byte ss=0;	//sec*2
byte mm=10;	//min
byte hh=10;	//std
byte mm1=10;	//min-alarm1
byte hh1=10;	//std-alarm1
byte mm2=10;	//min-alarm2
byte hh2=10;	//std-alarm2
// Cycle counter
byte zelle = 0;
//buttons
const byte bwd=4;
const byte fwd=5;
const byte toggleSetting=2;
const byte toggleMod=3;
//buzzer
const byte buzzer=7;
//Status-Variablen
byte alarmSetting = 0;
boolean skipHour = false;
boolean alarmOff = false;
byte mod = 0;

// int helligkeit = A5

void setup(){
  pixels.begin();
  Serial.begin(9600);
  // set pins
  pinMode(bwd, INPUT);
  pinMode(toggleSetting, INPUT_PULLUP);
  pinMode(fwd, INPUT);
  pinMode(toggleMod, INPUT_PULLUP);
  // set interrupts
  attachInterrupt(digitalPinToInterrupt(toggleMod), setMod, RISING);
  attachInterrupt(digitalPinToInterrupt(toggleSetting), setSetting, RISING);
  Timer1.initialize();
  Timer1.setPeriod(1000000); // parameterwert in nanosekunden
  Timer1.attachInterrupt(countSeconds);
}

void loop(){
  switch (mod){ // modus des Weckers
  	case 0:      // zeitmodus normal
    	setColor(0,0,55);
    	setLED(mm,hh); // LED entsprechend Zeit setzen
    	startAlarm();
    break;
    case 1:       // modus zeit stellen
    	if (zelle%25 < 15){
    		setColor(0,55,0);
        }else{
        	setColor(0,0,0);
        }
    	setLED(mm,hh); // LED entsprechend Zeit setzen
    break;
    case 2:       // modus alarm 1 stellen
    	if (zelle%25 < 15){
    		setColor(50,0,0);
        }else{
      		setColor(0,0,0);
    	}
    	setLED(mm1,hh1); // LED entsprechend Zeit setzen
    break;
    case 3:         // modus alarm 2 stellen
    	if (zelle%25 < 15){
    		setColor(50,0,0);
   		}else{
      	setColor(0,0,0);
    	};
    	setLED(mm2,hh2); // LED entsprechend Zeit setzen
    break;
  }
  setTime();
  if (zelle >249){zelle = 0;}
  zelle++;
  pixels.show(); // schickt die einstellung der LED zu den neoPixel
  delay(50);
}

void setColor (byte red, byte green, byte blue){
	  r=red;
  	g=green;
  	b=blue;
}

void setTime (){
  switch (mod) {
  	case 1:
      if (!skipHour && zelle%5 == 0){
        if (digitalRead(fwd)==1){
              mm++;
            }
        if (digitalRead(bwd)==1){
              mm--;
            }
      }
      if (skipHour && zelle%5 == 0){
      	if (digitalRead(fwd)==1){
            	hh++;
            }
      	if (digitalRead(bwd)==1){
            	hh--;
            }
      }
    break;
    case 2:
      if (!skipHour && zelle%10 == 0){
        if (digitalRead(fwd)){
              mm1++;
            }
        if (digitalRead(bwd)){
              mm1--;
            }
      }
      if (skipHour && zelle%10 == 0){
        if (digitalRead(fwd)){
            	hh1++;
            }
      	if (digitalRead(bwd)){
            	hh1--;
            }
      }
    break;
    case 3:
      if (!skipHour && zelle%10 == 0){
        if (digitalRead(fwd)){
              mm2++;
            }
        if (digitalRead(bwd)){
              mm2--;
            }
      }
      if (skipHour && zelle%10 == 0){
        if (digitalRead(fwd)){
              hh2++;
            }

        if (digitalRead(bwd)){
              hh2--;
            }
      }
    break;
  }
}

void setLED (byte m,byte h){
	for (int i=0;i<16;i++){								// LED setzen
        if(bitRead(m,i)!=1 && i<6){
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
        if(bitRead(m,i)==1 && i<6){
          pixels.setPixelColor(i, pixels.Color(r, g, b));
        }
        if(bitRead(h,(i-8))!=1 && i>7 && i<13){
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
        if(bitRead(h,(i-8))==1 && i>7 && i<13){
          pixels.setPixelColor(i, pixels.Color(r, g, b));
        }
        if(alarmSetting == 1 && i==14 || mod ==2 && i==14){
          pixels.setPixelColor(i, pixels.Color(50, 0, 0));
        }
      	if(alarmSetting == 2 && i==15 || mod ==3 && i==15){
          pixels.setPixelColor(i, pixels.Color(50, 0, 0));
        }

      	if(alarmSetting == 2 && i==14 && mod == 0 && mod !=2 || mod !=0 && i==14 && mod !=2){
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
      	if(alarmSetting != 2 && i==15 && mod == 0 && mod !=3 || mod !=0 && i==15 && mod !=3){
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
     }
}

void countSeconds(){
  ss++;
  if(ss>=60) {
    mm++;
    ss=00;
  }
  if(mm>=60){
    hh++;
    mm=00;
  }
  if(hh>=24){
    hh=00;
  }
}

void startAlarm(){
  if (alarmSetting ==1 && hh1==hh && mm1>=mm && mm1<=mm+2 && !alarmOff||
      alarmSetting ==2&& hh2==hh && mm2>=mm && mm2<=mm+2 && !alarmOff){
  	digitalWrite(buzzer, HIGH);
    if (digitalRead(bwd) || digitalRead(fwd)){
    	alarmOff = true;
    }
  } else {
    	digitalWrite(buzzer, LOW);
    if (alarmSetting ==1 && mm1==mm+4 || alarmSetting ==2&& mm2==mm+4){
    	alarmOff = false;
    }
  }
}

void setMod (){
  //disable interrupt while processing it
  detachInterrupt(digitalPinToInterrupt(toggleMod));
  // skip Modus wird standart auf Minuten gestellt.
  skipHour = false;
  mod++;
  if (mod > 3){mod=0;}
  // re-enable interrupt
  attachInterrupt(digitalPinToInterrupt(toggleMod), setMod, RISING);
}

void setSetting (){
  //disable interrupt while processing it
  detachInterrupt(digitalPinToInterrupt(toggleSetting));
  switch (mod){
  	case 0:
    // alarm modus 0=aus 1=alarm1 2=alarm2
 		alarmSetting++;
    	if (alarmSetting > 0){alarmOff = false;}
        if (alarmSetting > 2){alarmSetting=0;}
    break;
    default://zwischen skip moduls wechseln std/min
    	skipHour = !skipHour;
    break;
  }
  // re-enable interrupt
  attachInterrupt(digitalPinToInterrupt(toggleSetting), setSetting, RISING);
}
