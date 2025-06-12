#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definicje
const int doorSensorPin = 15; // Podłącz kontaktron do pinu GPIO15
bool lastDoorState = HIGH;    // Ostatni stan drzwi

void sendHttpRequest(const String& url) 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi not connected!");
    return;
  }

  HTTPClient http;
  http.begin(url);  // Inicjalizacja połączenia
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"Status\": \"ON\", \"Time\": \"60\"}");

  if (httpResponseCode > 0) 
  {
    String response = http.getString();  // Odbiór odpowiedzi
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } 
  else 
  {
    Serial.print("Error in HTTP request: ");
    Serial.println(httpResponseCode);
  }

  http.end();  // Zamknięcie połączenia
}


void setup() 
{
  Serial.begin(115200);

  WiFi.begin("", "");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  pinMode(doorSensorPin, INPUT_PULLUP); // Użyj wewnętrznego rezystora podciągającego
}

void onDoorOpened() 
{
  Serial.println("Drzwi otwarte!");
  sendHttpRequest("http://172.20.10.2:8898/TurnCameraON");

}

void onDoorClosed() 
{
  Serial.println("Drzwi zamkniete!");
}

void loop() 
{
  bool doorState = digitalRead(doorSensorPin);

  // Sprawdzenie czy stan się zmienił
  if (doorState != lastDoorState) 
  {
    lastDoorState = doorState;

    if (doorState == HIGH) 
    {
      onDoorOpened();
    } 
    else 
    {
      onDoorClosed();
    }
  }

  delay(10);
}
