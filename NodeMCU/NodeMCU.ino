// NodeMCU code

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>
// WiFi credentials
const char *ssid = "nikunja";
const char *password = "12345678";

// Socket.IO server settings
const char *host = "192.168.138.62";
const int port = 3001;




SoftwareSerial swSerial(D1, D2);  // RX, TX


// variable declaration for Socketio
int sio_DTH_temperature = 99;
int sio_DTH_humidity = 99;
int sio_LDR_lightIntensity = 99;
int sio_MQ135_gasConcentration = 99;
bool sio_isFanON = false;
bool sio_isLightON = false;
bool sio_isVentON = false;
bool sio_isAutomated = false;
int sio_DTH_Threshhold_Min = 99;
int sio_DTH_Threshhold_Max = 99;
int sio_MQ135_Threshhold_Min = 99;
int sio_MQ135_Threshhold_Max = 99;
int sio_LDR_Threshhold_Min = 99;
int sio_LDR_Threshhold_Max = 99;

int DTH_temperature;

ESP8266WiFiMulti WiFiMulti;
SocketIOclient socketIO;

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      {

        // Parse the incoming payload as JSON
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload, length);

        // Check if the event name is "isFanOn"
        JsonArray array = doc.as<JsonArray>();
        String eventName = array[0];  // Assuming the event name is the first element
        if (eventName == "time") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
           String timeValue = array[1].as<String>();  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.print(timeValue);
          Serial.println();
          // Serial.println(sio_isLightON ? "true" : "false");
        }
// DynamicJsonDocument doc(1024);
//         deserializeJson(doc, payload, length);

//         // Check if the event name is "isFanOn"
//         JsonArray array = doc.as<JsonArray>();
//         String eventName = array[0];  // Assuming the event name is the first element
        if (eventName == "isFanOn") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          sio_isFanON = data["isFanOn"];  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.print(sio_isFanON);
          // Serial.println(sio_isLightON ? "true" : "false");
        }

        // delay(1000);
        doc.clear();
      }
      break;
    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      hexdump(payload, length);
      break;
  }

  // // create message for Socket.IO (event)
  // DynamicJsonDocument doc(1);
  // JsonArray array = doc.to<JsonArray>();

  // // add evnet name
  // // Hint: socket.on('event_name', ....
  // array.add("mq135Data");

  // // add payload (parameters) for the event
  // JsonObject param1 = array.createNestedObject();
  // // param1["mqdata"] = 45;
  // // param1["ldr"] = 23;
  // param1["temperature"] = 25.5;

  // // JSON to String (serialization)
  // String output;
  // serializeJson(doc, output);

  // // Send event to socketio
  // socketIO.sendEVENT(output);





            // Send DATA TO ARDUINO
  StaticJsonDocument<512> docSendDataToArduino;  // Create a JSON document
  // docSendDataToArduino["DTH_Temperature"] = 123;
  docSendDataToArduino["DTH_Humidity"] = 2132;
  // docSendDataToArduino["LDR_LightIntensity"] = 123;
  // docSendDataToArduino["MQ135_GasConcentration"] = 312;
  docSendDataToArduino["isFanON"] = sio_isFanON;
  // docSendDataToArduino["isLightON"] = sio_isLightON;
  // docSendDataToArduino["isVentON"] = 43;
  // docSendDataToArduino["isAutomated"] = 234;
  // docSendDataToArduino["DTH_Threshhold_Min"] = 234;
  // docSendDataToArduino["DTH_Threshhold_Max"] = 234;
  // docSendDataToArduino["MQ135_Threshhold_Min"] = 234;
  // docSendDataToArduino["MQ135_Threshhold_Max"] = 342;
  // docSendDataToArduino["LDR_Threshhold_Min"] = 23894;
  // docSendDataToArduino["LDR_Threshhold_Max"] = 27983;

  // Serialize JSON document to string and send it via software serial
  serializeJson(docSendDataToArduino, swSerial);
  swSerial.println();
  docSendDataToArduino.clear();





}

void setup() {
  // Serial.begin(921600);
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate for debugging
  swSerial.begin(9600);      // Set the baud rate for software serial
  randomSeed(analogRead(A0));  // Seed the random number generator

  // Serial.setDebugOutput(true);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // disable AP
  if (WiFi.getMode() & WIFI_AP) {
    WiFi.softAPdisconnect(true);
  }

  WiFiMulti.addAP(ssid, password);

  // WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  String ip = WiFi.localIP().toString();
  Serial.printf("[SETUP] WiFi Connected %s\n", ip.c_str());

  // server address, port and URL
  socketIO.begin(host, port, "/socket.io/?EIO=4");

  // event handler
  socketIO.onEvent(socketIOEvent);
}



void loop() {
  socketIO.loop();


  // RECEIVE JSON DATA FROM ARDUINO
  if (swSerial.available()) {
    String data = swSerial.readStringUntil('\n');  // Read the incoming JSON data from Arduino

    // Parse the JSON data
    DynamicJsonDocument docReceivedFromArduino(512);
    DeserializationError error = deserializeJson(docReceivedFromArduino, data);

    if (!error) {
      DTH_temperature = docReceivedFromArduino["DTH_Temperature"];
      int DTH_humidity = docReceivedFromArduino["DTH_Humidity"];
      int LDR_lightIntensity = docReceivedFromArduino["LDR_LightIntensity"];
      int MQ135_gasConcentration = docReceivedFromArduino["MQ135_GasConcentration"];
      bool isFanON = docReceivedFromArduino["isFanON"];
      bool isLightON = docReceivedFromArduino["isLightON"];
      bool isVentON = docReceivedFromArduino["isVentON"];
      bool isAutomated = docReceivedFromArduino["isAutomated"];
      int DTH_Threshhold_Min = docReceivedFromArduino["DTH_Threshhold_Min"];
      int DTH_Threshhold_Max = docReceivedFromArduino["DTH_Threshhold_Max"];
      int MQ135_Threshhold_Min = docReceivedFromArduino["MQ135_Threshhold_Min"];
      int MQ135_Threshhold_Max = docReceivedFromArduino["MQ135_Threshhold_Max"];
      int LDR_Threshhold_Min = docReceivedFromArduino["LDR_Threshhold_Min"];
      int LDR_Threshhold_Max = docReceivedFromArduino["LDR_Threshhold_Max"];
      Serial.print("DTH Temperature: ");
      Serial.println(DTH_temperature);
      // Serial.print("DTH Humidity: ");
      // Serial.println(DTH_humidity);
      // Serial.print("LDR Light Intensity: ");
      // Serial.println(LDR_lightIntensity);
      // Serial.print("MQ135 Gas Concentration: ");
      // Serial.println(MQ135_gasConcentration);
      // Serial.print("isFanON: ");
      // Serial.println(isFanON);
      // Serial.print("isLightON: ");
      // Serial.println(isLightON);
      // Serial.print("isVentON: ");
      // Serial.println(isVentON);
      // Serial.print("isAutomated: ");
      // Serial.println(isAutomated);
      // Serial.print("DTH Threshold Min: ");
      // Serial.println(DTH_Threshhold_Min);
      // Serial.print("DTH Threshold Max: ");
      // Serial.println(DTH_Threshhold_Max);
      // Serial.print("MQ135 Threshold Min: ");
      // Serial.println(MQ135_Threshhold_Min);
      // Serial.print("MQ135 Threshold Max: ");
      // Serial.println(MQ135_Threshhold_Max);
      // Serial.print("LDR Threshold Min: ");
      // Serial.println(LDR_Threshhold_Min);
      // Serial.print("LDR Threshold Max: ");
      // Serial.println(LDR_Threshhold_Max);
      docReceivedFromArduino.clear();
    } else {
      // Handle JSON parsing error
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    }




      // socketio event part
    // create message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array.add("mq135Data");

    // add payload (parameters) for the event
    JsonObject param1 = array.createNestedObject();
    // param1["mqdata"] = 45;
    // param1["ldr"] = 23;
    param1["temperature"] = DTH_temperature;

    // JSON to String (serialization)
    String output;
    Serial.println("SOCKETIO SEND:");

    serializeJson(doc, output);

    // Send event to socketio
    socketIO.sendEVENT(output);
    doc.clear();



  }



  // delay(1000);  // Wait for 1 second before sending the next data
}