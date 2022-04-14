#ifndef DISPLAY_H
#define DISPLAY_H


#include <Arduino.h>
#include "main.h"
#include "ThermalCamera.h"
#include "GradientPalette_Structs.h"
#include "interpolation.h"
#include "GBlur.h"
#include <Adafruit_GFX.h>
//#include <Adafruit_ST7789.h> // uncomment to use ST7789, comment to use ILI9341
#include <Adafruit_ILI9341.h> // uncomment to use ILI9341, comment to use ST7789

#define ST77XX_ILI9341_CMD_COLMOD 0x3A

// following used to simplify generation of constant colors and the constants are generated by the compiler
// and thus don't have to use the tft.color565 function
#define TORGB565(red,green,blue) (((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3))
#define COLOR_RED TORGB565(0xFF,0,0)
#define COLOR_GREEN TORGB565(0, 0xFF, 0)
#define COLOR_BLUE TORGB565(0,0,0xFF)
#define COLOR_YELLOW TORGB565(0xFF,0xFF,0)
#define COLOR_CYAN TORGB565(0,0xFF,0xFF)
#define COLOR_MAGENTA TORGB565(0xFF,0,0xFF)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

namespace Display
{
    GBlur gblur;
    
#if defined(_ADAFRUIT_ST7789H_)
    #define INTERPOLATED_COLS_DEFAULT 224
    #define INTERPOLATED_ROWS_DEFAULT 168
    #define MAX_MID_MIN_TEXTS_Y_POS   180
    #define MAX_MID_MIN_TEXTS_SIZE    2
    #define COLOR_PALETTE_Y_POS       200
    Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);
#elif defined(_ADAFRUIT_ILI9341H_)
    #define INTERPOLATED_COLS_DEFAULT 288
    #define INTERPOLATED_ROWS_DEFAULT 208
    #define MAX_MID_MIN_TEXTS_Y_POS   210
    #define MAX_MID_MIN_TEXTS_SIZE    1
    #define COLOR_PALETTE_Y_POS       219
    Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS);
#endif
    uint16_t INTERPOLATED_COLS = INTERPOLATED_COLS_DEFAULT;
    uint16_t INTERPOLATED_ROWS = INTERPOLATED_ROWS_DEFAULT;

#if defined(_ADAFRUIT_ILI9341H_) || defined(_ADAFRUIT_ST7789H_)
    typedef enum {
#ifdef _ADAFRUIT_ST7789H_
        RGB444 = 0x03,
#endif
        RGB565 = 0x05,
        RGB666 = 0x06
    }ST77XX_ILI9341_ColorMode;
    void setColorMode(ST77XX_ILI9341_ColorMode mode)
    {
        uint8_t _mode = ((uint8_t)mode);
        tft.sendCommand(ST77XX_ILI9341_CMD_COLMOD, &_mode, 1);
    }
#endif

    void Init()
    {

#if defined(_ADAFRUIT_ST7789H_)
        tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
        tft.enableDisplay(true);
        tft.setRotation(2);
#elif defined(_ADAFRUIT_ILI9341H_)
        //tft.begin(10000000); // 10MHz from datasheet spec.
        tft.begin(20000000); // 20MHz overclock
        //tft.begin(62500000); // 62.5MHz super overclock
        tft.setRotation(1);
#endif
        //setColorMode(tft, ST77XX_ColorMode::RGB565);

        tft.setTextWrap(false); // Allow text to run off right edge
        tft.fillScreen(COLOR_BLACK);
        
    }

    /*
    * note this don't take care of clipping
    * or out of boundaries
    * so special care must be taken when using it
    */
    void drawRGBBitmap(int16_t x, int16_t y, CRGB *pcolors,
                                        int16_t w, int16_t h)
    {
        tft.startWrite();
        tft.setAddrWindow(x, y, w, h);
        for (int16_t r=0; r<h; r++) {
            for (int16_t c=0; c<w; c++) {
                int index = r*w + c;
                tft.SPI_WRITE16(pcolors[index].toRGB565());
            }
        }
        tft.endWrite();
    }

    void printMaxMin()
    {
        // clear prev text
        tft.fillRect(0, MAX_MID_MIN_TEXTS_Y_POS, COLOR_PALETTE_COUNT, MAX_MID_MIN_TEXTS_SIZE*7, COLOR_BLACK);
        //tft.fillRect(180, 180, 60, 14, COLOR_BLACK);

        // print new text
        tft.setTextSize(MAX_MID_MIN_TEXTS_SIZE);
        tft.setTextColor(COLOR_WHITE);
        tft.setCursor(0, MAX_MID_MIN_TEXTS_Y_POS);
        tft.print(ThermalCamera::minTemp);
        tft.setCursor(COLOR_PALETTE_COUNT/2-2.5*6*MAX_MID_MIN_TEXTS_SIZE, MAX_MID_MIN_TEXTS_Y_POS);
        tft.print((ThermalCamera::maxTemp-ThermalCamera::minTemp)/2+ThermalCamera::minTemp);
        tft.setCursor(COLOR_PALETTE_COUNT-5*6*MAX_MID_MIN_TEXTS_SIZE, MAX_MID_MIN_TEXTS_Y_POS);
        tft.print(ThermalCamera::maxTemp);
    }

    void print_temperatures()
    {
        tft.startWrite();
        tft.setAddrWindow(0, 0, INTERPOLATED_COLS, INTERPOLATED_ROWS);
        for (int16_t r=0; r<INTERPOLATED_ROWS; r++) {
            for (int16_t c=INTERPOLATED_COLS-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
                int index = r*INTERPOLATED_COLS + c;
                float t = Main::dest_2d[index];
                uint16_t colorIndex = constrain(map(t, ThermalCamera::minTemp, ThermalCamera::maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
                uint16_t color = Main::camColors[colorIndex].toRGB565();

                tft.SPI_WRITE16(color);
            }
        }
        
        tft.endWrite();
    }

    void printNonInterpolated()
    {
        for (uint8_t h=0; h<24; h++) {
            for (uint8_t w=0; w<32; w++) {
                float t = ThermalCamera::frame[h*32 + w];
                uint8_t colorIndex = map(t, ThermalCamera::minTemp, ThermalCamera::maxTemp, 0, COLOR_PALETTE_COUNT-1);
                colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
                tft.fillRect((31-w)*7, h*7, 7, 7, Main::camColors[colorIndex].toRGB565());
            }
        }
    }

    void printCurrentGradientColorPalette()
    {
        // print the colormap name
        tft.fillRect(120, 230, 100, 7, COLOR_BLACK);
        tft.setTextSize(1);
        tft.setTextColor(COLOR_WHITE);
        tft.setCursor(120, 230);
        tft.print(GradientPalettes::Def[Main::currentColorMapIndex].name);

        // draw the temp-range colormap 
        for (int i=0;i<7;i++)
            drawRGBBitmap(0, COLOR_PALETTE_Y_POS+i, Main::camColors, COLOR_PALETTE_COUNT, 1);
    }

    void printFps(float fps)
    {
        tft.setTextSize(1);
        tft.fillRect(0, 230, 8*6, 7, COLOR_BLACK);
        tft.setCursor(0, 230);
        tft.print("fps:");
        tft.print(fps);
    }

    void printStatusMsg(int read_status)
    {
        //tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
        if (read_status != 0)
        {
            Serial.printf("log Failed:%d", read_status);
            tft.fillRect(0, 220, 240, 14, COLOR_BLACK);
            tft.setTextSize(2);
            tft.setTextColor(COLOR_RED);
            tft.setCursor(0, 220);
            tft.printf("MLX Failed:%d", read_status);
        }
    }

    float gblurTemp[32*2*24*2];
    void execInterpolate()
    {
        //t = millis();
        gblur.calculate(ThermalCamera::frame, gblurTemp);//, 32, 24);
        interpolate_image(gblurTemp, 24*2, 32*2, Main::dest_2d, INTERPOLATED_ROWS/2, INTERPOLATED_COLS/2);
        interpolate_image(Main::dest_2d, INTERPOLATED_ROWS/2, INTERPOLATED_COLS/2, gblurTemp, 24, 32);
        interpolate_image(gblurTemp, 24, 32, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
        //interpolate_image(ThermalCamera::frame, 24, 32, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
        //interpolate_image(Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS, gblurTemp, 24*2, 32*2);
        //interpolate_image(gblurTemp, 24*2, 32*2, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);

        //Serial.print("Interpolation took "); Serial.print(millis()-t); Serial.println(" ms");
    }

    
    void print_BiqubicInterpolated()
    {
        //execInterpolate();
        //t = millis();
        printMaxMin();
        print_temperatures(); // this takes almost the same time ~34mS as the following two, but would not require additional ram usage
        //Serial.print("Interpolation draw took "); Serial.print(millis()-t); Serial.println(" ms");
    }

    /* not used in current design but keep it here just in case
    * as this will just consume unnecessary RAM memory
    void convertToImage(float *src, uint16_t *dest, uint8_t rows, uint8_t cols)
    {
        for (uint8_t r=0; r<rows; r++) {
            for (uint8_t c=0; c<cols; c++) {
                int index = r*cols + c;
                float t = src[index];
                uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
                colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
                dest[index] = camColors[colorIndex].toRGB565();
            }
        }
    }*/

}

#endif