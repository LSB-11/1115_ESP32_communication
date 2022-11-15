#include <WiFi.h>
#include <FirebaseESP32.h>
#include <string.h>

#define FIREBASE_HOST "xx"
#define FIREBASE_AUTH "xx"

#define WIFI_SSID "xx"
#define WIFI_PASSWORD "xx"

//dht11 sensor setup
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 13   
#define DHTTYPE    DHT11  
DHT_Unified dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;
FirebaseJson json;
uint32_t delayMS;
int adc_val = 0;
void setup()
{

  pinMode(A0, INPUT);
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
  
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //dht11
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  //adc
  pinMode(A0, INPUT);
}

void loop()
{
  String str= "Temp: ";
  float now_temp;
  float now_humid;
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    now_temp = event.temperature;
    Serial.print(now_temp);
    Serial.println(F("°C"));

  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    now_humid = event.relative_humidity;
    Serial.print(now_humid);
    Serial.println(F("%"));
  }
  adc_val = analogRead(A0);
  adc_val = (adc_val *100) / 4095;

    //sprintf(str, "Temp: %f, humid: %f", now_temp, now_humid);'
    //String str_temp = String(now_temp);
    //String str_hu = String(now_humid);
    
    json.set("Temp", now_temp);
    json.set("Humidity", now_humid);
    json.set("Resistance", adc_val);
    Firebase.pushJSON(firebaseData, "/HOME_IOT2", json); // 보내기, 송신
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("PUSH NAME: " + firebaseData.pushName());    
    
    delay(2000);
}
