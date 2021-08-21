
/*esta funcion se encarga de tomar deciciones acerca de los datos que llegan por
el bus*/
void casos (int buf0,float buf2,int tvent1, int tvent2,int tcort,int pinpwm,int chenable,int disenable,float vmin,float vmax,int vvent)
{

//crea el arbol de accion en funcion de si el dato es de voltaje o de temperatura
switch (buf0)
{
/*ejecuta comprobacion de temperatura y decide si cortar o no y cuando y como
activar los ventiladores por la señal pwm*/
case 1:
    if (buf2>=tvent1&&buf2<=tvent2){

      analogWrite(pinpwm,vvent*25);

    }else if(buf2>=tvent2){

      analogWrite(pinpwm,100);

    }
    if(buf2>=tcort){

      digitalWrite(chenable, HIGH);
      digitalWrite(disenable, HIGH);

    }
case 2:
  if(buf2<vmin){

    digitalWrite(disenable,HIGH);

  }
  if(buf2>vmin){

    digitalWrite(chenable,HIGH);

  }

default:

break;


}

}

/*ESTA FUNCION SE ENCARGA DE CALCULAR EL VALOR DE LA CORRIENTE EN FUNCION DE LO
QUE RECIBE POR EL PUERTO ANALOGICO Y DEVUELVE EL VALOR*/
//variables para la funcion
int supplyv=5;
float uc=5;
float voffset=2.5;
float g1=26.67/1000;
float g2=4/1000;
float corrchan1=0;
float corrchan2=0;
//empezamos la funcion
float corriente(int pin1,int pin2){

  float mpin1=analogRead(pin1);
  corrchan1=((5/5)*mpin1-voffset)*(1/g1);

  float mpin2=analogRead(pin2);
  corrchan2=((5/5)*mpin2-voffset)*(1/g2);

  if(corrchan1> -75 && corrchan1< 75){
    return corrchan1;
  }else{

    return corrchan2;
  }

}
/*ESTA FUNCION DECIDE COMO ACTUAR PARA LOS DISTINTOS CASOS SEGUN EL VALOR DE LA
CORRIENTE QUE CIRCULA POR EL TS
*/
int estado=0;
void docurrent(int chenable,int disenable,int corrientech,int corrientedch, float corriente){

if(corriente>0){
estado=1;
}else{
  estado=0;
}

switch(estado)
{
    /*en este caso se supone que esta en descarga el circuito,luego no permitimos
    que por el conector de carga entre corriente asi que forzamos a que el charge
    enable este cortado*/

    case 1://DESCARGA
      digitalWrite(chenable,HIGH);
      if(corriente>=corrientedch){

        digitalWrite(disenable,HIGH);

      }else{

        digitalWrite(disenable,LOW);

      }
    case 0://CARGA

      digitalWrite(disenable,HIGH);
      if(corriente>=corrientech){
        digitalWrite(chenable,HIGH);
      }else{

        digitalWrite(chenable,LOW);

      }
}

}
