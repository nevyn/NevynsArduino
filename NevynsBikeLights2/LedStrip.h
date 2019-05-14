#include <Adafruit_NeoPixel.h>

class LedStrip
{
public:
    Adafruit_NeoPixel *source;
    int start;
    int length;

    LedStrip(Adafruit_NeoPixel *source, int start, int length)
    {
        this->source = source;
        this->start = start;
        this->length = length;
    }

    int numPixels() {
        return length;
    }

    void setPixelColor(int i, uint32_t c) {
        source->setPixelColor(start+i, c);
    }

};