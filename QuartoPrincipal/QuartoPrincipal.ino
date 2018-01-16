#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <IRremoteESP8266.h> //Importa a Biblioteca IRremoteESP8266
#include <IRsend.h> //Importa a Biblioteca IRsend
#include "DHT.h" //Importa a Biblioteca de temperatura e humidade
#include <SimpleTimer.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif

//defines:
//defines de id mqtt e tópicos para publicação e subscribe
#define ID_MQTT  "QUARTO PRINCIPAL"     //id mqtt (para identificação de sessão)
#define TOPICO_SUBSCRIBE "quarto_principal"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "quarto_principal/luz/estado"    //tópico MQTT de envio de informações para Broker
#define TOPICO_PUBLISH_TEMP   "quarto_principal/temperatura"    //tópico MQTT de envio de informações para Broker
#define TOPICO_PUBLISH_HUMID   "quarto_principal/humidade"    //tópico MQTT de envio de informações para Broker


//defines - mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1


// WIFI
//const char* SSID = "Como estou dirigindo?"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "curupira cambota"; // Senha da rede WI-FI que deseja se conectar
const char* SSID = "TP-LINK_50CD7E";
// MQTT
const char* BROKER_MQTT = "192.168.0.101"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//TIMERS
//os_timer_t tmr0;//Cria o Timer. Maximo de 7 Timer's.
//os_timer_t tmr1;//Cria o Timer. Maximo de 7 Timer's.
SimpleTimer timer;


//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída
const int sensorTensao = A0; //PINO ANALÓGICO UTILIZADO PELO SENSOR DE TENSÃO
IRsend irsend(D2);  // An IR LED is controlled by GPIO pin 4 (D2) //Cria objeto para envidar Infra-vermelho
uint16_t rawData[93] = {1318, 384,  1302, 382,  460, 1224,  
    1296, 388,  1302, 382,  456, 1228,  462, 1222,  458, 1248,  
    430, 1232,  454, 1230,  460, 1224,  1304, 7102,  1314, 388,  
    1304, 382,  460, 1224,  1298, 388,  1296, 388,  456, 1228,  462, 
    1222,  460, 1248,  430, 1232,  454, 1228,  456, 1230,  1302, 7104,  
    1316, 386,  1294, 392,  450, 1234,  1298, 384,  1302, 382,  456, 1228,  
    460, 1224,  452, 1232,  456, 1228,  454, 1252,  434, 1228,  1296, 7106,  
    1320, 384,  1300, 384,  462, 1244,  1280, 382,  1302, 382,  458, 1228,  
    456, 1228,  452, 1232,  458, 1226,  452, 1256,  434};

//Temperatura e Humidade
#define DHTTYPE DHT11 //modelo do sensor
DHT dht(D4, DHTTYPE); //Cria objeto do sensor temp&humid
float temperatura = 0.0;
float humidade = 0.0;


//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);
void MedeTemperaturaHumidade(void);

/* 
*  Implementações das funções
*/
void setup() 
{
//inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();

    dht.begin();
    irsend.begin();
    


//timers
//os_timer_setfn(&tmr0, EnviaEstadoOutputMQTT, NULL);
//os_timer_arm(&tmr0, 5000, true);
    
    timer.setInterval(300000L, MedeTemperaturaHumidade);
}

//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
}

//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");

    reconectWiFi();
}

//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;

//obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
        char c = (char)payload[i];
        msg += c;
    }

//toma ação dependendo da string recebida:
//verifica se deve colocar nivel alto de tensão na saída D0:
//IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
//enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("ON"))
    {
        digitalWrite(D0, LOW);
        digitalWrite(D3, HIGH); //RELÉ ATIVADO
        EstadoSaida = '1';
    } else 
    {
//verifica se deve colocar nivel alto de tensão na saída D0:
        if (msg.equals("OFF"))
        {
            digitalWrite(D0, HIGH);
            digitalWrite(D3, LOW); //RELÉ ATIVADO
            EstadoSaida = '0';
        }else 
        {
//mensagem para ligar/deligar o ventilador
            if (msg.equals("VENTILADOR"))
            {
                EnviarIR();

            }
        }
    }
    EnviaEstadoOutputMQTT();

}

//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, "pi", "raspberry")) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
            MedeTemperaturaHumidade();
        } 
        else 
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
//se já está conectado a rede WI-FI, nada é feito. 
//Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;

//WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
WiFi.begin(SSID); 
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

//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita

reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

//Função: mede a temperatura e a humidade 
//Parâmetros: nenhum
//Retorno: temperatura e humidade
void MedeTemperaturaHumidade(void)
{
    float temp_atual;
    temp_atual = dht.readTemperature();
    float humid_atual;
    humid_atual = dht.readHumidity();
    if(isnan(temp_atual) && isnan(humid_atual)){

        Serial.println("SHIT!!");


    }else{
        MQTT.publish(TOPICO_PUBLISH_TEMP, String(temp_atual).c_str(), true);
        Serial.print("Temperature = ");
        Serial.println(temp_atual);
        temperatura = temp_atual;
        MQTT.publish(TOPICO_PUBLISH_HUMID, String(humid_atual).c_str(), true);
    }
}

//Função: envia comando para o infra-vermelho 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviarIR(void)
{
irsend.sendRaw(rawData, 67, 38);  // Send a raw data capture at 38kHz.
}

//Função: envia ao Broker o estado atual do output 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '0')
        MQTT.publish(TOPICO_PUBLISH, "OFF");

    else if (EstadoSaida == '1')
        MQTT.publish(TOPICO_PUBLISH, "ON");


}

//Função: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
//IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
//enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(D0, OUTPUT);
    digitalWrite(D0, HIGH);          
    pinMode(D3, OUTPUT); //DEFINE A PORTA COMO SAÍDA DO RELE
    pinMode(sensorTensao, INPUT); //DEFINE A PORTA COMO ENTRADA PARA O SENSOR DE TENSAO
}

//MÉTDODO RESPONSÁVEL POR VERIFICAR SE A LÂMPADA ESTÁ ACESA OU APAGADA
void verificaStatusLamp(){
  for (int i = 0; i < 100; i++){ //PARA "i" IGUAL A 0, ENQUANTO "i" MENOR QUE 100, INCREMENTA "i"
    if(analogRead(sensorTensao) > 70){ //SE LEITURA FOR MENOR QUE 70, FAZ
     // statusLamp = "ON"; //VARIÁVEL RECEBE O VALOR
      EstadoSaida = 1;
    }else{ //SENÃO
      //statusLamp = "OFF"; //VARIÁVEL RECEBE O VALOR
      EstadoSaida = 0;
    }
  }
  Serial.print("Status da lampada ");
  Serial.println(EstadoSaida);
  EnviaEstadoOutputMQTT();
}

//programa principal
void loop() 
{   
//garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();
    timer.run(); // Initiates SimpleTimer
//envia o status de todos os outputs para o Broker no protocolo esperado
//EnviaEstadoOutputMQTT();
//keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
