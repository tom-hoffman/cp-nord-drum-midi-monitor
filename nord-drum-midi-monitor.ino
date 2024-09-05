// NOTES:
// This currently listens on channel 14.
// Remember that the Nord Drum 1 uses one channel with 
// four notes specifying the four drum sounds.

// TODO:
// program labels
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_CircuitPlayground.h>
#include <USB-MIDI.h>

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();
using namespace MIDI_NAMESPACE;

// Gizmo setup

#define TFT_CS        0
#define TFT_RST       -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        1
#define TFT_BACKLIGHT PIN_A3 // Display backlight pin
// IMPORTANT!!!
// If you only get a black screen on the gizmo, check the board definition 
// under the Arduino IDE's boards manager as explained below:
// You will need to use Adafruit's CircuitPlayground Express Board Definition
// for Gizmos rather than the Arduino version since there are additional SPI
// ports exposed.
#if (SPI_INTERFACES_COUNT == 1)
  SPIClass* spi = &SPI;
#else
  SPIClass* spi = &SPI1;
#endif
Adafruit_ST7789 screen = Adafruit_ST7789(spi, TFT_CS, TFT_DC, TFT_RST);

// Color definitions
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07F1
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0 
#define WHITE     0xFFFF
#define GRAY      0xB618
#define ORANGE    0xFD00
#define DARK_GRAY 0x39CA

// Globals
const uint8_t   MIDI_LISTEN           = 1;
const uint8_t   SCREEN_WIDTH          = 240;
const uint8_t   SCREEN_HEIGHT         = 240;
const uint8_t   DOT_SPACING           = 240 / 4;
const uint8_t   DOT_Y                 = 90;
const uint8_t   DOT_SIZE              = 20;
const uint8_t   PROG_Y                = 140;

const uint32_t  DRUM_COLORS[4]        = {GREEN, RED, BLUE, YELLOW};
const uint8_t   NOTE_COUNT            = 4;
const uint8_t   NOTES[NOTE_COUNT]     = {36, 38, 59, 47}; // selection of MIDI note values

      uint32_t  last_pulse;
      uint8_t   pulse_count           = 0;
      uint8_t   ring_size[NOTE_COUNT] = {0, 0, 0, 0};
      uint16_t  tempo                 = 0;

// This is the factory soundback, categories slightly rearranged so
// none are larger than 10, and "ethno" is a little more correct.
const char SOUNDBANK[99][3][32] = {{"retro", "Monologue", "drums"},
{"rock", "Classic Vistalite", "drums"},
{"retro", "Blue House", "kit"},
{"real", "Brushford", "kit"},
{"rock", "Bebox Delux", "drums"},
{"world", "Always Hip Hop", "kit"},
{"real", "Gran Casa Timp", "kit"},
{"retro", "Thanx to Burgees", "drums"},
{"fx", "Reso Sweep", "percussion"},
{"retro", "Vince Gate", "drums"},
{"world", "UnoDosKickHat", "kit"},
{"real", "spectrum", "drums"},
{"rock", "Ateiste", "drums"},
{"retro", "Noisy Barrel Orchestra", "drums"},
{"retro", "Higgins Particle Hat", "kit"},
{"rock", "Clothed Funk Kit", "kit"},
{"world", "Komal Melodic", "percussion"},
{"world", "Lalalatin", "percussion"},
{"retro", "Bend Down Disco", "percussion"},
{"rock", "Flying Dront Circus", "kit"},
{"world", "Tribunal", "percussion"},
{"real", "King Kong Karma", "kit"},
{"fx", "Training with Kolal", "percussion"},
{"real", "Tiny Tiny Pic", "kit"},
{"world", "Red Beat", "percussion"},
{"real", "beatPerlife", "kit"},
{"retro", "Piccolosim", "percussion"},
{"real", "Acoustic Flower King", "kit"},
{"rock", "Apostasy Steam Noise", "percussion"},
{"fx", "DoReMinor Melodic", "percussion"},
{"fx", "Must Bend Tolotto", "drums"},
{"world", "Sambalasala", "percussion"},
{"fx", "Kiss the Click", "percussion"},
{"retro", "Sweep Type 4tonight", "drums"},
{"fx", "Noise Click Trap", "kit"},
{"real", "Bend Timpanic", "drums"},
{"retro", "dododrum", "kit"},
{"fx", "Fast Sweep Melodic", "drums"},
{"world", "Bella Balinese", "percussion"},
{"fx", "Noisy Royalty", "drums"},
{"world", "Steely Melodic", "drums"},
{"rock", "Soft Acoustic", "drums"},
{"real", "Tubular Bells and Triangle", "percussion"},
{"world", "Ultra Tribal Dance", "percussion"},
{"retro", "HeaHihat", "kit"},
{"fx", "Click Gate and Vinyl", "kit"},
{"real", "Double Snare", "kit"},
{"real", "Hells Bells", "percussion"},
{"real", "Rototsthile", "drums"},
{"retro", "Melodic Technocrat", "kit"},
{"rock", "Dull Dusty", "drums"},
{"fx", "Retrophile Gated Noise", "drums"},
{"world", "Real Cuba Conga Cola", "percussion"},
{"retro", "Retrograd", "drums"},
{"fx", "GasaGate", "percussion"},
{"real", "Clap Trap", "drums"},
{"real", "Krimsonite", "drums"},
{"real", "Serious Decay", "kit"},
{"world", "Dry Tribe", "drums"},
{"fx", "Cinematikino", "percussion"},
{"fx", "Toy Ambulance", "kit"},
{"retro", "Neophile", "drums"},
{"retro", "Stabby Hip Hop", "kit"},
{"real", "Retro Noise Reverb", "drums"},
{"retro", "Ulam Spiral", "drums"},
{"retro", "Sawkas Jungle Heat", "kit"},
{"retro", "Knick Knock Knack", "drums"},
{"real", "Bright Click Brush", "drums"},
{"retro", "New Romantic Tight", "kit"},
{"fx", "Intergalactic Battle", "percussion"},
{"retro", "Nosampled Drum", "drums"},
{"retro", "Poor Tone", "kit"},
{"fx", "Clicks&Pops", "drums"},
{"fx", "Fat Gated Chattanoga", "drums"},
{"real", "Retro Real Snap Snare", "drums"},
{"world", "Slitz Box", "percussion"},
{"world", "Real Tommy Steel", "percussion"},
{"fx", "Macro Sweeper", "kit"},
{"fx", "Darwin's Sex Machine", "kit"},
{"real", "Apparatorium", "drums"},
{"", "Default", ""}};      

void resetClock() {
  pulse_count = 0;
  last_pulse = millis();
}

uint8_t dotLocation(uint8_t n) {
  return (DOT_SPACING / 2) + (DOT_SPACING * n);
}

void blockDot(uint8_t n) {
  uint8_t x = dotLocation(n);
  screen.fillCircle(x, DOT_Y, 28, BLACK);
}

void drawDot(uint8_t n) {
  uint8_t x = dotLocation(n);
  screen.fillCircle(x, DOT_Y, ring_size[n], DRUM_COLORS[n]);
}

void drawRing(uint8_t n) {
  uint8_t x = dotLocation(n);
  screen.drawCircle(x, DOT_Y, ring_size[n], BLACK);
}

void updateBPM(uint32_t bpm) {
  screen.fillRect(0, 200, 240, 40, BLACK);
  screen.setFont();
  screen.setTextSize(2);
  screen.setTextColor(WHITE, BLACK);
  screen.setCursor(130, 220);

  screen.print("BPM: ");
  if (bpm) {
    screen.print(bpm);
  }
  else {
    screen.print("?");
  }
  tempo = bpm;
}

void fastUpdateDots() {
  for (int i = 0; i < NOTE_COUNT; i++) {
    if (ring_size[i] == 0) {}
    else if (ring_size[i] < 9) {
      blockDot(i);
      ring_size[i] = 0;
    }
    else { 
      ring_size[i] = ring_size[i] - 1;
    }
  }
}

void updateDots() {
  for (int i = 0; i < NOTE_COUNT; i++) {
    if (ring_size[i] > 1) {
      drawRing(i);
      ring_size[i] = ring_size[i] - 1;
    }
    else if (ring_size[i] == 1) {
      blockDot(i);
      ring_size[i] = ring_size[i] - 1;
    }
  }
}

void processNote(byte b, byte v) {
  for (int i = 0; i < NOTE_COUNT; i++) {
    if (b == NOTES[i]) {
      byte ring_mod = v / 25;
      ring_size[i] = 10 + (ring_mod * 3);
      drawDot(i);
    }
  }
}

void onClock() {
  if (tempo < 240) {
    updateDots();
  }
  else {
    fastUpdateDots();
  }
}

void onNoteOn(byte channel, byte note, byte velocity) {
  if (channel == MIDI_LISTEN) {
    processNote(note, velocity);
  }
}

void onSystemExclusive(byte * msg, unsigned length) {
  // re-combining two seven bit values into one 14 bit number.
  uint16_t first = msg[2] << 7;
  uint16_t total = first | msg[3];
  updateBPM(total);
} 

void setup() {
  Serial.begin(31250);
  MIDI.begin(MIDI_LISTEN);
  MIDI.setHandleClock(onClock);
  MIDI.setHandleSystemExclusive(onSystemExclusive);
  MIDI.setHandleNoteOn(onNoteOn);
  MIDI.setHandleProgramChange(onPC);
  CircuitPlayground.begin(1);  // arg. is neopixel brightness.
  // GIZMO hardware setup...
  screen.init(SCREEN_WIDTH, SCREEN_HEIGHT);   
  screen.setRotation(2);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
  screen.fillScreen(ST77XX_BLACK);
  screen.setCursor(0, 10);
  screen.setFont(&FreeSansBold9pt7b);
  screen.println("Modular Playground");
  screen.setFont(&FreeSans9pt7b);
  screen.setTextColor(GRAY);
  screen.println("nord drum midi monitor");
  last_pulse = millis();
  updateBPM(0);
}

void updateProgram(uint8_t p) {
    screen.fillRect(0, PROG_Y - 10, 240, 78, BLACK);
    screen.setFont(&FreeSansBold12pt7b);
    screen.setTextColor(WHITE);
    screen.setTextSize(1);
    screen.setTextWrap(false);
    screen.setCursor(0, PROG_Y + 20);
    screen.println(SOUNDBANK[p][1]);
    screen.setFont(&FreeSans12pt7b);
    screen.print(SOUNDBANK[p][0]);
    screen.print(" ");
    screen.println(SOUNDBANK[p][2]);
}

void onPC(byte channel, byte number) {
  if (channel == MIDI_LISTEN) {
    updateProgram(number);
  }
}

void loop() {
  MIDI.read();
}



