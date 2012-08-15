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

#define TWEET_DELTA (60L * 60L)

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
boolean parse_fail=true;

SoftwareSerial rfid_serial = SoftwareSerial(RF_RX_PIN, RF_TX_PIN);

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
    Serial.print("DHCP: ");
    Ethernet.localIP().printTo(Serial);
    Serial.println("");
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
    unsigned long now = twitter.get_time();

    if (last_tweet == 0)
    {
      /* First round after twitter initialization. */
      Serial.print("Time is ");
      Serial.println(now);

      /* Wait few seconds before making our first tweet.  This
       gives our sensors some time to get running (I hope). */
      last_tweet = now - TWEET_DELTA + 15L;
    }


    enableRFID(); 
    getRFIDTag();
    
    if(isCodeValid()) {
      disableRFID();

      //sendCode();

      // if (now > last_tweet + TWEET_DELTA){
      last_tweet = now;

      char* msg;

      if (strncmp(tag,"2700BA6F2A",10)==0){ //A
        msg = "@vanessa_idemolab is #outofoffice for #project_X";
        parse_fail=false;
        //Serial.println("A");
      }
      else if (strncmp(tag,"2600D659FB",10)==0){
        msg = "@mikkel_idemolab is in a #meeting for #project_Y";
        //Serial.println("B");
        parse_fail=false;
      }
      else if (strncmp(tag,"2C00AC6C33",10)==0){
        msg ="@morten_idemolab is having #coffee";
        parse_fail=false;
      }
//      else if (strncmp(tag,"8400071D72",10)==0){
//        msg ="@johan_idemolab is #coding for #project_XX";
//        parse_fail=false;
//      }
      else {  
        Serial.println("Parse FAIL");
        parse_fail=true;
      }
//      Serial.print("Posting: ");
//      Serial.println(msg);
      //char *tmp = strncpy(tmp,msg,strlen(msg)-1);
      if(!parse_fail){
        if (twitter.post_status(msg))
          Serial.println("Status updated");
        else
          Serial.println("Update failed");
      }     
    }
      
    //}

    delay(ITERATION_LENGTH);
  } 
  else {
    disableRFID();
    Serial.println("Got some noise");  
  }
  rfid_serial.flush();
  Serial.flush();
  clearCode();
  /*
	 * here goes the code to control the board 
   	 * the output will be the message to tweet
   * the message will be catched by physical interaction with 
   * RFID tags / buttons / etc etc 
   */
  // when appropriate tweet the message once please! :)




  //      char msg[48] = "@elena_idemolab is testing tweeting :)";
  //      tweet(msg);
}

/**
 * Clears out the memory space for the tag to 0s.
 */
void clearCode() {
  for(int i=0; i<CODE_LEN; i++) {
    tag[i] = 0; 
  }
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
  Serial.println(); 
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
  // Serial.println("getrfid");
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






