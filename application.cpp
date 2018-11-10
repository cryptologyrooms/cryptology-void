/* Arduino Includes */

#include <Adafruit_NeoPixel.h>

/* ADL Includes */

#include "adl.h"

#include "adafruit-neopixel-adl.h"

#include "analog-input.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "integer-param.h"
#include "rgb-param.h"
#include "debounced-input.h"
#include "grid-indexer.h"

/* Application Includes */

#include "display.h"

/* Defines, typedefs, constants */

static const uint8_t RELAY_PIN = 2;
static bool s_game_running = true;

static const uint8_t INPUT_PINS[] = {3,4,5,6,7};

/* Private Variables */
static uint32_t s_countdown[] = {0,0,0,0,0};
static uint32_t s_countdown_start[] = {0,0,0,0,0};

static AdafruitNeoPixelADL * s_pNeopixels = NULL;
static IntegerParam * s_pReloadInterval = NULL;
static AnalogInput * s_pReloadInput = NULL;
static RGBParam * s_pRGB = NULL;
static RGBParam * s_pFinishRGB = NULL;

static GridIndexer s_indexer = GridIndexer(5);

/* Private Functions */

static int32_t get_reload_ticks()
{
    int32_t nintervals = ((s_pReloadInput->reading() * 12) / 1023) + 1;
    nintervals *= s_pReloadInterval->get();
    if (nintervals < 1)
    {
        adl_logln(LOG_APP, "Invalid interval %d", nintervals);
        nintervals = 1;
    }
    return nintervals * 50;
}

static void update_strip(uint8_t substrip, uint8_t * pRGB)
{
    uint32_t per_led_countdown_interval;
    uint32_t partial_countdown;
    uint32_t full_led_countdown_base;

    per_led_countdown_interval = s_countdown_start[substrip] / 5;

    const uint8_t n_full_leds = (s_countdown[substrip] * 5) / s_countdown_start[substrip];
    const uint8_t npartial_led = n_full_leds + 1;
    full_led_countdown_base = (s_countdown_start[substrip] * n_full_leds) / 5;
    uint8_t n_led_start = (substrip * 5);
    uint8_t actual_led_idx = 0;

    for (uint8_t led=0; led<n_full_leds; led++)
    {
        actual_led_idx = s_indexer.get(n_led_start+led);
        s_pNeopixels->setPixelColor(actual_led_idx, pRGB[0], pRGB[1], pRGB[2]);
    }

    if (npartial_led < 5)
    {
        partial_countdown = s_countdown[substrip] - (per_led_countdown_interval * n_full_leds);
        actual_led_idx = s_indexer.get(n_led_start+npartial_led);
        s_pNeopixels->setPixelColor(actual_led_idx,
            (pRGB[0]*partial_countdown)/per_led_countdown_interval,
            (pRGB[0]*partial_countdown)/per_led_countdown_interval,
            (pRGB[0]*partial_countdown)/per_led_countdown_interval
        );
    }
}

static void update_display()
{
    uint8_t rgb[3];
    rgb[0] = s_pRGB->get(eR);
    rgb[1] = s_pRGB->get(eG);
    rgb[2] = s_pRGB->get(eB);

    for (uint8_t substrip=0; substrip<5; substrip++)
    {
        if (s_countdown[substrip])
        {
            update_strip(substrip, rgb);
        }
    }
    s_pNeopixels->show();
}

static void clear_display()
{
    s_pNeopixels->clear();
    s_pNeopixels->show();
}

static void countdown_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;

    bool update = false;

    for (uint8_t i=0; i<5; i++)
    {
        if (s_countdown[i])
        {
            s_countdown[i]--;   
        }
        update |= (s_countdown[i] > 0);
    }

    if (update)
    {
        update_display();
    }
    else
    {
        clear_display();
    }
}
static ADLTask countdown_task(20, countdown_task_fn, NULL);

/* Local Functions */

/* ADL Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pparams; (void)nparams;

    s_pNeopixels = (AdafruitNeoPixelADL*)pdevices[5];
    s_pReloadInput = (AnalogInput*)pdevices[6];

    s_pReloadInterval = (IntegerParam*)pparams[0];
    s_pRGB = (RGBParam*)pparams[1];
    s_pFinishRGB = (RGBParam*)pparams[2];

    pinMode(RELAY_PIN, OUTPUT);

    countdown_task.start();
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)ndevices; (void)nparams;

    if (s_game_running)
    {
        s_game_running = false;

        for (uint8_t i=0; i<5; i++)
        {
            if (((DebouncedInput *)(pparams[i]))->check_low_and_clear())
            {
                s_countdown[i] = s_countdown_start[i] = get_reload_ticks();
            }
            s_game_running |= (s_countdown[i] == 0);
        }
        countdown_task.run();
    }
    
    digitalWrite(RELAY_PIN, s_game_running ? LOW : HIGH);
}
