/*!
 * @file Adafruit_SPITFT.h
 *
 * Part of Adafruit's GFX graphics library. Originally this class was
 * written to handle a range of color TFT displays connected via SPI,
 * but over time this library and some display-specific subclasses have
 * mutated to include some color OLEDs as well as parallel-interfaced
 * displays. The name's been kept for the sake of older code.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries,
 * with contributions from the open source community.
 *
 * BSD license, all text here must be included in any redistribution.
 */

#ifndef _ADAFRUIT_SPITFT_H_
#define _ADAFRUIT_SPITFT_H_

#if !defined(__AVR_ATtiny85__) // Not for ATtiny, at all

//#include <SPI.h>
#include "Adafruit_GFX.h"
#include "usbstk5515_gpio.h"

#define OUTPUT 1
#define HIGH 1
#define LOW 0
#define INPUT 0

// HARDWARE CONFIG ---------------------------------------------------------

#if defined(__AVR__)
 typedef Uint8  PORT_t;            ///< PORT values are 8-bit
 #define USE_FAST_PINIO              ///< Use direct PORT register access
#elif defined(ARDUINO_STM32_FEATHER) // WICED
 typedef class HardwareSPI SPIClass; ///< SPI is a bit odd on WICED
 typedef Uint32 PORT_t;            ///< PORT values are 32-bit
#elif defined(__arm__)
 #if defined(ARDUINO_ARCH_SAMD)
  // Adafruit M0, M4
  typedef Uint32 PORT_t;           ///< PORT values are 32-bit
  #define USE_FAST_PINIO             ///< Use direct PORT register access
  #define HAS_PORT_SET_CLR           ///< PORTs have set & clear registers
 #elif defined(CORE_TEENSY)
  // PJRC Teensy 3.x
  typedef Uint8 PORT_t;            ///< PORT values are 8-bit
  #define USE_FAST_PINIO             ///< Use direct PORT register access
  #define HAS_PORT_SET_CLR           ///< PORTs have set & clear registers
 #else
  // Arduino Due?
  typedef Uint32 PORT_t;           ///< PORT values are 32-bit
  // USE_FAST_PINIO not available here (yet)...Due has a totally different
  // GPIO register set and will require some changes elsewhere (e.g. in
  // constructors especially).
 #endif
#else // !ARM
 // Probably ESP8266 or ESP32. USE_FAST_PINIO is not available here (yet)
 // but don't worry about it too much...the digitalWrite() implementation
 // on these platforms is reasonably efficient and already RAM-resident,
 // only gotcha then is no parallel connection support for now.
 typedef Uint32 PORT_t;            ///< PORT values are 32-bit
#endif // end !ARM
typedef volatile  PORT_t* PORTreg_t; ///< PORT register type

#if defined(__AVR__)
 #define DEFAULT_SPI_FREQ  8000000L  ///< Hardware SPI default speed
#else
 #define DEFAULT_SPI_FREQ 16000000L  ///< Hardware SPI default speed
#endif

#if defined(ADAFRUIT_PYPORTAL) || defined(ADAFRUIT_PYBADGE_M4_EXPRESS) || defined(ADAFRUIT_PYGAMER_M4_EXPRESS)
 #define USE_SPI_DMA                 ///< Auto DMA if using PyPortal
#else
 //#define USE_SPI_DMA               ///< If set, use DMA if available
#endif
// Another "oops" name -- this now also handles parallel DMA.
// If DMA is enabled, Arduino sketch MUST #include <Adafruit_ZeroDMA.h>
// Estimated RAM usage:
// 4 bytes/pixel on display major axis + 8 bytes/pixel on minor axis,
// e.g. 320x240 pixels = 320 * 4 + 240 * 8 = 3,200 bytes.

#if !defined(ARDUINO_ARCH_SAMD)
 #undef USE_SPI_DMA                  ///< DMA currently for SAMD chips only
#endif

#if defined(USE_SPI_DMA)
 #pragma message ("GFX DMA IS ENABLED. HIGHLY EXPERIMENTAL.")
 #include <Adafruit_ZeroDMA.h>
#endif

// This is kind of a kludge. Needed a way to disambiguate the software SPI
// and parallel constructors via their argument lists. Originally tried a
// Bool as the first argument to the parallel constructor (specifying 8-bit
// vs 16-bit interface) but the compiler regards this as equivalent to an
// integer and thus still ambiguous. SO...the parallel constructor requires
// an enumerated type as the first argument: tft8 (for 8-bit parallel) or
// tft16 (for 16-bit)...even though 16-bit isn't fully implemented or tested
// and might never be, still needed that disambiguation from soft SPI.
enum tftBusWidth { tft8bitbus, tft16bitbus }; ///< For first arg to parallel constructor

// CLASS DEFINITION --------------------------------------------------------

/*!
  @brief  Adafruit_SPITFT is an intermediary class between Adafruit_GFX
          and various hardware-specific subclasses for different displays.
          It handles certain operations that are common to a range of
          displays (address window, area fills, etc.). Originally these were
          all color TFT displays interfaced via SPI, but it's since expanded
          to include color OLEDs and parallel-interfaced TFTs. THE NAME HAS
          BEEN KEPT TO AVOID BREAKING A LOT OF SUBCLASSES AND EXAMPLE CODE.
          Many of the class member functions similarly live on with names
          that don't necessarily accurately describe what they're doing,
          again to avoid breaking a lot of other code. If in doubt, read
          the comments.
*/
class Adafruit_SPITFT : public Adafruit_GFX {

  public:

    // CONSTRUCTORS --------------------------------------------------------

    // Software SPI constructor: expects width & height (at default rotation
    // setting 0), 4 signal pins (cs, dc, mosi, sclk), 2 optional pins
    // (reset, miso). cs argument is required but can be -1 if unused --
    // rather than moving it to the optional arguments, it was done this way
    // to avoid breaking existing code (-1 option was a later addition).
    Adafruit_SPITFT(Uint16 w, Uint16 h,
      Int8 cs, Int8 dc, Int8 mosi, Int8 sck,
      Int8 rst = -1, Int8 miso = -1);

    // Hardware SPI constructor using the default SPI port: expects width &
    // height (at default rotation setting 0), 2 signal pins (cs, dc),
    // optional reset pin. cs is required but can be -1 if unused -- rather
    // than moving it to the optional arguments, it was done this way to
    // avoid breaking existing code (-1 option was a later addition).
    Adafruit_SPITFT(Uint16 w, Uint16 h,
      Int8 cs, Int8 dc, Int8 rst = -1);

//#if !defined(ESP8266) // See notes in .cpp
//    // Hardware SPI constructor using an arbitrary SPI peripheral: expects
//    // width & height (rotation 0), SPIClass pointer, 2 signal pins (cs, dc)
//    // and optional reset pin. cs is required but can be -1 if unused.
//    Adafruit_SPITFT(Uint16 w, Uint16 h, SPIClass *spiClass,
//      Int8 cs, Int8 dc, Int8 rst = -1);
//#endif // end !ESP8266

    // Parallel constructor: expects width & height (rotation 0), flag
    // indicating whether 16-bit (true) or 8-bit (false) interface, 3 signal
    // pins (d0, wr, dc), 3 optional pins (cs, rst, rd). 16-bit parallel
    // isn't even fully implemented but the 'wide' flag was added as a
    // required argument to avoid ambiguity with other constructors.
    Adafruit_SPITFT(Uint16 w, Uint16 h, tftBusWidth busWidth,
      Int8 d0, Int8 wr, Int8 dc,
      Int8 cs = -1, Int8 rst = -1, Int8 rd = -1);

    // CLASS MEMBER FUNCTIONS ----------------------------------------------

    // These first two functions MUST be declared by subclasses:

    /*!
        @brief  Display-specific initialization function.
        @param  freq  SPI frequency, in hz (or 0 for default or unused).
    */
    virtual void begin(Uint32 freq) = 0;

    /*!
        @brief  Set up the specific display hardware's "address window"
                for subsequent pixel-pushing operations.
        @param  x  Leftmost pixel of area to be drawn (MUST be within
                   display bounds at current rotation setting).
        @param  y  Topmost pixel of area to be drawn (MUST be within
                   display bounds at current rotation setting).
        @param  w  Width of area to be drawn, in pixels (MUST be >0 and,
                   added to x, within display bounds at current rotation).
        @param  h  Height of area to be drawn, in pixels (MUST be >0 and,
                   added to x, within display bounds at current rotation).
    */
    virtual void setAddrWindow(
                   Uint16 x, Uint16 y, Uint16 w, Uint16 h) = 0;

    // Remaining functions do not need to be declared in subclasses
    // unless they wish to provide hardware-specific optimizations.
    // Brief comments here...documented more thoroughly in .cpp file.

    // Subclass' begin() function invokes this to initialize hardware.
    // freq=0 to use default SPI speed. spiMode must be one of the SPI_MODEn
    // values defined in SPI.h, which are NOT the same as 0 for SPI_MODE0,
    // 1 for SPI_MODE1, etc...use ONLY the SPI_MODEn defines! Only!
    // Name is outdated (interface may be parallel) but for compatibility:
    void         initSPI(Uint32 freq, Uint8 spiMode);
    // Chip select and/or hardware SPI transaction start as needed:
    void         startWrite(void);
    // Chip deselect and/or hardware SPI transaction end as needed:
    void         endWrite(void);
    void         sendCommand(Uint8 commandByte, Uint8 *dataBytes = NULL, Uint8 numDataBytes = 0);
    void         sendCommand(Uint8 commandByte, const Uint8 *dataBytes, Uint8 numDataBytes);
    Uint8      readcommand8(Uint8 commandByte, Uint8 index = 0);

    // These functions require a chip-select and/or SPI transaction
    // around them. Higher-level graphics primitives might start a
    // single transaction and then make multiple calls to these functions
    // (e.g. circle or text rendering might make repeated lines or rects)
    // before ending the transaction. It's more efficient than starting a
    // transaction every time.
    void         writePixel(Int16 x, Int16 y, Uint16 color);
    void         writePixels(Uint16 *colors, Uint32 len,
                   Bool block=true, Bool bigEndian=false);
    void         writeColor(Uint16 color, Uint32 len);
    void         writeFillRect(Int16 x, Int16 y, Int16 w, Int16 h,
                   Uint16 color);
    void         writeFastHLine(Int16 x, Int16 y, Int16 w,
                   Uint16 color);
    void         writeFastVLine(Int16 x, Int16 y, Int16 h,
                   Uint16 color);
    // This is a new function, similar to writeFillRect() except that
    // all arguments MUST be onscreen, sorted and clipped. If higher-level
    // primitives can handle their own sorting/clipping, it avoids repeating
    // such operations in the low-level code, making it potentially faster.
    // CALLING THIS WITH UNCLIPPED OR NEGATIVE VALUES COULD BE DISASTROUS.
    inline void  writeFillRectPreclipped(Int16 x, Int16 y,
                   Int16 w, Int16 h, Uint16 color);
    // Another new function, companion to the new non-blocking
    // writePixels() variant.
    void dmaWait(void);


    // These functions are similar to the 'write' functions above, but with
    // a chip-select and/or SPI transaction built-in. They're typically used
    // solo -- that is, as graphics primitives in themselves, not invoked by
    // higher-level primitives (which should use the functions above).
    void         drawPixel(Int16 x, Int16 y, Uint16 color);
    void         fillRect(Int16 x, Int16 y, Int16 w, Int16 h,
                   Uint16 color);
    void         drawFastHLine(Int16 x, Int16 y, Int16 w,
                   Uint16 color);
    void         drawFastVLine(Int16 x, Int16 y, Int16 h,
                   Uint16 color);
    // A single-pixel push encapsulated in a transaction. I don't think
    // this is used anymore (BMP demos might've used it?) but is provided
    // for backward compatibility, consider it deprecated:
    void         pushColor(Uint16 color);

    using        Adafruit_GFX::drawRGBBitmap; // Check base class first
    void         drawRGBBitmap(Int16 x, Int16 y,
                   Uint16 *pcolors, Int16 w, Int16 h);

    void         invertDisplay(Bool i);
    Uint16     color565(Uint8 r, Uint8 g, Uint8 b);

    // Despite parallel additions, function names kept for compatibility:
    void         spiWrite(Uint8 b);       // Write single byte as DATA
    void         writeCommand(Uint8 cmd); // Write single byte as COMMAND
    Uint8      spiRead(void);             // Read single byte of data

    // Most of these low-level functions were formerly macros in
    // Adafruit_SPITFT_Macros.h. Some have been made into inline functions
    // to avoid macro mishaps. Despite the addition of code for a parallel
    // display interface, the names have been kept for backward
    // compatibility (some subclasses may be invoking these):
    void         SPI_WRITE16(Uint16 w); // Not inline
    void         SPI_WRITE32(Uint32 l); // Not inline
    // Old code had both a spiWrite16() function and SPI_WRITE16 macro
    // in addition to the SPI_WRITE32 macro. The latter two have been
    // made into functions here, and spiWrite16() removed (use SPI_WRITE16()
    // instead). It looks like most subclasses had gotten comfortable with
    // SPI_WRITE16 and SPI_WRITE32 anyway so those names were kept rather
    // than the less-obnoxious camelcase variants, oh well.

    // Placing these functions entirely in the class definition inlines
    // them implicitly them while allowing their use in other code:

    /*!
        @brief  Set the chip-select line HIGH. Does NOT check whether CS pin
                is set (>=0), that should be handled in calling function.
                Despite function name, this is used even if the display
                connection is parallel.
    */
    void SPI_CS_HIGH(void) {
    #if defined(USE_FAST_PINIO)
     #if defined(HAS_PORT_SET_CLR)
      #if defined(KINETISK)
        *csPortSet = 1;
      #else  // !KINETISK
        *csPortSet = csPinMask;
      #endif // end !KINETISK
     #else  // !HAS_PORT_SET_CLR
        *csPort   |= csPinMaskSet;
     #endif // end !HAS_PORT_SET_CLR
    #else  // !USE_FAST_PINIO
        USBSTK5515_GPIO_setOutput(_cs, HIGH);
    #endif // end !USE_FAST_PINIO
    }

    /*!
        @brief  Set the chip-select line LOW. Does NOT check whether CS pin
                is set (>=0), that should be handled in calling function.
                Despite function name, this is used even if the display
                connection is parallel.
    */
    void SPI_CS_LOW(void) {
    #if defined(USE_FAST_PINIO)
     #if defined(HAS_PORT_SET_CLR)
      #if defined(KINETISK)
        *csPortClr = 1;
      #else  // !KINETISK
        *csPortClr = csPinMask;
      #endif // end !KINETISK
     #else  // !HAS_PORT_SET_CLR
        *csPort   &= csPinMaskClr;
     #endif // end !HAS_PORT_SET_CLR
    #else  // !USE_FAST_PINIO
        USBSTK5515_GPIO_setOutput(_cs, LOW);
    #endif // end !USE_FAST_PINIO
    }

    /*!
        @brief  Set the data/command line HIGH (data mode).
    */
    void SPI_DC_HIGH(void) {
    #if defined(USE_FAST_PINIO)
     #if defined(HAS_PORT_SET_CLR)
      #if defined(KINETISK)
        *dcPortSet = 1;
      #else  // !KINETISK
        *dcPortSet = dcPinMask;
      #endif // end !KINETISK
     #else  // !HAS_PORT_SET_CLR
        *dcPort   |= dcPinMaskSet;
     #endif // end !HAS_PORT_SET_CLR
    #else  // !USE_FAST_PINIO
        USBSTK5515_GPIO_setOutput(_dc, HIGH);
    #endif // end !USE_FAST_PINIO
    }

    /*!
        @brief  Set the data/command line LOW (command mode).
    */
    void SPI_DC_LOW(void) {
    #if defined(USE_FAST_PINIO)
     #if defined(HAS_PORT_SET_CLR)
      #if defined(KINETISK)
        *dcPortClr = 1;
      #else  // !KINETISK
        *dcPortClr = dcPinMask;
      #endif // end !KINETISK
     #else  // !HAS_PORT_SET_CLR
        *dcPort   &= dcPinMaskClr;
     #endif // end !HAS_PORT_SET_CLR
    #else  // !USE_FAST_PINIO
        USBSTK5515_GPIO_setOutput(_dc, LOW);
    #endif // end !USE_FAST_PINIO
    }

  protected:

    // A few more low-level member functions -- some may have previously
    // been macros. Shouldn't have a need to access these externally, so
    // they've been moved to the protected section. Additionally, they're
    // declared inline here and the code is in the .cpp file, since outside
    // code doesn't need to see these.
    inline void  SPI_MOSI_HIGH(void);
    inline void  SPI_MOSI_LOW(void);
    inline void  SPI_SCK_HIGH(void);
    inline void  SPI_SCK_LOW(void);
    inline Bool  SPI_MISO_READ(void);
    inline void  SPI_BEGIN_TRANSACTION(void);
    inline void  SPI_END_TRANSACTION(void);
    inline void  TFT_WR_STROBE(void); // Parallel interface write strobe
    inline void  TFT_RD_HIGH(void);   // Parallel interface read high
    inline void  TFT_RD_LOW(void);    // Parallel interface read low

    // CLASS INSTANCE VARIABLES --------------------------------------------

    // Here be dragons! There's a big union of three structures here --
    // one each for hardware SPI, software (bitbang) SPI, and parallel
    // interfaces. This is to save some memory, since a display's connection
    // will be only one of these. The order of some things is a little weird
    // in an attempt to get values to align and pack better in RAM.

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
    PORTreg_t     csPortSet;       ///< PORT register for chip select SET
    PORTreg_t     csPortClr;       ///< PORT register for chip select CLEAR
    PORTreg_t     dcPortSet;       ///< PORT register for data/command SET
    PORTreg_t     dcPortClr;       ///< PORT register for data/command CLEAR
#else  // !HAS_PORT_SET_CLR
    PORTreg_t     csPort;          ///< PORT register for chip select
    PORTreg_t     dcPort;          ///< PORT register for data/command
#endif // end HAS_PORT_SET_CLR
#endif // end USE_FAST_PINIO
#if defined(__cplusplus) && (__cplusplus >= 201100)
    union {
#endif
//      struct {                     //   Values specific to HARDWARE SPI:
//        SPIClass   *_spi;          ///< SPI class pointer
//#if defined(SPI_HAS_TRANSACTION)
//        SPISettings settings;      ///< SPI transaction settings
//#else
//        Uint32    _freq;         ///< SPI bitrate (if no SPI transactions)
//#endif
//        Uint32    _mode;         ///< SPI data mode (transactions or no)
//      } hwspi;                     ///< Hardware SPI values
      struct {                     //   Values specific to SOFTWARE SPI:
#if defined(USE_FAST_PINIO)
        PORTreg_t misoPort;        ///< PORT (PIN) register for MISO
#if defined(HAS_PORT_SET_CLR)
        PORTreg_t mosiPortSet;     ///< PORT register for MOSI SET
        PORTreg_t mosiPortClr;     ///< PORT register for MOSI CLEAR
        PORTreg_t sckPortSet;      ///< PORT register for SCK SET
        PORTreg_t sckPortClr;      ///< PORT register for SCK CLEAR
 #if !defined(KINETISK)
        PORT_t    mosiPinMask;     ///< Bitmask for MOSI
        PORT_t    sckPinMask;      ///< Bitmask for SCK
 #endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
        PORTreg_t mosiPort;        ///< PORT register for MOSI
        PORTreg_t sckPort;         ///< PORT register for SCK
        PORT_t    mosiPinMaskSet;  ///< Bitmask for MOSI SET (OR)
        PORT_t    mosiPinMaskClr;  ///< Bitmask for MOSI CLEAR (AND)
        PORT_t    sckPinMaskSet;   ///< Bitmask for SCK SET (OR bitmask)
        PORT_t    sckPinMaskClr;   ///< Bitmask for SCK CLEAR (AND)
#endif // end HAS_PORT_SET_CLR
 #if !defined(KINETISK)
        PORT_t    misoPinMask;     ///< Bitmask for MISO
 #endif // end !KINETISK
#endif // end USE_FAST_PINIO
        Int8    _mosi;           ///< MOSI pin #
        Int8    _miso;           ///< MISO pin #
        Int8    _sck;            ///< SCK pin #
      } swspi;                     ///< Software SPI values
      struct {                     //   Values specific to 8-bit parallel:
#if defined(USE_FAST_PINIO)
        volatile Uint8 *writePort; ///< PORT register for DATA WRITE
        volatile Uint8 *readPort;  ///< PORT (PIN) register for DATA READ
#if defined(HAS_PORT_SET_CLR)
        // Port direction register pointers are always 8-bit regardless of
        // PORTreg_t -- even if 32-bit port, we modify a byte-aligned 8 bits.
        volatile Uint8 *dirSet;  ///< PORT byte data direction SET
        volatile Uint8 *dirClr;  ///< PORT byte data direction CLEAR
        PORTreg_t wrPortSet;       ///< PORT register for write strobe SET
        PORTreg_t wrPortClr;       ///< PORT register for write strobe CLEAR
        PORTreg_t rdPortSet;       ///< PORT register for read strobe SET
        PORTreg_t rdPortClr;       ///< PORT register for read strobe CLEAR
 #if !defined(KINETISK)
        PORT_t    wrPinMask;       ///< Bitmask for write strobe
 #endif // end !KINETISK
        PORT_t    rdPinMask;       ///< Bitmask for read strobe
#else  // !HAS_PORT_SET_CLR
        // Port direction register pointer is always 8-bit regardless of
        // PORTreg_t -- even if 32-bit port, we modify a byte-aligned 8 bits.
        volatile Uint8 *portDir; ///< PORT direction register
        PORTreg_t wrPort;          ///< PORT register for write strobe
        PORTreg_t rdPort;          ///< PORT register for read strobe
        PORT_t    wrPinMaskSet;    ///< Bitmask for write strobe SET (OR)
        PORT_t    wrPinMaskClr;    ///< Bitmask for write strobe CLEAR (AND)
        PORT_t    rdPinMaskSet;    ///< Bitmask for read strobe SET (OR)
        PORT_t    rdPinMaskClr;    ///< Bitmask for read strobe CLEAR (AND)
#endif // end HAS_PORT_SET_CLR
#endif // end USE_FAST_PINIO
        Int8    _d0;             ///< Data pin 0 #
        Int8    _wr;             ///< Write strobe pin #
        Int8    _rd;             ///< Read strobe pin # (or -1)
        Bool    wide;        ///< If true, is 16-bit interface
      } tft8;                      ///< Parallel interface settings
#if defined(__cplusplus) && (__cplusplus >= 201100)
    };                             ///< Only one interface is active
#endif
#if defined(USE_SPI_DMA) // Used by hardware SPI and tft8
    Adafruit_ZeroDMA dma;                  ///< DMA instance
    DmacDescriptor  *dptr          = NULL; ///< 1st descriptor
    DmacDescriptor  *descriptor    = NULL; ///< Allocated descriptor list
    Uint16        *pixelBuf[2];          ///< Working buffers
    Uint16         maxFillLen;           ///< Max pixels per DMA xfer
    Uint16         lastFillColor = 0;    ///< Last color used w/fill
    Uint32         lastFillLen   = 0;    ///< # of pixels w/last fill
    Uint8          onePixelBuf;          ///< For hi==lo fill
#endif
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
 #if !defined(KINETISK)
    PORT_t        csPinMask;       ///< Bitmask for chip select
    PORT_t        dcPinMask;       ///< Bitmask for data/command
 #endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    PORT_t        csPinMaskSet;    ///< Bitmask for chip select SET (OR)
    PORT_t        csPinMaskClr;    ///< Bitmask for chip select CLEAR (AND)
    PORT_t        dcPinMaskSet;    ///< Bitmask for data/command SET (OR)
    PORT_t        dcPinMaskClr;    ///< Bitmask for data/command CLEAR (AND)
#endif // end HAS_PORT_SET_CLR
#endif // end USE_FAST_PINIO
    Uint8       connection;      ///< TFT_HARD_SPI, TFT_SOFT_SPI, etc.
    Int8        _rst;            ///< Reset pin # (or -1)
    Int8        _cs;             ///< Chip select pin # (or -1)
    Int8        _dc;             ///< Data/command pin #

    Int16       _xstart;   ///< Internal framebuffer X offset
    Int16       _ystart;   ///< Internal framebuffer Y offset
    Uint8       invertOnCommand; ///< Command to enable invert mode
    Uint8       invertOffCommand; ///< Command to disable invert mode

    Uint32      _freq;       ///< Dummy var to keep subclasses happy
};

#endif // end __AVR_ATtiny85__
#endif // end _ADAFRUIT_SPITFT_H_
