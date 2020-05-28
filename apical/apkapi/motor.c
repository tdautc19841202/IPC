#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "apkapi.h"
#include "motor.h"

#define GPIO_MOTOR_0  44
#define GPIO_MOTOR_1  46
#define GPIO_MOTOR_2  45
#define GPIO_MOTOR_3  47
#define GPIO_MOTOR_H  53
#define GPIO_MOTOR_V  16

extern int pthread_setname_np(pthread_t __target_thread, const char *__name);

typedef struct {
    #define FLAG_EXIT (1 << 0)
    uint32_t  status;
    #define ACTION_MOTOR1_0  0
    #define ACTION_MOTOR1_1  1
    #define ACTION_MOTOR2_0  2
    #define ACTION_MOTOR2_1  3
    #define ACTION_STOPED    4
    uint32_t  action;
    uint32_t  run_cycles;
    uint32_t  sleep_time;
    uint32_t  h_sleep_time;
    uint32_t  v_sleep_time;
    pthread_t thread_vdfz;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
} CONTEXT;

static char GPIO_NUM_TAB[6] = {
    GPIO_MOTOR_0, GPIO_MOTOR_1, GPIO_MOTOR_2, GPIO_MOTOR_3, GPIO_MOTOR_H, GPIO_MOTOR_V,
};

static char GPIO_VAL_TAB[8][4] = {
    { 1, 0, 0, 0},
    { 1, 1, 0, 0},
    { 0, 1, 0, 0},
    { 0, 1, 1, 0},
    { 0, 0, 1, 0},
    { 0, 0, 1, 1},
    { 0, 0, 0, 1},
    { 1, 0, 0, 1},
};

static void gpio_request(int gpio)
{
    char strval [8  ];
    char strfile[256];
    snprintf(strval , sizeof(strval ), "%d", gpio);
    file_write("/sys/class/gpio/export", strval);
    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/direction", gpio);
    file_write(strfile, "out");
}

static void gpio_release(int gpio)
{
    char strval [8  ];
    char strfile[256];
    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/direction", gpio);
    file_write(strfile, "in");
    snprintf(strval , sizeof(strval ), "%d", gpio);
    file_write("/sys/class/gpio/unexport", strval);
}

static void gpio_setval(int gpio, int val)
{
    char strfile[256];
    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/value", gpio);
    file_write(strfile, val ? "1" : "0");
}

static void* motor_vdfz_proc(void *argv)
{
    CONTEXT *context   = (CONTEXT*)argv;
    uint32_t ticklast  = 0, tickcur = 0, ticknext = 0, step = 0;
    int32_t  ticksleep = 0, tickdiff, i;
    struct   sched_param sp = {};

    pthread_setname_np(pthread_self(), "motor");
    sp.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_setschedparam(pthread_self(), SCHED_RR, &sp);
    while (!(context->status & FLAG_EXIT)) {
        tickcur   = get_tick_count();
        tickdiff  = tickcur - ticklast;
        ticklast  = tickcur;
        ticknext  =(ticknext ? ticknext : tickcur) + context->sleep_time;
        ticksleep = ticknext - tickcur;

        if (context->run_cycles) context->run_cycles--;
        if (context->run_cycles == 0) {
//          printf("ACTION_STOPED\n");
            context->action = ACTION_STOPED;
            ticknext        = 0;
        }

        if (context->action != ACTION_STOPED) {
            step += (context->action & 1) ? -1 : 1;
            for (i=0; i<4; i++) gpio_setval(GPIO_NUM_TAB[i], GPIO_VAL_TAB[step & 0x7][i]);
        } else {
            for (i=0; i<6; i++) gpio_setval(GPIO_NUM_TAB[i], 0);
        }

        if (0) printf("tickcur = %d, ticknext = %d, tickdiff = %d, ticksleep = %d\n", tickcur, ticknext, tickdiff, ticksleep);
        if (ticksleep > 0) usleep(ticksleep * 1000);

        pthread_mutex_lock(&context->mutex);
        while (context->run_cycles == 0 && !(context->status & FLAG_EXIT)) pthread_cond_wait(&context->cond, &context->mutex);
        pthread_mutex_unlock(&context->mutex);
    }
    return NULL;
}

void* motor_init(void)
{
    CONTEXT *context = NULL;
    pthread_attr_t attr;
    //if (!HW_MOTOR_TYPE) return NULL;

    //++ gpio102 & gpio103 are special gpios, we need init them by special code
    file_write("/sys/class/gpio/export", "16");
    file_write("/sys/class/gpio/export", "53");
    file_write("/sys/class/gpio/gpio16/direction", "out");
    file_write("/sys/class/gpio/gpio53/direction", "out");
    gpio_setval(GPIO_MOTOR_H, 0);
    gpio_setval(GPIO_MOTOR_V, 0);
    //-- gpio102 & gpio103 are special gpios, we need init them by special code

    gpio_request(GPIO_MOTOR_0); gpio_setval(GPIO_MOTOR_0, 0);
    gpio_request(GPIO_MOTOR_1); gpio_setval(GPIO_MOTOR_1, 0);
    gpio_request(GPIO_MOTOR_2); gpio_setval(GPIO_MOTOR_2, 0);
    gpio_request(GPIO_MOTOR_3); gpio_setval(GPIO_MOTOR_3, 0);

    context = calloc(1, sizeof(CONTEXT));
    if (!context) {
        printf("failed to allocate context for motor !\n");
        goto failed;
    }

    // delay time 10ms
    context->sleep_time   = 10;
    context->h_sleep_time = 15;
    context->v_sleep_time = 22;

    // init mutex & cond
    pthread_mutex_init(&context->mutex, NULL);
    pthread_cond_init (&context->cond , NULL);

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 8 * 1024);
    pthread_create(&context->thread_vdfz, NULL, motor_vdfz_proc, context);
    return context;

failed:
    if (context) free (context);
    return NULL;
}

void motor_exit(void *ctxt)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!ctxt) return;
    pthread_mutex_lock(&context->mutex);
    context->status |= FLAG_EXIT;
    pthread_cond_signal(&context->cond);
    pthread_mutex_unlock(&context->mutex);
    pthread_join(context->thread_vdfz, NULL);
    pthread_mutex_destroy(&context->mutex);
    pthread_cond_destroy (&context->cond );
    free(context);

    gpio_release(GPIO_MOTOR_0);
    gpio_release(GPIO_MOTOR_1);
    gpio_release(GPIO_MOTOR_2);
    gpio_release(GPIO_MOTOR_3);
    gpio_release(GPIO_MOTOR_H);
    gpio_release(GPIO_MOTOR_V);
}

void motor_speed(void *ctxt, uint32_t hspeed, uint32_t vspeed) {
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!ctxt) return;
    context->h_sleep_time = hspeed;
    context->v_sleep_time = vspeed;
}

void motor_action(void *ctxt, int type, int direction)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    int      action;
    if (!ctxt) return;
    switch (type) {
    case MOTOR_TYPE_MOTOR1:
        context->sleep_time = context->h_sleep_time;
        action = direction == 0 ? ACTION_STOPED : direction > 0 ? ACTION_MOTOR1_1 : ACTION_MOTOR1_0;
        if (context->action != action) {
            context->action = action;
            gpio_setval(GPIO_MOTOR_V, 0);
            gpio_setval(GPIO_MOTOR_H, action != ACTION_STOPED);
        }
        pthread_mutex_lock(&context->mutex);
        context->run_cycles = abs(direction) / context->sleep_time;
        pthread_cond_signal(&context->cond);
        pthread_mutex_unlock(&context->mutex);
        break;
    case MOTOR_TYPE_MOTOR2:
        context->sleep_time = context->v_sleep_time;
        action = direction == 0 ? ACTION_STOPED : direction > 0 ? ACTION_MOTOR2_1 : ACTION_MOTOR2_0;
        if (context->action != action) {
            context->action = action;
            gpio_setval(GPIO_MOTOR_H, 0);
            gpio_setval(GPIO_MOTOR_V, action != ACTION_STOPED);
        }
        pthread_mutex_lock(&context->mutex);
        context->run_cycles = abs(direction) / context->sleep_time;
        pthread_cond_signal(&context->cond);
        pthread_mutex_unlock(&context->mutex);
        break;
    }
}




