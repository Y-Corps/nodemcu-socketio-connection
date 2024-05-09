// arduino code
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial swSerial(10, 11);  // RX, TX pins for software serial

const int BUFFER_SIZE = 512;
StaticJsonDocument<BUFFER_SIZE> docReceivedFromNodeMCU;  // JSON document for storing received data

void setup() {
  Serial.begin(9600);    // Initialize serial communication at 9600 baud rate for debugging
  swSerial.begin(9600);  // Initialize software serial communication at 9600 baud rate for receiving data from NodeMCU
  Serial.println("Arduino Uno is ready");
}

void loop() {
  // recieve data from NodeMCU
  if (swSerial.available()) {
    String data = swSerial.readStringUntil('\n');                                // Read the incoming JSON data from NodeMCU
    DeserializationError error = deserializeJson(docReceivedFromNodeMCU, data);  // Parse the JSON data

    if (!error) {
      // int DTH_temperature = docReceivedFromArduino["DTH_Temperature"];
      // int DTH_humidity = docReceivedFromNodeMCU["DTH_Humidity"];
      // int LDR_lightIntensity = docReceivedFromArduino["LDR_LightIntensity"];
      // int MQ135_gasConcentration = docReceivedFromArduino["MQ135_GasConcentration"];
      bool isFanON = docReceivedFromNodeMCU["isFanON"];
      bool isLightON = docReceivedFromNodeMCU["isLightON"];
      bool isVentON = docReceivedFromNodeMCU["isVentON"];
      // bool isAutomated = docReceivedFromArduino["isAutomated"];
      // int DTH_Threshhold_Min = docReceivedFromArduino["DTH_Threshhold_Min"];
      // int DTH_Threshhold_Max = docReceivedFromArduino["DTH_Threshhold_Max"];
      // int MQ135_Threshhold_Min = docReceivedFromArduino["MQ135_Threshhold_Min"];
      // int MQ135_Threshhold_Max = docReceivedFromArduino["MQ135_Threshhold_Max"];
      // int LDR_Threshhold_Min = docReceivedFromArduino["LDR_Threshhold_Min"];
      // int LDR_Threshhold_Max = docReceivedFromArduino["LDR_Threshhold_Max"];
      Serial.print("isFanON: ");
      Serial.println(isFanON);
      Serial.print("isLightON: ");
      Serial.println(isLightON);
      Serial.print("isVentON: ");
      Serial.println(isVentON);
      docReceivedFromNodeMCU.clear();
    } else {
      // Handle JSON parsing error
      // Serial.print("Failed to parse JSON: ");
      // Serial.println(error.c_str());
    }
  }
  // SEND JSON DATA TO NODEMCU
  StaticJsonDocument<1024> docSendDataToArduino;  // Create a JSON document

  docSendDataToArduino["DTH_Temperature"] = random(10,99);
  docSendDataToArduino["DTH_Humidity"] = random(10,99);
  docSendDataToArduino["LDR_LightIntensity"] = random(1,999);
  docSendDataToArduino["MQ135_GasConcentration"] = random(10,99);
  // docSendDataToArduino["isFanON"] = true;
  // docSendDataToArduino["isLightON"] = true;
  // docSendDataToArduino["isVentON"] = true;
  // docSendDataToArduino["isAutomated"] = 234;
  // docSendDataToArduino["DTH_Threshhold_Min"] = 234;
  // docSendDataToArduino["DTH_Threshhold_Max"] = 234;
  // docSendDataToArduino["MQ135_Threshhold_Min"] = 234;
  // docSendDataToArduino["MQ135_Threshhold_Max"] = 342;
  // docSendDataToArduino["LDR_Threshhold_Min"] = 23894;
  // docSendDataToArduino["LDR_Threshhold_Max"] = 27983;
  // char buffer[1024];
  serializeJson(docSendDataToArduino, swSerial);
  swSerial.println();
  docSendDataToArduino.clear();
  delay(500);  // Wait for 1 second before sending the next data
}