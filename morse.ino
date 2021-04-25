#include <Arduino.h>
#include <U8x8lib.h>

#define BUTTON_PIN 2
#define SPACE_TIME 500
#define MAX_DOT_TIME 150
#define MAX_PRESSES 5

#define DISPLAY_CS_PIN 10
#define DISPLAY_DC_PIN 9
#define DISPLAY_RS_PIN 8

U8X8_PCD8544_84X48_4W_HW_SPI u8x8(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RS_PIN);

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 20;    // the debounce time; increase if the output flickers
unsigned long lastReleaseTime = 0;
unsigned long lastPressTime = 0;
unsigned long presses[MAX_PRESSES];
uint8_t pressCount = 0;
uint8_t rowLen = 0;

typedef struct mapping {
  char letter;
  uint8_t pressCount;
  char pattern;
} mapping;

mapping charmap[] = {
  {'a', 2, 0b01},
  {'b', 4, 0b1000},
  {'c', 4, 0b1010},
  {'d', 3, 0b100},
  {'e', 1, 0b0},
  {'f', 4, 0b0010},
  {'g', 3, 0b110},
  {'h', 4, 0b0000},
  {'i', 2, 0b00},
  {'j', 4, 0b0111},
  {'k', 3, 0b101},
  {'l', 4, 0b0100},
  {'m', 2, 0b11},
  {'n', 2, 0b10},
  {'o', 3, 0b111},
  {'p', 4, 0b0110},
  {'q', 4, 0b1101},
  {'r', 3, 0b010},
  {'s', 3, 0b000},
  {'t', 1, 0b1},
  {'u', 3, 0b001},
  {'v', 4, 0b0001},
  {'w', 3, 0b011},
  {'x', 4, 0b1001},
  {'y', 4, 0b1011},
  {'z', 4, 0b1100}
};

char recognizeChar() {
  uint8_t length = sizeof(charmap) / sizeof(charmap[0]);
  for (int i = 0; i < length; i++) {
    if (charmap[i].pressCount != pressCount) continue;
    int press;
    for (press = 0; press < pressCount; press++) {
      int isDash = presses[press] > MAX_DOT_TIME;
      char pattern = charmap[i].pattern;
      int shouldBeDash = pattern & (1 << pressCount >> press >> 1);
      if ((!isDash && shouldBeDash) || (isDash && !shouldBeDash)) break;
    }
    if (press == pressCount) return charmap[i].letter;
  }
  return '?'; // unknown combination
}

void emitChar() {
  Serial.print("pressCount: ");
  Serial.println(pressCount);
  for (int i = 0; i < pressCount; i++) {
    Serial.print("time: ");
    Serial.println(presses[i]);
  }
  
  char letter = recognizeChar();
  Serial.print(letter);
  u8x8.print(letter);
  rowLen++;
  if (rowLen >= 10) {
    u8x8.print("\n");
    rowLen = 0;
  }
  pressCount = 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);  
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (reading != lastButtonState) {
    lastDebounceTime = now;
  }

  if ((now - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        // button pressed
        lastPressTime = now;
      } else {
        // button released
        lastReleaseTime = now;
        if (pressCount < MAX_PRESSES) {
          presses[pressCount] = lastReleaseTime - lastPressTime;
          pressCount++;
        }
      }
    } else {
      if (buttonState == LOW && pressCount > 0 && now - lastReleaseTime > SPACE_TIME) {
        emitChar();
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
