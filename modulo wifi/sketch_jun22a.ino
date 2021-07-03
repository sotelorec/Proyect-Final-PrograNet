#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <FS.h>

const char* ssid = "IZZI-37B9";
const char* password = "98F781F737B9";

#define TRIGGER 16 //D0
#define ECHO    5  //D1

long duracion = 0;
int16_t distancia = 0;
int16_t ultimoValor = 0;
uint8_t contador = 0;
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);

void setup() {
  delay(1000);
  Serial.begin(115200);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
  IPAddress myIP = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(myIP);

  SPIFFS.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.onNotFound([]() {
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "Archivo no encontrado");
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado");

}

void loop() {
  webSocket.loop();
  server.handleClient();
  contador++;
  if (contador == 255) {
    contador = 0;
    //digitalWrite(TRIGGER, LOW);
    //delayMicroseconds(2);
    digitalWrite(TRIGGER, HIGH);
    delay(1);
    digitalWrite(TRIGGER, LOW);
    duracion = pulseIn(ECHO, HIGH);
    distancia = duracion / 58.2;
    //if (distancia < 0) distancia = 0;
    if (distancia != ultimoValor) {
      String msj = String(distancia);
      webSocket.broadcastTXT(msj);
    }
    ultimoValor = distancia;
    Serial.println(ultimoValor);
  }
  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED: {
      Serial.printf("Usuario %u - Desconnectado", num);
      break;
    }
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("Nueva Conexion: %d.%d.%d.%d Nombre: %s ID: %u\n", ip[0], ip[1], ip[2], ip[3], payload, num);
      String msj = String(ultimoValor);
      webSocket.broadcastTXT(msj);
      break;
    }
    case WStype_TEXT: {
      break;
    }
  }
}

String getContentType(String filename) {
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-zip";
}

bool handleFileRead(String path) {
  #ifdef DEBUG
    Serial.println("handlFileRead: " + path);
  #endif
  if(path.endsWith("/"))
    path += "index.html";
  if(SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}
