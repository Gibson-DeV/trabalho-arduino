#include <SoftwareSerial.h>

// Definição dos pinos para o módulo SIM900A
#define GSM_RX 10 // Pino 10 para RX do SIM900A
#define GSM_TX 11 // Pino 11 para TX do SIM900A
#define ANALOG_PIN A0 // Porta analógica A0
#define LED_PIN 13 // Pino do LED

SoftwareSerial mySerial(GSM_RX, GSM_TX);
String numero = "+5521995982226"; // Número de destino principal
String numeroLight = "+5554448"; // Número de destino para notificação de falta de luz
int contadorSMS = 0; // Contador de SMS enviados para Light
bool notificado = false; // Flag para rastrear se a notificação final foi enviada

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println("Sistema iniciando...");
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  mySerial.println("AT"); // Handshaking com o SIM900A
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configura o modo de SMS para texto
  updateSerial();
}

void loop() {
  int sensorPorta = analogRead(ANALOG_PIN); // Lê o valor do sensor
  int sensorTensao = map(sensorPorta, 0, 1023, 0, 500); // Mapeia o valor do sensor para a faixa de 0.00V - 5.00V

  if (sensorTensao < 60) { // Se a tensão for menor que 0.60, execute o seguinte código
    digitalWrite(LED_PIN, LOW); // Liga o LED para sinalizar que a tensão está abaixo do limite

    if (contadorSMS < 3) { // Verifica se o contador é menor que 3
      Serial.println("Enviando SMS para " + numeroLight + ": Conta 410474750"); // Mostra no monitor serial o SMS sendo enviado
      mySerial.println("AT+CMGS=\"" + numeroLight + "\""); // Define o número de destino do SMS
      delay(500);
      mySerial.print("Conta 410474750"); // Insere a mensagem do SMS
      mySerial.write(26); // Termina a mensagem do SMS com o código ASCII
      delay(500);
      contadorSMS++; // Incrementa o contador

      Serial.println("Enviando SMS para " + numero + ": Acabou a luz"); // Mostra no monitor serial o SMS sendo enviado
      mySerial.println("AT+CMGS=\"" + numero + "\""); // Define o número de destino do SMS
      delay(500);
      mySerial.print("Acabou a luz"); // Insere a mensagem do SMS
      mySerial.write(26); // Termina a mensagem do SMS com o código ASCII para Ctrl+Z
      delay(500);

    } else if (contadorSMS == 3 && !notificado) { // Se o contador for 3 e a notificação final ainda não foi enviada
      Serial.println("Enviando SMS para " + numero + ": Já foram feitas 3 notificações para Light. Agora faça isso por meio do WhatsApp ou sms."); // Mostra no monitor serial o SMS sendo enviado
      mySerial.println("AT+CMGS=\"" + numero + "\""); // Define o número de destino do SMS
      delay(500);
      mySerial.print("Já foram feitas 3 notificações para Light. Agora faça isso por meio do WhatsApp ou sms."); // Insere a mensagem do SMS
      mySerial.write(26); // Termina a mensagem do SMS com o código ASCII para Ctrl+Z
      delay(500);

      notificado = true; // Define a flag como verdadeira para evitar enviar a notificação múltiplas vezes
    }
  } else {
    digitalWrite(LED_PIN, HIGH); // Desliga o LED se a tensão estiver acima do limite
  }

  delay(500); // Aguarda 30 minutos antes de verificar novamente
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read()); // Encaminha o que o Serial recebeu para a porta Software Serial
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Encaminha o que a porta Software Serial recebeu para o Serial
  }
}