//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include <FirebaseESP8266.h>  // Permite a tu ESP8266 comunicarse con Firebase
#include <ESP8266WiFi.h>      // Proporciona métodos para conectarte a una red WiFi
#include <SoftwareSerial.h>   // Permite la comunicación serie en otros pines digitales
#include <TinyGPS++.h>        // Biblioteca GPS personalizada para Arduino

//Definición de constantes de conexión para Firebase y WiFi
#define FIREBASE_HOST "proyecto-gps-d4cb3-default-rtdb.firebaseio.com" // Dirección de la base de datos Firebase
#define FIREBASE_AUTH "0bloeGMvsckqPhjzWebl0ZDkthk7oejwiFNBsuM35BrmKW" // Clave de autenticación de Firebase
#define WIFI_SSID "Net" // Nombre de la red WiFi a la que se conectará
#define WIFI_PASSWORD "ldub3365" // Contraseña de la red WiFi a la que se conectará
//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
//Define FirebaseESP8266 data object
FirebaseData firebaseData; // Almacenará datos y resultados devueltos por el servidor Firebase
FirebaseJson json; // Para manipular datos en formato JSON
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Definir los pines de conexión para el módulo GPS
const int RXPin = 4, TXPin = 5; // RX y TX para la comunicación con el módulo GPS
SoftwareSerial neo6m(RXPin, TXPin); // SoftwareSerial para el módulo GPS
TinyGPSPlus gps; // Objeto para decodificar los datos del GPS
//-----------------------------------------------------------------------------------


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Configuración inicial del sistema
void setup()
{

  Serial.begin(115200); // Inicia la comunicación serie a 115200 baudios

  neo6m.begin(9600);  // Inicia la comunicación serie con el módulo GPS a 9600 baudios
  
  wifiConnect();  // Llama a la función para conectar al WiFi

  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Inicia la conexión con Firebase
  Firebase.reconnectWiFi(true); // Se asegura de mantener la conexión WiFi
  Serial.println("Firebase OK.");

}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// Bucle principal de ejecución
void loop() {
  
  smartdelay_gps(1000); // Se esperan 1000 milisegundos y se aprovecha inteligentemente ese tiempo para llamar a la función que lee y procesa los datos del GPS


  if(gps.location.isValid())  // Si los datos del GPS son válidos
  {
    float latitude = gps.location.lat(); // Obtiene la latitud
    float longitude = gps.location.lng(); // Obtiene la longitud
    
    //-------------------------------------------------------------
    //Send to Serial Monitor for Debugging
    //Serial.print("LAT:  ");
    //Serial.println(latitude);  // float to x decimal places
    //Serial.print("LONG: ");
    //Serial.println(longitude);
    //-------------------------------------------------------------
    
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude)) // Envía la latitud a Firebase como un valor flotante
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude)) // Envía la longitud a Firebase como un valor flotante
      {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
  }
  else
  {
    Serial.println("No valid GPS data found."); // Si los datos del GPS no son válidos, imprime un mensaje de error
  }
  
  delay(5000); // Espera 5 segundos antes de la próxima iteración
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Función para procesar los datos del GPS
static void smartdelay_gps(unsigned long ms) // Recibe como argumento el número de ms a esperar
{
  unsigned long start = millis(); // Asigna el tiempo actual en milisegundos desde que el programa de Arduino comenzó a ejecutarse.
  do 
  {
    while (neo6m.available()) // Este bucle se ejecutará mientras haya datos disponibles para leer desde el módulo GPS.
      gps.encode(neo6m.read()); // Si hay datos disponibles para leer desde el módulo GPS, lee los datos y los pasa a la función "encode()" de la biblioteca TinyGPS++. Esta función procesa los datos del GPS.
  } while (millis() - start < ms); // Este bucle continuará ejecutándose hasta que la diferencia entre el tiempo actual y el valor almacenado en "start" sea mayor o igual que "ms".
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Función para conectar al WiFi
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Se inicia el intento de conexión a la red Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) // Mientras no haya conexión se imprimen puntitos.
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: "); // Cuando se logra la conexión se muestra la IP.
  Serial.println(WiFi.localIP());
  Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//Funciones para imprimir mensajes de éxito o error
void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//Función para reconectar a Firebase si la conexión se pierde
void firebaseReconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
