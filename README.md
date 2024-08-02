# LogicBox

LogicBox es un prototipo didáctico interactivo diseñado para facilitar el aprendizaje de la electrónica digital. El proyecto consiste en un tablero que incluye una pantalla LCD touch, tres botones tipo arcade conectados a un microcontrolador ESP32. El objetivo es permitir a los usuarios realizar prácticas y experimentos con compuertas lógicas utilizando elementos físicos y virtuales.

## Características Principales

- **Interfaz Interactiva:** Una pantalla LCD touch muestra el funcionamiento de los distintos principios que hacen funcionar a los comparadores y memorias electrónicas.
- **Botones Arcade:** Tres botones que permiten la entrada de señales lógicas (0 o 1) para interactuar con la interfaz de usuario.
- **Conexión con ESP32:** Un microcontrolador ESP32 gestiona la lógica del sistema, proporcionando una interfaz de usuario y permitiendo la programación y control del dispositivo.
- **Realidad Aumentada (AR):** Materiales de apoyo adicionales accesibles mediante tecnología AR para mejorar la comprensión y visualización de los conceptos.
- **IoT (Internet de las Cosas):** Posibilidad de programar y controlar la pantalla LCD a través de dispositivos móviles, integrando funcionalidades HMI (Human-Machine Interface).

## Requisitos

- ESP32 Devkit V1
- Pantalla TFT ILI9341
- Botones tipo arcade
- LED
- Otros componentes electrónicos (resistencias, cables, etc.)

## Instalación

1. **Configurar el entorno de desarrollo para ESP32:**
   - Descargar e instalar [Arduino IDE](https://www.arduino.cc/en/Main/Software).
   - Configurar el ESP32 en el Arduino IDE siguiendo las instrucciones de [Espressif](https://github.com/espressif/arduino-esp32#installation-instructions).

2. **Clonar este repositorio:**
   \`\`\`sh
   git clone https://github.com/tuusuario/logicbox.git
   cd logicbox
   \`\`\`

3. **Subir el sistema de archivos SPIFFS:**
   - Asegúrate de tener el plugin de ESP32 SPIFFS instalado.
   - Sube las imágenes y la página web al sistema de archivos del ESP32.

4. **Cargar el código en el ESP32:**
   - Abre \`logicbox.ino\` en Arduino IDE.
   - Configura el puerto y la placa correctos.
   - Sube el código al ESP32.

## Uso

1. **Conexión WiFi:**
   - El ESP32 crea una red WiFi llamada \`ESP32_Hotspot\` con la contraseña \`12345678\`.
   - Conéctate a esta red con tu dispositivo móvil o computadora.

2. **Acceder a la interfaz web:**
   - Abre un navegador web y navega a \`http://192.168.4.1\`.
   - Interactúa con la página web para enviar expresiones lógicas personalizadas al ESP32.

3. **Interacción con el hardware:**
   - Usa los botones físicos para interactuar con las compuertas lógicas y memorias en el LogicBox.
   - Mantén presionado el botón de selección durante 3 segundos para regresar al menú principal.

## Contribución

¡Contribuciones son bienvenidas! Si deseas mejorar el proyecto, por favor abre un issue o envía un pull request.

## Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo [LICENSE](LICENSE) para más detalles.
