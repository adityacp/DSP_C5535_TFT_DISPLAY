#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

//#if ARDUINO >= 100
// #include "Arduino.h"
// #include "Print.h"
//// #include "WProgram.h"
//#endif
#include "gfxfont.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { Int16 t = a; a = b; b = t; }
#endif

/// A generic graphics superclass that can handle all sorts of drawing. At a minimum you can subclass and provide drawPixel(). At a maximum you can do a ton of overriding to optimize. Used for any/all Adafruit displays!
class Adafruit_GFX {

 public:

  Adafruit_GFX(Int16 w, Int16 h); // Constructor

  // This MUST be defined by the subclass:
  virtual void drawPixel(Int16 x, Int16 y, Uint16 color) = 0;    ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass. @param x X coordinate.  @param y Y coordinate. @param color 16-bit pixel color.

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void startWrite(void);
  virtual void writePixel(Int16 x, Int16 y, Uint16 color);
  virtual void writeFillRect(Int16 x, Int16 y, Int16 w, Int16 h, Uint16 color);
  virtual void writeFastVLine(Int16 x, Int16 y, Int16 h, Uint16 color);
  virtual void writeFastHLine(Int16 x, Int16 y, Int16 w, Uint16 color);
  virtual void writeLine(Int16 x0, Int16 y0, Int16 x1, Int16 y1, Uint16 color);
  virtual void endWrite(void);

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void setRotation(Uint8 r);
  virtual void invertDisplay(Bool i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    // It's good to implement those, even if using transaction API
    drawFastVLine(Int16 x, Int16 y, Int16 h, Uint16 color),
    drawFastHLine(Int16 x, Int16 y, Int16 w, Uint16 color),
    fillRect(Int16 x, Int16 y, Int16 w, Int16 h, Uint16 color),
    fillScreen(Uint16 color),
    // Optional and probably not necessary to change
    drawLine(Int16 x0, Int16 y0, Int16 x1, Int16 y1, Uint16 color),
    drawRect(Int16 x, Int16 y, Int16 w, Int16 h, Uint16 color);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(Int16 x0, Int16 y0, Int16 r, Uint16 color),
    drawCircleHelper(Int16 x0, Int16 y0, Int16 r, Uint8 cornername,
      Uint16 color),
    fillCircle(Int16 x0, Int16 y0, Int16 r, Uint16 color),
    fillCircleHelper(Int16 x0, Int16 y0, Int16 r, Uint8 cornername,
      Int16 delta, Uint16 color),
    drawTriangle(Int16 x0, Int16 y0, Int16 x1, Int16 y1,
      Int16 x2, Int16 y2, Uint16 color),
    fillTriangle(Int16 x0, Int16 y0, Int16 x1, Int16 y1,
      Int16 x2, Int16 y2, Uint16 color),
    drawRoundRect(Int16 x0, Int16 y0, Int16 w, Int16 h,
      Int16 radius, Uint16 color),
    fillRoundRect(Int16 x0, Int16 y0, Int16 w, Int16 h,
      Int16 radius, Uint16 color),
    drawBitmap(Int16 x, Int16 y, const Uint8 bitmap[],
      Int16 w, Int16 h, Uint16 color),
    drawBitmap(Int16 x, Int16 y, const Uint8 bitmap[],
      Int16 w, Int16 h, Uint16 color, Uint16 bg),
    drawBitmap(Int16 x, Int16 y, Uint8 *bitmap,
      Int16 w, Int16 h, Uint16 color),
    drawBitmap(Int16 x, Int16 y, Uint8 *bitmap,
      Int16 w, Int16 h, Uint16 color, Uint16 bg),
    drawXBitmap(Int16 x, Int16 y, const Uint8 bitmap[],
      Int16 w, Int16 h, Uint16 color),
    drawGrayscaleBitmap(Int16 x, Int16 y, const Uint8 bitmap[],
      Int16 w, Int16 h),
    drawGrayscaleBitmap(Int16 x, Int16 y, Uint8 *bitmap,
      Int16 w, Int16 h),
    drawGrayscaleBitmap(Int16 x, Int16 y,
      const Uint8 bitmap[], const Uint8 mask[],
      Int16 w, Int16 h),
    drawGrayscaleBitmap(Int16 x, Int16 y,
      Uint8 *bitmap, Uint8 *mask, Int16 w, Int16 h),
    drawRGBBitmap(Int16 x, Int16 y, const Uint16 bitmap[],
      Int16 w, Int16 h),
    drawRGBBitmap(Int16 x, Int16 y, Uint16 *bitmap,
      Int16 w, Int16 h),
    drawRGBBitmap(Int16 x, Int16 y,
      const Uint16 bitmap[], const Uint8 mask[],
      Int16 w, Int16 h),
    drawRGBBitmap(Int16 x, Int16 y,
      Uint16 *bitmap, Uint8 *mask, Int16 w, Int16 h),
    drawChar(Int16 x, Int16 y, unsigned char c, Uint16 color,
      Uint16 bg, Uint8 size),
    drawChar(Int16 x, Int16 y, unsigned char c, Uint16 color,
	      Uint16 bg, Uint8 size_x, Uint8 size_y),
    getTextBounds(const char *string, Int16 x, Int16 y,
      Int16 *x1, Int16 *y1, Uint16 *w, Uint16 *h),
//    getTextBounds(const __FlashStringHelper *s, Int16 x, Int16 y,
//      Int16 *x1, Int16 *y1, Uint16 *w, Uint16 *h),
    getTextBounds(const String &str, Int16 x, Int16 y,
      Int16 *x1, Int16 *y1, Uint16 *w, Uint16 *h),
    setTextSize(Uint8 s),
    setTextSize(Uint8 sx, Uint8 sy),
    setFont(const GFXfont *f = NULL);

  /**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
  void setCursor(Int16 x, Int16 y) { cursor_x = x; cursor_y = y; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  void setTextColor(Uint16 c) { textcolor = textbgcolor = c; }

  /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
  */
  /**********************************************************************/
  void setTextColor(Uint16 c, Uint16 bg) {
    textcolor   = c;
    textbgcolor = bg;
  }

  /**********************************************************************/
  /*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
  */
  /**********************************************************************/
  void setTextWrap(Bool w) { wrap = w; }

  /**********************************************************************/
  /*!
    @brief  Enable (or disable) Code Page 437-compatible charset.
            There was an error in glcdfont.c for the longest time -- one
            character (#176, the 'light shade' block) was missing -- this
            threw off the index of every character that followed it.
            But a TON of code has been written with the erroneous
            character indices. By default, the library uses the original
            'wrong' behavior and old sketches will still work. Pass
            'true' to this function to use correct CP437 character values
            in your code.
    @param  x  true = enable (new behavior), false = disable (old behavior)
  */
  /**********************************************************************/
  void cp437(Bool x=true) { _cp437 = x; }

#if ARDUINO >= 100
  virtual size_t write(Uint8);
#else
  virtual void   write(Uint8);
#endif

  /************************************************************************/
  /*!
    @brief      Get width of the display, accounting for current rotation
    @returns    Width in pixels
  */
  /************************************************************************/
  Int16 width(void) const { return _width; };

  /************************************************************************/
  /*!
    @brief      Get height of the display, accounting for current rotation
    @returns    Height in pixels
  */
  /************************************************************************/
  Int16 height(void) const { return _height; }

  /************************************************************************/
  /*!
    @brief      Get rotation setting for display
    @returns    0 thru 3 corresponding to 4 cardinal rotations
  */
  /************************************************************************/
  Uint8 getRotation(void) const { return rotation; }

  // get current cursor position (get rotation safe maximum values,
  // using: width() for x, height() for y)
  /************************************************************************/
  /*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
  */
  /************************************************************************/
  Int16 getCursorX(void) const { return cursor_x; }

  /************************************************************************/
  /*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
  */
  /************************************************************************/
  Int16 getCursorY(void) const { return cursor_y; };

 protected:
  void
    charBounds(char c, Int16 *x, Int16 *y,
      Int16 *minx, Int16 *miny, Int16 *maxx, Int16 *maxy);
  Int16
    WIDTH,          ///< This is the 'raw' display width - never changes
    HEIGHT;         ///< This is the 'raw' display height - never changes
  Int16
    _width,         ///< Display width as modified by current rotation
    _height,        ///< Display height as modified by current rotation
    cursor_x,       ///< x location to start print()ing text
    cursor_y;       ///< y location to start print()ing text
  Uint16
    textcolor,      ///< 16-bit background color for print()
    textbgcolor;    ///< 16-bit text color for print()
  Uint8
    textsize_x,      ///< Desired magnification in X-axis of text to print()
    textsize_y,      ///< Desired magnification in Y-axis of text to print()
    rotation;       ///< Display rotation (0 thru 3)
  Bool
    wrap,           ///< If set, 'wrap' text at right edge of display
    _cp437;         ///< If set, use correct CP437 charset (default is off)
  GFXfont
    *gfxFont;       ///< Pointer to special font
};


/// A simple drawn button UI element
class Adafruit_GFX_Button {

 public:
  Adafruit_GFX_Button(void);
  // "Classic" initButton() uses center & size
  void initButton(Adafruit_GFX *gfx, Int16 x, Int16 y,
   Uint16 w, Uint16 h, Uint16 outline, Uint16 fill,
   Uint16 textcolor, char *label, Uint8 textsize);
  void initButton(Adafruit_GFX *gfx, Int16 x, Int16 y,
   Uint16 w, Uint16 h, Uint16 outline, Uint16 fill,
   Uint16 textcolor, char *label, Uint8 textsize_x, Uint8 textsize_y);
  // New/alt initButton() uses upper-left corner & size
  void initButtonUL(Adafruit_GFX *gfx, Int16 x1, Int16 y1,
   Uint16 w, Uint16 h, Uint16 outline, Uint16 fill,
   Uint16 textcolor, char *label, Uint8 textsize);
  void initButtonUL(Adafruit_GFX *gfx, Int16 x1, Int16 y1,
   Uint16 w, Uint16 h, Uint16 outline, Uint16 fill,
   Uint16 textcolor, char *label, Uint8 textsize_x, Uint8 textsize_y);
  void drawButton(Bool inverted = false);
  Bool contains(Int16 x, Int16 y);

  /**********************************************************************/
  /*!
    @brief    Sets button state, should be done by some touch function
    @param    p  True for pressed, false for not.
  */
  /**********************************************************************/
  void press(Bool p) { laststate = currstate; currstate = p; }

  Bool justPressed();
  Bool justReleased();

  /**********************************************************************/
  /*!
    @brief    Query whether the button is currently pressed
    @returns  True if pressed
  */
  /**********************************************************************/
  Bool isPressed(void) { return currstate; };

 private:
  Adafruit_GFX *_gfx;
  Int16       _x1, _y1; // Coordinates of top-left corner
  Uint16      _w, _h;
  Uint8       _textsize_x;
  Uint8       _textsize_y;
  Uint16      _outlinecolor, _fillcolor, _textcolor;
  char          _label[10];

  Bool currstate, laststate;
};


/// A GFX 1-bit canvas context for graphics
class GFXcanvas1 : public Adafruit_GFX {
 public:
  GFXcanvas1(Uint16 w, Uint16 h);
  ~GFXcanvas1(void);
  void     drawPixel(Int16 x, Int16 y, Uint16 color),
           fillScreen(Uint16 color);
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  Uint8 *getBuffer(void) const { return buffer; }
 private:
  Uint8 *buffer;
};


/// A GFX 8-bit canvas context for graphics
class GFXcanvas8 : public Adafruit_GFX {
 public:
  GFXcanvas8(Uint16 w, Uint16 h);
  ~GFXcanvas8(void);
  void     drawPixel(Int16 x, Int16 y, Uint16 color),
           fillScreen(Uint16 color),
           writeFastHLine(Int16 x, Int16 y, Int16 w, Uint16 color);
  /**********************************************************************/
  /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  Uint8 *getBuffer(void) const { return buffer; }
 private:
  Uint8 *buffer;
};


///  A GFX 16-bit canvas context for graphics
class GFXcanvas16 : public Adafruit_GFX {
 public:
  GFXcanvas16(Uint16 w, Uint16 h);
  ~GFXcanvas16(void);
  void      drawPixel(Int16 x, Int16 y, Uint16 color),
            fillScreen(Uint16 color),
            byteSwap(void);
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  Uint16 *getBuffer(void) const { return buffer; }
 private:
  Uint16 *buffer;
};

#endif // _ADAFRUIT_GFX_H
