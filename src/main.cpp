#include "FastLED.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include <Adafruit_GFX.h>
#include <GFX_Lite.h>
#include "image.h"

// Define constants
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define DATA_PIN 3 // Define the data pin for FastLED

// LED matrix
CRGB myMatrix[PANEL_RES_X * PANEL_RES_Y]; // 16-bit color buffer
MatrixPanel_I2S_DMA *display = nullptr; 

// Graphics library for mirroring
GFXcanvas16 *canvas;

// Define LED array
CRGB leds[PANEL_RES_X * PANEL_RES_Y];

void setup() {
    Serial.begin(115200); // For debugging (optional)
    
    // Initialize the matrix
    HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y);
    mxconfig.gpio.e = 15;
    mxconfig.clkphase = false;
    mxconfig.latch_blanking = 1;

    display = new MatrixPanel_I2S_DMA(mxconfig);
    display->begin();
    display->setBrightness8(96); 

    // Set up FastLED
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, PANEL_RES_X * PANEL_RES_Y);

    // Configure mirroring panel
    canvas = new GFXcanvas16(display->width(), display->height());
    canvas->setRotation(2);

    // Load the image into the buffers
    memcpy(leds, image, sizeof(CRGB) * PANEL_RES_X * PANEL_RES_Y);
    memcpy(myMatrix, image, sizeof(CRGB) * PANEL_RES_X * PANEL_RES_Y);
}

void loop() {
    // 1. Render the image on the first panel
    FastLED.show();
    display->flipDMABuffer(); // Display the image

    // 2. Render mirrored image on the second panel using the canvas
    canvas->fillScreen(0); 
    for (int y = 0; y < display->height(); y++) {
        for (int x = 0; x < display->width(); x++) {
            CRGB pixel = leds[y * display->width() + x];
            canvas->drawPixel(x, y, pixel.r << 8 | pixel.g << 3 | pixel.b >> 3); // Direct color manipulation for mirroring
        }
    }
    display->flipDMABuffer();

    // Add a small delay to prevent unnecessary CPU usage
    delay(100);
}
