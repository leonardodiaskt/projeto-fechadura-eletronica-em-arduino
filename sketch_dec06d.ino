// Inclusão da biblioteca do módulo teclado
#include <Keypad.h>
#include <Password.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

/************************ DEFINIÇÃO DE SENHA ******************************* */

#define SS_PIN 10
#define RST_PIN 9

/** Objetos RFID **/

MFRC522 mfrc522(SS_PIN, RST_PIN);

void rfid_func();

Servo servo;

char senha[ ]= {'2','1','1','0','0','4','\0'};  // defina a senha aqui, ela deve conter seis dígitos
char palavra[ ] = {'0','0','0','0','0','0','\0'};

/************************ CONFIGURAÇÕES TECLADO ******************************* */

const byte LINHAS = 4; // quantidade de linhas do teclado
const byte COLUNAS = 4; // quantidade de colunas do teclado

char digitos[LINHAS][COLUNAS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}}; // todos os caracteres do teclado

byte pinosLinha[LINHAS] = { 17, 16, 15, 8 }; // pinos de conexão das linhas no arduino 
byte pinosColuna[COLUNAS] = { 7, 6, 5, 4 }; // pinos de conexão das colunas no arduino 

Keypad keypad = Keypad( makeKeymap(digitos), pinosLinha, pinosColuna, LINHAS, COLUNAS ); // objeto de leitura das teclas com os parâmetros lidos

/************************ VARIÁVEIS AUXILIARES ******************************* */

//int position = 0;
int ledVermelho = 19;
int ledVerde = 18;
int ledAzul = 3;
const int buzzer = 2;
int contador = 0;
int tempoBuzz = 20;
//int tempoAberto = 3000;

/***************************************************************************** */

void setup()
{ 
 servo.attach(14); // Pino de entrada
 Serial.begin(9600);
 pinMode(buzzer,OUTPUT); // define os pinos de saída do arduino
 pinMode(ledVermelho, OUTPUT);
 pinMode(ledVerde, OUTPUT);
 pinMode(ledAzul, OUTPUT);
 
 digitalWrite(buzzer, LOW);
 SPI.begin();
 mfrc522.PCD_Init();

 Serial.println("Fechadura Eletronica da 1DT");
 Serial.println("Feito pelos alunos: Amanda Ribeiro, Felipe Sousa");
 Serial.println("Gabriel Felix, Kaike Carmona, Leonardo Dias");
 Serial.println();
 
 servo.write(0);

}

void loop()
{

  rfid_func();

  Serial.begin(9600);
  char digito = keypad.getKey(); // faz a leitura das teclas

  if(digito){
    Serial.println("Digite a senha: ");
    Serial.println(digito);
    tone(buzzer, 2500);
    delay(10);
    noTone(buzzer);
    
    delay(100);
    palavra[contador] = digito;
    contador++;
    if(contador==6){
      palavra[contador] = '\0';
      if((palavra[0] == senha[0]) &&(palavra[1] == senha[1]) && (palavra[2] == senha[2]) && (palavra[3] == senha[3])&& (palavra[4] == senha[4])&& (palavra[5] == senha[5]) && (palavra[6] == senha[6])){
        Serial.println("Senha Correta!");
        delay(500);
        tone(buzzer, 2500);
        delay(100);
        tone(buzzer, 900);
        delay(100);
        noTone(buzzer);

        Serial.println("Porta Aberta");
        servo.write(180);
        digitalWrite(ledAzul, LOW);
        digitalWrite(ledVerde, HIGH);
        digitalWrite(ledVermelho, LOW);
        delay(10000);
        digitalWrite(ledVermelho, HIGH);
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAzul, LOW);

        Serial.println("Trave a Porta");
        tone(buzzer, 300);
        delay(500);
        noTone(buzzer);
        delay(500);
        digitalWrite(ledVermelho, HIGH);
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAzul, LOW);
        servo.write(0);
      }
      else{
        Serial.println("Senha Incorreta!");
        tone(buzzer, 300);
        delay(500);
        noTone(buzzer);
        delay(500);
        tone(buzzer, 300);
        delay(100);
        noTone(buzzer);
        delay(500);
        delay(500);
        digitalWrite(ledAzul, HIGH);
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledVermelho, LOW);
        contador = 0;
      }
    }
  }
 
}

void rfid_func(){
  if(!mfrc522.PICC_IsNewCardPresent()) return; //Verifica novas tags

  if(!mfrc522.PICC_ReadCardSerial()) return; // Seleciona uma das tags

  Serial.println("UID da tag: ");
  String conteudo = "";
  byte letra;
  for(byte i = 0; i < mfrc522.uid.size; i++){
    //Serial.println("Teste");
    //Serial.println(mfrc522.uid.uidByte[i] < 0x10? " 0" : " ");
    //Serial.println(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println("Mensagem: ");
  conteudo.toUpperCase();
  Serial.println(conteudo);

  //tag para liberar acesso

  if(conteudo == "D2E42B1B" || conteudo == "9361C997" )
  {
     Serial.println("Acesso liberado!");
     delay(500);
     tone(buzzer, HIGH);
     delay(100);
     tone(buzzer, LOW);
     delay(100);
     noTone(buzzer);
    
    //Libera trava
    servo.write(180);
    digitalWrite(ledAzul, LOW);
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
    delay(10000);
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);

    Serial.println("Trave a Porta");
    tone(buzzer, 300);
    delay(500);
    noTone(buzzer);
    delay(500);
    digitalWrite(ledVermelho, HIGH);
    servo.write(0);
    
  }

  else{
    Serial.println("Acesso Negado");
    Serial.println();

    //Aciona o buzzer 2 vezes
    

    tone(buzzer, 300);
    delay(500);
    noTone(buzzer);
    delay(500);
    tone(buzzer, 300);
    delay(100);
    noTone(buzzer);
    delay(500);
    delay(500);
    digitalWrite(ledAzul, HIGH);
    contador = 0;
  }
}
