//I don t really know if this actually work xD

#include <LedControl.h>
#include <SoftwareWire.h>

#define DIN_PIN 5
#define CS_PIN  6
#define CLK_PIN 7
#define DEVICES 4
#define SDA_PIN 9
#define SCL_PIN 8
#define TOUCH_PIN 10

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, DEVICES);
SoftwareWire tempWire(SDA_PIN, SCL_PIN);

bool displayOn = true;

// i asked chat for the charcters
const byte font[][5] = {
  {0x3E,0x51,0x49,0x45,0x3E}, // 0
  {0x00,0x42,0x7F,0x40,0x00}, // 1
  {0x42,0x61,0x51,0x49,0x46}, // 2
  {0x21,0x41,0x45,0x4B,0x31}, // 3
  {0x18,0x14,0x12,0x7F,0x10}, // 4
  {0x27,0x45,0x45,0x45,0x39}, // 5
  {0x3C,0x4A,0x49,0x49,0x30}, // 6
  {0x01,0x71,0x09,0x05,0x03}, // 7
  {0x36,0x49,0x49,0x49,0x36}, // 8
  {0x06,0x49,0x49,0x29,0x1E}, // 9
  {0x00,0x00,0x02,0x00,0x00}, // .
  {0x3E,0x41,0x41,0x41,0x22}  // C
};

void setup() {
  pinMode(TOUCH_PIN, INPUT);

  for (int i = 0; i < DEVICES; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 8);
    lc.clearDisplay(i);
  }

  tempWire.begin();
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    delay(200);
    displayOn = !displayOn;

    if (!displayOn) clearAll();
  }

  if (displayOn) {
    float temp = readTemperature();
    showTemperature(temp);
  }

  delay(1000);
}

float readTemperature() {
  tempWire.beginTransmission(0x48);
  tempWire.write(0x00);
  tempWire.endTransmission();

  tempWire.requestFrom(0x48, 2);
  if (tempWire.available() == 2) {
    int msb = tempWire.read();
    int lsb = tempWire.read();
    int raw = ((msb << 8) | lsb) >> 4;
    return raw * 0.0625;
  }
  return 0;
}

void showChar(int x, char c) {
  int index;
  if (c >= '0' && c <= '9') index = c - '0';
  else if (c == '.') index = 10;
  else index = 11; // C

  for (int col = 0; col < 5; col++) {
    byte data = font[index][col];
    for (int row = 0; row < 7; row++) {
      bool pixel = data & (1 << row);
      int device = (x + col) / 8;
      int colInDevice = (x + col) % 8;
      lc.setLed(device, row, colInDevice, pixel);
    }
  }
}

void showTemperature(float t) {
  clearAll();

  int temp = (int)(t * 10); // 23.4 → 234
  char buf[6];
  sprintf(buf, "%2d.%dC", temp / 10, abs(temp % 10));

  int x = 1; // poziție start
  for (int i = 0; buf[i]; i++) {
    showChar(x, buf[i]);
    x += 6;
  }
}

void clearAll() {
  for (int i = 0; i < DEVICES; i++) {
    lc.clearDisplay(i);
  }
}
