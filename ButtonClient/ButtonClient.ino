
#include <WiFi.h>
#define BUTTONCLIENTFLAG "B"
#define BUTTONPRESSED "BP"
#define TURNGREENLEDON "greenon"
#define TURNREDLEDON "redon"
#define MAX_MESSAGE_LEN 512
#define DO_NOTHING "0"
#define REDLED 5
#define GREENLED 13
#define BUTTONPIN 25

const char* ssid="POCO X4 Pro 5G";
const char* pass="testwemos";
const int port=21000;
const char* serverIp="192.168.0.79";

void turnLedON(int PIN){
  digitalWrite(PIN, HIGH); 
}

void turnLedsOff(){
  digitalWrite(REDLED, LOW); 
  digitalWrite(GREENLED, LOW);
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
void buttonPressed(WiFiClient* client){
  turnLedsOff();
  client->println(BUTTONPRESSED);
  while (client->connected() && !client->available()) {delay(1);}
  String ServerAnswer=readServerAnswer(client);
  Serial.println(ServerAnswer);
  if(ServerAnswer==TURNGREENLEDON)
  {
    Serial.println("entro");
    turnLedON(GREENLED);
  }
  else if(ServerAnswer==TURNREDLEDON)
  {
    turnLedON(REDLED);
  }
  Serial.println("\nDisconecting");
  client->stop();
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
  client->println(BUTTONCLIENTFLAG);
  return true;
}
void setup() {
    Serial.begin(115200);
    pinMode(REDLED, OUTPUT);   
    pinMode(GREENLED, OUTPUT);
    pinMode(BUTTONPIN, INPUT);
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
  int buttonState = digitalRead(BUTTONPIN);
  Serial.println(buttonState);
  if(buttonState==HIGH && ConnectToServer(&client))
  {
    buttonPressed(&client);
    delay(5000);
  }
  else
  {
    client.println(DO_NOTHING);
  }
  delay(100);
}
