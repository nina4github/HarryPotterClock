HarryPotterClock repository
---------------


---------------
first created in August 2012
updated in October 2014
---------------
This repository contains the components for the HarryPotter Clock project that I conducted as intern at the idemolab at Delta in August 2012.
Aim of the project was to trace the activities of the members of the idemolab and try to answer questions like:
Where is Mikkel? Which project is he working on at the moment? Can I disturb Mikkel or is he busy at the moment?

The goal is not just to be able to track each other activities but to offer an overview of what is going on to the guests, clients and other colleagues popping by for coffee or for more formal issues. 

The solution explored uses physical interaction with common artefacts, avatars and tags augmented with RFID to give the members of the lab a physical mean to state their current activity. The activities are then communicated to a twitter account and displayed on a big screen in the lab. The big screen offers an overview of the current activities of the members of the lab, the projects they are working on and more in general an overview of what is going on in the lab.

The future aim of the solution is also to leave open the possibility to post your own activity through twitter and eventually any other social media used by the members of the lab.


The repository is divided into three folders, each containing a part of the complete experience. Here a short description of each folder and the technologies involved

---------------
IdemoLabWeb --- web application for visualizing the activity feeds collected from the twitter account. 
---------------

TECHNOLOGY: 
node.js [socket.io, twitter] -- idemolab_activity/server/streamapp.js -- connects to twitter APIs, collects the user timeline and opens a stream listening to new incoming tweets
node.js -- idemolab_activity/server/restifyserver.js -- offers a database that translates the RFID codes detected by the rfid reader through arduino (see IdemoLabBoard) into default messages
HTML5 + CSS + javascript/jQuery + TwitterBootstrap -- idemolab_activity/index.html, idemolab_activity/assets/... -- 


---------------
IdemoLabBoard --- ARDUINO sketches to update twitter feeds from a physical board augmented with an RFID reader and RFID tags at the idemolab. -- LAST UPDATE AUGUST 2012: needs an update of the library to connect to Twitter APIs
---------------

TECHNOLOGY: 
Arduino Mega board + 
RFID reader + 
libraries to connect and authenticate with OAuth to Twitter APIs +

INTERACTION: 
A member of the lab first swipes his personal avatar, then he swipes the activity artefact and/or the project artefact and the activity is communicated. A blinking LED signals that the communication went through.


---------------
IdemoLabShareButton --- UNFINISHED - Android application containing a widget to visualize twitter activity feeds from the idemolab
---------------

TECHNOLOGY: 
java -- Android OS
