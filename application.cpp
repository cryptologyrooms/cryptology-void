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
static const uint8_t COUNTDOWN_TICK_MS = 20;

/* Private Variables */
static uint32_t s_countdown_ms[] = {0,0,0,0,0};
static uint32_t s_countdown_ms_start[] = {0,0,0,0,0};

static AdafruitNeoPixelADL * s_pNeopixels = NULL;
static DebouncedInput * s_pInputs[5] = {NULL};

static IntegerParam * s_pReloadInterval = NULL;
static AnalogInput * s_pReloadInput = NULL;
static RGBParam * s_pRGB = NULL;
static RGBParam * s_pFinishRGB = NULL;

static GridIndexer s_indexer = GridIndexer(5);

/* Private Functions */

static int32_t get_reload_ticks()
{
    int32_t reload_ms = ((s_pReloadInput->reading() * 12) / 1023) + 1;
    reload_ms *= s_pReloadInterval->get();
    if (reload_ms < 1)
    {
        adl_logln(LOG_APP, "Invalid reload %d", reload_ms);
        reload_ms = 1000;
    }
    reload_ms = (reload_ms / COUNTDOWN_TICK_MS) * COUNTDOWN_TICK_MS;
    adl_logln(LOG_APP, "Reload: %d", reload_ms);
    return reload_ms;
}

static void update_strip(uint8_t substrip, uint8_t const * const pFullRGB)
{
    uint8_t rgb[3];
    uint8_t n_led_start = substrip * 5;

    for (uint8_t i=0; i<5;i++)
    {
        get_rgb_value(i, s_countdown_ms[i], s_countdown_ms_start[i], rgb, pFullRGB);
        s_pNeopixels->setPixelColor(s_indexer.get(n_led_start+i), rgb[0], rgb[1], rgb[2]);
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
        if (s_countdown_ms[substrip])
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
        if (s_countdown_ms[i])
        {
            s_countdown_ms[i]-=COUNTDOWN_TICK_MS;   
        }
        update |= (s_countdown_ms[i] > 0);
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
static ADLTask countdown_task(COUNTDOWN_TICK_MS, countdown_task_fn, NULL);

static void debug_task_fn(ADLTask& pThisTask, void * pTaskData)
{
    (void)pThisTask;
    (void)pTaskData;
    adl_logln(LOG_APP, "Counts: %lu,%lu,%lu,%lu,%lu",
        s_countdown_ms[0], s_countdown_ms[1],s_countdown_ms[2],s_countdown_ms[3],s_countdown_ms[4]
    );
}
static ADLTask debug_task(1000, debug_task_fn, NULL);

static void play_intro()
{
    adl_logln(LOG_APP, "Cryptology Void Puzzle");
    for (uint8_t i=0; i<5;i++)
    {
        s_countdown_ms[i] = 3000;
        s_countdown_ms_start[i] = 3000;
    }

    while(s_countdown_ms[0])
    {
        countdown_task.run();
    }
}

/* ADL Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)nparams; (void)ndevices;

    s_pInputs[0] = (DebouncedInput*)pdevices[0];
    s_pInputs[1] = (DebouncedInput*)pdevices[1];
    s_pInputs[2] = (DebouncedInput*)pdevices[2];
    s_pInputs[3] = (DebouncedInput*)pdevices[3];
    s_pInputs[4] = (DebouncedInput*)pdevices[4];
    s_pNeopixels = (AdafruitNeoPixelADL*)pdevices[5];
    s_pReloadInput = (AnalogInput*)pdevices[6];

    s_pReloadInterval = (IntegerParam*)pparams[0];
    s_pRGB = (RGBParam*)pparams[1];
    s_pFinishRGB = (RGBParam*)pparams[2];

    pinMode(RELAY_PIN, OUTPUT);

    countdown_task.start();
    play_intro();

    for (uint8_t i=0; i<5;i++)
    {
        s_countdown_ms[i] = 0;
        s_countdown_ms_start[i] = get_reload_ticks();
    }

    update_display();

    for (uint8_t i=0; i<5; i++)
    {
        ((DebouncedInput *)(pdevices[i]))->check_high_and_clear();
    }
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)pparams; (void)ndevices; (void)nparams;

    if (s_game_running)
    {
        s_game_running = false;

        for (uint8_t i=0; i<5; i++)
        {
            if (s_pInputs[i]->check_high_and_clear())
            {
                s_countdown_ms[i] = s_countdown_ms_start[i] = get_reload_ticks();
                adl_logln(LOG_APP, "Reload #%d: %" PRIu32, i, s_countdown_ms[i]);
            }
            s_game_running |= (s_countdown_ms[i] == 0);
        }
        countdown_task.run();
        debug_task.run();
    }
    
    digitalWrite(RELAY_PIN, s_game_running ? LOW : HIGH);
}
