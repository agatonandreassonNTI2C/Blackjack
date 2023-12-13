/*
 * BLACK JACK
 * Spelet går ut på att
 */

#include "Wire.h"
#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include "U8glib.h"


U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);


//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64
 

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


int Shock = 6;  // sensor signal
const int buttonPin = 2;
int player = 0; // 0 if bank 1 if player
int gameStarted = 0; // 0 not started 1 started
int playerTotal = 0; // total player score with aces counted as 1
int numAcesPlayer = 0;
int bankTotal = 0; // total player score with aces counted as 1
int numAcesBank = 0;
int initCards = 1;

void setup() {
  randomSeed(analogRead(0)); // Seed the random number generator with an analog reading
  pinMode(Shock, INPUT);  // define input for sensor signal
  Wire.begin();
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  oled.setFont(u8g_font_helvB10);


}

void loop() {

  int buttonState = digitalRead(buttonPin);
  unsigned long lastSensorReadTime = 0;
  const unsigned long sensorReadInterval = 20; // Adjust this interval as needed

  unsigned long currentTime = millis();

  int shockVal = digitalRead(Shock);

  if (gameStarted == 0) {
    updateStartscreen();
  }
  
  else
  {
    //Serial.println(val);

    // Bank
    if (player == 0 and initCards == 1) {
      //Serial.println("player");
      //Serial.println(player);
      updateHit();
      player = 1;
    }
    if (player == 1 and initCards == 1) {
      updateHit();
      initCards = 0;
      updateHit();
    }
    if (player == 0 and initCards == 0) {
      updateHit();
    }
    if (shockVal == LOW) {
        updateHit();
    }
    if (buttonState == HIGH) {
      updateStand();
    }
  }
}

void updateStartscreen() {
   if (gameStarted == 0) {
     updateOled("BlackJack", "The Game");
     delay(1500);
     gameStarted = 1;
   }
}

String getSymbol() {
  int randomSymbol = random(1, 5);

  if (randomSymbol == 1) {
    return " of hearts";
  }
  else if (randomSymbol == 2) {
    return " of diamonds";
  }
  else if (randomSymbol == 3) {
    return " of clubs";
  }
  else {
    return " of spades";
  }

  return "";
}

int getCard() {
  int number[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  int numberSize = sizeof(number) / sizeof(number[0]);
  int randomList = random(numberSize);
  int selectedNumber = number[randomList];
  

  //Serial.print("Randomly selected number: ");
  //Serial.print(selectedNumber);
  return selectedNumber;
}

String getCharacter(int card) {
  if (card == 11) {
    return "Jack";
  }
  if (card == 12) {
    return "Queen";
  }
  if (card == 13) {
    return "King";
  }
  if (card == 1) {
    return "Ace";
  }
  return String(card);
}

/*
 * Denna funktion konverterar knekt / dam / kung till värdet 10
 * samt tar hand om ess som kan vara 1 eller 11. Båda värdena sparas
 * för att hantera bägge fallen.
 */
void getTotals(int card) {

  int cardMax = min(card, 10);
  if (player == 0) {
    bankTotal += cardMax;
  }
  else {
    playerTotal += cardMax;
  }
  
  if (card == 1) {
    if (player == 0) {
      numAcesBank += 1;
    }
    else {
      numAcesPlayer += 1;
    }
  }
}
void updateHitDisplay(String symbol, int card) {

  getTotals(card);
  
  //Serial.println("absolutTotal");
  //Serial.println(absolutTotal);
  //Serial.println("absolutTotalAce");
  //Serial.println(absolutTotalAce);

  String cardStr = getCharacter(card);
  String totalStr = "";
  String totalStrAce = ""; 
  if (player == 0) {
    
    totalStr = String(bankTotal);
    if (numAcesBank > 0 and bankTotal < 12) {
      totalStrAce = "/" + String(bankTotal+10);
    }
  }
  else {
    totalStr = String(playerTotal);  
    if (numAcesPlayer > 0 and playerTotal < 12) {
     totalStrAce = "/" + String(playerTotal+10);
    }
  }
    
  
  //Serial.println(totalStrAce);
  
  if (player == 1) {
    updateOled("You got: ", cardStr + symbol);
  }
  else {
    updateOled("Bank got: ", cardStr + symbol);
  }
  
  delay(1500);
  
  
  if (playerTotal > 21 and player == 1) {
    updateStand();
    return;
  }
  if (player == 1) {
    if (initCards == 0) {
      updateOled("Hit or stand", "Total: " + totalStr + totalStrAce);
    }
    else {
      updateOled("Player", "Total: " + totalStr + totalStrAce);
    }
  }
  else
  {
    updateOled("Bank total", totalStr + totalStrAce);
  }

  delay(1500);
  
  /*
   * Bank must stop if more than 16
   */
  if (player == 0 and (bankTotal > 16 or (bankTotal + 10 > 16 and bankTotal + 10 < 22 and numAcesBank > 0))) {
    updateStand();
  }
}

void updateHit() {

  String symbol = getSymbol();
  int card = getCard();
    
  updateHitDisplay(symbol, card);
}


void updateStand() {
  
  Serial.println("player");
  Serial.println(player);
  if (player == 1) {
    if (numAcesPlayer > 0 and playerTotal < 12) {
      playerTotal += 10;
    }
    if (playerTotal > 21) {
      updateOled("Lose", "You are worst!");
      delay(2500);
      updateReset();
    }
    else {
      updateOled("Stand", "Dealers turn");
      player = 0;
      delay(1500);
    }
    
  }
  else {
    if (numAcesBank > 0 and bankTotal < 12) {
      bankTotal += 10;
    }
    if (bankTotal > 21) {
      updateOled("WIN", "You are best!");
      delay(1500);
      updateReset();
    }
    else if (playerTotal > bankTotal) {
      updateOled("WIN", "You are best!");
      delay(1500);
      updateReset();
    }
    else {
      updateOled("Lose", "You are worst!");
      delay(1500);
      updateReset();
    }
  }
}


void updateReset() {
  playerTotal = 0;
  bankTotal = 0;
  numAcesPlayer = 0;
  numAcesBank = 0;
  player = 0;
  gameStarted = 0;
  initCards = 1;
  //Serial.println("player");
  //Serial.println(player);
}

void updateOled(String text1, String text2) {
  oled.firstPage();
  do {
    oled.drawStr(0, 20, text1.c_str());
    oled.drawStr(0, 40, text2.c_str());
  } while (oled.nextPage());
}
