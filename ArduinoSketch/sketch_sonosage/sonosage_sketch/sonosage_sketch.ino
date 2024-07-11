
#include <SPI.h>
#include <WiFiS3.h>
#include <LiquidCrystal_I2C.h>

///////colocar dados sensíveis em ficheiro separado/arduino_secrets.h
// dados Wi-Fi 
char ssid[] = "Galaxy-RC";          //  SSID (name) / 
char pass[] = "ttqo3746";   //  senha 
int status = WL_IDLE_STATUS;                     // Wifi radio's status
//  site URL para onde serão enviados os dados
char* host = "46.105.31.193";
const int postPorta = 8080;
// Variáveis globais que irão armazenar os valores dos sensores
int db;

const long postDuracao = 10000; //intervalo entre cada envio para a base de dados
unsigned long ultimoPost = 0;
bool conectado = true;
  WiFiClient client;
//---------------------

const int sampleWindow = 50;
unsigned int sample;
#define SENSOR_PIN A0
#define RGB_LED_RED_PIN 2
#define RGB_LED_GREEN_PIN 3
#define RGB_LED_BLUE_PIN 4
#define BUZZER_PIN 5

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
 * Método WIFICONNECT para efetuar a ligação a uma determinada rede
 * wi-fi através de ssid e password
 */
void wificonnect(char ssid[], char pass[]) {
  Serial.begin(9600); //INICIALIZA A SERIAL
    while (!Serial) {
    ; // espera pela porta série para conectar. 
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
    Serial.println("Please upgrade the firmware");
  }
  // Tentativa de ligação à Rede Wifi:
  while (status != WL_CONNECTED) {
    Serial.print("Tentativa de ligação à Rede Wifi, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // espera 10 segundos para tentar ligar de novo:
    delay(10000);
  }

Serial.println(WiFi.localIP());
  // definimos a duração do último post e fazemos o post imediatamente
  // desde que o main loop inicía
  ultimoPost = postDuracao;

  Serial.println("Está conectado à rede");
}


/*
 * obtém leituras/medições and armazena em variáveis globais
 */
int obterLeituras() {
  unsigned long startMillis = millis();
  unsigned long beepStartMillis = 0;
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  bool orangeReached = false;

  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(SENSOR_PIN);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;
  db = map(peakToPeak, 20, 900, 49.5, 90);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loudness: ");
  lcd.print(db);
  lcd.print(" dB");

  // Verifique o nível de dB e ajuste a cor do LED RGB
  if (db < 60) {
    // Abaixo de 60 dB: Cor verde
    analogWrite(RGB_LED_RED_PIN, 0);
    analogWrite(RGB_LED_GREEN_PIN, 90);
    analogWrite(RGB_LED_BLUE_PIN, 0);
    noTone(BUZZER_PIN); // Desligar o buzzer
    orangeReached = false; // Redefinir o estado da cor laranja
  } else if (db >= 60 && db < 70) {
    // Entre 60 e 70 dB: Cor amarela
    analogWrite(RGB_LED_RED_PIN, 255);
    analogWrite(RGB_LED_GREEN_PIN, 255);
    analogWrite(RGB_LED_BLUE_PIN, 0);
    noTone(BUZZER_PIN); // Desligar o buzzer
    orangeReached = false; // Redefinir o estado da cor laranja
  } else if (db >= 70 && db <= 80) {
    // Entre 70 e 80 dB: Cor laranja
    analogWrite(RGB_LED_RED_PIN, 255);
    analogWrite(RGB_LED_GREEN_PIN, 165); // Valor ajustado para laranja
    analogWrite(RGB_LED_BLUE_PIN, 0);
    
    // Se ainda não atingiu a cor laranja, faça 1 beep
    if (!orangeReached) {
      tone(BUZZER_PIN, 1000, 50);
      orangeReached = true; // Marcar que a cor laranja foi atingida
    }
  } else if (db > 80) {
    // Acima de 80 dB: Cor vermelha
    analogWrite(RGB_LED_RED_PIN, 255);
    analogWrite(RGB_LED_GREEN_PIN, 0);
    analogWrite(RGB_LED_BLUE_PIN, 0);
    
    // Se o beep não foi iniciado, inicie e marque o tempo de início
    if (beepStartMillis == 0) {
      tone(BUZZER_PIN, 1000);
      beepStartMillis = millis();
    }
    
    // Verifique se passaram 10 segundos desde o início do beep
    if (millis() - beepStartMillis > 5000) {
      noTone(BUZZER_PIN); // Desligar o buzzer após 5 segundos
    }
    
    orangeReached = false; // Redefinir o estado da cor laranja
  }

  delay(100);
}


/* Método que realiza a tarefa de enviar as leituras 
 *para um serviço externo neste caso um servidor web  
 */
void inserir_leituras(int decibel) {
  Serial.begin(9600);
  Serial.println("Envio de Dados - Início ");
  Serial.print(" - A conectar-se a ");
  Serial.println(host);
  Serial.print(" - na porta ");
  Serial.println(postPorta);  
  

  // Criar o URI para o request/pedido
  String url = String("/Home/InserirLeituras") + String("?dB=") + db;
  Serial.println(" - A solicitar o URL: ");
  Serial.print("     ");
  Serial.println(url);
  
  // envia o request/pedido para o servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  //delay(500);

  // Lê todas as linhas de resposta que vem do servidor web 
  // e escreve no serial monitor
  Serial.println(" - Resposta do SERVIDOR: ");
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println("");
  Serial.println(" - Fechar a conexão");
  Serial.println("");

  Serial.println("Envio de Dados - FIM");
  Serial.println("");
}

void setup() {
  //chama a função para conectar ao wifi
  wificonnect(ssid, pass);

  
  // class WiFiClient para criar ligações TCP
  //É aqui  que faz a ligação com o ip do servidor na porta de destino

  if (!client.connect(host, postPorta)) {
    Serial.println(" - Ligação ao host falhou!");
    return;
  }

  pinMode(SENSOR_PIN, INPUT);
  pinMode(RGB_LED_RED_PIN, OUTPUT);
  pinMode(RGB_LED_GREEN_PIN, OUTPUT);
  pinMode(RGB_LED_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  // Inicialmente, acenda o LED RGB com a cor vermelha
  analogWrite(RGB_LED_RED_PIN, 255);
  analogWrite(RGB_LED_GREEN_PIN, 0);
  analogWrite(RGB_LED_BLUE_PIN, 0);
}


void loop() {
 if (conectado) {
   int decibel =  obterLeituras();
    unsigned long diff = millis() - ultimoPost;
    if (diff > postDuracao) {
      inserir_leituras(decibel);
      ultimoPost = millis();
    }
  } else {

    Serial.println(" - Não conseguiu conectar-se ao host!");
    delay(1000);
  }
}
