//Libraries
#include <MQ135.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
//Sensors
#define GAS_SENSOR A0 //MQ-135
#define TEMP_SENSOR 2 //DHT11
#define LDR_SENSOR A1 //LDR Photo resistor
#define DHTTYPE DHT11 //DHT Sensor Version
//LED & Fan
#define RED_LED 9
#define YELLOW_LED 10
#define GREEN_LED 5
#define BLUE_LED 11
#define FAN 8
//Array Size
#define MAX_DATA_SIZE 100 // Define the maximum size of the data array

SoftwareSerial swSerial(12, 13);  



// Define a lookup table that maps ADC values to lux values
const int ADC_RES = 1024; // Resolution of the Arduino's ADC (10-bit)
const int LUX_TABLE_SIZE = 10; // Size of the lookup table
int adcValues[LUX_TABLE_SIZE] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
float luxValues[LUX_TABLE_SIZE] = {10.0, 50.0, 100.0, 200.0, 400.0, 700.0, 1000.0, 1500.0, 2000.0, 3000.0};

MQ135 mq135_sensor(GAS_SENSOR);
DHT dht(TEMP_SENSOR, DHTTYPE);

String dataArray[MAX_DATA_SIZE]; // Array to store the received data
int arrayIndex = 0; // Index to keep track of the array position


const int BUFFER_SIZE = 512;
StaticJsonDocument<BUFFER_SIZE> docReceivedFromNodeMCU;  // JSON document for storing received data

float temperature, humidity; // Temp and Humid floats, will be measured by the DHT
int brightness = 0;
void setup() {
  Serial.begin(9600);
  swSerial.begin(9600);  // Initialize software serial communication at 9600 baud rate for receiving data from NodeMCU
  pinMode(2, INPUT);
  dht.begin();
  Serial.println("Arduino Uno is ready");

}




void loop() {



// // recieve data from NodeMCU
//   if (swSerial.available()) {
//     String data = swSerial.readStringUntil('\n');                                // Read the incoming JSON data from NodeMCU
//     DeserializationError error = deserializeJson(docReceivedFromNodeMCU, data);  // Parse the JSON data
//       Serial.print(".......");

//     if (!error) {
//       // int DTH_temperature = docReceivedFromArduino["DTH_Temperature"];
//       // int DTH_humidity = docReceivedFromNodeMCU["DTH_Humidity"];
//       // int LDR_lightIntensity = docReceivedFromArduino["LDR_LightIntensity"];
//       // int MQ135_gasConcentration = docReceivedFromArduino["MQ135_GasConcentration"];
//       bool isFanON = docReceivedFromNodeMCU["isFanON"];
//       bool isLightON = docReceivedFromNodeMCU["isLightON"];
//       bool isVentON = docReceivedFromNodeMCU["isVentON"];
//       // bool isAutomated = docReceivedFromArduino["isAutomated"];
//       // int DTH_Threshhold_Min = docReceivedFromArduino["DTH_Threshhold_Min"];
//       // int DTH_Threshhold_Max = docReceivedFromArduino["DTH_Threshhold_Max"];
//       // int MQ135_Threshhold_Min = docReceivedFromArduino["MQ135_Threshhold_Min"];
//       // int MQ135_Threshhold_Max = docReceivedFromArduino["MQ135_Threshhold_Max"];
//       // int LDR_Threshhold_Min = docReceivedFromArduino["LDR_Threshhold_Min"];
//       // int LDR_Threshhold_Max = docReceivedFromArduino["LDR_Threshhold_Max"];
//       Serial.print("isFanON: ");
//       Serial.println(isFanON);
//       Serial.print("isLightON: ");
//       Serial.println(isLightON);
//       Serial.print("isVentON: ");
//       Serial.println(isVentON);
//       docReceivedFromNodeMCU.clear();



//         if(isFanON) {
//         digitalWrite(FAN, LOW);
//       }
//       else {
//         digitalWrite(FAN, HIGH);
//       }


//       if(isLightON) {
//         digitalWrite(BLUE_LED, LOW);
//       }
//       else {
//         digitalWrite(BLUE_LED, HIGH);
//       }


//       if(isVentON) {
//         digitalWrite(YELLOW_LED, LOW);
//       }
//       else {
//         digitalWrite(YELLOW_LED, HIGH);
//       }
//     } else {
//       // Handle JSON parsing error
//       // Serial.print("Failed to parse JSON: ");
//       // Serial.println(error.c_str());
//     }
//   }






  // SEND JSON DATA TO NODEMCU
  StaticJsonDocument<1024> docSendDataToArduino;  // Create a JSON document
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
  // Wait a few seconds between measurements.
  delay(1000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
  float hic = dht.computeHeatIndex(temperature, humidity, false);

  /*Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.println("");
  Serial.print("Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.println("");
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.println("");
  Serial.print("PPM: ");
  Serial.print(ppm);
  Serial.print("ppm");
  Serial.println("");*/
  Serial.print("Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print("%");
  Serial.println("");
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println("°C");
  Serial.println("");
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C"));
  Serial.println("");

  int adcValue = analogRead(LDR_SENSOR); // Read LDR sensor value
  adjust_Brightness(adcValue, RED_LED); // RED LED to indicate the light intensity

  float luxValue = mapValue(adcValue, adcValues, luxValues, LUX_TABLE_SIZE);
  // Serial.print("ADC Value: ");
  // Serial.print(adcValue);
  Serial.print("Lux Value: ");
  Serial.print(luxValue);
  Serial.print(luxvalues)
  Serial.print(" Lux");

  Serial.println();
  Serial.println();



  docSendDataToArduino["DTH_Temperature"] = temperature;
  docSendDataToArduino["DTH_Humidity"] = humidity;
  docSendDataToArduino["LDR_LightIntensity"] = luxValue;
  docSendDataToArduino["MQ135_GasConcentration"] = correctedPPM;
  docSendDataToArduino["adcValue"] = adcValue;
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




void adjust_Brightness(int ldrValue, int LED_PIN) {
  // Map the LDR sensor value from range 400 to 1023 to the range 0 to 255
  brightness = map(ldrValue, 400, 1023, 0, 255);

  // Limit the brightness value to 0-255 range
  brightness = constrain(brightness, 0, 255);

  // Set the brightness of the LED
  digitalWrite(LED_PIN, brightness);

  //delay(50); // Add a small delay for stability
}
// Function to map an ADC value to a lux value using linear interpolation
float mapValue(int x, int* x_arr, float* y_arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    if (x >= x_arr[i] && x <= x_arr[i + 1]) {
      float slope = (y_arr[i + 1] - y_arr[i]) / (x_arr[i + 1] - x_arr[i]);
      return y_arr[i] + (x - x_arr[i]) * slope;
    }
  }
  return 0.0; // Return 0.0 if the input value is out of range
}