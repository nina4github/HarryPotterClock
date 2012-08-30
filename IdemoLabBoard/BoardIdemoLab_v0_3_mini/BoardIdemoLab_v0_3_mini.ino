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
//  0x90, 0xA2, 0xDA, 0x00, 0x00, 0xA0};
  0x90, 0xA2, 0xDA, 0x0D, 0x6E, 0x85};

/* 
 * The "IPAddress dns" parameter parameter gives me a compilation error 
 * therefore I have commented it and the software runs (tweets) without problems
 */
//IPAddress dns(192, 168, 0, 195); 

IPAddress ip(192, 168, 0, 100);
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


#define CODE_LEN 10      //Max length of RFID tag
#define VALIDATE_TAG 1  //should we validate tag?
#define VALIDATE_LENGTH  200 //maximum reads b/w tag read and validate
#define ITERATION_LENGTH 2000 //time, in ms, given to the user to move hand away
#define START_BYTE 0x0A 
#define STOP_BYTE 0x0D
/* RFID pins. */
#define RFID_ENABLE 6   //to RFID ENABLE
#define RF_RX_PIN 7
#define RF_TX_PIN 8

char tag[CODE_LEN];  
//boolean parse_fail=true;

SoftwareSerial rfid_serial = SoftwareSerial(RF_RX_PIN, RF_TX_PIN);


//IPAddress server(192,168,0,175); // change to PRODUCTION SERVER (mac mini of the lab) on idemolab network
IPAddress server(192,168,0,175); // TEST SERVER IP elna mac book pro through CABLE on TAGTEC router 
EthernetClient client;

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
    Serial.println("");
  }
  else
    Serial.println("DHCP configuration failed");
#else
  Ethernet.begin(mac, ip, dns, gateway, subnet);
#endif

 connectTwitter();
 delay(500);
// connectClient();
}


void
loop()
{
   // SUMMARY:
  // first read rfid
  // if there is a new tag
  // second get rfid tag values from server
  // if we have actor, activity | project set
  // third send tweet 

  if (true){
    /* FIRST STEP */
    enableRFID();

    getRFIDTag();

    /* SECOND STEP */
    if(isCodeValid()) {
      disableRFID();
      
      // DEBUG
      //sendCode();
      Serial.print("found valid tag: ");
      Serial.println(tag);
      // end DEBUG
      
      /* here goes the parsing :)*/

     if(!client.connected()){
      // client.flush();
       connectClient();
      }
      requestTagInfo(); // get the values associated with the tag just scanned
      delay(10); // important!!
      read_and_parseTagInfo(); // read get response

      delay(ITERATION_LENGTH);
    } 
    else {
      disableRFID();
      Serial.println("Got some noise");  
    }
    
    rfid_serial.flush();
    clearCode();

  }/* THIRD STEP */
  else{ // we have a message to send
    //    if (twitter.is_ready())
    //    {
    //      //unsigned long now = twitter.get_time();
    //      // DEBUG
    //      Serial.println("twitter is ready ");
    //      // end DEBUG
    //      sendTweet();
    //    }
    Serial.println("here tweet the message :)");
    //clearVariables();
  }
  
}

/**
 * Clears out the memory space for the tag to 0s.
 */
void clearCode() {
  for(int i=0; i<CODE_LEN; i++) {
    tag[i] = 0; 
  }
}


void connectClient(){
  Serial.println("GETTAG connecting...");
  client.connect(server, 8080);
  
  delay(200);

  if (!client.connected()){
    // kf you didn't get a connection to the server:
    Serial.println("GETTAG connection failed");
  }
  else Serial.println("GETTAG connected");
}

void connectTwitter(){
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
}

void sendTweet(){
  
  // SEND TWEET
    if (twitter.post_status("ready"))
      Serial.println("Status updated");
    else
      Serial.println("Update failed");
//   clearVariables();

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
  Serial.println("ready for RFID");
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


void requestTagInfo(){
    // Make a HTTP request:
    Serial.println("making request");
    client.print("GET /tag/");
    //client.print(tag); // this gives many troubles!! :)
    for(int i=0; i<CODE_LEN; i++) {
      client.print(tag[i]); 
    } 
    client.println(" HTTP/1.1");
    //    client.println("GET /tag/2C00AC6C33 HTTP/1.0");
    client.println("Connection: keep-alive");
    client.println();
}

void read_and_parseTagInfo(){
  int counter = 0;
  int i = 0;

  while (client.available()){
    char c = client.read();
    
    // counter 4 new line and print what comes next
    if (c=='\n'){//&& counter<6){
      counter = counter+1;
    }
    else if (counter==6){
      // put c in the buffer!
      buffer[i]=c;
      i=i+1;
      Serial.print(c);
    } 

  }

  char response[i-1];
  strcpy(response,buffer);
  char* _text;
  char* _type;
  _text =  strtok(response,","); // the first element returned before the ,
  _type =  strtok(NULL,","); // the second element returned after the ,
  
  
    Serial.println();
  Serial.println("---------------"); 
  Serial.print("text ");
  Serial.print(_text); 
  Serial.print(" & type ");  
  Serial.println(_type);  
 // parseTagInfo(_text,_type);
 _type = 0;
 _text = 0;
   for(int j=0; j<i-1; j++) {
    response[j] = 0; 
    buffer[j]=0;
  } 
}






