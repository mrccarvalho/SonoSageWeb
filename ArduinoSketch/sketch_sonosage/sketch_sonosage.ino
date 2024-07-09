#include <SPI.h>
#include <WiFiNINA.h>
#include <TMP36.h> //INCLUSÃO DE BIBLIOTECA

///////colocar dados sensíveis em ficgeiro separado/arduino_secrets.h
// Wi-Fi info - IDs e passwords
char ssid[] = "NOS-51F4";          //  network SSID (name)
char pass[] = "7CTYNR4T";   // network password
int keyIndex = 0;                                // network key Index number
int status = WL_IDLE_STATUS;                     // Wifi radio's status

const int wifinum = 5;
int retries = 15;

//  site URL para onde serão enviados os dados
char* host = "192.168.1.26";
const int postPorta = 8086;

//  ID do dispositivo para inserir na base de dados
String dispositivoId = "1";
 
float decib;  // Valor lido pelos sensores

String webString = "";     // String to display

// Geralmente, devemos usar "unsigned long" para variáveis que armazenam tempo
unsigned long previousMillis = 0;        // irá armazenar a última vez que foi lido
const long interval = 2000;              // intervalo de leitura de cada sensor
bool connected = true;
const long postDuracao = 60000;
unsigned long ultimoPost = 0;

TMP36 myTMP36(A0, 5.0); //DEFINE O PINO ANALÓGICO UTILIZADO PELO SENSOR E
//DEFINE A TENSÃO DE REFERÊNCIA (5V OU 3.3V)
/*
 * Método para efetuar a ligação a uma determinada rede
 * wi-fi através de ssid e password
 */
void wificonnect(char ssid[], char pass[]) {
 //pinMode(8, OUTPUT);  
  Serial.begin(9600); //INICIALIZA A SERIAL
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);
  // verificar de existe o módulo wifi:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communicação com o módulo wifi falhou!");
    // não continúa
    while (true);
  }
  // verificar se o firmware do módulo wireless está atualizado:
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Por favor atualize o firmware do Módulo wireless");
  }

  // Tentativa de ligação à Rede Wifi:
  while (status != WL_CONNECTED) {
    Serial.print("Tentativa de ligação à Rede Wifi, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

Serial.println(WiFi.localIP());
  // definimos a duração do último post e fazemos o post imediatamente
  // desde que o main loop inicía
  ultimoPost = postDuracao;
  Serial.println("Setup completo");
  Serial.println("");
  Serial.print("Está conectado à rede");
}

/*
 * obtém leituras/medições and armazena em variáveis globais
 */
void getMedicoes() {
  Serial.println(" - A obter Medições...");
  decib = myTMP36.getTempC(); //VARIÁVEL LOCAL QUE ARMAZENA A TEMPERATURA EM GRAUS CELSIUS
  //OBTIDA ATRAVÉS DA FUNÇÃO myTMP36.getTempC()
  // mostra as leituras no serial
  Serial.println(decib); 
}

/* Método que realiza a tarefa de obter as leituras 
 * e depois envia para um serviço externom neste caso um webserver 
 */
 String IpAddressToString(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}
void post_data() {
  Serial.println("Post/Envio de Dados - Início ");
  //digitalWrite(READLED, HIGH);
  //digitalWrite(ERRORLED, HIGH);
  getMedicoes();
  Serial.print(" - A conectar-se a ");
  Serial.println(host);
  Serial.print(" - na porta ");
  Serial.println(postPorta);  
  
  // class WiFiClient para criar ligações TCP
  WiFiClient client;
  if (!client.connect(host, postPorta)) {
    Serial.println(" - Ligação ao host falhou!");
    return;
  }

  //digitalWrite(ERRORLED, LOW);

  // Criar o URI para o request/pedido
  String url = String("/Home/PostData") +
    String("?id=") + dispositivoId +
    String("&ip=") + IpAddressToString(WiFi.localIP()) +
    String("&decib=") + decib;
  Serial.println(" - A solicitar o URL: ");
  Serial.print("     ");
  Serial.println(url);
  
  // envia o request/pedido para o servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);

  Serial.println(" - Resposta do Cliente: ");

  // Lê todas as linhas de resposta que vem do servidor web 
  // e escreve no serial monitor
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println("");
  Serial.println(" - Closing connection");
  Serial.println("");
  
  //digitalWrite(READLED, LOW);
  Serial.println("Post/Envio Dados - FIM");
  Serial.println("");
}

void setup() {
   wificonnect(ssid, pass);
}

void loop() {
  if (connected) {
    unsigned long diff = millis() - ultimoPost;
    if (diff > postDuracao) {
      post_data();
      ultimoPost = millis();
    }
  } else {
    //digitalWrite(ERRORLED, HIGH);
    Serial.println(" - Could not connect!");
    delay(1000);
    //digitalWrite(ERRORLED, LOW);
  }
}