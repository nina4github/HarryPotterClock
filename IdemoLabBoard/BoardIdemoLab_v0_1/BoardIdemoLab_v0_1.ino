/* -*- c++ -*-
 *
 * BoardIdemoLab_v0_1.ino
 * using arduino ethernet board
 * RFID + GET tag info + Tweet
 * 
 * Edited by: Elena Nazzi <elenanazzi@gmail.com>
 *
 * For: Delta IdemoLab <delta.dk/idemolab>
 */

/* 
 * This sketch uses different sketches taken from: 
 * Twitter.pde of Markku Rossi <mtr@iki.fi>
 *
 */

/* -*- c++ -*- 
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
  //  0x90, 0xA2, 0xDA, 0x00, 0x00, 0xA0}; // old ethernet shield
  0x90, 0xA2, 0xDA, 0x0D, 0x6E, 0x85}; // arduino ETHERNET BOARD  
  
/* 
 * The "IPAddress dns" parameter parameter gives me a compilation error 
 * therefore I have commented it and the software runs (tweets) without problems
 */
//IPAddress dns(192, 168, 0, 195); // old ethernet shield on TAGTEC router

IPAddress ip(192, 168, 0, 100); // ETHERNET BOARD on TAGTEC router
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

// TEST put the credentials of the idemolab application 
//(now it is an application of @elenina :) another account of mine on twitter)
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

char* msg = "";
char* actor = 0;
char* activity = 0;
char* project= 0;

char* text;
char* type;

EthernetClient tag_client;
//IPAddress server(192,168,0,175); // change to PRODUCTION SERVER (mac mini of the lab) on idemolab network
IPAddress server(192,168,0,175); // TEST SERVER IP elna mac book pro through CABLE on TAGTEC router 


void
setup()
{
  Serial.begin(9600); // ethernet communication happens through SERIAL 9600 
  Serial.println("Arduino Twitter+RFID demo");

  pinMode(RFID_ENABLE,OUTPUT);
  // software serial configuration
  pinMode(RF_RX_PIN, INPUT);
  pinMode(RF_TX_PIN, OUTPUT);

  rfid_serial.begin(2400);  // RFID communication happens through SERIAL 2400

// configure ETHERNET
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

  //configureTwitter();

  delay(500); // give it some time

  connectClient();
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

  if (!isMessageReady()){
    /* FIRST STEP */
    enableRFID();
     
    getRFIDTag();

    /* SECOND STEP */
    if(isCodeValid()) {
      disableRFID();
      sendCode();
      delay(1000);
      /* here goes the parsing :)*/

      getTagInfo(); // get the values associated with the tag just scanned
    //  parseTagInfo(); 

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

void clearVariables(){
  actor=0;
  activity=0;
  project = 0;
  msg = 0;
}


/**************************************************************/
/********************   TWITTER Functions  ********************/
/**************************************************************/
void configureTwitter(){
  
  twitter.set_twitter_endpoint(PSTR("api.twitter.com"),
  PSTR("/1/statuses/update.json"),
  twitter_ip, twitter_port, false);
  twitter.set_client_id(consumer_key, consumer_secret);

  // TEST
  // authorization details of the account we are going to tweet with
  // this is ninatesting2012
  // you will need to change this account with the office activity account
  // OAuth token and secret generated for the account for the application
  twitter.set_account_id(PSTR("744944190-XztmpL4NCaE8mzIVPzSbCgQeDOU1CezXLwc2UMWY"),
  PSTR("YRTwgF9QsQRqF7FBcEuhadfpTbBKKXbuhveBnGMZk"));

 Serial.println("twitter configured");
}

void sendTweet(){

  // DEBUG
  //  Serial.print("MESSAGE: ");
  //  Serial.print( actor);
  //  Serial.print( "  ");
  //  if(activity!=0) Serial.print( activity);
  //  Serial.print( "  ");
  //  if (project!=0)     Serial.println(project);
  //  Serial.println( "  ");
  // end DEBUG

  strcpy(msg,actor);
  strcat(msg, " ");
  if(activity!=0) strcat(msg,activity);
  strcat(msg, " ");
  if(project!=0) strcat(msg,project);

  // DEBUG
  //  Serial.print("MESSAGE: ");
  //  Serial.println(msg);
  // end DEBUG

  // SEND TWEET
  if (twitter.post_status(msg))
    // DEBUG
    Serial.println("Status updated");
    // end DEBUG
  else
    // DEBUG
    Serial.println("Update failed");
    // endDEBUG
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
  // DEBUG
  Serial.println("ready for rfid");
  // end DEBUG
  
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

/**
 * Sends the tag to the computer.
 */
void sendCode() {
  Serial.print("TAG:");  
//  Serial.println(tag);
for(int i=0; i<CODE_LEN; i++) {
    Serial.print(tag[i]); 
  } 
  Serial.println();
}
/**************************************************************/
/*************  TAG DB connection Functions  ******************/
/**************************************************************/

void connectClient(){
  Serial.println("GETTAG connecting...");
  tag_client.connect(server, 8080);
  delay(1000);

  if (!tag_client.connected()){
    // kf you didn't get a connection to the server:
    Serial.println("GETTAG connection failed");
  }
  else Serial.println("GETTAG connected");
}


void getTagInfo(){

  if(!tag_client.connected()){

    Serial.println();
    Serial.println("GETTAG disconnecting.");
    tag_client.stop();
    // TODO: CHANGE THIS 
    //    // do nothing forevermore:
    //    for(;;)
    //      ;

  }
  else{
//    Serial.print("building request for tag: ");
//    Serial.println(tag);
    // Make a HTTP request:
    tag_client.print("GET /tag/");
    tag_client.print(tag);
    tag_client.println(" HTTP/1.0");
    //    tag_client.println("GET /tag/2C00AC6C33 HTTP/1.0");
    tag_client.println();
  }
//
//  int counter = 0;
//  int i= 0;
//
//  char info_buffer[50]={};
//
//  while (tag_client.available()) {
//    char c = tag_client.read();
//    // counter 4 new line and print what comes next
//    if (c=='\n'&& counter<4){
//      counter = counter+1;
//    }
//    else if (counter>=4){
//      // put c in the buffer!
//      info_buffer[i]=c;
//      i=i+1;
//    } 
//  }
//  char response[i-1];
//  strcpy(response,info_buffer);
//  //  char* text;
//  //  char* type;
//  text =  strtok(response,","); // the first element returned before the ,
//  type =  strtok(NULL,","); // the second element returned after the ,
//  //  Serial.println(text);   
//  //  Serial.println(type);  
}


void parseTagInfo(){

  if (strncmp(type,"verb",4)){ 
    activity=text;
  }
  else if(strncmp(type,"actor",5)){
    actor=text;
  }
  else if(strncmp(type,"object",6)){
    project=text;
  }
  else {
    Serial.println("error while parsing the type");
  }
  
  // ok, I have parsed and set the info from the tag, now I can clean the temporary variable that were hosting these info and proceed
  text = 0;
  type = 0;
 
}


boolean isMessageReady(){
  if (actor!=0&&(activity!=0||project!=0)){
     // disconnect tag_client here!
     if (tag_client.connected()){
        tag_client.stop();
        //DEBUG
        Serial.println("done with the tags job; GETTag disconnecting");
        // end DEBUG
     }
    return true;
  }
  else return false;
}









