#include "HX711-multi.h"

// Pins to the load cell amp
#define CLK A0      // clock pin to the load cell amp
#define BG A1    //1 data pin capteur bas gauche
#define HG A4    //2 data pin capteur haut gauche
#define HD A3    //3 data pin capteur haut droite
#define BD A2    //4 data pin capteur Bas droite

#define TARE_TIMEOUT_SECONDS 4

#define NB_CAPTEUR 4

byte DOUTS[NB_CAPTEUR] = {BG, HG, HD, BD};
long facteur[4] = {12765, 20550, 20550, 12650};
long int results[NB_CAPTEUR];

HX711MULTI scales(NB_CAPTEUR, DOUTS, CLK);

void setup_load_cell() {
  
  scales.tare();
}


//nb : moyenne sur nb mesure
//revoie un tableau 
//byte 0&1 = bg
//byte 2&3 = hg
//byte 4&5 = hd
//byte 6&7 = bd
void getWeights(int nb, long *data){
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
  
  for(int j=0; j<nb; j++)
  { 
    scales.read(results);
    for(int i=0; i<NB_CAPTEUR; ++i) {
      data[i] += round(results[i]/facteur[i]);
    }
  }

  //average
  for(int i=0; i<NB_CAPTEUR; i++)
  { 
      data[i] /= nb;
  }
}

void tare(){
  scales.tare();
}

int a = 0; // pour compter combien de fois la valeur est proche de zero pour refaire le tare
void keepScale0(double w){
  if (w < 10){//< 1k (10hg)
    a++;
    //Serial.println(String(i));
      if (a > 60){
        tare();
        //Serial.println('z');
        a = 0;
      }
   }
   else {
    a = 0;
   }
}

//prend un nombre < 65 000 et revoi un tableau de 2 octets
void toBits(long nb, byte* tabByte){
  tabByte[0] = nb/256;
  tabByte[1] = nb%256;
}

//prend un nombre < 1023 et revoi un tableau de 2 octets
//corespondan au protocol
//x                   xx                xxxxx
//debut/fin           //numero capteur  //vlauer
//0=debut             //0=1
//1=fin               //...             
void toBits(long nb, byte* tabByte, int capteur){
  tabByte[0] = nb/32;
  tabByte[1] = nb%32;

  //on indique le numereau du capteur
  tabByte[0] += 32 * capteur;
  tabByte[1] += 32 * capteur;
  
  //on dit que c'est le debut du nombre
  tabByte[1] += 128;
  
}

//data from -16 383 to 16 383
//preimer bit de poid fort indique si c'est le dernier capteur
//deuixème indique si c'est negatif
void buildPacket(long data[], byte packet[]){
  for(int i = 0; i < NB_CAPTEUR; i++)
  {
    if(data[i] < 0){
      packet[i] = 0;
      packet[i+1] = 0;
    }
    else{
      packet[i*2] = data[i] / 256;
      packet[i*2+1] = data[i] % 256;
    }
  }
}
