#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 4
#define DATA_PIN 4
#define CS_PIN 5
#define CLK_PIN 6

// Arbitrary output pins
 MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setupAfficheur()
{
  P.begin();
  P.print(0.0, DEC);
}

void affiche(String w)
{
  P.print(w);
}
