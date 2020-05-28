#ifndef __MOTOR_H__
#define __MOTOR_H__

enum {
    MOTOR_TYPE_MOTOR1,
    MOTOR_TYPE_MOTOR2,
};

void* motor_init(void);
void  motor_exit(void *ctxt);

// direction:
// for motor1 or motor2, 0 - stop, >0 - closewise rotate n steps, <0 - anti-closewise rotate n steps
void motor_action(void *ctxt, int type, int direction);

// set motor speed
void motor_speed(void *ctxt, uint32_t hspeed, uint32_t vspeed);


#endif

