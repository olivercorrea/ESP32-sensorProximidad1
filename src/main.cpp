#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

// #include <NTPClient.h>
// #include <WiFiUdp.h>

//-------------------------------------------------------
// Variables

#define LED_BUILTIN 2

int sensorIzquierdo = 25; // Sensor 1

int contadorAcertado = 0; // Contador de aciertos.
int contadorFallidos = 0; // Contador de fallos.

// Variables de url
String ip = "192.168.43.20"; //192.168.0.105 / 192.168.86.20 / 172.22.146.121 / 192.168.184.20
int puerto = 3000;
String ruta = "/save-counter"; // Ruta del recurso

String urlCompleta = "http://" + ip + ":" + String(puerto) + ruta;

HTTPClient http;

//------------------Variables de zumbador-------------------------------------

int pinZumbador = 4;
int canal = 0;
int frec = 10000;
int resolucion = 8;


// Para tiempo
// WiFiUdp ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", -5 * 3600); // Zona horaria GMT-5 (Lima)


//-------------------------------------------------------

//---------------------------------------------------------Credenciales--------------------------------------------------------------------------

//.-.-.-.-.-.-Credenciales WiFi.-.-.-.-.-.-.-.-.-.

/*
|----------------------|--------------|
|         SSID         | Contraseña   |
|----------------------|--------------|
|TP-Link_B0C6          | 86150903     |
|Comunidad Innovadores | INn0V4-2K23!*|
|moto g(100) _1502     | 12345678     |
|Dronexx               | 123456789    |
|HONOR 90 Lite         | potito123    |
|Redmi 10c             | 12345678     |
|Movistar Home         | tony123456   |
|----------------------|--------------|
*/
  // SSID & contraseña
  // Esta sección define dos variables constantes que almacenan el nombre de la red WiFi (SSID) y su contraseña.
const char* ssid = "Movistar Home";
const char* contrasena = "tony123456";
//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-..-.-.-.-.-.-.-.-.

//----------------------------------------------------------------------------------------------------------------------------------------------
void wificonect() {

  // Se intenta establecer la conexión a la red WiFi
  WiFi.begin(ssid, contrasena, 6); // Inicia la conexión a la red WiFi utilizando el SSID y la contraseña especificados.
  // El parámetro 6 indica el canal WiFi a utilizar.

  // Se espera a que la conexión se establezca
  while (WiFi.status() != WL_CONNECTED) { // Entra en un ciclo que se repite hasta que la conexión se establezca.
    Serial.println("Aun no conectado");
    delay(250); // Se introduce un retardo de 250 mili segundos entre cada iteración del ciclo.
    // spinner(); // Se llama a la función spinner() para mostrar un spinner en el display LCD.
  }

  // Muestra mensajes de confirmación de conexión
  Serial.println("");
  Serial.println("¡WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0); // ponemos el cursor en la posición de la columna 0 y fila 0
  lcd.print("Bienvenido ...");

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200); // Comunicación con nuestro monitor serial

  // Inicializamos la salida del PWM
  ledcSetup(canal, frec, resolucion);
  // Declaramos pin donde se conecta el zumbador
  ledcAttachPin(pinZumbador, 0);

  wificonect(); // Llamamos a la función wificonect() para conectarnos a una red WiFi
  // sensor de proximidad
  pinMode(sensorIzquierdo, INPUT);
  // http.begin(urlCompleta); // Inicia conexión con el server
  delay(500); // Esperamos medio segundo
}

void loop() {

  Serial.print("IP: "); // Imprime el texto "IP: " en el monitor serial.
  Serial.println(WiFi.localIP()); // Imprime la dirección IP local asignada al dispositivo en el monitor serial.

  lcd.clear();

  digitalRead(sensorIzquierdo);

  if (!digitalRead(sensorIzquierdo)) {
    contadorAcertado = contadorAcertado + 1;  // Incrementamos el contador cuando se detecta
    Serial.print("Acertado: ");
    Serial.println(contadorAcertado);

    lcd.setCursor(0, 0);

    lcd.print("Total: " + String(contadorAcertado) + "       ");

    // Enviamos un tono
    ledcWriteTone(0, frec);
    delay(500);
    ledcWriteTone(0, 0); // apagamos el zumbador

    if (WiFi.status() == WL_CONNECTED) {

      http.begin(urlCompleta); // Inicia conexión con el server
      http.addHeader("Content-Type", "application/json");

      String requestBody = "{\"contadorAcertado\":";
      requestBody += contadorAcertado;
      requestBody += "}";

      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode == 200) {
        Serial.println("Counter data sent successfully");
      } else {
        Serial.print("Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }

      http.end();
    }

  } else {
    contadorFallidos++;
    Serial.print("Fallido: ");
    Serial.println(contadorFallidos);
  }

  delay(1000);

}
