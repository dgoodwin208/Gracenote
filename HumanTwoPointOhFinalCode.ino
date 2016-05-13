

#include <SoftwareSerial.h>

// define the pins used
#define VS1053_RX  2 // This is the pin that connects to the RX pin on VS1053

#define VS1053_RESET 9 // This is the pin that connects to the RESET pin on VS1053
// If you have the Music Maker shield, you don't need to connect the RESET pin!

// If you're using the VS1053 breakout:
// Don't forget to connect the GPIO #0 to GROUND and GPIO #1 pin to 3.3V
// If you're using the Music Maker shield:
// Don't forget to connect the GPIO #1 pin to 3.3V and the RX pin to digital #2

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79


// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_RAIN 87
#define VS1053_GM1_PIANO 1
#define VS1053_GM1_SLAPBASS 38
#define VS1053_GM1_OOH 54
#define VS1053_GM1_ORGAN 54
int instrument_choice = VS1053_GM1_OCARINA ;

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_STEREO 0x0A
#define MIDI_CHAN_PROGRAM 0xC0

//Set pins for ultrasound device
#define US_1_TRIG 12 //CENTER
#define US_1_ECHO 13
#define US_2_TRIG 10 //RIGHT
#define US_2_ECHO 11
#define US_3_TRIG 6 //LEFT
#define US_3_ECHO 7 


//Set some parameters for the distance sensor
#define US_VALUES_MAX 300
#define US_VALUES_MIN 10

//Set parameters for notes
#define SOUND_SCALE_NUM_NOTES 24
#define SOUND_SCALE_MAX 68
#define SOUND_DURATION_MAX 200
#define SOUND_DURATION_MIN 10

#define SOUND_PERIOD_MAX 2000
#define SOUND_PERIOD_MIN 100

#define SOUND_LEFT_NOTE 60 // C4
#define SOUND_RIGHT_NOTE 67 // G4
long dist_cm_us1 = 0;
long last_dist_1 = 0;
long dist_cm_us2 = 0;
long dist_cm_us3 = 0;
int current_note = 0;
int next_note = 0;
int note_duration = 0;
SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side
// on a Mega/Leonardo you may have to change the pin to one that
// software serial support uses OR use a hardware serial port!

// timing things
unsigned long compareTime1,compareTime2,compareTime3 ;
unsigned long delayAmountMillis1 = 10; //millis to delay between sound files
unsigned long delayAmountMillis2 = 10; //millis to delay between sound files
unsigned long delayAmountMillis3 = 10; //millis to delay between sound files
boolean bDelayStarted1 = false; //Center
boolean bDelayStarted2 = false; //Right
boolean bDelayStarted3 = false; //Left

void setup() {
  Serial.begin(9600);
  Serial.println("VS1053 MIDI test");

  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'

  pinMode(VS1053_RESET, OUTPUT);
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(10);

  midiSetChannelBank(0, VS1053_BANK_MELODY);
  midiSetInstrument(0, instrument_choice);
  midiSetChannelVolume(0, 127);

  //Set an instrument for the right ear (sensor #2)
  midiSetChannelBank(1, VS1053_BANK_MELODY);
  midiSetInstrument(1, VS1053_GM1_PIANO);
  midiSetChannelVolume(1, 127);
  midiSetChannelStereo(1, 127); 
  
  //Set an instrument for the left ear (sensor #3)
  midiSetChannelBank(2, VS1053_BANK_MELODY);
  midiSetInstrument(2, VS1053_GM1_PIANO);
  midiSetChannelVolume(2, 127);
  midiSetChannelStereo(1, 0); 
}

void loop() {


  dist_cm_us1 = getDistance(1);
  delay(20);
  dist_cm_us2 = getDistance(2);
  delay(20);
  dist_cm_us3 = getDistance(3);
  
  Serial.print(dist_cm_us1);
  Serial.print(" | ");
  Serial.print(dist_cm_us2);
  Serial.print(" | ");
  Serial.print(dist_cm_us3);
  Serial.print(" b ");
  Serial.print(bDelayStarted1);
  Serial.print(" | ");
  Serial.print(bDelayStarted2);
  Serial.print(" | ");
  Serial.println(bDelayStarted3);


  
  // Map the distance from the device to the number of notes from the lowest note
  //Front sensor
  if (!bDelayStarted1 && (dist_cm_us1!=0)){
    
    compareTime1 = millis(); //set the comparison to store the current time
    next_note = map(dist_cm_us1, US_VALUES_MIN, US_VALUES_MAX, 0, SOUND_SCALE_NUM_NOTES); //calcuates number of notes
    next_note = SOUND_SCALE_MAX - next_note; //number of notes from highest note

    midiNoteOn(0, next_note , 127);
    delay(50);
    midiNoteOff(0, next_note , 127);
    Serial.print("Playing ");
    Serial.print(next_note);
    Serial.println();
  
    note_duration = map(dist_cm_us1, US_VALUES_MIN, US_VALUES_MAX, SOUND_DURATION_MIN, SOUND_DURATION_MAX); //calcuates number of notes
  
    delayAmountMillis1 = note_duration;
     bDelayStarted1 = true; // switch this off so the events are momentary
  }
  else {
    if(millis() - compareTime1 > delayAmountMillis1){ //wait for the delay time to pass
      //Serial.println("time's up!"); //just for debug
      bDelayStarted1 = false; //switch back to momentary trigger
    }
  }

  
  //Right one
  if (!bDelayStarted2 && (dist_cm_us2 != 0)){

    compareTime2 = millis(); //set the comparison to store the current time
    next_note = SOUND_RIGHT_NOTE;
    
    midiNoteOn(1, next_note , 127);
    delay(50);
    midiNoteOff(1, next_note , 127);
    Serial.print("Playing ");
    Serial.print(next_note);
    Serial.println();
  
    note_duration = map(dist_cm_us2, US_VALUES_MIN, US_VALUES_MAX, SOUND_PERIOD_MIN, SOUND_PERIOD_MAX); //calcuates number of notes
  
    delayAmountMillis2 = note_duration;
     
    bDelayStarted2 = true; 
  }
  else { if(millis() - compareTime2 > delayAmountMillis2){ //wait for the delay time to pass
      bDelayStarted2 = false; //switch back to momentary trigger
    }
  }
  
  
  //Left one
  if (!bDelayStarted3 && (dist_cm_us3 != 0)){
 
    compareTime3 = millis(); //set the comparison to store the current time
    
    next_note = SOUND_LEFT_NOTE;
    
    midiNoteOn(2, next_note , 127);
    delay(50);
    midiNoteOff(2, next_note , 127);
    Serial.print("Playing LEFT ");
    Serial.print(next_note);
    Serial.println();
  
    note_duration = map(dist_cm_us3, US_VALUES_MIN, US_VALUES_MAX, SOUND_PERIOD_MIN, SOUND_PERIOD_MAX); //calcuates number of notes
  
    delayAmountMillis3 = note_duration;
     
    bDelayStarted3 = true; 
  }
  else { if(millis() - compareTime3 > delayAmountMillis3){ //wait for the delay time to pass
      bDelayStarted3 = false; //switch back to momentary trigger
    }
  }
  
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  VS1053_MIDI.write(inst);
}

void midiSetChannelStereo(uint8_t chan, uint8_t panVal) {
  if (chan > 15) return;
  if (panVal > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_STEREO);
  VS1053_MIDI.write(panVal);
}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

long getDistance(int usDevice)
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;

  int trigPin = US_1_TRIG;
  int echoPin = US_1_ECHO;
  
  if (usDevice==2){
    trigPin = US_2_TRIG;
    echoPin = US_2_ECHO;
  }
  if (usDevice==3){
    trigPin = US_3_TRIG;
    echoPin = US_3_ECHO;
  }
  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH, 25000);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);

  return cm;
}
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


