/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 modified 9 Apr 2012
 by David A. Mellis
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 
 // 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
   0x90, 0xA2, 0xDA, 0x0D, 0x6E, 0x85}; // arduino board ethernet

IPAddress server(192,168,0,175); // elna mac book pro on CABLE

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  connectClient();
  
}

void loop()
{

 
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}


void connectClient(){
  Serial.println("connecting...");
  
  char tag[11] = "2C00AC6C33";
  // if you get a connection, report back via serial:
  if (client.connect(server, 8080)) {
    Serial.println("connected");
    // Make a HTTP request:
   client.print("GET /tag/");
   client.print(tag);
   client.println(" HTTP/1.0");
  
    //client.println("GET /tag/"+tag+" HTTP/1.0");
    
  //    client.println("GET /tag/2C00AC6C33 HTTP/1.0");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}
