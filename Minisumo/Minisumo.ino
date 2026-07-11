// =================================================================
// 1. DECLARACIÓN DE PINES (Sin cambios)
// =================================================================
// ... (Toda la sección de pines se mantiene igual)
// Sensores de línea Infrarrojos (QTR)
const int IR_FRENTE_IZQ_PIN = A7;
const int IR_TRASERO_PIN = A6;
const int IR_FRENTE_DER_PIN = A5;
int pines[] = {IR_FRENTE_IZQ_PIN, IR_TRASERO_PIN, IR_FRENTE_DER_PIN};
int sensores[3];
int digital[3];
int lectura_fondo[3];
int lectura_linea[3];
int umbral[3];
// Sensores de Oponente Ultrasónicos
const int ULTRA_IZQ_TRIGGER_PIN = 17;
const int ULTRA_IZQ_ECHO_PIN = 18;
const int ULTRA_FRENTE_TRIGGER_PIN = 15;
const int ULTRA_FRENTE_ECHO_PIN = 16;
const int ULTRA_DER_TRIGGER_PIN = 13;
const int ULTRA_DER_ECHO_PIN = 14;
// Motores
const int MOTOR_IZQ_PWM_PIN = 3;
const int MOTOR_IZQ_IN1_PIN = 5;
const int MOTOR_IZQ_IN2_PIN = 4;
const int MOTOR_DER_PWM_PIN = 11;
const int MOTOR_DER_IN1_PIN = 7;
const int MOTOR_DER_IN2_PIN = 6;
// Botón de Inicio
const int BOTON_PIN = 9;
int tiempogiro2=0;
#define LED_ON  12
#define LED     13

// =================================================================
// 2. VARIABLES GLOBALES Y PARÁMETROS DE ESTRATEGIA
// =================================================================

long distanciaFrente, distanciaIzquierda, distanciaDerecha;

// --- ¡¡PARÁMETROS CLAVE DE LA ESTRATEGIA!! ---
// Zona de Ataque: Si el oponente está más cerca que esto, ¡ataque total!
int distanciaAtaque = 20; // en cm. Aumenta si quieres que ataque desde más lejos.

// Zona de Detección: Si el oponente está dentro de este rango, lo perseguimos.
// Debe ser mayor que el radio del dohyo (35 cm) para cubrir todo el ring.
int distanciaDeteccion = 50; // en cm.

// Velocidad de los motores (0 = parado, 255 = máxima velocidad)
int velocidadAtaque = 255;   // Máxima potencia para empujar.
int velocidadBusqueda = 180; // Velocidad para buscar y aproximarse.

// --- ¡NUEVO! Variable para recordar la última posición del oponente ---
const int NINGUNA = 0;
const int IZQUIERDA = 1;
const int DERECHA = 2;
const int FRENTE = 3;
int ultimaDireccionVista = NINGUNA; // Inicialmente, no hemos visto a nadie

const int FRENTE_IZQUIERDA = 1;
const int TRASERO = 2;
const int FRENTE_DERECHA = 3;
const int FRENTE_COMPLETO = 4;
int ultimaLineaNegraVista = NINGUNA;

int direccionGiroBusqueda = IZQUIERDA;

bool girando = false; 

// =================================================================
// 3. FUNCIÓN SETUP (Sin cambios)
// =================================================================

void fondos() {
  for (int i = 0; i < 3; i++) {
    long suma = 0;
    for (int j = 0; j < 50; j++) {
      suma += analogRead(pines[i]);
      digitalWrite(LED,0); 
      delay(20); 
      digitalWrite(LED,1); 
      delay(20);
    }
    lectura_fondo[i] = suma / 50; // promedio de las 50 lecturas
    Serial.print("Fondo sensor "); Serial.print(i);
    Serial.print(": "); Serial.println(lectura_fondo[i]);
  }
}

void lineas() {
  for (int i = 0; i < 3; i++) {
    long suma = 0;
    for (int j = 0; j < 50; j++) {
      suma += analogRead(pines[i]);
      digitalWrite(LED,0); 
      delay(20); 
      digitalWrite(LED,1); 
      delay(20);
    }
    lectura_linea[i] = suma / 50;
    Serial.print("Línea sensor "); Serial.print(i);
    Serial.print(": "); Serial.println(lectura_linea[i]);
  }
}

void promedio() {
  for (int i = 0; i < 3; i++) {
    umbral[i] = (lectura_fondo[i] + lectura_linea[i]) / 2;
    Serial.print("Umbral sensor "); Serial.print(i);
    Serial.print(": "); Serial.println(umbral[i]);
    digitalWrite(LED,0); 
    delay(20); 
    digitalWrite(LED,1); 
    delay(20);
  }
}


void setup() {
  Serial.begin(9600);
  
  pinMode(MOTOR_IZQ_PWM_PIN, OUTPUT);
  pinMode(MOTOR_IZQ_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IZQ_IN2_PIN, OUTPUT);
  pinMode(MOTOR_DER_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DER_IN1_PIN, OUTPUT);
  pinMode(MOTOR_DER_IN2_PIN, OUTPUT);
  
  pinMode(ULTRA_IZQ_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRA_IZQ_ECHO_PIN, INPUT);
  pinMode(ULTRA_FRENTE_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRA_FRENTE_ECHO_PIN, INPUT);
  pinMode(ULTRA_DER_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRA_DER_ECHO_PIN, INPUT);
  
  pinMode(BOTON_PIN, INPUT_PULLUP);
  
  Serial.println("MODO PRUEBA DE ATAQUE (Dohyo 70cm). Presiona el boton.");

  for (int i = 0; i < 3; i++) {pinMode(pines[i], INPUT);}
  pinMode(LED,OUTPUT);
  pinMode(LED_ON,OUTPUT);
  digitalWrite(LED_ON,1);
  digitalWrite(LED,1);
  delay(500);
  while(digitalRead(BOTON_PIN));
  fondos();
  while(digitalRead(BOTON_PIN));
  lineas();
  promedio();

  while (digitalRead(BOTON_PIN) == HIGH) {
    delay(50);
  }
  
  Serial.println("Boton presionado. Iniciando en 5 segundos...");
  delay(5000);
  Serial.println("¡A PROBAR!");
}

void estrategiaLinea() {
  static unsigned long inicioManiobra = 0;
  static bool enManiobra = false;

  // Iniciar maniobra solo la primera vez que se detecta línea
  if (!enManiobra) {
    enManiobra = true;
    inicioManiobra = millis();
  }

  unsigned long tiempo = millis() - inicioManiobra;
Serial.println(ultimaLineaNegraVista);
  // --- Limitar duración total de maniobra (1.6 segundos) ---
  if (tiempo > 1600) {
    parar();
    girando = false;
    enManiobra = false;
    ultimaLineaNegraVista = NINGUNA;
    return;
  }

  // --- CASO 1: línea trasera detectada ---
  if (ultimaLineaNegraVista == TRASERO) {
    Serial.println("Adentro");
    if (tiempo < 1500) {
      // 600–1500 ms → avanza fuerte para volver al centro
      adelante(255);
    } 
    else {
      parar();
      enManiobra = false;
      girando = false;
      ultimaLineaNegraVista = NINGUNA;
    }
  }

  // --- CASO 2: línea al frente derecha ---
  else if (ultimaLineaNegraVista == FRENTE_DERECHA) {
    atras(255);
    direccionGiroBusqueda = IZQUIERDA;
    if (tiempo > 800) {
      parar();
      enManiobra = false;
      girando = false;
      ultimaLineaNegraVista = NINGUNA;
    }
  }

  // --- CASO 3: línea al frente izquierda o ambos sensores frontales ---
  else if (ultimaLineaNegraVista == FRENTE_IZQUIERDA || ultimaLineaNegraVista == FRENTE_COMPLETO) {
    atras(255);
    direccionGiroBusqueda = DERECHA;
    if (tiempo > 800) {
      parar();
      enManiobra = false;
      girando = false;
      ultimaLineaNegraVista = NINGUNA;
    }
  }

  // --- CASO 4: sin línea detectada ---
  else {
    enManiobra = false;
    girando = false;
  }
}

/*
void estrategiaLinea() {
  tiempogiro2++;
  Serial.println(tiempogiro2);
  if(tiempogiro2 < 160 && !(distanciaFrente < distanciaAtaque)){
    girando = true;

    if(ultimaLineaNegraVista == TRASERO) {
      adelante(255);
      if(tiempogiro2 > 80){
        tiempogiro2 = 500;
      }
    }
    else if(tiempogiro2 < 80){
      
      Serial.print("tiempo giro 2: ");
      Serial.println(tiempogiro2);
      atras(255);
    }
    else if(ultimaLineaNegraVista == FRENTE_DERECHA) {
      girarIzquierda(255);
      direccionGiroBusqueda = IZQUIERDA;
    }
    // (ultimaLineaNegraVista == FRENTE_IZQUIERDA o FRENTE_COMPLETO)
    else{
      girarDerecha(255);
      direccionGiroBusqueda = DERECHA;
    }
  }else{
    tiempogiro2=0;
    girando = false;
    ultimaLineaNegraVista = NINGUNA;
  }
}*/

bool detectarLineaLimite() {
  for (int i = 0; i < 3; i++) {
    sensores[i]=analogRead(pines[i]);
    if(sensores[i]<=umbral[i]){digital[i]=0;}else{digital[i]=1;};
  }
  if (digital[2] && digital[0]) {
    ultimaLineaNegraVista = FRENTE_COMPLETO;
  }
  else if (digital[2]) {
    ultimaLineaNegraVista = FRENTE_DERECHA;
  }
  else if (digital[1]) {
    ultimaLineaNegraVista = TRASERO;
  }
  else if (digital[0]) {
    ultimaLineaNegraVista = FRENTE_IZQUIERDA;
  }
  else {
    ultimaLineaNegraVista = NINGUNA;
  }
  return digital[0] || digital[1] || digital[2];
}


// =================================================================
// 4. FUNCIÓN LOOP (Sin cambios)
// =================================================================

void loop() {
  bool limite = detectarLineaLimite(); // devuelve true si detectó línea negra
  leerSensores();  // ultrasonidos
  if (!limite && !girando) {
    estrategia();
  } else {
    estrategiaLinea(); 
  }
}

// =================================================================
// 5. LÓGICA Y ESTRATEGIA (CON MEMORIA DE OPONENTE)
// =================================================================

void estrategia() {
  // --- PRIORIDAD 1: ZONA DE ATAQUE ---
  if (distanciaFrente < distanciaAtaque) {
    Serial.println("Oponente en zona de ataque. ¡EMPUJAR!");
    ultimaDireccionVista = FRENTE;
    adelante(velocidadAtaque);
    // Cuando atacamos de frente, no necesitamos actualizar la memoria de búsqueda.
  }
  // --- PRIORIDAD 2: ZONA DE DETECCIÓN ---
  else if (distanciaFrente < distanciaDeteccion) {
    Serial.println("Oponente al frente. Aproximando...");
    ultimaDireccionVista = FRENTE;
    adelante(velocidadBusqueda);
  }
  else if (distanciaIzquierda < distanciaDerecha && distanciaIzquierda < distanciaDeteccion) {
    Serial.println("Oponente detectado a la IZQUIERDA. Girando...");
    ultimaDireccionVista = IZQUIERDA; // ¡Guardamos que lo vimos a la izquierda!
    girarIzquierda(velocidadBusqueda);
  }
  else if (distanciaDerecha < distanciaIzquierda && distanciaDerecha < distanciaDeteccion) {
    Serial.println("Oponente detectado a la DERECHA. Girando...");
    ultimaDireccionVista = DERECHA; // ¡Guardamos que lo vimos a la derecha!
    girarDerecha(velocidadBusqueda);
  }
  // --- PRIORIDAD 3: ESTRATEGIA DE BÚSQUEDA CON MEMORIA ---
  else {
    // Si no vemos a nadie, usamos nuestra memoria.
    Serial.print("Oponente perdido. Buscando en la última dirección vista: ");
    
    if (ultimaDireccionVista == DERECHA) {
      Serial.println("DERECHA");
      girarDerecha(velocidadBusqueda); // Seguir girando a la derecha.
    }
    else if (ultimaDireccionVista == IZQUIERDA) {
      Serial.println("IZQUIERDA");
      girarIzquierda(velocidadBusqueda); // Seguir girando a la izquierda.
    }
    else { // Esto solo ocurre al principio del combate (ultimaDireccionVista == NINGUNA)
      Serial.println("INICIANDO BÚSQUEDA (por defecto a la derecha)");
      if(direccionGiroBusqueda == DERECHA) {
        girarDerecha(velocidadBusqueda);
      } else {
        girarIzquierda(velocidadBusqueda);
      }
    }
  }
}

// =================================================================
// 6. FUNCIONES PARA LEER SENSORES (Sin cambios)
// =================================================================
// ... (La función leerSensores y leerUltrasonico se mantienen igual)
void leerSensores() {
  distanciaFrente = leerUltrasonico(ULTRA_FRENTE_TRIGGER_PIN, ULTRA_FRENTE_ECHO_PIN);
  distanciaIzquierda = leerUltrasonico(ULTRA_IZQ_TRIGGER_PIN, ULTRA_IZQ_ECHO_PIN);
  distanciaDerecha = leerUltrasonico(ULTRA_DER_TRIGGER_PIN, ULTRA_DER_ECHO_PIN);
}

long leerUltrasonico(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duracion = pulseIn(echoPin, HIGH);
  return duracion / 58;
}


// =================================================================
// 7. FUNCIONES DE CONTROL DE MOTORES (CON NUEVA FUNCIÓN DE BÚSQUEDA)
// =================================================================

void adelante(int velocidad) {
  digitalWrite(MOTOR_IZQ_IN1_PIN, HIGH);
  digitalWrite(MOTOR_IZQ_IN2_PIN, LOW);
  analogWrite(MOTOR_IZQ_PWM_PIN, velocidad);
  digitalWrite(MOTOR_DER_IN1_PIN, HIGH);
  digitalWrite(MOTOR_DER_IN2_PIN, LOW);
  analogWrite(MOTOR_DER_PWM_PIN, velocidad);
}

void atras(int velocidad) {
  digitalWrite(MOTOR_IZQ_IN1_PIN, LOW);
  digitalWrite(MOTOR_IZQ_IN2_PIN, HIGH);
  analogWrite(MOTOR_IZQ_PWM_PIN, velocidad);
  digitalWrite(MOTOR_DER_IN1_PIN, LOW);
  digitalWrite(MOTOR_DER_IN2_PIN, HIGH);
  analogWrite(MOTOR_DER_PWM_PIN, velocidad);
}

void girarDerecha(int velocidad) {
  digitalWrite(MOTOR_IZQ_IN1_PIN, LOW);
  digitalWrite(MOTOR_IZQ_IN2_PIN, HIGH);
  analogWrite(MOTOR_IZQ_PWM_PIN, velocidad);
  digitalWrite(MOTOR_DER_IN1_PIN, HIGH);
  digitalWrite(MOTOR_DER_IN2_PIN, LOW);
  analogWrite(MOTOR_DER_PWM_PIN, velocidad);
  
}

void girarIzquierda(int velocidad) {
  
  digitalWrite(MOTOR_IZQ_IN1_PIN, HIGH);
  digitalWrite(MOTOR_IZQ_IN2_PIN, LOW);
  analogWrite(MOTOR_IZQ_PWM_PIN, velocidad);
  digitalWrite(MOTOR_DER_IN1_PIN, LOW);
  digitalWrite(MOTOR_DER_IN2_PIN, HIGH);
  analogWrite(MOTOR_DER_PWM_PIN, velocidad);
}

void parar() {
  analogWrite(MOTOR_IZQ_PWM_PIN, 0);
  analogWrite(MOTOR_DER_PWM_PIN, 0);
}

