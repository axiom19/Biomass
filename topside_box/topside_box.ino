
/*
 * LoRa Transmitting Station for Biomass Project
 * Acts as a LoRa client
 * Designed for Heltec Wifi-LoRa 32 V2 Board
 * Driver: RH_RF95
 * Manager: RHReliableDatagram
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>


// for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13


//for HELTEC LORA
//#define RFM95_CS 18
//#define RFM95_RST 14
//#define RFM95_INT 26
//#define LED 25

#define SERVER_ADDRESS 1
#define CLIENT_ADDRESS 2

//Set Frequency
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 driver(RFM95_CS, RFM95_INT);
// Class to manage message delivery and receipt
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

/* TEST DATA */
uint8_t test_charging = 0;

int test_golay_a[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33,
                        35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63};
                        
int test_golay_b[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34,
                        36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64};

/* Buffers */
uint8_t golay_message[32 * 2 + 2];
uint8_t status_message[4];

/* COUNTERS */
unsigned long prev_update = 0;

void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  if (!manager.init())
    Serial.println("init failed");

  /*  MODEM CONFIG IMPORTANT!!!
   *   
   *  Bw125Cr45Sf128   -> Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range. 
   *  Bw500Cr45Sf128   -> Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range. 
   *  Bw31_25Cr48Sf512 -> Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
   *  Bw125Cr48Sf4096  -> Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range.
   *  Bw125Cr45Sf2048  -> Bw = 125 kHz, Cr = 4/5, Sf = 2048chips/symbol, CRC on. Slow+long range.
   *  
   *  If data rate is slower, increase timeout below
   */
  manager.setTimeout(5000);
  driver.setModemConfig(RH_RF95::Bw125Cr45Sf128);
  driver.setFrequency(RF95_FREQ);
  driver.setTxPower(23, false);
  
}

//for receiving messages
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  //update every 5 seconds
  if ((millis() - prev_update) > 30000 + CLIENT_ADDRESS){
    prev_update = millis();
    digitalWrite(LED, HIGH);
    
    //write golay data A -> message ID 2
    Serial.println("Sending Golay A");
    golay_message[0] = 2;
    golay_message[1] = sizeof(golay_message) - 2;
    for(int i = 2; i < sizeof(golay_message); i += 2){
        golay_message[i] = test_golay_a[(i - 2) / 2] >> 8;
        golay_message[i + 1] = test_golay_a[(i - 2) / 2];
    }
    //send golay message A
    if (manager.sendtoWait(golay_message, sizeof(golay_message), SERVER_ADDRESS))
      Serial.println("Golay A Message Acknowledged");
    else
      Serial.println("Golay A Message Not Received");
    digitalWrite(LED,LOW);
    delay(200);
    digitalWrite(LED,HIGH);

    //write golay data B -> message ID 3
    Serial.println("Sending Golay B");
    golay_message[0] = 3;
    golay_message[1] = sizeof(golay_message) - 2;
    for(int i = 2; i < sizeof(golay_message); i += 2){
        golay_message[i] = test_golay_b[(i - 2) / 2] >> 8;
        golay_message[i + 1] = test_golay_b[(i - 2) / 2];
    }
    //send golay message A
    if (manager.sendtoWait(golay_message, sizeof(golay_message), SERVER_ADDRESS))
      Serial.println("Golay B Message Acknowledged");
    else
      Serial.println("Golay B Message Not Received");
    digitalWrite(LED,LOW);
    delay(200);
    digitalWrite(LED,HIGH);

    
    //write status message -> message ID 1
    int vbat = read_battery();
    status_message[0] = 1;
    status_message[1] = vbat >> 8;
    status_message[2] = vbat;
    status_message[3] = 0;

    //send status_message
    if (manager.sendtoWait(status_message, sizeof(status_message), SERVER_ADDRESS))
      Serial.println("Status Message Acknowledged");
    else
      Serial.println("Status Message Not Received");
    digitalWrite(LED, LOW);
  }
}


int read_battery(){
  int measured_vbat = 0;
  for(int i = 0; i < 10; i++){
    measured_vbat += analogRead(A9);
  }
  Serial.print("Batt V ");
  float vbat_f = measured_vbat / 10 * 6.6 / 1024;
  Serial.println(vbat_f);
  return measured_vbat / 10;
}
