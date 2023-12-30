#include <WiFi.h>  
#include <Stepper.h>

const int pasosPorMinuto = 200;
String p = "off";  //ESTADO DEL MOTOR

Stepper motorPasos(pasosPorMinuto, 15, 2, 0, 4);

// Credenciales de wifi
const char *ssid = "Molinassss_2.4Gnormal";
const char *password = "Vasquez05";

// Crea servidor llamado "server" y establece el puerto 80 para acceder
WiFiServer server(80);

void setup() {

  //Velocidad del Motor
  motorPasos.setSpeed(60);

  //Inicia puerto serial a velocidad de 115200 baudios
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configurando: Puerto Serial");

  // Inicia conexión WiFi con las datos de la red especificados
  WiFi.begin(ssid, password);  //configurando las credenciales

  Serial.print("Conectandome");
  while (WiFi.status() != WL_CONNECTED)  // Espera conexión a la red
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("La direccion IP es: ");
  Serial.println(WiFi.localIP());  // Imprime la IP del ESP32

  server.begin();  // Inicia el servidor
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient client = server.available();  // Recibe las conexciones de clientes

  if (client) {                        // Si hay un cliente
    Serial.println("Nuevo cliente.");  // Indica el acceso de un cliente
    String currentLine = "";           // Variable para datos de fin de linea
    while (client.connected()) {       // Cliente conectado
      if (client.available()) {        // Datos disponibles para ser leido
        char c = client.read();        // Lectura de byte en variable C
        Serial.write(c);               // Muestra los datos

        if (c == '\n') {  // Si el byte es un caracter de nuevo salto de linea

          if (currentLine.length() == 0) {  // Si no hay caracteres, entonces lanza el codgo HTML
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html{font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button {border: none;color: white;padding: 15px 32px; text-align: center;");
            client.println("text-decoration: none;display: inline-block;font-size: 16px; margin: 4px 2px;cursor: pointer;}");
            client.println(".button1 {background-color: #4CAF50;} /* Green */");
            client.println(".button2 {background-color: #008CBA;} /* Blue */");
            client.println("</style></head>");

            // Creación de botones
            client.print("<body><h1>WebServer con ESP32</h1>");

            if (p == "off") {
              client.println("<button type='button' class='button button1' onClick=location.href='/MOTOR=OPEN'> ABRIR </button><br><br>");
            } else {
              client.println("<button type='button' class='button button2' onClick=location.href='/MOTOR=CLOSE'> CERRAR </button><br><br>");
            }

            client.print("</body></html>");
            client.println();

            // Rompe el codigo del while-loop
            break;
          } else {  // Limpiando variable
            currentLine = "";
          }
        } else if (c != '\r') {  // Si no hay retorno de carro
          currentLine += c;      // agrega al final de la linea
        }

        // Revisando el datos recibido del url
        if (currentLine.indexOf("GET /MOTOR=OPEN") != -1) {
          motorPasos.step(pasosPorMinuto);
          p = "on";
        }
        if (currentLine.indexOf("GET /MOTOR=CLOSE") != -1) {
          motorPasos.step(-pasosPorMinuto);
          p = "off";
        }
      }
    }
    // cerrando la conexión
    client.stop();
    Serial.println("Cliente Desconectado");
  }
}