#include <WiFiManager.h>          //https://github.com/kentaylor/WiFiManager

#include <Artnet.h>

Artnet artnet;

int currentPumpValue = 0;
int tickCount = 0;
int frameNb = 0;
int currentTimeSlot = 1;
#define TICK_DURATION 10
#define TIME_SLOT_DURATION 25 //*10 = 250ms

#define FOG_DMX_ADDRESS 1


#define RELAY D1
#define LED   D3


bool ledValue = LOW;

 
void setup() {
  //todo : use button to trigger fog manually 
 // pinMode(BUTTON, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);

  //force turn Off
  digitalWrite(RELAY, HIGH);
  //force LED ON
  digitalWrite(LED, LOW);
 
  Serial.begin(115200);
  Serial.println("\n Starting");
  unsigned long startedAt = millis();
 
  Serial.println("Opening configuration portal");
  //Local intialization. Once its business is done, there is no need to keep it around
 
 WiFiManager wifiManager;  

  wifiManager.autoConnect("AutoConnectAP");
  if (WiFi.status()!=WL_CONNECTED)
  {
      Serial.println("failed to connect, finishing setup anyway");
  } 
  else
  {
    Serial.print("local ip: ");
    Serial.println(WiFi.localIP());
  }


  //SetupArtnet
  artnet.begin();
  
}
 
void toggleLed()
{
  if(ledValue == HIGH)
    ledValue = LOW;
  else
    ledValue = HIGH;
 
  digitalWrite(LED, ledValue);
}

void loop() {
  tickCount++;

 if (artnet.read() == ART_DMX)
  {
   frameNb++;
    //TODO : get Address from config
    uint8_t* dmxFrames = artnet.getDmxFrame();
   // Serial.println("received artnet frame");
    currentPumpValue = dmxFrames[FOG_DMX_ADDRESS-1];

    //don't toggle at each frame.
    if(frameNb % 10 == 0)
      toggleLed();
  }


  if(tickCount >= TIME_SLOT_DURATION)
  {
    currentTimeSlot++;
    tickCount = 0;
  }
  if(currentTimeSlot >= 255)
  {
    currentTimeSlot = 1;
  }

  //turn on for a duration inside a minute cycle
  if(currentTimeSlot <= currentPumpValue)
  {
    //Serial.print(currentTimeSlot);
    //Serial.println("FOG - ON");
    digitalWrite(RELAY, LOW);
  }  
  else
  {
    //Serial.print(currentTimeSlot);
    //Serial.println("FOG - OFF");
    digitalWrite(RELAY, HIGH);    
  }
  delay(TICK_DURATION);
}
