#include <SD.H>
#include <SPI.H>



void initializeSD (const int chipSelect){

  if (!SD.begin(chipSelect)) {

      Serial.println("initialization failed. Things to check:");

      Serial.println("1. is a card inserted?");

      Serial.println("2. is your wiring correct?");

      Serial.println("3. did you change the chipSelect pin to match your shield or module?");

      Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");

      while (true);

    }


 Serial.println("initialization done.");

}

void guardadato (int tiempo,int dato[5], int posdato){





}
