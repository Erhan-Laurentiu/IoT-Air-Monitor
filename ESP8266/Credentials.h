//-------- Customise these values -----------
const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
#define ORG "i2ac1y"
#define DEVICE_TYPE "ESP01"
#define DEVICE_ID "Bedroom"
#define TOKEN "xxxxxxxxxxxxxxxxxx"
//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char eventTopic[] = "iot-2/evt/status/fmt/json";
const char cmdTopic[] = "iot-2/cmd/led/fmt/json";
