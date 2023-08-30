#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "HUTCH SUCKS"
#define WIFI_PASSWORD "DNDslic.5962"

#define API_KEY "AIzaSyAUjZGklTQr49G1W8st3mt7uH_eyxNACoc"
#define DATABASE_URL "https://dcdevice-73ac7-default-rtdb.firebaseio.com/"

#define LED_BUILTIN LED_BUILTIN

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

String dataarray[] = {"1693166903","0.00000000","0.00000000","32.80","101285.26","3.31","77.30","32.00","43.24","5", "160","5.00"};
String csvheaders[] = {"Timestamp", "Latitude", "Longitude", "Temperature", "Pressure", "Altitude", "Humidity", "DTHTemperature", "HeatIndex", "AirQuality", "SoilMoisture","Lux"};
String path;
int arrayLength = sizeof(dataarray) / sizeof(dataarray[0]);
bool oneTimeRead = false;

void switchled()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Signup OK");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  // RTD to ESP32

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println(arrayLength);
}

void loop()
{
  // if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
  //   sendDataPrevMillis = millis();
  //   if (Firebase.RTDB.getJSON(&fbdo, deviceid)) {
  //     Serial.println(fbdo.payload());
  //     if(fbdo.payload()=="true"){
  //       switchled();
  //     }else if(fbdo.payload()=="false"){
  //       digitalWrite(LED_BUILTIN,LOW );
  //     }
  //   }
  //   else {
  //     Serial.println(fbdo.errorReason());
  //   }
  // }
  while (oneTimeRead == true)
  {
    Serial.println("looping");
  }
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/String
    for (int i = 0; i <= arrayLength-1; i++)
    {
      path = dataarray[0] + "/" + csvheaders[i];
      if (Firebase.RTDB.setString(&fbdo, path, dataarray[i]))
      {
        Serial.println("PASSED");
        Serial.print("PATH: ");
        Serial.println(fbdo.dataPath());

        Serial.print("TYPE: ");
        Serial.println(fbdo.dataType());
      }
      else
      {
        Serial.println("FAILED");
        Serial.print("REASON: ");
        Serial.println(fbdo.errorReason());
      }
    }
    oneTimeRead = true;
  }
  delay(1000);
}
