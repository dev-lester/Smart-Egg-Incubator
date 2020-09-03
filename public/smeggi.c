#include <DHT.h> // Including library for dht(temperature and humidity sensor)
#include <ESP8266WiFi.h>

extern volatile unsigned long timer0_overflow_count;

String apiKey = "your API"; // Enter your Write API key from ThingSpeak

const char *ssid = "Bunny"; // replace with your wifi ssid and wpa2 key
const char *pass = "secrethaha";
float h;
float t;
const char* server = "api.thingspeak.com";

#define DHTPIN 13 //D7 pin where the dht11 is connected
#define eight 480000 //milliseconds in eight minutes
#define hour 3600000 //milliseconds in an hour


DHT dht(DHTPIN, DHT22);

WiFiClient client;

const int R1 = 0;
const int R2 = 2;


void turner() {
  digitalWrite(R2, HIGH);
  Serial.println("R2 is on \n rotating...");
  delay(7500);
  Serial.println("rotating finished");
  digitalWrite(R2, LOW);
}


void setup()
{
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  Serial.begin(115200);
  delay(10);
  dht.begin();
  h = dht.readHumidity();
  t = dht.readTemperature();
  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{
  timer0_overflow_count = 0; //reset millis to 0 at top of each loop so no overflow issues
  unsigned long loopstart = millis();
  unsigned long nextEvent = loopstart + eight;
  for (int x = 0; x < 8; x++) {
    doSomething();
    Serial.println("dosomething");
    while (millis() < nextEvent);
    nextEvent += eight;
    doNextThing();
    Serial.println("donextthing");
    while (millis() < nextEvent);
    nextEvent += eight;
  }
  nextEvent += hour;
  Serial.println("hour event");
  while (millis() < nextEvent);
 
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
  }

  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);

  if (t < 37.5) {
    digitalWrite(R1 , HIGH);
    Serial.println("R1 is on");
  }
  else if (t > 37.5) {
    digitalWrite(R1 , LOW);
    Serial.println("R1 is off");
  }
  Serial.println("starting algo for heater");
}

void doSomething() {
  turner();
  }
void doNextThing() {
  turner();
  }
