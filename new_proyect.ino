#include <LiquidCrystal_I2C.h>
#include<Wire.h>
#include<Keypad.h>
#include<EEPROM.h>
#include<Servo.h>
Servo motor;
LiquidCrystal_I2C lcd(0x27, 16, 2);
//Keypad
const byte Fila = 4;
const byte Cols = 4;
byte pinFila[Fila] = {9,8,7,6};
byte pinCols[Cols] = {5,4,3,2};
char teclas[Fila][Cols] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
Keypad teclado = Keypad(makeKeymap(teclas),pinFila,pinCols,Fila,Cols);
//variables
int index =4;
String pwObten;
String DatapwTemp;
int Default[6]={1,2,3,4,5,6};
int DataTemp =0;
const byte beep = 10;
int dataObtenDF;
byte check=0;
int statusLogin=0;
int alerta=0;
bool pinFault(void)
{
  if(EEPROM.read(0)==255)
  {
   
    
    for(int i=0;i<6;i++)
    {
      EEPROM.put(i,Default[i]);
      delay(3.3);
    }
    
    for(int i=0;i<6;i++)
    {
       dataObtenDF = EEPROM.read(i);
      if(Default[i]==dataObtenDF)
      {
         check++;
      }else{check=0;}
      delay(3.3);
    }
    if(check==6)
    {
      return true;
    }else{return false;}
  }
}
void homeS()
{
  //int statusDoor1 = digitalRead(11);
  if(/*statusDoor1!=0&&*/statusLogin==0){ 
    lcd.clear();
    lcd.home();
    lcd.print("PIN>");
    motor.write(0);
  }  
  
}
bool veriFy(String data)
{
  if(data.length()==6)
  {
    check=0;
    for(int i=0;i<6;i++)
    {
      DatapwTemp = data[i];
      DataTemp = DatapwTemp.toInt();
      if(DataTemp==EEPROM.read(i)){check++;}else{check=0;break;}
    }
    if(check==6){statusLogin=1; return true;}else{return false;}
  }
}
void setup()
{
  Serial.begin(9600);  
   lcd.init();
   lcd.backlight();
   lcd.home();
   lcd.print("Iniciando...");
  if(pinFault()==true||pinFault()==false)
  {
    lcd.clear();
    lcd.home();
    lcd.print("Completo...");
    delay(500);
    homeS();
  }
    pinMode(beep,OUTPUT);
    motor.attach(12);
   
}
void loop()
{
  char tecla = teclado.getKey();
  int statusDoor = digitalRead(11);
  Serial.println(statusDoor);
  if(tecla!=NO_KEY&&tecla=='C'&&tecla!='*')
  {
     homeS();
  }
  if(statusDoor==0&&statusLogin==0)
  {
    
    lcd.clear();
    lcd.print("ABIERTO MANUALMENTE");
    alerta=1;
    delay(500);
  }
  if(statusDoor==1&&statusLogin==0&&alerta==1)
  {
     alerta=0;
    delay(500);
     homeS();
  }
  
  if(tecla!=NO_KEY&&tecla!='*'&&tecla!='C'&&statusDoor==1)
  {     
    lcd.setCursor(index,0);
    lcd.print(tecla);
    index++;
    tone(beep,400);
    delay(5);
    index--;
    lcd.setCursor(index,0);
    noTone(beep);       
    lcd.print("*");
    index++;  
    pwObten+=tecla;
  }
  if(pwObten.length()==6)
  {
    if(veriFy(pwObten))
    {
      lcd.clear();
      lcd.print("Abrir Puerta");//abrirPuerta
      motor.write(90);
      delay(3000);
      statusDoor=digitalRead(11);
      if(statusDoor==0&&statusLogin==1)
      {
         lcd.clear();
         lcd.home();
         lcd.print("ESPERANDO PUERTA");  
         
      }
     if(statusDoor==1&&statusLogin==1)
      {
         lcd.clear();
         lcd.print("CERRADO");
         statusLogin=0;
         index=4;
         delay(500);
         pwObten="";
         homeS();
      }
      
   
    }else{/*contrasena incorrecta*/}
    
  }
  
 }
  
