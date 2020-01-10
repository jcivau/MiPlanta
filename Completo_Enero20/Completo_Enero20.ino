//Display LCD
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x3F

LiquidCrystal_I2C             lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);

// ----- Temperaura y humedad AIRE -----
#include "DHT.h" // Para sensor de temperatura y humedad
#define DHTTYPE DHT22
 
const int DHTPin = 5;     // what digital pin we're connected to
DHT dht(DHTPin, DHTTYPE);

// ----- Humedad SUELO -----
int val = 0; // valor para almacenar el valor de humedad en la función
int soilPin = A0; // Declara una variable para el sensor de humedad del suelo
int soilPower = 7; // Variable para la humedad del suelo
int val_humedad; // Almaceno el valor de la humedad

// Ultrasonido
const int pinecho = 11;
const int pintrigger = 12;
unsigned int tiempo, distancia;

// Varios
int errores = 0;
int led = 13; 
int bomba = 8; //Conectada al relay, configuracion al contrarias a led común.
int ledAzul = 4;
int i = 0;
int riegos = 0;
int lecturas = 0;

void setup()
   {
      Serial.begin(9600);
      
      //Display LCD
      lcd.begin (16,2);    // Inicializar el display con 16 caraceres 2 lineas
      lcd.setBacklightPin(3,POSITIVE);
      lcd.setBacklight(HIGH);
     
      // DTH22 Humedad y Temperatura Digital
      dht.begin();

      //Humedad Suelo
      pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
      digitalWrite(soilPower, LOW);//Establezca en LOW para que no pase corriente por el sensor

      //Ultrasonido
      pinMode(pinecho, INPUT);
      pinMode(pintrigger, OUTPUT);

      //Varios
      pinMode(led, OUTPUT); 
      pinMode(bomba, OUTPUT);
      digitalWrite(bomba, HIGH);//Apago la bomba, al reves por el relay
      pinMode(ledAzul, OUTPUT);

      //Impresion display
      lcd.home (); // go home
      lcd.print("Iniciando");
      lcd.setCursor ( 0, 1 ); // go to the 2nd line
      lcd.print("Prototipo");
      
      delay(2000);//Delay para inicializar sistema
   }
void loop()
   {  
     //Inicio sistema
     digitalWrite(ledAzul, HIGH);
     
     // Reading temperature or humidity takes about 250 milliseconds!
     float h = dht.readHumidity();
     float t = dht.readTemperature();
   
     if (isnan(h) || isnan(t)) {
        Serial.println("Error de DTH detectado, esperamos 5 segundos y volvemos!");
        errores = errores + 1;
        delay(5000);
        return;
        }

      //Humedad Suelo
      val_humedad = readSoil();
      int val_humedad_porcentaje = map(val_humedad,0,911,0,100);

      //Ultrasonido
      digitalWrite(pintrigger, LOW);
      delayMicroseconds(2);
      digitalWrite(pintrigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(pintrigger, LOW);
      tiempo = pulseIn(pinecho, HIGH);
      distancia = tiempo / 58;
      
      //Impresion Consola
      Serial.print("Distancia: ");
      Serial.println(distancia);
      Serial.print("Humedad Suelo: ");
      Serial.println(val_humedad);
      Serial.print("Humedad Suelo: ");
      Serial.print(val_humedad_porcentaje);
      Serial.println(" %");
      Serial.print("Humedad Aire: ");
      Serial.print(h);
      Serial.println(" %");
      Serial.print("Temperatura Aire: ");
      Serial.print(t);
      Serial.println(" °C ");
      Serial.print("Errores ");
      Serial.println(errores);
      Serial.println("------------");

      //Impresion display
      lcd.clear();
      lcd.home ();                   // go home
      lcd.print("H ");
      lcd.print(val_humedad);
      lcd.print(" ");
      lcd.print("%H ");
      lcd.print(val_humedad_porcentaje);
      lcd.print(" ");
      lcd.print("D ");
      lcd.print(distancia);
      
      lcd.setCursor ( 0, 1 );        // go to the 2nd line
      lcd.print("E ");
      lcd.print(errores);
      lcd.print(" ");
      lcd.print("R ");
      lcd.print(riegos);
      lcd.print(" ");
      lcd.print("L ");
      lcd.print(lecturas);

      //Maximo de distancia sin agua 18cm
      if  (distancia > 14) // 9 cm = 1000 mm Agua
      {
        digitalWrite(led, HIGH);
        Serial.println("Enciendo Led AGUA");
        
        //Impresion display
        lcd.clear();
        lcd.home ();                   // go home
        lcd.print("AYUDA!");
        lcd.setCursor ( 0, 1 );        // go to the 2nd line
        lcd.print("REVISAR AGUA");
        }else 
          {
            digitalWrite(led, LOW);
            }

     if (val_humedad_porcentaje < 96)
     {
        digitalWrite(bomba, LOW); //Al reves por el tema del relay
        Serial.println("Enciendo bomba");
                
        //Impresion display
        lcd.clear();
        lcd.home ();                   // go home
        lcd.print("Estoy");
        lcd.setCursor ( 0, 1 ); // go to the 2nd line
        lcd.print("Regando!");
        
        riegos = riegos + 1;
        
        for (i=0 ; i<3; i++)
          {
        Serial.println(i, " segundos");
        delay(i+"000");
        }

        Serial.println("-------------");
        digitalWrite(bomba, HIGH); //Al reves por el tema del relay

        //Impresion display
        lcd.clear();
        lcd.home ();                   // go home
        lcd.print("Termine de");
        lcd.setCursor ( 0, 1 ); // go to the 2nd line
        lcd.print("Regar! ;)");
      }
    

     // Wait a few seconds between measurements.
     for (i=0 ; i<=600; i++)
      {
        Serial.println(i, " segundos");
        delay(i+"000");

        lcd.setCursor ( 13 , 1 ); // go to the 2nd line
        lcd.print(i);
        }
      // Cuento lecturas
      lecturas = lecturas +1;
    }

    int readSoil()
      {
        digitalWrite(soilPower, HIGH);//turn D7 "On" Enciende el sensor
        delay(10);//wait 10 milliseconds - Espera 10 segundos
        val = analogRead(soilPin);//Read the SIG value form sensor - Lee el valor
        digitalWrite(soilPower, LOW);//turn D7 "Off" - Apaga el sensor
        return val;//send current moisture value - Devuelve el valor
      }

