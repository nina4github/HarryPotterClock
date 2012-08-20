/**
 * author Benjamin Eckel
 * date 10-17-2009
 * from http://www.gumbolabs.org/2009/10/17/parallax-rfid-reader-arduino/
 *
 */
#include <SoftwareSerial.h>
#define RFID_ENABLE 7   //to RFID ENABLE
#define CODE_LEN 10      //Max length of RFID tag
#define VALIDATE_TAG 1  //should we validate tag?
#define VALIDATE_LENGTH  200 //maximum reads b/w tag read and validate
#define ITERATION_LENGTH 2000 //time, in ms, given to the user to move hand away
#define START_BYTE 0x0A 
#define STOP_BYTE 0x0D
/* RFID pins. */
#define RF_RX_PIN 8
#define RF_TX_PIN 9

char tag[CODE_LEN];  
char* person = 0;
char* activity =0;
char* project= 0;

SoftwareSerial rfid_serial = SoftwareSerial(RF_RX_PIN, RF_TX_PIN); 

void setup() { 
  Serial.begin(9600);  
  Serial.println("Starting");
  pinMode(RFID_ENABLE,OUTPUT);
  pinMode(RF_RX_PIN, INPUT);
  pinMode(RF_TX_PIN, OUTPUT);

  rfid_serial.begin(2400);  
}

void loop() { 
  enableRFID(); 
  getRFIDTag();
  if(isCodeValid()) {
    disableRFID();
    if (isActivityTag()&&person==0){
      // FEEDBACK
      Serial.print("Good the activity is set to ");
      Serial.print(activity);
      Serial.println( "  , now add who you are or your project");
      // end FEEDBACK
    }
    else if (isProjectTag()&&person==0){
      // FEEDBACK
      Serial.print("Good the project is set to ");
      Serial.print( project);
      Serial.println( " , now add who you are or your current activity");
      // end FEEDBACK
    }

    else if ((isPersonTag()&&(activity!=0||project!=0))
      ||((isActivityTag()||isProjectTag())&&person!=0)){

      // FEEDBACK
      Serial.print("Good ");
      Serial.print( person);
      Serial.println( " now we send your message");  
      // end FEEDBACK

      sendMessage();

    }
    else if (isPersonTag()&&(activity==0||project==0)){
      //FEEDBACK
      Serial.print("Good ");
      Serial.print(person);
      Serial.println( " now add your activity or project");
      // end FEEDBACK
    }
    else {
      Serial.println("tag not recognized :( start again!");
      clearVariables();
    }

    delay(ITERATION_LENGTH);
  } 
  else {
    disableRFID();
    Serial.println("Got some noise");  
  }
  rfid_serial.flush();
  clearCode();
} 

/**
 * Clears out the memory space for the tag to 0s.
 */
void clearCode() {
  for(int i=0; i<CODE_LEN; i++) {
    tag[i] = 0; 
  }
}

void clearVariables(){
  person=0;
  activity=0;
  project = 0;
}

/**
 * Sends the tag to the computer.
 */
void sendCode() {
  Serial.print("TAG:");  
  //Serial.println(tag);
  for(int i=0; i<CODE_LEN; i++) {
    Serial.print(tag[i]); 
  } 
}

void sendMessage(){
  // SEND MESSAGE
  // FEEDBACK
  Serial.print("MESSAGE: ");
  Serial.print( person);
  Serial.print( "  ");
  if(activity!=0) Serial.print( activity);
  Serial.print( "  ");
  if (project!=0)     Serial.println(project);
  Serial.println( "  ");
  // end FEEDBACK

  // reset variables for the next activity message 
  clearVariables();
}

/**************************************************************/
/********************   RFID Functions  ***********************/
/**************************************************************/

void enableRFID() {
  digitalWrite(RFID_ENABLE, LOW);    
}

void disableRFID() {
  digitalWrite(RFID_ENABLE, HIGH);  
}

/**
 * Blocking function, waits for and gets the RFID tag.
 */
void getRFIDTag() {
  //Serial.println("getrfid");
  byte next_byte; 
  while(rfid_serial.available() <= 0) {
  }
  if((next_byte = rfid_serial.read()) == START_BYTE) {      
    byte bytesread = 0; 
    while(bytesread < CODE_LEN) {
      if(rfid_serial.available() > 0) { //wait for the next byte
        if((next_byte = rfid_serial.read()) == STOP_BYTE) break;       
        tag[bytesread++] = next_byte;                   
      }
    }                
  }    
}

/**
 * Waits for the next incoming tag to see if it matches
 * the current tag.
 */
boolean isCodeValid() {
  byte next_byte; 
  int count = 0;
  while (rfid_serial.available() < 2) {  //there is already a STOP_BYTE in buffer
    delay(1); //probably not a very pure millisecond
    if(count++ > VALIDATE_LENGTH) return false;
  }
  rfid_serial.read(); //throw away extra STOP_BYTE
  if ((next_byte = rfid_serial.read()) == START_BYTE) {  
    byte bytes_read = 0; 
    while (bytes_read < CODE_LEN) {
      if (rfid_serial.available() > 0) { //wait for the next byte      
        if ((next_byte = rfid_serial.read()) == STOP_BYTE) break;
        if (tag[bytes_read++] != next_byte) return false;                     
      }
    }                
  }
  return true;   
}

boolean isPersonTag(){

  if (strncmp(tag,"2700BA6F2A",10)==0){ //A
    person = "morten_idemolab";
    return true;
  }
  else if (strncmp(tag,"2600D659FB",10)==0){
    person = "@mikkel_idemolab";

    return true;
  }
  else return false;

}

boolean isActivityTag(){
  if (strncmp(tag,"2C00AC6C33",10)==0){
    activity ="#coffee";
    return true;
  }
  else return false;  
}

boolean isProjectTag(){
  if (strncmp(tag,"8400071D72",10)==0){
    project ="#projectA";
    return true;
  }
  else return false;  
}   




