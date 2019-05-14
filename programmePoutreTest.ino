#include <SoftwareSerial.h>
#include <math.h>

#define BT_RX 3
#define BT_TX 2
SoftwareSerial mySerial(BT_RX, BT_TX); // RX, TX

bool resetAffichePoid = true;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Bienvenu!!");
  setup_load_cell();
  setupAfficheur();
  cmd("AT+CLEAR");
  cmd("AT+RESET");
}

double bufferPoid = 0;
void loop() {
   //écoute du module bluetooth
   while (mySerial.available()) {
      Serial.println("\n\nreceved\n\n");
      char query = mySerial.read();
      if(query == 'z'){
        tare();
        Serial.println("tare");
      }
   }

   //scale /!\ echelle en hectogramme
   long weights[4];
   getWeights(1, weights);

   //reset auto et met a zero quand poit < 1kg (10hg)
   keepScale0(weights[0]+weights[1]+weights[2]+weights[3]);

   //packet contenant la valeur des 4 capteurs
   byte dataBit[8] = {0,0,0,0,0,0,0,0};
   buildPacket(weights, dataBit);

   //envoie des données
   mySerial.write(dataBit,8);  

   //log
   Serial.print("Send to device : ");
   Serial.println(String(weights[0])+' '+String(weights[1])+' '+String(weights[2])+' '+String(weights[3]));

  //AFFICHER afficheur LED
  double poid = (weights[0]+weights[1]+weights[2]+weights[3])/10;
  
  if(poid > bufferPoid)
  {
    bufferPoid = poid;
  }

  if(poid == 0){
    resetAffichePoid = true;
  }

  if(poid > 1 && resetAffichePoid){
    resetAffichePoid = false;
    bufferPoid = 0;
  }
  
  affiche(String(bufferPoid));
}

void cmd(String cmd){
  cmd.toUpperCase();
  Serial.println("Commande: "+cmd);
  mySerial.print(cmd);
}
