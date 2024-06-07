[// NodeMCU code

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>
//LED & Fan
#define RED_LED D4
#define YELLOW_LED D3
#define GREEN_LED D6
#define BLUE_LED D2
#define FAN D5
// WiFi credentials
const char *ssid = "nikunja";
const char *password = "12345678";

// Socket.IO server settings
const char *host = "192.168.218.62";
const int port = 3001;
int brightness = 0;





SoftwareSerial swSerial(D7, D0);  // RX, TX


// Define a lookup table that maps ADC values to lux values
const int ADC_RES = 1024;       // Resolution of the Arduino's ADC (10-bit)
const int LUX_TABLE_SIZE = 10;  // Size of the lookup table
int adcValues[LUX_TABLE_SIZE] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
float luxValues[LUX_TABLE_SIZE] = { 10.0, 50.0, 100.0, 200.0, 400.0, 700.0, 1000.0, 1500.0, 2000.0, 3000.0 };


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




int DTH_temperature = 0;
int DTH_humidity = 0;
int LDR_lightIntensity = 0;
int MQ135_gasConcentration = 0;
bool isFanON = false;
bool isLightON = false;
bool isVentON = false;
bool isAutomated = false;
int DTH_Threshhold_Min = 0;
int DTH_Threshhold_Max = 1;
int MQ135_Threshhold_Min = 0;
int MQ135_Threshhold_Max = 1;
int LDR_Threshhold_Min = 0;
int LDR_Threshhold_Max = 1;
float adcValue = 1 ;


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
          // Serial.print(timeValue);
          // Serial.println();
          // Serial.println(sio_isLightON ? "true" : "false");
        }
        // DynamicJsonDocument doc(1024);
        //         deserializeJson(doc, payload, length);

        //         // Check if the event name is "isFanOn"
        //         JsonArray array = doc.as<JsonArray>();
        //         String eventName = array[0];  // Assuming the event name is the first element

        if (eventName == "isFanOn") {
          JsonObject data = array[1];
          isFanON = data["isFanOn"];  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.println("Fan");
          Serial.println(isFanON ? "true" : "false");
        }

        if (eventName == "isVentOn") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          isVentON = data["isVentOn"];  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.print("VentON");
          Serial.println(isVentON ? "true" : "false");
        }

        if (eventName == "isLightOn") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          isLightON = data["isLightOn"];  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.print("isLightON");
          Serial.println(isLightON ? "true" : "false");
        }

        if (eventName == "isAutomated") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          isAutomated = data["isAutomated"];  // Extract the value of "islight"

          // Print the value of "islight"
          Serial.print("isAutomated");
          Serial.println(isAutomated ? "true" : "false");
        }


        if (eventName == "DTHThreshold") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          DTH_Threshhold_Min = data["min"];  // Extract the value of "islight"
          DTH_Threshhold_Max = data["max"];  // Extract the value of "islight"

          // Print the value of "islight"
          // Serial.print(sio_isLightON);
          Serial.println(DTH_Threshhold_Min);
          Serial.println(DTH_Threshhold_Max);
        }


        if (eventName == "MQ135Threshold") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          MQ135_Threshhold_Min = data["min"];  // Extract the value of "islight"
          MQ135_Threshhold_Max = data["max"];  // Extract the value of "islight"

          // Print the value of "islight"
          // Serial.print(sio_isLightON);
          Serial.println(MQ135_Threshhold_Min);
          Serial.println(MQ135_Threshhold_Max);
        }


        if (eventName == "LDRThreshold") {
          // Print the event name
          // Serial.print("Event Name: ");
          // Serial.println(eventName);
          JsonObject data = array[1];
          LDR_Threshhold_Min = data["min"];  // Extract the value of "islight"
          LDR_Threshhold_Max = data["max"];  // Extract the value of "islight"

          // Print the value of "islight"
          // Serial.print(sio_isLightON);
          Serial.println(LDR_Threshhold_Min);
          Serial.println(LDR_Threshhold_Max);
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


    if (isAutomated) {
      // Serial.println("Automated..........................................");
      
    if (!((DTH_Threshhold_Min <= DTH_temperature) && (DTH_Threshhold_Max >= DTH_temperature))) {
      Serial.println("DTH_Threshhold_Min");
      Serial.println("DTH_Threshhold_Max");
      Serial.println(DTH_Threshhold_Max);
      
      digitalWrite(D5, LOW);
    } else {
      // Serial.println("FAN is  OFF..........................................");

      digitalWrite(D5, HIGH);
    }
    if ((MQ135_Threshhold_Min <= MQ135_gasConcentration) && (MQ135_Threshhold_Max >= MQ135_gasConcentration))
     {
      digitalWrite(D3, LOW);

    } else {
      digitalWrite(D3, HIGH);

    }
    if (!((LDR_Threshhold_Min <= LDR_lightIntensity) && (LDR_Threshhold_Max >= LDR_lightIntensity)))
     {
      digitalWrite(BLUE_LED, LOW);

    } else {
      digitalWrite(BLUE_LED, HIGH);

    }


  } else {
      // Serial.println(" .....................not Automated..........................................");

    if (isFanON) {
      digitalWrite(D5, LOW);
            // Serial.println("FAN IS ON");

    } else {
      digitalWrite(D5, HIGH);
            // Serial.println("FAN IS off");

    }
    if (isVentON) {
      digitalWrite(YELLOW_LED, LOW);
    } else {
      digitalWrite(YELLOW_LED, HIGH);
    }
    if (isLightON) {
      digitalWrite(BLUE_LED, LOW);
    } else {
      digitalWrite(BLUE_LED, HIGH);
    }
  }



    //   DynamicJsonDocument isVentOn(124);
    // JsonArray array3 = isVentOn.to<JsonArray>();

    // // add evnet name
    // // Hint: socket.on('event_name', ....
    // array4.add("isVentOn");

    // // add payload (parameters) for the event
    // JsonObject param3 = array3.createNestedObject();
    // // param1["mqdata"] = 45;
    // // param1["ldr"] = 23;
    // param4["isVentOn"] = isVentON;
    // // JSON to String (serialization)
    // String isVentOnsend;
    // // Serial.println("SOCKETIO SEND:");

    // serializeJson(isVentOn, isVentOnsend);

    // // Send event to socketio

    // socketIO.sendEVENT(isVentOnsend);
    // ldrData.clear();

}

void setup() {
  // Serial.begin(921600);
  Serial.begin(9600);    // Initialize serial communication at 9600 baud rate for debugging
  swSerial.begin(9600);  // Set the baud rate for software serial
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  // Serial.setDebugOutput(true);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    // delay(1000);
  }

  // disable AP
  if (WiFi.getMode() & WIFI_AP) {
    WiFi.softAPdisconnect(true);
  }

  WiFiMulti.addAP(ssid, password);

  // WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    // delay(100);
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

// Serial.println("..........................................................................................");

    String data = swSerial.readStringUntil('\n');  // Read the incoming JSON data from Arduino

    // Parse the JSON data
    DynamicJsonDocument docReceivedFromArduino(512);
    DeserializationError error = deserializeJson(docReceivedFromArduino, data);
      // Serial.print("Data coming....");
    if (!error) {
      DTH_temperature = docReceivedFromArduino["DTH_Temperature"];
      DTH_humidity = docReceivedFromArduino["DTH_Humidity"];
      LDR_lightIntensity = docReceivedFromArduino["LDR_LightIntensity"];
      MQ135_gasConcentration = docReceivedFromArduino["MQ135_GasConcentration"];
      // adcValue = docReceivedFromArduino["adcValue"];

      Serial.print("DTH Temperature: ");
      Serial.println(DTH_temperature);
      sio_DTH_temperature = DTH_temperature;



      Serial.print("DTH Humidity: ");
      Serial.println(DTH_humidity);
      sio_DTH_humidity = DTH_humidity;

      Serial.print("LDR Light Intensity: ");
      Serial.println(LDR_lightIntensity);
      sio_LDR_lightIntensity = LDR_lightIntensity;
      // adjust_Brightness(adcValue, RED_LED);  // RED LED to indicate the light intensity

      Serial.print("MQ135 Gas Concentration: ");
      Serial.println(MQ135_gasConcentration);
      sio_MQ135_gasConcentration = MQ135_gasConcentration;

      // Serial.print("ADC Values: ");
      // Serial.println(adcValue);

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

    digitalWrite(RED_LED, HIGH);


    // socketio event part
    // create message for Socket.IO (event) 
    DynamicJsonDocument dhtData(124);
    JsonArray array1 = dhtData.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array1.add("dhtData");

    // add payload (parameters) for the event
    JsonObject param1 = array1.createNestedObject();
    // param1["mqdata"] = 45;
    // param1["ldr"] = 23;
    param1["temperature"] = sio_DTH_temperature;
    param1["humidity"] = sio_DTH_humidity;

    // JSON to String (serialization)
    String output;
    // Serial.println("SOCKETIO SEND:");

    serializeJson(dhtData, output);

    // Send event to socketio
    socketIO.sendEVENT(output);
    dhtData.clear();


    // socketio event part
    // create message for Socket.IO (event)
    DynamicJsonDocument mq135Data(124);
    JsonArray array2 = mq135Data.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array2.add("mq135Data");

    // add payload (parameters) for the event
    JsonObject param2 = array2.createNestedObject();
    // param1["mqdata"] = 45;
    // param1["ldr"] = 23;
    param2["concentration"] = sio_MQ135_gasConcentration;

    // JSON to String (serialization)
    String output2;
    // Serial.println("SOCKETIO SEND:");

    serializeJson(mq135Data, output2);

    // Send event to socketio
    socketIO.sendEVENT(output2);
    mq135Data.clear();




    // socketio event part
    // create message for Socket.IO (event)
    DynamicJsonDocument ldrData(124);
    JsonArray array3 = ldrData.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array3.add("ldrData");

    // add payload (parameters) for the event
    JsonObject param3 = array3.createNestedObject();
    // param1["mqdata"] = 45;
    // param1["ldr"] = 23;
    param3["light_intensity"] = sio_LDR_lightIntensity;
    // JSON to String (serialization)
    String output3;
    // Serial.println("SOCKETIO SEND:");

    serializeJson(ldrData, output3);

    // Send event to socketio

    socketIO.sendEVENT(output3);
    ldrData.clear();
  }


  // if (swSerial.available()) {
  //     Serial.println("data recieved");


  // }

}
void adjust_Brightness(int ldrValue, int LED_PIN) {
  // Map the LDR sensor value from range 400 to 1023 to the range 0 to 255
  brightness = map(ldrValue, 400, 1023, 0, 255);

  // Limit the brightness value to 0-255 range
  brightness = constrain(brightness, 0, 255);

  // Set the brightness of the LED
  digitalWrite(LED_PIN, brightness);

  //delay(50); // Add a small delay for stability
}
