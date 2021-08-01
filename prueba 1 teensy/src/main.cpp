#include <Arduino.h>
#include <SD.H>
#include <SPI.H>

File myFile;

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  Serial.begin(9600);


    if (!SD.begin(BUILTIN_SDCARD)){
      Serial.println("initialization failed");
      while(1);

    }else{
    Serial.println("initialization done");
    }



  }



void loop() {

  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(1000);
  Serial.printf("sip");


  myFile = SD.open("test.txt",FILE_WRITE);

  if(myFile){
    Serial.print("Writing to test.txt...");
      myFile.println("sipe");
      myFile.println("\n");
      // close the file:
      myFile.close();
      Serial.println("done.");


  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // put your main code here, to run repeatedly:
}
