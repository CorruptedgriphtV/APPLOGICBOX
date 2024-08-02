#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

// Definición de pines
#define BTN_IZQUIERDA_PIN 33
#define BTN_DERECHA_PIN 32
#define BTN_SELECCION_PIN 12
#define LED_PIN 25

WebServer server(80);

// Inicialización de la pantalla
TFT_eSPI pantalla = TFT_eSPI();
int estadoBtnIzquierda = 0;
int estadoBtnDerecha = 0;
int estadoBtnSeleccion = 0;
int estadoPrevioBtnSeleccion = 0;
int tipoCompuerta = 0;
int tipoMemoria = 0;
int indiceMenu = 0;
unsigned long tiempoPresionBtnSeleccion = 0;
bool enMenu = true;
bool enCompuerta = false;
bool enMemoria = false;
bool enPlayground = false;

// Variables de estado para los registros secuenciales
bool latchOutput = false;
byte shiftRegister = 0;
bool srLatchOutput = false;
String customExpression = "false"; // Expresión lógica personalizada

// Imágenes de las compuertas lógicas
const char *imagenesCompuertas[] = {
  "/and.jpg", "/or.jpg", "/nand.jpg", "/nor.jpg",
  "/xor.jpg", "/xnor.jpg", "/not.jpg"
};

// Imágenes y nombres de las opciones del menú
const char *imagenesMenu[] = {
  "/combinacional.jpg", "/secuencial.jpg", "/playground.jpg", "/manual.jpg"
};

const char *opcionesMenu[] = {
  "Combinacional", "Secuencial", "Playground", "Manual"
};

// Imágenes y nombres de las opciones secuenciales
const char *imagenesMemorias[] = {
  "/datalatch.jpg", "/shiftregister.jpg", "/srlatch.jpg"
};

const char *opcionesMemorias[] = {
  "Data Latch", "Shift Register", "SR Latch"
};

// Callback para dibujar imágenes JPG
bool salidaPantalla(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  if (y >= pantalla.height()) return 0;
  pantalla.pushImage(x, y, w, h, bitmap);
  return 1;
}

// Función para dibujar el menú principal
void dibujarMenu() {
  pantalla.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, imagenesMenu[indiceMenu]); // Dibuja la imagen del menú a pantalla completa
  pantalla.setTextColor(TFT_WHITE, TFT_BLACK);
  pantalla.setTextDatum(MC_DATUM);
  pantalla.drawString(opcionesMenu[indiceMenu], pantalla.width() / 2, pantalla.height() - 30, 2);
}

// Función para dibujar las compuertas lógicas
void dibujarCompuerta() {
  pantalla.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, imagenesCompuertas[tipoCompuerta]);
}

// Función para dibujar las memorias secuenciales
void dibujarMemoria() {
  pantalla.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, imagenesMemorias[tipoMemoria]);
}

// Función para dibujar la pantalla custom
void dibujarCustom() {
  pantalla.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, "/custom.jpg");
}

// Evaluar la expresión lógica personalizada
bool evaluarExpresionCustom(bool A, bool B) {
  String expr = customExpression;
  expr.replace("A", A ? "true" : "false");
  expr.replace("B", B ? "true" : "false");
  expr.replace("&", "&&");
  expr.replace("+", "||");
  expr.replace("!", "!");
  expr.replace("#", "!=");
  expr.replace("!&", "!&&"); // NAND
  expr.replace("!+", "!||"); // NOR
  expr.replace("!#", "==");  // XNOR
  bool resultado = false;
  if (expr == "true") {
    resultado = true;
  }
  return resultado;
}

void handleRoot() {
  if (SPIFFS.exists("/index.html")) {
    File file = SPIFFS.open("/index.html", "r");
    String html = file.readString();
    file.close();
    server.send(200, "text/html", html);
  } else {
    server.send(404, "text/plain", "File Not Found");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP("ESP32_Hotspot", "12345678");

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  if (!SPIFFS.begin(true)) {
    Serial.println("Ocurrió un error al montar SPIFFS");
    return;
  }

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(salidaPantalla);

  pantalla.begin();
  pantalla.setRotation(1);
  pantalla.setSwapBytes(true);
  dibujarMenu();
}

void loop() {
  server.handleClient();

  estadoBtnIzquierda = digitalRead(BTN_IZQUIERDA_PIN);
  estadoBtnDerecha = digitalRead(BTN_DERECHA_PIN);
  estadoBtnSeleccion = digitalRead(BTN_SELECCION_PIN);

  if (enMenu) {
    if (estadoBtnIzquierda == LOW) {
      indiceMenu = (indiceMenu - 1 + 4) % 4;
      dibujarMenu();
      delay(300); // delay para debounce
    }
    if (estadoBtnDerecha == LOW) {
      indiceMenu = (indiceMenu + 1) % 4;
      dibujarMenu();
      delay(300); // delay para debounce
    }
    if (estadoBtnSeleccion == LOW && estadoPrevioBtnSeleccion == HIGH) {
      if (indiceMenu == 0) { // Combinacional
        enMenu = false;
        enCompuerta = true;
        tipoCompuerta = 0;
        dibujarCompuerta();
      } else if (indiceMenu == 1) { // Secuencial
        enMenu = false;
        enMemoria = true;
        tipoMemoria = 0;
        dibujarMemoria();
      } else if (indiceMenu == 2) { // Playground
        enMenu = false;
        enPlayground = true;
        dibujarCustom();
      }
    }
  } else if (enCompuerta) {
    if (estadoBtnSeleccion == LOW && estadoPrevioBtnSeleccion == HIGH) {
      tipoCompuerta = (tipoCompuerta + 1) % 7;
      dibujarCompuerta();
    } else if (estadoBtnSeleccion == LOW && millis() - tiempoPresionBtnSeleccion > 3000) {
      enCompuerta = false;
      enMenu = true;
      dibujarMenu();
    } else {
      tiempoPresionBtnSeleccion = (estadoBtnSeleccion == LOW) ? millis() : 0;

      int resultado = 0;
      switch (tipoCompuerta) {
        case 0: resultado = (estadoBtnIzquierda == LOW) && (estadoBtnDerecha == LOW); break;
        case 1: resultado = (estadoBtnIzquierda == LOW) || (estadoBtnDerecha == LOW); break;
        case 2: resultado = !((estadoBtnIzquierda == LOW) && (estadoBtnDerecha == LOW)); break;
        case 3: resultado = !((estadoBtnIzquierda == LOW) || (estadoBtnDerecha == LOW)); break;
        case 4: resultado = (estadoBtnIzquierda == LOW) != (estadoBtnDerecha == LOW); break;
        case 5: resultado = (estadoBtnIzquierda == LOW) == (estadoBtnDerecha == LOW); break;
        case 6: resultado = !(estadoBtnIzquierda == LOW); break;
      }
      digitalWrite(LED_PIN, resultado ? HIGH : LOW);
      pantalla.fillCircle(30, 110, 20, estadoBtnIzquierda == LOW ? TFT_BLUE : TFT_WHITE);
      pantalla.fillCircle(30, 180, 20, estadoBtnDerecha == LOW ? TFT_BLUE : TFT_WHITE);
      pantalla.fillCircle(290, 140, 20, resultado ? TFT_RED : TFT_WHITE);
    }
  } else if (enMemoria) {
    if (estadoBtnSeleccion == LOW && estadoPrevioBtnSeleccion == HIGH) {
      tipoMemoria = (tipoMemoria + 1) % 3;
      dibujarMemoria();
    } else if (estadoBtnSeleccion == LOW && millis() - tiempoPresionBtnSeleccion > 3000) {
      enMemoria = false;
      enMenu = true;
      dibujarMenu();
    } else {
      tiempoPresionBtnSeleccion = (estadoBtnSeleccion == LOW) ? millis() : 0;

      if (tipoMemoria == 0) { // Data Latch
        bool enable = estadoBtnIzquierda == LOW;
        bool data = estadoBtnDerecha == LOW;
        if (enable) latchOutput = data;
        digitalWrite(LED_PIN, latchOutput ? HIGH : LOW);
        pantalla.fillCircle(30, 110, 20, enable ? TFT_BLUE : TFT_WHITE);
        pantalla.fillCircle(30, 180, 20, data ? TFT_BLUE : TFT_WHITE);
        pantalla.fillCircle(290, 140, 20, latchOutput ? TFT_RED : TFT_WHITE);
      } else if (tipoMemoria == 1) { // Shift Register
        if (estadoBtnIzquierda == LOW) shiftRegister = (shiftRegister << 1) | 1;
        if (estadoBtnDerecha == LOW) shiftRegister = (shiftRegister << 1);
        for (int i = 0; i < 8; i++) {
          int color = (shiftRegister & (1 << i)) ? TFT_GREEN : TFT_WHITE;
          pantalla.fillCircle(30 + i * 30, 140, 20, color);
        }
      } else if (tipoMemoria == 2) { // SR Latch
        bool set = estadoBtnIzquierda == LOW;
        bool reset = estadoBtnDerecha == LOW;
        if (set) srLatchOutput = true;
        if (reset) srLatchOutput = false;
        digitalWrite(LED_PIN, srLatchOutput ? HIGH : LOW);
        pantalla.fillCircle(30, 110, 20, set ? TFT_BLUE : TFT_WHITE);
        pantalla.fillCircle(30, 180, 20, reset ? TFT_BLUE : TFT_WHITE);
        pantalla.fillCircle(290, 140, 20, srLatchOutput ? TFT_RED : TFT_WHITE);
      }
    }
  } else if (enPlayground) {
    if (estadoBtnSeleccion == LOW && millis() - tiempoPresionBtnSeleccion > 3000) {
      enPlayground = false;
      enMenu = true;
      dibujarMenu();
    } else {
      tiempoPresionBtnSeleccion = (estadoBtnSeleccion == LOW) ? millis() : 0;
      if (SerialBT.available()) {
        customExpression = SerialBT.readString();
        Serial.println("Expresión recibida: " + customExpression);
      }
      bool resultado = evaluarExpresionCustom(estadoBtnIzquierda == LOW, estadoBtnDerecha == LOW);
      digitalWrite(LED_PIN, resultado ? HIGH : LOW);
      pantalla.fillCircle(30, 110, 20, estadoBtnIzquierda == LOW ? TFT_BLUE : TFT_WHITE);
      pantalla.fillCircle(30, 180, 20, estadoBtnDerecha == LOW ? TFT_BLUE : TFT_WHITE);
      pantalla.fillCircle(290, 140, 20, resultado ? TFT_RED : TFT_WHITE);
    }
  }

  estadoPrevioBtnSeleccion = estadoBtnSeleccion;
  delay(100);
}
s
