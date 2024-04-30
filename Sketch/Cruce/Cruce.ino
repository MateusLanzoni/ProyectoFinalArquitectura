#include <Keypad.h>
// Definición de los pines de los sensores
const int sens1 = A0;
const int sens2 = A1;
const int sens3 = A2;
const int sens4 = A3;

// Definición de los pines para los LEDs de los semáforos
const int R1 = 13;  // Rojo Semáforo 1
const int G1 = 12;  // Verde Semáforo 1
const int Y1 = 11;  // Amarillo Semáforo 1

const int R2 = 10;  // Rojo Semáforo 2
const int G2 = 9;   // Verde Semáforo 2
const int Y2 = 8;   // Amarillo Semáforo 2

const int R3 = 7;   // Rojo Semáforo 3
const int G3 = 6;   // Verde Semáforo 3
const int Y3 = 5;   // Amarillo Semáforo 3

const int R4 = 4;   // Rojo Semáforo 4
const int G4 = 3;   // Verde Semáforo 4
const int Y4 = 2;   // Amarillo Semáforo 4

// Tiempo inicial en verde. Podría ser configurado por un teclado numérico.
int tiempoVerde = 30;  // Valor por defecto
int tiempoAmarillo = 5;

// Configuración del teclado numérico
const byte ROWS = 4; // Cuatro filas
const byte COLS = 4; // Cuatro columnas
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; // Conectar a los pines de fila del Arduino
byte colPins[COLS] = {18, 19, 6, 7}; // Conectar a los pines de columna del Arduino
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// Función para manejar el estado de los semáforos
void estadoSemaforo(int semaforo, char color) {
  // Apaga todos los LEDs de todos los semáforos
  digitalWrite(R1, LOW); digitalWrite(G1, LOW); digitalWrite(Y1, LOW);
  digitalWrite(R2, LOW); digitalWrite(G2, LOW); digitalWrite(Y2, LOW);
  digitalWrite(R3, LOW); digitalWrite(G3, LOW); digitalWrite(Y3, LOW);
  digitalWrite(R4, LOW); digitalWrite(G4, LOW); digitalWrite(Y4, LOW);
  
  // Enciende el LED correspondiente al color y semáforo seleccionado
  int pinRojo, pinVerde, pinAmarillo;
  switch(semaforo) {
    case 1: pinRojo = R1; pinVerde = G1; pinAmarillo = Y1; break;
    case 2: pinRojo = R2; pinVerde = G2; pinAmarillo = Y2; break;
    case 3: pinRojo = R3; pinVerde = G3; pinAmarillo = Y3; break;
    case 4: pinRojo = R4; pinVerde = G4; pinAmarillo = Y4; break;
  }

  if(color == 'R') digitalWrite(pinRojo, HIGH);
  if(color == 'G') digitalWrite(pinVerde, HIGH);
  if(color == 'Y') digitalWrite(pinAmarillo, HIGH);
}

void setup() {
  // Inicializa los pines de los sensores
  pinMode(sens1, INPUT);
  pinMode(sens2, INPUT);
  pinMode(sens3, INPUT);
  pinMode(sens4, INPUT);
  
  // Inicializa los pines de los semáforos
  pinMode(R1, OUTPUT); pinMode(G1, OUTPUT); pinMode(Y1, OUTPUT);
  pinMode(R2, OUTPUT); pinMode(G2, OUTPUT); pinMode(Y2, OUTPUT);
  pinMode(R3, OUTPUT); pinMode(G3, OUTPUT); pinMode(Y3, OUTPUT);
  pinMode(R4, OUTPUT); pinMode(G4, OUTPUT); pinMode(Y4, OUTPUT);
  
  // Configura los semáforos en rojo
  estadoSemaforo(1, 'R');
  estadoSemaforo(2, 'R');
  estadoSemaforo(3, 'R');
  estadoSemaforo(4, 'R');

   Serial.begin(9600);
  while (!Serial); // Para Leonardo/Micro/Zero
  
  Serial.println("Ingrese el tiempo de verde para los semaforos y presione '#':");
  char key = 0;
  String input = "";
  while (key != '#') {
    key = keypad.getKey();
    if (key) {
      if (key == '*' || key == 'A' || key == 'B' || key == 'C' || key == 'D') {
        // Ignorar teclas no numéricas y '*'
        continue;
      }
      Serial.print(key);
      input += key;
    }
  }
  tiempoVerde = input.toInt();
  Serial.println("\nTiempo de verde establecido en: " + String(tiempoVerde) + " segundos");
  
}

// Variables para recordar el último semáforo con luz verde
int ultimoSemaforoConVerde = 0;

// Función para activar el semáforo seleccionado
void activarSemaforo(int semaforo) {
  // Apagar todos los semáforos primero
  apagarTodosLosSemaforos();

  // Activar el semáforo correspondiente
  estadoSemaforo(semaforo, 'G'); // Encender verde
  delay((tiempoVerde - 3) * 1000); // Espera el tiempo de verde menos los 3 segundos de intermitencia

  // Parpadeo verde antes de amarillo
  parpadeoVerde(semaforo);
  
  // Cambiar a amarillo
  estadoSemaforo(semaforo, 'Y');
  delay(tiempoAmarillo * 1000); // Espera el tiempo de amarillo

  // Cambiar a rojo
  estadoSemaforo(semaforo, 'R');
}

void apagarTodosLosSemaforos() {
   // Apagar LEDs de semáforo 1
  digitalWrite(R1, LOW);
  digitalWrite(G1, LOW);
  digitalWrite(Y1, LOW);

  // Apagar LEDs de semáforo 2
  digitalWrite(R2, LOW);
  digitalWrite(G2, LOW);
  digitalWrite(Y2, LOW);

  // Apagar LEDs de semáforo 3
  digitalWrite(R3, LOW);
  digitalWrite(G3, LOW);
  digitalWrite(Y3, LOW);

  // Apagar LEDs de semáforo 4
  digitalWrite(R4, LOW);
  digitalWrite(G4, LOW);
  digitalWrite(Y4, LOW);
}

// Variables globales para controlar el estado del parpadeo
bool estadoParpadeoVerde = LOW; // Estado inicial del LED
unsigned long ultimoCambioParpadeoVerde = 0; // Última vez que el LED cambió de estado
const long intervaloParpadeo = 500; // Intervalo entre parpadeos en milisegundos

void parpadeoVerde(int semaforo) {
 static unsigned long previousMillis = 0;
  const long interval = 500; // intervalo de parpadeo de 500 ms
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // guarda la última vez que se activó el parpadeo
    estadoParpadeoVerde = !estadoParpadeoVerde; // cambiar estado de parpadeo
    // Cambia el estado del LED verde basado en el semáforo y el estado actual del parpadeo
    switch(numSemaforo) {
      case 1: digitalWrite(G1, estadoParpadeoVerde); break;
      case 2: digitalWrite(G2, estadoParpadeoVerde); break;
      case 3: digitalWrite(G3, estadoParpadeoVerde); break;
      case 4: digitalWrite(G4, estadoParpadeoVerde); break;
    }
  }
}

// Duraciones en milisegundos
const unsigned long tiempoRojo = 45000; // 45 segundos
const unsigned long tiempoAmarillo = 5000; // 5 segundos
const unsigned long tiempoVerde = 30000; // 30 segundos por defecto
const unsigned long tiempoIntermitencia = 3000; // 3 segundos

// Variables de tiempo
unsigned long tiempoInicioCicloActual;
unsigned long tiempoInicioVerde;
unsigned long tiempoInicioAmarillo;
unsigned long tiempoInicioRojo;

// Estados de los semáforos
enum EstadoSemaforo {ROJO, VERDE_INTERMITENTE, VERDE, AMARILLO};
EstadoSemaforo estadoSemaforoActual = ROJO;


void loop() {

 unsigned long tiempoActual = millis();

  // Lee los sensores
  // Aquí, HIGH significa que un vehículo fue detectado
  bool vehicleAt1 = digitalRead(sens1) == HIGH;
  bool vehicleAt2 = digitalRead(sens2) == HIGH;
  bool vehicleAt3 = digitalRead(sens3) == HIGH;
  bool vehicleAt4 = digitalRead(sens4) == HIGH;

  // Comprobar la prioridad de los semáforos 1 y 3 primero
  if (vehicleAt1 && ultimoSemaforoConVerde != 1) {
    activarSemaforo(1);
    ultimoSemaforoConVerde = 1;
  } else if (vehicleAt3 && ultimoSemaforoConVerde != 3) {
    activarSemaforo(3);
    ultimoSemaforoConVerde = 3;
  } 
  // Si no hay vehículos en 1 y 3, comprobar 2 y 4
  else {
    if (vehicleAt2 && ultimoSemaforoConVerde != 2) {
      activarSemaforo(2);
      ultimoSemaforoConVerde = 2;
    } else if (vehicleAt4 && ultimoSemaforoConVerde != 4) {
      activarSemaforo(4);
      ultimoSemaforoConVerde = 4;
    }
  }
  
  // Asumamos que simplemente vamos a alternar los semáforos en secuencia: 1, 2, 3, y 4.
  for(int semaforo = 1; semaforo <= 4; semaforo++) {
    // Primero todos en rojo
    estadoSemaforo(semaforo, 'R');
  }
  delay(45000); // Tiempo en rojo
  
  // Luego, para cada semáforo:
  for(int semaforo = 1; semaforo <= 4; semaforo++) {
    // Verifica si hay un vehículo esperando
    if((semaforo == 1 && vehicleAt1) || 
       (semaforo == 2 && vehicleAt2) ||
       (semaforo == 3 && vehicleAt3) ||
       (semaforo == 4 && vehicleAt4)) {
      
      estadoSemaforo(semaforo, 'G'); // Semáforo en verde
      delay((tiempoVerde - 3) * 1000); // Espera el tiempo de verde menos los 3 segundos de intermitencia
      
      // Intermitencia del verde
      for(int i = 0; i < 6; i++) {
        digitalWrite(G1, (i % 2 == 0) ? HIGH : LOW);
        delay(500);
      }
    
      estadoSemaforo(semaforo, 'Y'); // Semáforo en amarillo
      delay(tiempoAmarillo * 1000); // Espera el tiempo de amarillo
    }
  }
  switch(estadoSemaforoActual) {
    case ROJO:
      if (tiempoActual - tiempoInicioCicloActual >= tiempoRojo) {
        cambiarSemaforosA(VERDE);
        tiempoInicioVerde = tiempoActual;
        estadoSemaforoActual = VERDE;
      }
      break;
      
    case VERDE:
      if (tiempoActual - tiempoInicioVerde >= tiempoVerde - tiempoIntermitencia) {
        estadoSemaforoActual = INTERMITENTE;
      }
      break;

    case INTERMITENTE:
      if (tiempoActual - tiempoInicioVerde >= tiempoVerde) {
        cambiarSemaforosA(AMARILLO);
        tiempoInicioAmarillo = tiempoActual;
        estadoSemaforoActual = AMARILLO;
      } else {
        // Parpadeo del semáforo verde
        if ((tiempoActual - tiempoInicioVerde) % 500 < 250) {
          // Enciende los LEDs verdes
        } else {
          // Apaga los LEDs verdes
        }
      }
      break;

    case AMARILLO:
      if (tiempoActual - tiempoInicioAmarillo >= tiempoAmarillo) {
        cambiarSemaforosA(ROJO);
        tiempoInicioCicloActual = tiempoActual;
        estadoSemaforoActual = ROJO;
      }
      break;
  }
}