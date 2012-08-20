/* -*- c++ -*-
 *
 * Twitter_idemolab.ino
 * 
 * Edited by: Elena Nazzi <elenanazzi@gmail.com>
 *
 * For: Delta IdemoLab <delta.dk/idemolab>
 */
/* -*- c++ -*- 
 *
 * From: 
 *
 * Twitter.pde
 *
 * Author: Markku Rossi <mtr@iki.fi>
 *
 * Copyright (c) 2011-2012 Markku Rossi
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <Time.h>
#include <EEPROM.h>
#include <Twitter.h>
#include <SoftwareSerial.h>

/* Local network configuration. */
uint8_t mac[6] =     {
  0x90, 0xA2, 0xDA, 0x00, 0x00, 0xA0};


/* 
 * The "IPAddress dns" parameter parameter gives me a compilation error 
 * therefore I have commented it and the software runs (tweets) without problems
 */
//IPAddress dns(192, 168, 0, 195); 

IPAddress ip(192, 168, 0, 195);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

/* The IP address to connect to: Twitter or local HTTP proxy. */
IPAddress twitter_ip(199, 59, 149, 232); // this is a static address of the twitter service
uint16_t twitter_port = 80;

unsigned long last_tweet = 0;

//#define TWEET_DELTA (60L * 60L)

/* Work buffer for twitter client.  This shold be fine for normal
 operations, the biggest items that are stored into the working
 buffer are URL encoded consumer and token secrets and HTTP response
 header lines. */
char buffer[200];

const static char consumer_key[] PROGMEM = "XqEWPKcib9LYeV0BoHfCrA";
const static char consumer_secret[] PROGMEM= "rVWbCfvQKi27smxxTeNsLaAsBJtNPmR70BwvPaEDY";

Twitter twitter(buffer, sizeof(buffer));

int count = 0;


/*RFID*/

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
//boolean parse_fail=true;

SoftwareSerial rfid_serial = SoftwareSerial(RF_RX_PIN, RF_TX_PIN);

char* msg = "";
char* person = 0;
char* activity =0;
char* project= 0;

void
setup()
{
  Serial.begin(9600);
  Serial.println("Arduino Twitter+RFID demo");

  pinMode(RFID_ENABLE,OUTPUT);

  pinMode(RF_RX_PIN, INPUT);
  pinMode(RF_TX_PIN, OUTPUT);

  rfid_serial.begin(2400);  


#if 1
  if (Ethernet.begin(mac))
  {
    Serial.println("DHCP: OK");
    //Ethernet.localIP().printTo(Serial);
   // Serial.println("");
  }
  else
    Serial.println("DHCP configuration failed");
#else
  Ethernet.begin(mac, ip, dns, gateway, subnet);
#endif

  twitter.set_twitter_endpoint(PSTR("api.twitter.com"),
  PSTR("/1/statuses/update.json"),
  twitter_ip, twitter_port, false);
  twitter.set_client_id(consumer_key, consumer_secret);

  // authorization details of the account we are going to tweet with
  // this is ninatesting2012
  twitter.set_account_id(PSTR("744944190-XztmpL4NCaE8mzIVPzSbCgQeDOU1CezXLwc2UMWY"),
  PSTR("YRTwgF9QsQRqF7FBcEuhadfpTbBKKXbuhveBnGMZk"));

  //#if 1
  //  /* Read OAuth account identification from EEPROM. */
  //  twitter.set_account_id(256, 384);  
  //#else
  //  /* Set OAuth account identification from program memory. */
  //  twitter.set_account_id(PSTR("14266044-by72PfntsUVNEmpFeBbFZWhbQK3zQvTHfx7WqXucl"),
  //                         PSTR("pfIrL41dCz9qnXW8PGHFWdNhCJlCiE1TcG4LD7RRyOY"));
  //#endif

  delay(500);
}

void
loop()
{
  if (twitter.is_ready())
  {
    //unsigned long now = twitter.get_time();
    //Serial.println("twitter is ready ");
    enableRFID(); 
    getRFIDTag();

    if(isCodeValid()) {
      disableRFID();

      /* here goes the logic :)*/

      if (isActivityTag()&&person==0){
        // FEEDBACK
        //Serial.print("Good the activity is set to ");
        Serial.print(activity);
        Serial.println( " now ADD WHO you are OR your PROJECT");
        // end FEEDBACK
      }
      else if (isProjectTag()&&person==0){
        // FEEDBACK
        Serial.print( project);
        Serial.println( " now ADD WHO you are OR your current ACTIVITY");
        // end FEEDBACK
      }

      else if ((isPersonTag()&&(activity!=0||project!=0))
        ||((isActivityTag()||isProjectTag())&&person!=0)){

        // FEEDBACK
        Serial.print( person);
        Serial.println( " now we can SEND your message");  
        // end FEEDBACK

        sendTweet();

      }
      else if (isPersonTag()&&(activity==0||project==0)){
        //FEEDBACK
        Serial.print(person);
        Serial.println( " now ADD your activity or project");
        // end FEEDBACK
      }
      else {
        Serial.println("not recognized :( start again!");
        clearVariables();
      }

      delay(ITERATION_LENGTH);
    } 
    else {
      disableRFID();
      Serial.println("Got some noise");  
    }
}
    rfid_serial.flush();
    Serial.flush();
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
  msg = 0;
}

void sendTweet(){

  // SEND MESSAGE
  // FEEDBACK
//  Serial.print("MESSAGE: ");
//  Serial.print( person);
//  Serial.print( "  ");
//  if(activity!=0) Serial.print( activity);
//  Serial.print( "  ");
//  if (project!=0)     Serial.println(project);
//  Serial.println( "  ");
  // end FEEDBACK
  strcpy(msg,person);
  strcat(msg, " ");
  if(activity!=0) strcat(msg,activity);
  strcat(msg, " ");
  if(project!=0) strcat(msg,project);
  
//  Serial.print("MESSAGE: ");
//  Serial.println(msg);
  // reset variables for the next activity message 
  

  // SEND TWEET
    if (twitter.post_status(msg))
      Serial.println("Status updated");
    else
      Serial.println("Update failed");
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
 // Serial.println("get_rfid");
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
    person = "@morten_idemolab";
    return true;
  }
//  else if (strncmp(tag,"2600D659FB",10)==0){
//    person = "@mikkel_idemolab";
//
//    return true;
//  }
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








