#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>
// WiFi credentials
const char *ssid = "nikunja";
const char *password = "12345678";

// Socket.IO server settings
const char *host = "192.168.235.62";
const int port = 3001;

ESP8266WiFiMulti WiFiMulti;
SocketIOclient socketIO;

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
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
        String eventName = array[0]; // Assuming the event name is the first element
        if (eventName == "isFanOn")
        {
            // Print the event name
            // Serial.print("Event Name: ");
            // Serial.println(eventName);
            JsonObject data = array[1];
            bool isLight = data["isFanOn"]; // Extract the value of "islight"

            // Print the value of "islight"
            Serial.print("ISfan Value: ");
            Serial.println(isLight ? "true" : "false");
            // Print the complete JSON object
            // Serial.println("Data:");
            // Serial.println(doc);
            // serializeJsonPretty(doc, Serial);  // Print the JSON object in a human-readable format

            // String output;
            // serializeJson(doc, output);
            // Serial.println(output);
        }
        // if (Serial.available())
        // {
        //     String data = Serial.readStringUntil('\n');
        //     // Serial.print("Received from Arduino: ");
        //     // Serial.println(data);

            // Parse the incoming JSON data
            // DynamicJsonDocument docReceive(1024);
            // deserializeJson(docReceive, data);

            // // Extract and process the JSON data
            // String message = docReceive["message"]; // Example: Extracting a message field
            // // Serial.print("Received Message: ");
            // Serial.println(message);

        //     // Store the received data in the array
        //     dataArray[arrayIndex] = data;
        //     arrayIndex++;

        //     // Reset the array index if it reaches the maximum size
        //     if (arrayIndex >= MAX_DATA_SIZE)
        //     {
        //         arrayIndex = 0;
        //     }
        // }

        // // Send data to Arduino in JSON format
        // DynamicJsonDocument docSend(128);
        // docSend["isFanOn"] = isLight ? "true" : "false";

        // // Serialize JSON to a string
        // String output;
        // serializeJson(docSend, output);

        // // Send JSON data to Arduino
        // // Serial.println(output);
        // Serial.print("Sent to Arduino: ");
        // // Serial.println(output);
    }
    break;
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
}

void setup()
{
    // Serial.begin(921600);
    Serial.begin(115200);

    // Serial.setDebugOutput(true);
    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    // disable AP
    if (WiFi.getMode() & WIFI_AP)
    {
        WiFi.softAPdisconnect(true);
    }

    WiFiMulti.addAP(ssid, password);

    // WiFi.disconnect();
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }

    String ip = WiFi.localIP().toString();
    Serial.printf("[SETUP] WiFi Connected %s\n", ip.c_str());

    // server address, port and URL
    socketIO.begin(host, port, "/socket.io/?EIO=4");

    // event handler
    socketIO.onEvent(socketIOEvent);
}

unsigned long messageTimestamp = 0;
void loop()
{
    socketIO.loop();

    // creat JSON message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array.add("mq135Data");

    // add payload (parameters) for the event
    JsonObject param1 = array.createNestedObject();
    // param1["mqdata"] = 45;
    // param1["ldr"] = 23;
    param1["temperature"] = 25.5;

    // JSON to String (serialization)
    String output;
    serializeJson(doc, output);

    // Send event
    socketIO.sendEVENT(output);

    // Print JSON for debugging
    // Serial.println(output);
}