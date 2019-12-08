#include <libmaple/iwdg.h>
#include <SegaController.h>
#include <USBXBox360.h>

#define LED PC13

const uint32_t watchdogSeconds = 3;

// NB: connect A10 to TX, A9 to RX

SegaController sega(PA5, PA0, PA1, PA2, PA3, PA4, PA6);
USBXBox360 XBox360;

#define XBOX_A 13
#define XBOX_B 14
#define XBOX_X 15
#define XBOX_Y 16
#define XBOX_DUP 1
#define XBOX_DDOWN 2
#define XBOX_DLEFT 3
#define XBOX_DRIGHT 4
#define XBOX_START 5
#define XBOX_LSHOULDER 9
#define XBOX_RSHOULDER 10
#define XBOX_GUIDE  11

/*
 *     SC_CTL_ON    = 1, // The controller is connected
    SC_BTN_UP    = 2,
    SC_BTN_DOWN  = 4,
    SC_BTN_LEFT  = 8,
    SC_BTN_RIGHT = 16,
    SC_BTN_START = 32,
    SC_BTN_A     = 64,
    SC_BTN_B     = 128,
    SC_BTN_C     = 256,
    SC_BTN_X     = 512,
    SC_BTN_Y     = 1024,
    SC_BTN_Z     = 2048,
    SC_BTN_MODE  = 4096,

 */

const uint16_t remap_retroarch[16] = {
  0xFFFF,
  0xFFFF | XBOX_DUP,
  0xFFFF | XBOX_DDOWN,
  0xFFFF | XBOX_DLEFT,

  0xFFFF | XBOX_DRIGHT,
  XBOX_START,
  XBOX_B, // A
  XBOX_A, // B
  
  XBOX_X, // C
  XBOX_LSHOULDER, // X
  XBOX_Y, // Y
  XBOX_RSHOULDER, // Z
  
  XBOX_GUIDE, // MODE

  0xFFFF, 0xFFFF, 0xFFFF
};

const uint16_t* remap = remap_retroarch;

inline int16_t range10u16s(uint16_t x) {
  return (((int32_t)(uint32_t)x - 512) * 32767 + 255) / 512;
}

void setup() {
  iwdg_init(IWDG_PRE_256, watchdogSeconds*156);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);
  USBComposite.setProductString("GenesisToXBox360");
  XBox360.begin();
  XBox360.setManualReportMode(true);
}

void loop() {
  iwdg_feed();
  word state = sega.getState();
  if (state & SC_CTL_ON) {
      digitalWrite(LED,0);
      if (state & SC_BTN_LEFT) 
          XBox360.X(-32768);
      else if (state & SC_BTN_RIGHT)
          XBox360.X(32767);
      else
          XBox360.X(0);
  
      if (state & SC_BTN_UP) 
          XBox360.Y(32767);
      else if (state & SC_BTN_DOWN) 
          XBox360.Y(-32768);
      else
          XBox360.Y(0);

    uint16_t mask = 1;
    for (int i = 0; i < 16; i++, mask <<= 1) {
      uint16_t xb = remap[i];
      if (xb != 0xFFFF)
        XBox360.button(xb, 0 != (state & mask));
    }
    XBox360.send();
  }
  else {
      digitalWrite(LED,1);
  }
}

