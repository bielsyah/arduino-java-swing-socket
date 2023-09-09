#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 20,4);

int port = 5050;
WiFiServer server(port);

const char* ssid = "Your WiFi";
const char* pass = "Your WiFi Password";

Servo myServo;

int redLED = D8;
int greenLED =D5;
int blueLED = D4;

int POTENTIOMETER_PIN = A0;
String dataInput="";

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  myServo.attach(D0,500, 2400);
  myServo.write(180);
}

void loop() 
{
  WiFiClient client = server.available();
  
  if (client) 
  {
    if(client.connected())
    {
      Serial.println("Client Connected");
      lcd.setCursor(0,1);
    }
    
    while(client.connected())
    {      
      while(client.available()>0)
      {
        char receivedData = client.read();
        dataInput += receivedData; 
        
        if (receivedData == '\n') 
        {
          myServo.write(dataInput.toInt());
          Serial.println(dataInput.toInt());
          dataInput = "";
          lcd.clear();
        } 

        if (dataInput.startsWith("GON") || dataInput.startsWith("GOFF")) 
        {
          digitalWrite(greenLED, dataInput.startsWith("GON") ? HIGH : LOW);
          lcd.clear();
        }
        else if (dataInput.startsWith("BON") || dataInput.startsWith("BOFF")) 
        {
          digitalWrite(blueLED, dataInput.startsWith("BON") ? HIGH : LOW);
          lcd.clear();
        }
        else if (dataInput.startsWith("RON") || dataInput.startsWith("ROFF")) 
        {
          digitalWrite(redLED, dataInput.startsWith("RON") ? HIGH : LOW);
          lcd.clear();
        }
        else if(dataInput.equals("clr"))
        {
          lcd.clear();
        }
        else
        { 
          lcd.print(receivedData);
          Serial.print(receivedData);
        }      
      }
      dataInput ="";
      int data = analogRead(POTENTIOMETER_PIN);
      int percentage = map(data, 0, 1023, 0, 100);
      //Serial.println(percentage);
      client.print(percentage);
      delay(80);
    }
    client.stop();
    Serial.println("Client disconnected");    
  }
}
