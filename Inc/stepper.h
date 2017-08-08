/*
 * stepper.h
 *
 *  Created on: Jul 22, 2017
 *      Author: Robin
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "tim.h"
#include "gpio.h"

#define STEPPER_DIR_CW	0
#define STEPPER_DIR_CCW	1
#define STEPPER_TIMER_FREQ		20000
#define STEPPER_STEPS_PER_MM	40
#define STEPPER_MAX_FEED_RATE	50

#define STEPPER_MODE_NORMAL		0
#define STEPPER_MODE_HOMING		1

#define STEPPER_X_MAX			155
#define STEPPER_Y_MAX			155

#define PLANE_XY	0
#define PLANE_ZX	1
#define PLANE_YZ	2

struct tool_state_t {
	float current_x;
	float current_y;
	float current_z;
	float current_feedrate;
	int current_plane;
	int mode;
};

struct stepper_axis_state_t {
	unsigned int enabled;
	unsigned int increment_counter;
	unsigned int step_counter;
	unsigned int step_target;
	unsigned int step_period;
	unsigned int step_index;
	unsigned int direction;
	GPIO_TypeDef *port;
	GPIO_TypeDef *dirport;
	GPIO_TypeDef *enport;
	unsigned int pin;
	unsigned int dirpin;
	unsigned int enpin;
};

extern struct tool_state_t tool_state;
extern struct stepper_axis_state_t steppers[4];

void stepper_init(void);
void stepper_set_feedrate(float feedrate);
void stepper_move_interpolated(float x, float y, float z);
void stepper_center_arc(int plane, float x, float y, float offset_x, float offset_y, int turns);
unsigned int distance_to_steps(float distance);
unsigned int speed_to_step_period(float speed);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* STEPPER_H_ */
