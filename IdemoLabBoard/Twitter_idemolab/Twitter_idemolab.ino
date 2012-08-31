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


/* Local network configuration. */
uint8_t mac[6] =     {
//0x90, 0xA2, 0xDA, 0x00, 0x00, 0xA0}; // old ethernet shield
  0x90, 0xA2, 0xDA, 0x0D, 0x6E, 0x85}; // ethernet board


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
char buffer[512];

const static char consumer_key[] PROGMEM = "XqEWPKcib9LYeV0BoHfCrA";
const static char consumer_secret[] PROGMEM= "rVWbCfvQKi27smxxTeNsLaAsBJtNPmR70BwvPaEDY";

Twitter twitter(buffer, sizeof(buffer));

int count = 0;

void
setup()
{
  Serial.begin(9600);
  Serial.println("Arduino Twitter demo");

 // sensors.begin();

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
    /*
	 * here goes the code to control the board 
	 * the output will be the message to tweet
         * the message will be catched by physical interaction with 
         * RFID tags / buttons / etc etc 
    */ 
    // when appropriate tweet the message once please! :)

      char msg[48] = "@elena_idemolab is testing tweeting :)";
      tweet(msg);
}
void tweet(char msg[48]){

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

      if (now > last_tweet + TWEET_DELTA){
          /* char msg[32] = "another trial to arduino tweets";*/
          Serial.print("Posting to Twitter: ");
          Serial.println(msg);
          last_tweet = now;

          if (twitter.post_status(msg))
            Serial.println("Status updated");
            
          else
            Serial.println("Update failed");
       }
    }

  delay(5000);
}

