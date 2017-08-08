/*
 * stepper.c
 *
 *  Created on: Jul 22, 2017
 *      Author: Robin
 */

#include "stepper.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#define EVENT_BITS_X_COMPLETE	0x01
#define EVENT_BITS_Y_COMPLETE	0x02
#define EVENT_BITS_Z_COMPLETE	0x04

#define EVENT_BITS_HOMING_COMPLETE	0x01

#include <errno.h>
#include <stdlib.h>
#include <math.h>

struct tool_state_t tool_state = { 0 };
struct stepper_axis_state_t steppers[4] = { 0 };

EventGroupHandle_t xStepperEventGroup;

void stepper_init(void)
{
	xStepperEventGroup = xEventGroupCreate();

	tool_state.mode = STEPPER_MODE_NORMAL;
	tool_state.current_x = 0;
	tool_state.current_y = 0;
	tool_state.current_z = 0;
	tool_state.current_feedrate = 10;
	tool_state.current_plane = PLANE_XY;

	steppers[0].port = STEPXCLK_GPIO_Port;
	steppers[0].dirport = STEPXDIR_GPIO_Port;
	steppers[0].enport = STEPXEN_GPIO_Port;
	steppers[0].pin = STEPXCLK_Pin;
	steppers[0].dirpin = STEPXDIR_Pin;
	steppers[0].enpin = STEPXEN_Pin;
	steppers[0].increment_counter = 0;
	steppers[0].step_counter = 0;
	steppers[0].step_target = 0;
	steppers[0].step_period = speed_to_step_period(10);
	steppers[0].step_index = 0;
	steppers[0].direction = STEPPER_DIR_CW;
	steppers[0].enabled = 0;

	steppers[1].port = STEPYCLK_GPIO_Port;
	steppers[1].dirport = STEPYDIR_GPIO_Port;
	steppers[1].enport = STEPYEN_GPIO_Port;
	steppers[1].pin = STEPYCLK_Pin;
	steppers[1].dirpin = STEPYDIR_Pin;
	steppers[1].enpin = STEPYEN_Pin;
	steppers[1].increment_counter = 0;
	steppers[1].step_counter = 0;
	steppers[1].step_target = 0;
	steppers[1].step_period = speed_to_step_period(10);
	steppers[1].step_index = 0;
	steppers[1].direction = STEPPER_DIR_CW;
	steppers[1].enabled = 0;
}

void stepper_set_feedrate(float feedrate)
{
	tool_state.current_feedrate = feedrate;
}

void stepper_home_axes(void)
{

}

void stepper_move_interpolated(float x, float y, float z)
{
	EventBits_t stepper_bits = 0;

	float linear_dist = sqrt(
			pow((tool_state.current_x - x), 2) +
			pow((tool_state.current_y - y), 2) +
			pow((tool_state.current_z - z), 2));
	/*
	 * Feed rate is in mm/s
	 * Distance is in mm
	 * Time = rate x distance (seconds)
	 */
	float linear_time = linear_dist / tool_state.current_feedrate;
	float x_distance = fabs(tool_state.current_x - x);
	float y_distance = fabs(tool_state.current_y - y);

	if (x_distance != 0) {
		float x_speed = x_distance / linear_time;

		if (x < tool_state.current_x) {
			steppers[0].direction = STEPPER_DIR_CCW;
		}
		else {
			steppers[0].direction = STEPPER_DIR_CW;
		}

		steppers[0].step_target = distance_to_steps(x_distance);
		steppers[0].step_period = speed_to_step_period(x_speed) - 1;
		steppers[0].increment_counter = 0;
		steppers[0].enabled = 1;

		tool_state.current_x = x;
		stepper_bits |= EVENT_BITS_X_COMPLETE;
	}

	if (y_distance != 0) {
		float y_speed = y_distance / linear_time;

		if (y < tool_state.current_y) {
			steppers[1].direction = STEPPER_DIR_CCW;
		}
		else {
			steppers[1].direction = STEPPER_DIR_CW;
		}

		steppers[1].step_target = distance_to_steps(y_distance);
		steppers[1].step_period = speed_to_step_period(y_speed) - 1;
		steppers[1].increment_counter = 0;
		steppers[1].enabled = 1;

		tool_state.current_y = y;
		stepper_bits |= EVENT_BITS_Y_COMPLETE;
	}

	if (stepper_bits != 0) {
		HAL_GPIO_WritePin(steppers[0].enport, steppers[0].enpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(steppers[1].enport, steppers[1].enpin, GPIO_PIN_RESET);
		HAL_TIM_Base_Start_IT(&htim3);

		/*
		 * Wait for the activated steppers to complete their movements
		 */
		if (xEventGroupWaitBits(xStepperEventGroup, stepper_bits, pdTRUE, pdTRUE, portMAX_DELAY) == stepper_bits) {
			HAL_TIM_Base_Stop_IT(&htim3);
		}

		HAL_GPIO_WritePin(steppers[0].enport, steppers[0].enpin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(steppers[1].enport, steppers[1].enpin, GPIO_PIN_SET);
	}
}

void stepper_center_arc(int plane, float x, float y, float offset_x, float offset_y, int turns)
{
	float start_x = tool_state.current_x;
	float start_y = tool_state.current_y;
	float offset_pos_x = start_x + offset_x;
	float offset_pos_y = start_y + offset_y;
	int i;

	switch (plane) {
	case PLANE_XY:
		float x_dist = x - start_x;
		float y_dist = y - start_y;
		int x_steps = distance_to_steps(x_dist);
		int y_steps = distance_to_steps(y_dist);
		float radius = sqrt(pow(offset_x, 2) + pow(offset_y));
		float theta_1 = atan(y_dist / x_dist);

		for (i = 0; i < x_steps; i++) {
			float theta = acos((x_dist - (i * (1 / STEPPER_STEPS_PER_MM))) / radius);
		}
	}
}

/**
 * Converts speed (in mm/sec) to the step period
 */
unsigned int speed_to_step_period(float speed)
{
	float steps_per_sec = speed * STEPPER_STEPS_PER_MM;
	float steps_spacing = STEPPER_TIMER_FREQ / steps_per_sec;
	return (unsigned int)roundf(steps_spacing);
}

unsigned int distance_to_steps(float distance)
{
	return (unsigned int)roundf(distance * STEPPER_STEPS_PER_MM);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	int i = 0;
	static int homing_counter = 0;
	BaseType_t xResult;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (tool_state.mode == STEPPER_MODE_NORMAL)
	{
		for (; i < 4; i++) {
			struct stepper_axis_state_t *stp = &steppers[i];

			if (stp->enabled == 1) {
				HAL_GPIO_WritePin(stp->port, stp->pin, GPIO_PIN_RESET);

				if (stp->step_counter == stp->step_target) {
					stp->enabled = 0;
					stp->increment_counter = 0;
					stp->step_counter = 0;
					stp->step_period = 0;
					stp->step_target = 0;

					xResult = xEventGroupSetBitsFromISR(xStepperEventGroup, (1 << i), &xHigherPriorityTaskWoken);

					if (xResult != pdFAIL) {
						portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					}
				}
				else if (stp->increment_counter == stp->step_period) {
					stp->increment_counter = 0;
					stp->step_counter++;

					HAL_GPIO_WritePin(stp->dirport, stp->dirpin, stp->direction);
					HAL_GPIO_WritePin(stp->port, stp->pin, GPIO_PIN_SET);
				}
				else {
					stp->increment_counter++;
				}
			}
		}
	}
	else if (tool_state.mode == STEPPER_MODE_HOMING)
	{
		if (homing_counter == speed_to_step_period(50)) {
			homing_counter = 0;
		}
	}
}
