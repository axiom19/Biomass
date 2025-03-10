#include <Wire.h>
#include <avr/sleep.h>

/*
   Make Sure to Burn Bootloader before trying program a new board!
*/
#define I2C_ADDRESS 1
#define LED A0
#define DIODE A1
#define LASER 4

int state = 0;
int counter = 0;
uint8_t data[10];


void setup() {
  //set pinmodes
  pinMode(LED, OUTPUT);
  pinMode(LASER, OUTPUT);
  pinMode(DIODE, INPUT);
  digitalWrite(LED, HIGH);

  //setup i2c
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  delay(1000);
  digitalWrite(LED, LOW);


}


/**
 * State Machine
 * 
 * State 0 -> go to deep sleep
 * State 1 -> sample ADC
 * State 2 -> wait to send data
 * 
 */
 
void loop() {
  switch (state) {

    case 1:
      takeSample(100);
      state = 2;
      counter = 0;
      break;

    case 2:
      if (counter > 100){
        state = 0;
      }
      delay(100);
      counter ++;
      break;
      
    default:
      if(counter > 10){
        //go to sleep
        set_sleep_mode (SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        digitalWrite(LED, LOW);
        sleep_cpu();
        //wake up
        digitalWrite(LED, HIGH);
        sleep_disable();
        counter = 0;
        TWCR = bit(TWEN) | bit(TWIE) | bit(TWEA) | bit(TWINT);// release I2C
        Wire.begin(I2C_ADDRESS);
        counter = 0;
      }
      delay(100);
      counter ++;
      break;
  }
}

void requestEvent() {
  if (state == 2) {
    Wire.write(data, 4);
    state = 0;
  }
}

void receiveEvent(int howMany) {
  if (howMany == 1) {
    int cmd = Wire.read();
    if (cmd == 2)
      state = 1;
  }
}

void takeSample(int samples) {
  uint32_t local_off = 0;
  uint32_t local_on = 0;

  digitalWrite(LASER, LOW);
  for (int i = 0; i < samples; i ++) {
    local_off += analogRead(DIODE);
  }
  digitalWrite(LASER, HIGH);
  delay(500);
  for (int i = 0; i < samples; i ++) {
    local_on += analogRead(DIODE);
  }
  digitalWrite(LASER, LOW);
  local_off /= samples;
  local_on /= samples;

  data[0] = (local_off >> 8) & 0xFF;
  data[1] = local_off & 0xFF;
  data[2] = (local_on >> 8)  & 0xFF;
  data[3] = local_on & 0xFF;
}
