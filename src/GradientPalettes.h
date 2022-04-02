//#include "HTMLColors.h"
#include <Arduino.h>

struct CRGB {
    union {
		struct {
            union {
                uint8_t padding;
            };
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
		uint32_t raw;
	};

    inline uint16_t toRGB565() const {
        return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
    }

    // default values are UNINITIALIZED
    inline CRGB() __attribute__((always_inline)) = default;
    /// allow construction from R, G, B
    inline CRGB( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
        : r(ir), g(ig), b(ib)
    {
    }

    /// allow construction from 32-bit (really 24-bit) bit 0xRRGGBB color code
    inline CRGB( uint32_t colorcode)  __attribute__((always_inline))
    : raw(colorcode)//r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
    {
    }
};

struct GradientPaletteItem {
    float procent;
    const struct CRGB color;
};

struct GradientPaletteDef {
    char *name;
    uint16_t itemCount;
};

uint16_t getIndex(uint16_t gpDefIndex)
{
    uint16_t index = 0;
    for (int i=0;i<GP_Def_Count;i++)
    {
        if (i == gpDefIndex) return index;
        index += GP_Def[i].itemCount;
    }
    return 0;
}

// this defines all the names and the sizes
// to get a specific index where to find the first
// data belonging to a specific GradientPalette
// this array needs to get throught every item 
// up to the one to find
// to calculate the GP_Data index
const uint16_t GP_Def_Count = 2;
const struct GradientPaletteDef GP_Def[] = {
    {"Iron Bow", 6},
    {"Rain Bow 0", 7},
    {"arctic",7},
    {"blackHot",2},
    {"blueRed",7},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},
    {"",},

};


const struct GradientPaletteItem GP_Data[] = {
    // IronBow
        {0, {0x00,0x00,0x00}}, // black
        {10.0f, 0x20008C}, // dark blue
        {35.0f, 0xCC0077}, //magenta red
        {70.0f, 0xFFA500}, // orange
        {85.0f, 0xFFD700}, // gold
        {100.0f,0xFFFFFF},  // white
    // RainBow0
        {0, 0x000000},
        {100/6, 0x0000FF},
        {200/6, 0x00FFFF},
        {300/6, 0x00FF00},
        {400/6, 0xFFFF00},
        {500/6, 0xFF0000},
        {100, 0xFFFFFF},

    // arctic[
        {0, {15,16,146}},
        {11, {15,34,239}},   // 66/6  = 11
        {29, {54,227,232}}, // 168/6 = 28
        {50, {107,101,97}}, // 300/6 = 50
        {72, {237,101,13}}, // 432/6 = 72
        {87, {235,202,13}}, // 522/6 = 87
        {100, {235,230,194}},
    //],

    //blackHot:[
        {0, {235,235,235}},
        {100, {16,16,16}},
    //],

    //blueRed:[
        {0, {19,64,206}},
        {15, {32,141,232}},
        {34, {83,225,161}},
        {50, {156,227,87}},
        {67, {230,147,33}},
        {85, {207,64,16}},
        {100, {126,28,15}},

    //],

    //coldest:[
        {0, {15,15,239}}, // blue
        {13.5, {15,15,239}}, // blue
        {13.5, {45,45,45}}, // dark-grey
        {100, {255,255,255}}, // white
    //],

    //contrast:[
        {0, {16,16,16}}, // black-ish
        {99/8, {217,14,216}}, // magenta-ish
        {195/8, {15,15,149}}, // blue-ish
        {290/8, {12,214,214}}, // cyan-ish
        {386/8, {15,84,15}}, // green-ish
        {492/8, {213,215,6}}, // yellow-ish
        {595/8, {121,15,15}}, // darkred-ish
        {695/8, {203,55,55}}, // lightred-ish
        {100, {255,255,255}}, // white
    //],

    //doubleRainbow:[
        {0, {18,15,18}}, // black-ish
        {75/12, {155,42,136}}, // magenta-ish
        {165/12, {59,103,238}}, // blue-ish
        {250/12, {27,153,75}}, // green-ish
        {318/12, {194,193,13}}, // yellow-ish
        {505/12, {169,45,45}}, // red-ish
        {565/12, {193,146,238}}, // pink-ish
        {596/12, {203,168,236}}, // pink2-ish
        {680/12, {68,171,202}}, // blue2-ish
        {800/12, {99,189,71}}, // green2-ish
        {910/12, {236,234,13}}, // yellow2-ish
        {1065/12, {225,48,52}}, // red2-ish
        {100, {235,232,235}}, // white
    //],

    //glowBow:[
        {0, {16,16,16}},
        {25, {194,34,48}},
        {50, {236,105,13}},
        {75, {236,201,11}},
        {100, {235,235,235}},
    //],

    //grayRed:[
        {0, {218,186,175}}, // gray-ish
        {100/3, {61,130,123}}, // bluegreen-ish
        {170/3, {103,93,90}}, // gray-ish
        {100, {227,15,18}}, // red-ish
    //],

    //grayscale:[
        {0, {0,0,0}},
        {100, {255,255,255}},
    //],

    //hottest:[
        {0, {16,16,16}}, // black-ish
        {87, {206,206,206}}, // white-ish
        {87, {190,14,13}}, // red
        {100, {190,14,13}}, // red
    //],

    //lava:[
        {0, {17,19,22}},
        {70/8, {41,76,156}},
        {270/8, {14,137,131}},
        {335/8, {115,62,122}},
        {470/8, {194,34,48}},
        {600/8, {237,97,13}},
        {730/8, {235,208,45}},
        {100, {235,235,235}},
    //],

    //medical:[
        {0, {36,36,198}},    // blue-ish
        {10, {36,36,198}},   // blue-ish
        {10, {70,71,238}},   // violet-ish
        {20.5, {70,71,238}}, // violet-ish
        {20.5, {25,172,193}},// cyan-ish
        {30, {25,172,193}},  // cyan-ish
        {30, {14,158,13}},   // green-ish
        {40.5, {14,158,13}}, // green-ish
        {40.5, {15,15,123}}, // darkblue-ish
        {50, {15,15,123}},   // darkblue-ish
        {50, {237,65,197}},  // darkpink-ish
        {60.5, {237,65,197}},// darkpink-ish
        {60.5, {238,28,28}}, // red-ish
        {70.5, {238,28,28}},   // red-ish
        {70.5, {236,152,93}},  // beige-ish
        {80.5, {236,152,93}},  // beige-ish
        {80.5, {230,125,12}},  // orange-ish
        {90.5, {230,125,12}},  // orange-ish
        {90.5, {236,196,37}},  // yellow-ish
        {100, {236,196,37}}, // yellow-ish
    //],

    //rainbow:[
        {0, {1,3,74}},
        {25, {3,129,212}},
        {38, {67,171,60}},
        {55, {236,209,7}},
        {67, {255,157,23}},
        {78, {250,27,65}},
        {100, {255,233,208}},
    //],

    //wheel1:[
        {0, {238,13,239}},
        {20, {14,236,13}},
        {40, {20,18,235}},
        {60, {236,234,13}},
        {80, {14,236,237}},
        {100, {240,14,16}},
    //],

    //wheel2:[
        {0, {17,14,17}}, // black
        {10.5, {14,236,13}}, // green
        {21.4, {238,14,239}}, // magenta
        {39.5, {15,235,237}}, // cyan
        {59, {239,14,18}}, // red
        {77.5, {23,21,232}}, // blue
        {100, {235,233,14}}, // yellow
    //],

    //wheel3:[
        {0, {17,14,17}}, // black
        {100/11, {128,13,240}}, // violet
        {200/11, {15,235,12}}, // green
        {300/11, {238,14,239}}, // magenta
        {398/11, {13,236,125}}, // green (lighter)
        {495/11, {238,14,125}}, // red
        {596/11, {15,235,237}}, // cyan
        {695/11, {239,14,18}}, // red
        {794/11, {15,122,238}}, // blue
        {890/11, {237,122,15}}, // orange
        {990/11, {15,14,241}}, // blue
        {100, {235,233,14}}, // yellow
    //],

    //whiteHot:[
        {0, {16,16,16}},
        {100, {235,235,235}},
    //],

    //yellow:[
        {0, {62,16,15}},
        {20, {79,65,14}},
        {60, {185,125,14}},
        {100, {212,235,11}}
    //],
};
