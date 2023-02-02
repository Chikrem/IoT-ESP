
#include "WiFi.h"
#include <WiFiMulti.h>
#include <PubSubClient.h>

#define TOPICO_SUBSCRIBE "INCB_ESP32_recebe_informacao" 
#define TOPICO_PUBLISH   "INCB_ESP32_envia_string" 
#define ID_MQTT  "INCB_Cliente_MQTT" 

const char* SSID = "dlink-E970";
const char* PASSWORD = "coigd97023"; 
const char* BROKER_MQTT = "broker.hivemq.com";
int BROKER_PORT = 1883;



/* Variáveis e objetos globais */
WiFiClient espClient;
PubSubClient MQTT(espClient);
  
//Prototypes
void init_serial(void);
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);

void setup()
{
    init_serial();
    init_wifi();
    init_mqtt();
}

void init_serial() 
{
    Serial.begin(115200);
}
 
void init_wifi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconnect_wifi();
}

void init_mqtt(void) 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    MQTT.setCallback(mqtt_callback);            
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    Serial.print("[MQTT] Mensagem recebida: ");
    Serial.println(msg);     
}

void reconnect_mqtt(void) 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void reconnect_wifi() 
{
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD);
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void verifica_conexoes_wifi_mqtt(void)
{
    /* se não há conexão com o WiFI, a conexão é refeita */
    reconnect_wifi(); 
    /* se não há conexão com o Broker, a conexão é refeita */
    if (!MQTT.connected()) 
        reconnect_mqtt(); 
} 

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}


void loop()
{
    verifica_conexoes_wifi_mqtt();
    String networks = "";
    Serial.println("Scan Start");
    int n = WiFi.scanNetworks();
    int quality = getQuality();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            
          networks += (i + 1);
          networks += ": ";
          networks += WiFi.SSID(i);
          networks += " (";
          networks += WiFi.RSSI(i);
          networks += ") dbm ";
          networks += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*";
          networks += "|||";
          networks += "\n";
            
            //sprintf(SSID_str, "%s", WiFi.RSSI(i));
            //sprintf(RSSI_str,"RSSI:\t%d dBm\r", WiFi.RSSI(i));

            //MQTT.publish(TOPICO_PUBLISH, SSID_str);
            //MQTT.publish(TOPICO_PUBLISH, RSSI_str);
    
            
            delay(100);
        }
          Serial.println(networks);
          MQTT.publish(TOPICO_PUBLISH, networks.c_str());
          Serial.println("Dados publicados no Broker!");
    }
    Serial.println("");

    MQTT.loop();

    // Wait a bit before scanning again
    delay(5000);
}
