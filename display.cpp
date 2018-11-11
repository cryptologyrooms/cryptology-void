/* C/C++ Includes */

#include <stdint.h>
#include <string.h>

/* Application Includes */

#include "display.h"

void get_partial_rgb_value(uint8_t pixel, uint32_t countdown, uint32_t countdown_max, uint8_t * rgb, uint8_t const * const rgb_full)
{
    uint32_t countdown_bottom = (countdown_max * pixel) / 5;
    uint32_t countdown_partial = countdown - countdown_bottom;
    uint32_t countdown_per_px = countdown_max / 5;

    rgb[0] = (rgb_full[0] * countdown_partial) / countdown_per_px;
    rgb[1] = (rgb_full[1] * countdown_partial) / countdown_per_px;
    rgb[2] = (rgb_full[2] * countdown_partial) / countdown_per_px;
}

void get_rgb_value(uint8_t pixel, uint32_t countdown, uint32_t countdown_max, uint8_t * rgb, uint8_t const * const rgb_full)
{
    rgb[0] = 0; rgb[1] = 0; rgb[2] = 0;
    
    if (countdown == 0) { return; }

    uint8_t last_full_pixel = ((countdown * PIXELS_PER_STRIP) / countdown_max) - 1;
    uint8_t partial_pixel = last_full_pixel + 1;

    if (last_full_pixel < 5)
    {
        if (pixel <= last_full_pixel)
        {
            memcpy(rgb, rgb_full, sizeof(uint8_t) * 3);
        }
    }
    
    if (pixel == partial_pixel)
    {
        get_partial_rgb_value(pixel, countdown, countdown_max, rgb, rgb_full);
    }
}

#ifdef UNIT_TEST

#define CHECK_RGB_EQUAL(r, g, b, rgb) \
CPPUNIT_ASSERT_EQUAL(r, (int)rgb[0]); \
CPPUNIT_ASSERT_EQUAL(g, (int)rgb[1]); \
CPPUNIT_ASSERT_EQUAL(b, (int)rgb[2]);

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

static const uint8_t RGB[3] = {255, 255, 255};

class DisplayTest : public CppUnit::TestFixture { 

    CPPUNIT_TEST_SUITE(DisplayTest);
    
    CPPUNIT_TEST(testGetRGBForCountdownZero);
    CPPUNIT_TEST(testGetRGBForCountdownPartialPixelZero);
    CPPUNIT_TEST(testGetRGBForCountdownCompletePixelZero);
    CPPUNIT_TEST(testGetRGBForCountdownPartialPixelOne);
    CPPUNIT_TEST(testGetRGBForCountdownFullPixelOne);
    CPPUNIT_TEST(testGetRGBForCountdownPartialPixelFour);
    CPPUNIT_TEST(testGetRGBForCountdownFullPixelFour);

    CPPUNIT_TEST_SUITE_END();
 
    void testGetRGBForCountdownZero()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 0, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(1, 0, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(2, 0, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(3, 0, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(4, 0, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
    }

    void testGetRGBForCountdownPartialPixelZero()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 1500, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(127, 127, 127, rgb);
        get_rgb_value(1, 1500, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(2, 1500, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(3, 1500, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(4, 1500, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
    }

    void testGetRGBForCountdownCompletePixelZero()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 3002, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(1, 3002, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(2, 3002, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(3, 3002, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(4, 3002, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
    }

    void testGetRGBForCountdownPartialPixelOne()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 4503, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(1, 4503, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(127, 127, 127, rgb);
        get_rgb_value(2, 4503, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(3, 4503, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(4, 4503, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
    }

    void testGetRGBForCountdownFullPixelOne()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 6004, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(1, 6004, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(2, 6004, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(3, 6004, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
        get_rgb_value(4, 6004, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(0, 0, 0, rgb);
    }

    void testGetRGBForCountdownPartialPixelFour()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 14012, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(1, 14012, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(2, 14012, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(3, 14012, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(4, 14012, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(170, 170, 170, rgb);
    }

    void testGetRGBForCountdownFullPixelFour()
    {
        uint8_t rgb[3] = {0,0,0};
        get_rgb_value(0, 15013, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(1, 15013, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(2, 15013, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(3, 15013, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
        get_rgb_value(4, 15013, 15013, rgb, RGB);
        CHECK_RGB_EQUAL(255, 255, 255, rgb);
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(DisplayTest);


int main()
{
   CppUnit::TextUi::TestRunner runner;

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}


#endif
