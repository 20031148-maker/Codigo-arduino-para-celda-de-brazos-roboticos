// --- BIBLIOTECAS ---
#include <Servo.h> // Incluye la librería Servo para poder controlar servomotores.

// --- INICIALIZACIÓN DE OBJETOS SERVO ---
// Se crea un objeto por cada servomotor que se va a controlar en el brazo robótico.
Servo B4Servo1; // Crea el objeto 'B4Servo1' para controlar el primer servomotor.
Servo B4Servo2; // Crea el objeto 'B4Servo2' para controlar el segundo servomotor.
Servo B4Servo3; // Crea el objeto 'B4Servo3' para controlar el tercer servomotor.
Servo B4Servo4; // Crea el objeto 'B4Servo4' para controlar el cuarto servomotor.

// --- DECLARACIÓN DE VARIABLES GLOBALES ---
String mensaje;     // Almacenará el texto completo recibido a través del puerto serial (ej: "Azul,cam1").
String color;       // Almacenará la parte del mensaje que corresponde al color (ej: "Azul").
String camara;      // Almacenará la parte del mensaje que corresponde a la cámara (ej: "cam1").
int delayTime = 10; // Define un pequeño retraso (en milisegundos) para suavizar el movimiento de los servos.
int delayMov = 500; // Define un retraso mayor (en milisegundos) entre movimientos complejos del brazo.
int delayPar = 1;   // Define un retraso mínimo (en milisegundos) para el movimiento de parada.
int posActual = 0;  // Variable de estado para saber la posición actual del brazo (0=Home, 1=Parada, etc.).
int B4Ang1;         // Variable para almacenar el ángulo actual del servo 1 durante los movimientos.
int B4Ang2;         // Variable para almacenar el ángulo actual del servo 2 durante los movimientos.
int B4Ang3;         // Variable para almacenar el ángulo actual del servo 3 durante los movimientos.
int B4Ang4;         // Variable para almacenar el ángulo actual del servo 4 durante los movimientos.

// --- VECTORES DE POSICIONES (ÁNGULOS PARA LOS SERVOS) ---
// Cada vector (arreglo) representa una pose específica del brazo robótico.
// El orden de los valores es {ángulo_servo1, ángulo_servo2, ángulo_servo3, ángulo_servo4}.
int B4S1PosH [4] = {90, 25, 155, 180}; // Define los ángulos para la posición "Home" (inicial o de reposo).
int B4S1PosP [4] = {90, 50, 180, 90};  // Define los ángulos para la posición de "Parada".
int B4S1PosR [4] = {42, 110, 138, 90};  // Define los ángulos para la posición de "Tomar la pieza".
int B4S1PosL [4] = {90, 41, 128, 180};  // Define los ángulos para la posición de "Levantar la pieza".
int B4S1PosD [4] = {145, 105, 130, 90}; // Define los ángulos para la posición de "Dejar la pieza".

// --- DECLARACIÓN DE CONSTANTES PARA PINES ---
const int LEDR = 2; // Asigna el pin digital 2 a una constante para el LED Rojo.
const int LEDG = 3; // Asigna el pin digital 3 a una constante para el LED Verde.
const int LEDB = 4; // Asigna el pin digital 4 a una constante para el LED Azul.

// --- FUNCIÓN DE CONFIGURACIÓN (SE EJECUTA UNA SOLA VEZ AL INICIO) ---
void setup() {
  Serial.begin(9600); // Inicia la comunicación serial a una velocidad de 9600 baudios.

  // Configura los pines de los LEDs como salidas digitales.
  pinMode(LEDR, OUTPUT); // El pin del LED Rojo se usará para enviar señales (encender/apagar).
  pinMode(LEDG, OUTPUT); // El pin del LED Verde se usará para enviar señales.
  pinMode(LEDB, OUTPUT); // El pin del LED Azul se usará para enviar señales.

  // Asocia cada objeto Servo a su pin digital correspondiente en la placa Arduino.
  B4Servo1.attach(22); // El servo 1 está conectado al pin 22.
  B4Servo2.attach(24); // El servo 2 está conectado al pin 24.
  B4Servo3.attach(26); // El servo 3 está conectado al pin 26.
  B4Servo4.attach(28); // El servo 4 está conectado al pin 28.

  // Envía el brazo robótico a su posición inicial "Home" al encenderse.
  B4Servo1.write(B4S1PosH[0]); // Mueve el servo 1 a su ángulo "Home".
  B4Servo2.write(B4S1PosH[1]); // Mueve el servo 2 a su ángulo "Home".
  B4Servo3.write(B4S1PosH[2]); // Mueve el servo 3 a su ángulo "Home".
  B4Servo4.write(B4S1PosH[3]); // Mueve el servo 4 a su ángulo "Home".
}

// --- FUNCIÓN PRINCIPAL (SE EJECUTA EN BUCLE CONSTANTEMENTE) ---
void loop() {
  if (Serial.available()) { // Comprueba si hay datos disponibles para leer en el puerto serial.
    mensaje = Serial.readStringUntil('\n'); // Lee los datos como una cadena de texto hasta encontrar un salto de línea.

    int separador = mensaje.indexOf(','); // Busca la posición del carácter ',' que separa los datos.

    if (separador != -1) { // Si se encontró una coma en el mensaje...
      color = mensaje.substring(0, separador);  // Extrae el texto ANTES de la coma (el color).
      camara = mensaje.substring(separador + 1); // Extrae el texto DESPUÉS de la coma (la cámara).

      // Imprime en el Monitor Serial los datos recibidos para depuración.
      Serial.print("Color detectado: "); // Imprime una etiqueta de texto.
      Serial.println(color);             // Imprime el valor de la variable 'color' y hace un salto de línea.
      Serial.print("Desde la cámara: ");  // Imprime otra etiqueta de texto.
      Serial.println(camara);            // Imprime el valor de la variable 'camara'.

      // Lógica de decisión basada en el color recibido.
      if (color == "Rojo") { // Si el color es "Rojo"...
        Serial.println("Detener proceso"); // Informa la acción a realizar.
        B4Parar(); // Llama a la función para mover el brazo a la posición de parada.
      } else if (color == "Azul" || color == "Verde") { // Si el color es "Azul" O "Verde"...
        if (camara == "cam1") { // Y si además el mensaje proviene de la "cam1"...
          // Ejecuta la secuencia completa para recoger y soltar una pieza.
          B4Tomar();        // Llama a la función para tomar la pieza.
          delay(delayMov);  // Espera un momento.
          B4Levantar();     // Llama a la función para levantar la pieza.
          delay(delayMov);  // Espera un momento.
          B4Dejar();        // Llama a la función para dejar la pieza.
          delay(delayMov);  // Espera un momento.
          posActual = 4;    // Actualiza el estado para indicar que viene de la posición "Dejar".
          B4Home();         // Llama a la función para regresar a la posición inicial.
        }
      } else if (color == "Ninguno") { // Si no se detecta ningún color de interés...
        B4Home(); // Llama a la función para mover el brazo a la posición inicial.
      }
    }
  }
}

// --- FUNCIÓN PARA MOVER EL BRAZO A LA POSICIÓN "TOMAR" ---
void B4Tomar() {
  if (posActual == 0) { // Si el brazo está en "Home"...
    // Mueve cada servo gradualmente desde su posición "Home" a su posición "Tomar".
    for (B4Ang4 = B4S1PosH[3]; B4Ang4 >= B4S1PosR[3]; B4Ang4--) { B4Servo4.write(B4Ang4); delay(delayTime); }
    for (B4Ang1 = B4S1PosH[0]; B4Ang1 >= B4S1PosR[0]; B4Ang1--) { B4Servo1.write(B4Ang1); delay(delayTime); }
    for (B4Ang3 = B4S1PosH[2]; B4Ang3 <= B4S1PosR[2]; B4Ang3++) { B4Servo3.write(B4Ang3); delay(delayTime); }
    for (B4Ang2 = B4S1PosH[1]; B4Ang2 <= B4S1PosR[1]; B4Ang2++) { B4Servo2.write(B4Ang2); delay(delayTime); }
    posActual = 2; // Actualiza el estado: el brazo ahora está en la posición "Tomar".

  } else if (posActual == 1) { // Si el brazo está en "Parada"...
    // Mueve cada servo gradualmente desde su posición "Parada" a su posición "Tomar".
    for (B4Ang4 = B4S1PosP[3]; B4Ang4 >= B4S1PosR[3]; B4Ang4--) { B4Servo4.write(B4Ang4); delay(delayTime); }
    for (B4Ang1 = B4S1PosP[0]; B4Ang1 >= B4S1PosR[0]; B4Ang1--) { B4Servo1.write(B4Ang1); delay(delayTime); }
    for (B4Ang2 = B4S1PosP[1]; B4Ang2 <= B4S1PosR[1]; B4Ang2++) { B4Servo2.write(B4Ang2); delay(delayTime); }
    for (B4Ang3 = B4S1PosP[2]; B4Ang3 >= B4S1PosR[2]; B4Ang3--) { B4Servo3.write(B4Ang3); delay(delayTime); }
  }
}

// --- FUNCIÓN PARA MOVER EL BRAZO A LA POSICIÓN "LEVANTAR" ---
void B4Levantar() {
  // Mueve cada servo gradualmente desde su posición "Tomar" a su posición "Levantar".
  for (B4Ang4 = B4S1PosR[3]; B4Ang4 <= B4S1PosL[3]; B4Ang4++) { B4Servo4.write(B4Ang4); delay(delayTime); }
  for (B4Ang2 = B4S1PosR[1]; B4Ang2 >= B4S1PosL[1]; B4Ang2--) { B4Servo2.write(B4Ang2); delay(delayTime); }
  for (B4Ang3 = B4S1PosR[2]; B4Ang3 >= B4S1PosL[2]; B4Ang3--) { B4Servo3.write(B4Ang3); delay(delayTime); }
  for (B4Ang1 = B4S1PosR[0]; B4Ang1 <= B4S1PosL[0]; B4Ang1++) { B4Servo1.write(B4Ang1); delay(delayTime); }
}

// --- FUNCIÓN PARA MOVER EL BRAZO A LA POSICIÓN "DEJAR" ---
void B4Dejar() {
  // Mueve cada servo gradualmente desde su posición "Levantar" a su posición "Dejar".
  for (B4Ang1 = B4S1PosL[0]; B4Ang1 <= B4S1PosD[0]; B4Ang1++) { B4Servo1.write(B4Ang1); delay(delayTime); }
  for (B4Ang3 = B4S1PosL[2]; B4Ang3 <= B4S1PosD[2]; B4Ang3++) { B4Servo3.write(B4Ang3); delay(delayTime); }
  for (B4Ang2 = B4S1PosL[1]; B4Ang2 <= B4S1PosD[1]; B4Ang2++) { B4Servo2.write(B4Ang2); delay(delayTime); }
  for (B4Ang4 = B4S1PosL[3]; B4Ang4 >= B4S1PosD[3]; B4Ang4--) { B4Servo4.write(B4Ang4); delay(delayTime); }
}

// --- FUNCIÓN PARA MOVER EL BRAZO A LA POSICIÓN "HOME" (INICIAL) ---
void B4Home() {
  if (posActual == 1) { // Si el brazo está en "Parada"...
    // Mueve cada servo gradualmente desde su posición "Parada" a su posición "Home".
    for (B4Ang4 = B4S1PosP[3]; B4Ang4 <= B4S1PosH[3]; B4Ang4++) { B4Servo4.write(B4Ang4); delay(delayTime); }
    for (B4Ang1 = B4S1PosP[0]; B4Ang1 >= B4S1PosH[0]; B4Ang1--) { B4Servo1.write(B4Ang1); delay(delayTime); }
    for (B4Ang2 = B4S1PosP[1]; B4Ang2 >= B4S1PosH[1]; B4Ang2--) { B4Servo2.write(B4Ang2); delay(delayTime); }
    for (B4Ang3 = B4S1PosP[2]; B4Ang3 >= B4S1PosH[2]; B4Ang3--) { B4Servo3.write(B4Ang3); delay(delayTime); }
    posActual = 0; // Actualiza el estado: el brazo ahora está en "Home".

  } else if (posActual == 4) { // Si el brazo está en "Dejar"...
    // Mueve cada servo gradualmente desde su posición "Dejar" a su posición "Home".
    for (B4Ang2 = B4S1PosD[1]; B4Ang2 >= B4S1PosH[1]; B4Ang2--) { B4Servo2.write(B4Ang2); delay(delayTime); }
    for (B4Ang3 = B4S1PosD[2]; B4Ang3 >= B4S1PosH[2]; B4Ang3--) { B4Servo3.write(B4Ang3); delay(delayTime); }
    for (B4Ang4 = B4S1PosD[3]; B4Ang4 <= B4S1PosH[3]; B4Ang4++) { B4Servo4.write(B4Ang4); delay(delayTime); }
    for (B4Ang1 = B4S1PosD[0]; B4Ang1 >= B4S1PosH[0]; B4Ang1--) { B4Servo1.write(B4Ang1); delay(delayTime); }
    posActual = 0; // Actualiza el estado: el brazo ahora está en "Home".
  }
}

// --- FUNCIÓN PARA MOVER EL BRAZO A LA POSICIÓN DE "PARADA" ---
void B4Parar() {
  if (posActual == 1) { // Si el brazo ya está en "Parada"...
    Serial.println("Posición OK"); // Simplemente informa que ya está en la posición correcta.
  } else if (posActual == 0) { // Si el brazo está en "Home"...
    // Mueve cada servo gradualmente desde su posición "Home" a su posición "Parada".
    for (B4Ang1 = B4S1PosH[0]; B4Ang1 >= B4S1PosP[0]; B4Ang1--) { B4Servo1.write(B4Ang1); delay(delayPar); }
    for (B4Ang2 = B4S1PosH[1]; B4Ang2 <= B4S1PosP[1]; B4Ang2++) { B4Servo2.write(B4Ang2); delay(delayPar); }
    for (B4Ang3 = B4S1PosH[2]; B4Ang3 <= B4S1PosP[2]; B4Ang3++) { B4Servo3.write(B4Ang3); delay(delayPar); }
    for (B4Ang4 = B4S1PosH[3]; B4Ang4 >= B4S1PosP[3]; B4Ang4--) { B4Servo4.write(B4Ang4); delay(delayPar); }
    posActual = 1; // Actualiza el estado: el brazo ahora está en "Parada".
  }
}
