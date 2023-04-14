
/*
  EDITED BY Gabriel Tornich

  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>

const char* ssid = "wifi;
const char* password = "senha123";

#define BOTtoken "TOKEN***************" 

#define CHAT_ID "ID*************"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int temp = 14;
const int umid = 12;
const int ilum = 13;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/temp to turn temperature \n";
      welcome += "/umid to turn humidity \n";
      welcome += "/ilum to turn illumination \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/temp") {
      digitalWrite(temp, HIGH);
      digitalWrite(umid, LOW);
      digitalWrite(ilum, LOW);
      delay(1000);
      bot.sendMessage(chat_id, "Temperature: " + String(analogRead(A0)), "");
    }
    
    if (text == "/umid") {
      digitalWrite(temp, LOW);
      digitalWrite(umid, HIGH);
      digitalWrite(ilum, LOW);
      delay(1000);
      bot.sendMessage(chat_id, "Humidity: " + String(analogRead(A0)), "");
    }
    
    if (text == "/ilum") {
      digitalWrite(temp, LOW);
      digitalWrite(umid, LOW);
      digitalWrite(ilum, HIGH);
      delay(1000);
      bot.sendMessage(chat_id, "Illumination: " + String(analogRead(A0)), "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");  
    client.setTrustAnchors(&cert);
  #endif

  pinMode(temp, OUTPUT);
  pinMode(umid, OUTPUT);
  pinMode(ilum, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(temp, LOW);
  digitalWrite(umid, LOW);
  digitalWrite(ilum, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
