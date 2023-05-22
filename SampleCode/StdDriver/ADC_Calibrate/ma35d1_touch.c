/**************************************************************************//**
 * @file     ma35d1_touch.c
 *
 * @brief    This file implements ts_calibration and ts_test from tslib for MA35D1.
 * 			 Once the calibrated data is determined, user can fill then into
 * 			 default_cali and run test without calibration process.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "ma35d1_touch.h"

#define CROSS_BOUND_DIST	50
#define VALIDATE_BOUNDARY_MIN	10
#define VALIDATE_LOOPS_DEFAULT	3

static int palette[] = {
    0x000000, 0xffe080, 0xffffff, 0xe0c0a0, 0xff0000, 0x00ff00
};
static int palette1[] = {
    0x000000, 0xffe080, 0xffffff, 0xe0c0a0, 0x304050, 0x80b8c0
};

int default_cali[7] = {
    68216160, -17236, 106, -1715185, 41, 10705, 65536
};

#define NR_COLORS (sizeof(palette) / sizeof(palette[0]))
#define NR_BUTTONS 3
#define SAMPLE_BUFFER 10
static struct ts_button buttons[NR_BUTTONS];

int ts_mode = 0;
struct ts_sample samp[MAX_SAMPLES + SAMPLE_BUFFER];
static volatile int samp_index = 0;
calibration cal_buffer;
static int calibrated = 0;
static volatile uint32_t u32PenDown = 2;

/* Delay execution for given amount of ticks */
void Delay0(uint32_t ticks)  {
    uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
    while (msTicks0 < tgtTicks);
}

uint32_t getticks()
{
    return msTicks0;
}

/* 200kHz timer */
void TMR2_IRQHandler(void)
{
    // Start menu if pd detected
    if(u32PenDown == 1)
    {
        ADC_START_CONV(ADC0);
    }
    TIMER_ClearIntFlag(TIMER2);
}

void detect_pendown(void)
{
    // Start to detect pen down event
    ADC_DETECT_PD_MODE(ADC0);
    Delay0(200); // deglitch
    ADC_CLR_INT_FLAG(ADC0, ADC_ISR_PEDEF_Msk|ADC_ISR_PEUEF_Msk);

    u32PenDown = 0;
    // Enable ADC pen down complete interrupt
    ADC_EnableInt(ADC0, ADC_IER_PEDEIEN_Msk);
}

void ts_stop(void)
{
    u32PenDown = 2;
    TIMER_Stop(TIMER2);
}

void ADC0_IRQHandler(void)
{
    if(u32PenDown == 0) {
        // Clear interrupt flag
        ADC_CLR_INT_FLAG(ADC0, ADC_ISR_PEDEF_Msk);
        // Switch from detect pen down mode to convert X/Y value
        ADC_DisableInt(ADC0, ADC_IER_PEDEIEN_Msk);
        ADC_EnableInt(ADC0, ADC_IER_MIEN_Msk);
        ADC_CONVERT_XY_MODE(ADC0);
        u32PenDown = 1;
        TIMER_Start(TIMER2);
    }
    else if (u32PenDown == 1) {
        // Clear interrupt flag
        ADC_CLR_INT_FLAG(ADC0, ADC_ISR_MF_Msk);
        // Get ADC convert result
        if(ADC_GET_CONVERSION_Z1DATA(ADC0) < Z_TH)
        {
            if(ts_mode == 1) {
                // Pen up, switch from convert X/Y value to detect pen down event
                ADC_DisableInt(ADC0, ADC_IER_MIEN_Msk);
                ts_stop();
            }
            else {
                samp[samp_index].pressure = 0;
                if(++samp_index > MAX_SAMPLES - 1)
                    samp_index = 0;
            }
        } else {
            samp[samp_index].x = ADC_GET_CONVERSION_XDATA(ADC0);
            samp[samp_index].y = ADC_GET_CONVERSION_YDATA(ADC0);
            samp[samp_index].pressure = ADC_GET_CONVERSION_Z1DATA(ADC0);
            if(ts_mode == 1) {
                if(samp_index < MAX_SAMPLES - 1)
                    ++samp_index;
            }
            else {
                if(++samp_index > MAX_SAMPLES - 1)
                    samp_index = 0;
            }
        }
    }
}

static int sort_by_x(const void *a, const void *b)
{
    return (((struct ts_sample *)a)->x - ((struct ts_sample *)b)->x);
}

static int sort_by_y(const void *a, const void *b)
{
    return (((struct ts_sample *)a)->y - ((struct ts_sample *)b)->y);
}

/* Waits for the screen to be touched, averages x and y sample
 * coordinates until the end of contact
 */
static void getxy(int *x, int *y)
{
    int middle;

    samp_index = 0;

    memset(samp, 0, sizeof(struct ts_sample) * MAX_SAMPLES);

    detect_pendown();

    // Waiting for pen down
    while(u32PenDown != 1) {}

    /* Now collect up to MAX_SAMPLES touches into the samp array. */
    samp_index = 0;

    // Waiting for pen up
    while(u32PenDown != 2) {}

    sysprintf("Took %d samples...\n", samp_index);

    /*
     * At this point, we have samples in indices zero to (index-1)
     * which means that we have (index) number of samples.  We want
     * to calculate the median of the samples so that wild outliers
     * don't skew the result.  First off, let's assume that arrays
     * are one-based instead of zero-based.  If this were the case
     * and index was odd, we would need sample number ((index+1)/2)
     * of a sorted array; if index was even, we would need the
     * average of sample number (index/2) and sample number
     * ((index/2)+1).  To turn this into something useful for the
     * real world, we just need to subtract one off of the sample
     * numbers.  So for when index is odd, we need sample number
     * (((index+1)/2)-1).  Due to integer division truncation, we
     * can simplify this to just (index/2).  When index is even, we
     * need the average of sample number ((index/2)-1) and sample
     * number (index/2).  Calculate (index/2) now and we'll handle
     * the even odd stuff after we sort.
     */
    middle = samp_index/2;
    if (x) {
        qsort(samp, samp_index, sizeof(struct ts_sample), sort_by_x);
        if (samp_index & 1)
            *x = samp[middle].x;
        else
            *x = (samp[middle-1].x + samp[middle].x) / 2;
    }
    if (y) {
        qsort(samp, samp_index, sizeof(struct ts_sample), sort_by_y);
        if (samp_index & 1)
            *y = samp[middle].y;
        else
            *y = (samp[middle-1].y + samp[middle].y) / 2;
    }
}

static void get_sample(calibration *cal,
                       int index, int x, int y, char *name, short redo)
{
    static int last_x = -1, last_y;

    if (redo) {
        last_x = -1;
        last_y = 0;
    }

    if (last_x != -1) {
#define NR_STEPS 10
        int dx = ((x - last_x) << 16) / NR_STEPS;
        int dy = ((y - last_y) << 16) / NR_STEPS;
        int i;

        last_x <<= 16;
        last_y <<= 16;
        for (i = 0; i < NR_STEPS; i++) {
            put_cross(last_x >> 16, last_y >> 16, 2 | XORMODE);
            Delay0(1);
            put_cross(last_x >> 16, last_y >> 16, 2 | XORMODE);
            last_x += dx;
            last_y += dy;
        }
    }

    put_cross(x, y, 2 | XORMODE);
    getxy(&cal->x[index], &cal->y[index]);
    put_cross(x, y, 2 | XORMODE);

    last_x = cal->xfb[index] = x;
    last_y = cal->yfb[index] = y;

    sysprintf("%s : X = %4d Y = %4d\n", name, cal->x[index], cal->y[index]);
}

void cali_data_rearrange(int *dest, int *src)
{
    dest[2] = src[0];
    dest[0] = src[1];
    dest[1] = src[2];
    dest[5] = src[3];
    dest[3] = src[4];
    dest[4] = src[5];
    dest[6] = src[6];
}

int ts_init(void)
{
    // Power on ADC
    ADC_POWER_ON(ADC0);

    // Load default
    if(!calibrated) {
        cali_data_rearrange(cal_buffer.a, default_cali);
    }

    return 0;
}

extern int fbsize;
extern unsigned char *fbuffer;
extern unsigned char **line_addr;
extern int fb_fd;
extern int bytes_per_pixel;

int fb_init(void)
{
    int line_length;
    unsigned int addr;

    bytes_per_pixel = 4;

    xres_orig = DISP_RESOLUTION_X;
    yres_orig = DISP_RESOLUTION_Y;

    xres = DISP_RESOLUTION_X;
    yres = DISP_RESOLUTION_Y;
    line_length = xres * bytes_per_pixel;

    rotation = 0; // just force to 0
    if (rotation & 1) {
        /* 1 or 3 */
        xres = DISP_RESOLUTION_Y;
        yres = DISP_RESOLUTION_X;
    } else {
        /* 0 or 2 */
        xres = DISP_RESOLUTION_X;
        yres = DISP_RESOLUTION_Y;
    }

    fbuffer = (unsigned char *)((uint64_t)DDR_ADR_FRAMEBUFFER | NON_CACHE);

    fbsize = line_length * yres;

    memset(fbuffer, 0, (size_t)fbsize);

    // memory head addr of each line
    line_addr = malloc(sizeof(*line_addr) * yres);
    addr = 0;
    for (int y = 0; y < yres; y++, addr += line_length)
        line_addr[y] = fbuffer + addr;

    DISPLIB_EnableOutput(eLayer_Video);

    return 0;
}

void clearbuf(void)
{
    memset(fbuffer, 0, (size_t)fbsize);
}

void ts_calibrate()
{
    int32_t ret, i;
    uint32_t tick;
    int32_t min_interval = 500; // if double touched -> redo
    calibration cal = {
        .x = { 0 },
        .y = { 0 },
    };

    ts_init();

    fb_init();


    for (i = 0; i < NR_COLORS; i++)
        setcolor(i, palette[i]);

    put_string_center(xres / 2, yres / 4,
                      "Touchscreen calibration utility", 1);
    put_string_center(xres / 2, yres / 4 + 20,
                      "Touch crosshair to calibrate", 2);

    sysprintf("xres = %d, yres = %d\n", xres, yres);

    clearbuf();

    short redo = 0;

redocalibration:
    // Calibration start
    tick = getticks();
    get_sample(&cal, 0, CROSS_BOUND_DIST, CROSS_BOUND_DIST, "Top left", redo);
    redo = 0;
    if (getticks() - tick < min_interval) {
        redo = 1;
#ifdef DEBUG
        printf("ts_calibrate: time before touch press < %dms. restarting.\n",
               min_interval);
#endif
        goto redocalibration;
    }
    clearbuf();

    tick = getticks();
    get_sample(&cal, 1, xres - CROSS_BOUND_DIST, CROSS_BOUND_DIST, "Top right", redo);
    if (getticks() - tick < min_interval) {
        redo = 1;
#ifdef DEBUG
        printf("ts_calibrate: time before touch press < %dms. restarting.\n",
               min_interval);
#endif
        goto redocalibration;
    }
    clearbuf();

    tick = getticks();
    get_sample(&cal, 2, xres - CROSS_BOUND_DIST, yres - CROSS_BOUND_DIST, "Bot right", redo);
    if (getticks() - tick < min_interval) {
        redo = 1;
#ifdef DEBUG
        printf("ts_calibrate: time before touch press < %dms. restarting.\n",
               min_interval);
#endif
        goto redocalibration;
    }
    clearbuf();

    tick = getticks();
    get_sample(&cal, 3, CROSS_BOUND_DIST, yres - CROSS_BOUND_DIST, "Bot left", redo);
    if (getticks() - tick < min_interval) {
        redo = 1;
#ifdef DEBUG
        printf("ts_calibrate: time before touch press < %dms. restarting.\n",
               min_interval);
#endif
        goto redocalibration;
    }
    clearbuf();

    tick = getticks();
    get_sample(&cal, 4, xres_orig / 2,  yres_orig / 2, "Center", redo);
    if (getticks() - tick < min_interval) {
        redo = 1;
#ifdef DEBUG
        printf("ts_calibrate: time before touch press < %dms. restarting.\n",
               min_interval);
#endif
        goto redocalibration;
    }

    if (perform_calibration (&cal)) {
        sysprintf("Calibration constants: ");
        for (i = 0; i < 6; i++)
            sysprintf("%d, ", cal.a[i]);
        sysprintf("%d\n", cal.a[6]);
        sysprintf("Calibration finished.\n");
    } else {
        sysprintf("Calibration failed.\n");
        return;
    }

    cali_data_rearrange(cal_buffer.a, cal.a);
    calibrated = 1;

    fillrect(0, 0, xres - 1, yres - 1, 0);
    clearbuf();
    DISPLIB_DisableOutput(eLayer_Video);
}

/* [inactive] border fill text [active] border fill text */
static int button_palette[6] = {
    1, 4, 2,
    1, 5, 0
};
void button_draw(struct ts_button *button)
{
    int s = (button->flags & BUTTON_ACTIVE) ? 3 : 0;

    rect(button->x, button->y, button->x + button->w,
         button->y + button->h, button_palette[s]);
    fillrect(button->x + 1, button->y + 1,
             button->x + button->w - 2,
             button->y + button->h - 2, button_palette[s + 1]);
    put_string_center(button->x + button->w / 2,
                      button->y + button->h / 2,
                      button->text, button_palette[s + 2]);
}

int button_handle(struct ts_button *button, int x, int y, unsigned int p)
{
    int inside = (x >= button->x) && (y >= button->y) &&
                 (x < button->x + button->w) &&
                 (y < button->y + button->h);

    if (p > 0) {
        if (inside) {
            if (!(button->flags & BUTTON_ACTIVE)) {
                button->flags |= BUTTON_ACTIVE;
                button_draw(button);
            }
        } else if (button->flags & BUTTON_ACTIVE) {
            button->flags &= ~BUTTON_ACTIVE;
            button_draw(button);
        }
    } else if (button->flags & BUTTON_ACTIVE) {
        button->flags &= ~BUTTON_ACTIVE;
        button_draw(button);
        return 1;
    }

    return 0;
}

static void refresh_screen(void)
{
    int i;

    fillrect(0, 0, xres - 1, yres - 1, 0);
    put_string_center(xres / 2, yres / 4, "Touchscreen test program", 1);
    put_string_center(xres / 2, yres / 4 + 20,
                      "Touch screen to move crosshair", 2);

    for (i = 0; i < NR_BUTTONS; i++)
        button_draw(&buttons[i]);
}

static int linear_read(struct ts_sample *tssamp, int nr_samples)
{
    calibration *lin = &cal_buffer;
    int xtemp, ytemp;
    int nr;

    for(nr = 0; nr < nr_samples; nr++, tssamp++) {
        xtemp = tssamp->x;
        ytemp = tssamp->y;
        tssamp->x = (lin->a[2] + lin->a[0]*xtemp + lin->a[1]*ytemp) / lin->a[6];
        tssamp->y = (lin->a[5] + lin->a[3]*xtemp + lin->a[4]*ytemp) / lin->a[6];

        switch (rotation) {
            int rot_tmp;
        case 0:
            break;
        case 1:
            rot_tmp = tssamp->x;
            tssamp->x = tssamp->y;
            tssamp->y = xres - rot_tmp - 1;
            break;
        case 2:
            tssamp->x = xres - tssamp->x - 1;
            tssamp->y = yres - tssamp->y - 1;
            break;
        case 3:
            rot_tmp = tssamp->x;
            tssamp->x = yres - tssamp->y - 1;
            tssamp->y = rot_tmp ;
            break;
        default:
            break;
        }
    }

    return 0;
}

/* Contiguous mode for ts_test */
int ts_sample(struct ts_sample *tssamp, int nr, unsigned int *mode)
{
    int refresh, first, pre;
    uint32_t ts;

    if(u32PenDown != 1)
    {
        *mode &= ~0x80000000;
        detect_pendown();

        // Waiting for pen down
        while(u32PenDown != 1) {}
    }

redosamp:
    first = samp_index;

    ts = getticks();

    /* Filter glitch */
    pre = first - 2;
    if(pre < 0)
        pre += MAX_SAMPLES;
    if((samp[pre].pressure) == 0)
    {
        *mode &= ~0x80000000;
        goto redosamp;
    }

    refresh = first + nr;
    if(refresh > (MAX_SAMPLES - 1))
        refresh -= MAX_SAMPLES;

    while(samp_index != refresh) {}

    pre = refresh - 1;
    if(pre < 0)
        pre += MAX_SAMPLES;

    if(samp[pre].pressure == 0)
        *mode &= ~0x80000000;

    /* Report and calibrate */
    tssamp->x = samp[first].x;
    tssamp->y = samp[first].y;
    tssamp->pressure = samp[first].pressure;
    tssamp->tv.tv_sec = ts/1000;
    tssamp->tv.tv_usec = ts % 1000; // in ms here

    linear_read(tssamp, 1);

    return 1;
}

void ts_test()
{
    int x, y, p; // last one
    unsigned int i;
    unsigned int mode = 0;
    int quit_pressed = 0;
    int nr = 3;

    ts_init();

    fb_init();

    x = xres / 2;
    y = yres / 2;

    for (i = 0; i < NR_COLORS; i++)
        setcolor(i, palette1[i]);

    /* Initialize buttons */
    memset(&buttons, 0, sizeof(buttons));
    buttons[0].w = buttons[1].w = buttons[2].w = xres / 4;
    buttons[0].h = buttons[1].h = buttons[2].h = 20;
    buttons[0].x = 0;
    buttons[1].x = (3 * xres) / 8;
    buttons[2].x = (3 * xres) / 4;
    buttons[0].y = buttons[1].y = buttons[2].y = 10;
    buttons[0].text = "Drag";
    buttons[1].text = "Draw";
    buttons[2].text = "Quit";

    refresh_screen();
    samp_index = 0;

    while (1) {
        struct ts_sample samp;
        int ret;

        /* Show the cross */
        if ((mode & 15) != 1)
            put_cross(x, y, 2 | XORMODE);

        ret = ts_sample(&samp, nr, &mode);

        /* Hide it */
        if ((mode & 15) != 1)
            put_cross(x, y, 2 | XORMODE);

        if (ret != 1)
            continue;

        for (i = 0; i < NR_BUTTONS; i++)
            if (button_handle(&buttons[i], samp.x, samp.y, samp.pressure)) {
                switch (i) {
                case 0:
                    mode = 0;
                    refresh_screen();
                    break;
                case 1:
                    mode = 1;
                    refresh_screen();
                    break;
                case 2:
                    quit_pressed = 1;
                }
            }

        sysprintf("%ld.%03ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec,
                  samp.x, samp.y, samp.pressure);

        if (samp.pressure > 0) {
            if ((mode == 0x80000001) && abs(p - samp.pressure) < 100)
                line(x, y, samp.x, samp.y, 2);
            x = samp.x;
            y = samp.y;
            p = samp.pressure;
            mode |= 0x80000000;
        } else
            mode &= ~0x80000000;
        if (quit_pressed)
            break;
    }

    fillrect(0, 0, xres - 1, yres - 1, 0);
    clearbuf();
    ts_stop();
    DISPLIB_DisableOutput(eLayer_Video);
}
