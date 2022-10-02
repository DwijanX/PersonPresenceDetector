
#include <WiFi.h>
#define MAX_MESSAGE_LEN 512
#define DO_NOTHING "0"
#define ULTRASONICCLIENTFLAG "U" //if the server gets this flag it will use the HandleButtonClient definition
#define READ_DISTANCE "RD"
#define TRIGGERPIN 17
#define ECHOPIN 5
#define SPEAKERPIN 18
#define BEEP "beep"

const char* ssid="POCO X4 Pro 5G";
const char* pass="testwemos";
const int port=21000;
const char* serverIp="192.168.0.79";

void beepSpeaker()
{
  Serial.println("Comando de beep");
  for(int i=0;i<500;i++)
  {
    digitalWrite(SPEAKERPIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(SPEAKERPIN, LOW);
    delayMicroseconds(500);
  }
}
String readServerAnswer(WiFiClient* client)
{
  int len =client->available();
  String  answer="";
  if (len > 0) 
  {
    byte buffer[MAX_MESSAGE_LEN];
    if (len > MAX_MESSAGE_LEN) 
    {
      len = MAX_MESSAGE_LEN;
    }
    client->read(buffer, MAX_MESSAGE_LEN);
    if (buffer > 0) {
      answer += (char*)buffer;
    }
  }
  return answer.substring(0,len);
}
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  long duration;
  int distance;
   digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  
  return distance;
}
int waitServerInstructions(WiFiClient* client)
{
  while (client->connected() && !client->available()) {delay(1);}
    String ServerAnswer=readServerAnswer(client);
    if(ServerAnswer==DO_NOTHING)
    {
      return 0;
    }
    else if(ServerAnswer==BEEP)
    {
      beepSpeaker();
    }
    return 0;
}
int interactWithServer(WiFiClient* client){
  while (client->connected() && !client->available()) {delay(1);}
  String ServerAnswer=readServerAnswer(client);
  if (ServerAnswer=="0")
    Serial.println("No se solicito distancia");
  int Distance=0;
  if(ServerAnswer==READ_DISTANCE)
  {
    Distance=readUltrasonicDistance(TRIGGERPIN,ECHOPIN);
    Serial.print("Distance readed: ");
    Serial.println(Distance);
    client->println(Distance);
    waitServerInstructions(client);
  }
  Serial.println("Disconecting");
  client->stop();
  return 0;
}
bool ConnectToServer(WiFiClient* client)
{
  if(!client->connect(serverIp,port))
  {
    Serial.println("Connection Failed");
    delay(4000);
    return false;
  }
  Serial.println("Connected");
  client->println(ULTRASONICCLIENTFLAG);
  return true;
}
void setup() {
    Serial.begin(115200);
    pinMode(TRIGGERPIN, OUTPUT); 
    pinMode(SPEAKERPIN, OUTPUT); 
    pinMode(ECHOPIN, INPUT);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client;
  
  if(ConnectToServer(&client))
  {
    interactWithServer(&client);
    delay(5000);
  }
  delay(100);
}
