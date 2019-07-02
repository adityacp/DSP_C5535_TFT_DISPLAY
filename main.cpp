#include "Adafruit_ILI9341.h"
#include "usbstk5515.h"

#define CS 12
#define DC 13
#define MOSI 14
#define MISO 15
#define RST 16
#define SCLK 17


int main(void) {
    Adafruit_ILI9341 tft = Adafruit_ILI9341(CS, DC, MOSI, SCLK, RST, MISO);
    int w, i, i2, cx = tft.width()  / 2 - 1, cy = tft.height() / 2 - 1;

    USBSTK5515_init();
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);

    // Sample code from Adafruit examples

    // Draw a rounded rectangle
    w = min(tft.width(), tft.height());
    for(i=0; i<w; i+=6) {
       i2 = i / 2;
       tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
    }

    USBSTK5515_wait(1000);

    // Draw circles to fit the screen
    int radius = 10;
    int x, y, r2 = radius * 2, wi = tft.width()  + radius, ht = tft.height() + radius;

    // Screen is not cleared for this one -- this is
    // intentional and does not affect the reported time.
//      start = micros();
    for(x=0; x<wi; x+=r2) {
      for(y=0; y<ht; y+=r2) {
        tft.drawCircle(x, y, radius, ILI9341_WHITE);
      }
    }

    return 0;
}


