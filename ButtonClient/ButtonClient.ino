
#include <WiFi.h>
#define BUTTONCLIENTFLAG "B" //if the server gets this flag it will use the HandleUltrasonicSensorClient definition
#define BUTTONPRESSED "BP"
#define TURNGREENLEDON "greenon"
#define TURNREDLEDON "redon"
#define MAX_MESSAGE_LEN 512
#define DO_NOTHING "0"
#define REDLED 5
#define GREENLED 13
#define BUTTONPIN 25

const char* ssid="POCO X4 Pro 5G"; //set local network
const char* pass="testwemos"; //local network password
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
void interactWithServer(WiFiClient* client){
  turnLedsOff();
  client->println(BUTTONPRESSED);
  while (client->connected() && !client->available()) {delay(1);} //we wait for server answer
  String ServerAnswer=readServerAnswer(client);
  Serial.print("Instruccion del servidor: ");
  Serial.println(ServerAnswer);
  if(ServerAnswer==TURNGREENLEDON)
  {
    turnLedON(GREENLED);
  }
  else if(ServerAnswer==TURNREDLEDON)
  {
    turnLedON(REDLED);
  }
  Serial.println("Disconecting");
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
<<<<<<< HEAD
  
  if(buttonState==HIGH && ConnectToServer(&client))
=======
  Serial.println(buttonState);
  if(buttonState==HIGH && ConnectToServer(&client)) //when we push our button and server is online
>>>>>>> de4e02531881cc720ebdb258fe38c66cf0b436f5
  {
    Serial.println("Boton presionado");
    interactWithServer(&client);
    delay(5000);
  }
  else
  {
    Serial.println("Boton no presionado");
    client.println(DO_NOTHING);
  }
  delay(100);
}
