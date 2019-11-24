
#include <EEPROM.h>
#include "DisplayHelper.h"

#define BUTTON 2
#define ELM327_INIT_FAIL 1

int ADDR_PAGE_COUNTER = 0;

DisplayHelper display;
byte pageCounter = 0;
bool pageChanged = false;

void setup() {
  display.init();
  display.drawLogo();

  Serial.begin(38400);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  String conteudo = "";
  char caractere = 0;
  Serial.print("ATD\r");
  delay(1000);
  Serial.flush();
  Serial.print("ATZ\r");
  delay(1000);
  
  while(Serial.available() > 0) {
    caractere = Serial.read();
    conteudo.concat(caractere);
  }

  if (conteudo.indexOf("ELM327 v1.5") == -1) {
    finalizarComFalha(ELM327_INIT_FAIL);
  }
  
  Serial.print("ATE0\r");
  delay(1000);
  Serial.print("ATL0\r");
  delay(1000);
  Serial.print("ATS0\r");
  delay(1000);
  Serial.print("ATH0\r");
  delay(1000);
  
  pageCounter = EEPROM.read(ADDR_PAGE_COUNTER);
}

void readButtonState() {
  if (digitalRead(BUTTON) == LOW) {
    delay(100);
    pageCounter++;
    pageChanged = true;
    if (pageCounter > 3) {
      pageCounter = 0;
    }
    while (digitalRead(BUTTON) == LOW);
  }
}

void savePageChange() {
  if (pageChanged) {
     EEPROM.write(ADDR_PAGE_COUNTER, pageCounter);
  }
}

/*
 * rpm
 * bateria
 * intake temp
 * temperatura motor
 * avanço de ponto
 * MAP
 * TPS
 */

void loop() {
  readButtonState();
  
  switch (pageCounter) {
    case 0:
      display.showRpm(3500);
      break;
    
    case 1:
      display.showRpm(1500);
      break;      
      
    case 2:
      display.showBatteryVolts(getBatteryVolts());
      break;
      
    case 3:
      display.showIntakeTemperature(35);
      break;      
  } 
  
  savePageChange();
  pageChanged = false;
}

String getBatteryVolts() {
  String conteudo = "";
  char caractere = 0;
  Serial.print("ATRV\r");
  delay(100);
  while(Serial.available() > 0) {
    caractere = Serial.read();
    conteudo.concat(caractere);
  }

  int idx = conteudo.indexOf('>');
  if(idx > -1) {
    conteudo = conteudo.substring(0, idx);
    return conteudo;
  }
  return "0";
}

void finalizarComFalha(byte cod) {
  display.showError(cod);
  while(true);
}




