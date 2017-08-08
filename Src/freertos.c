/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "server.h"
#include "stepper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Variables -----------------------------------------------------------------*/

osThreadId defaultTaskHandle;
osThreadId serverTaskHandle;

osThreadDef(serverTask, server_thread, osPriorityNormal, 0, 256);

#define PARSE_MODE_IDLE			0
#define PARSE_MODE_COMMAND		1
#define PARSE_MODE_AXES			2

#define COMMAND_TYPE_G 1
#define COMMAND_TYPE_M 2


/* Function prototypes -------------------------------------------------------*/

void StartDefaultTask(void const * argument);

extern void MX_LWIP_Init(void);
extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void)
{
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
}

/* StartDefaultTask function */

float parse_number(const char *line, char code, int def)
{
	const char *ptr = line;

	while (ptr && *ptr && ptr < line + strlen(line)) {
		if (*ptr == code) {
			return atof(ptr + 1);
		}
		ptr = strchr(ptr, ' ') + 1;
	}

	return def;
}

void process_line(const char *line)
{
	double arg_x = 0;
	double arg_y = 0;
	double arg_z = 0;
	double arg_f = 0;

	if (line[0] == 'G') {
		switch ((int)parse_number(line, 'G', -1)) {
		case 0:
			arg_x = parse_number(line, 'X', tool_state.current_x);
			arg_y = parse_number(line, 'Y', tool_state.current_y);
			arg_z = parse_number(line, 'Z', tool_state.current_z);
			arg_f = parse_number(line, 'F', tool_state.current_feedrate);

			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
			stepper_set_feedrate(arg_f);
			stepper_move_interpolated(arg_x, arg_y, arg_z);
			server_write("OK\r\n");
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			// do move
			break;
		case 17:
			tool_state.current_plane = PLANE_XY;
			server_write("OK\r\n");
			break;
		case 18:
			tool_state.current_plane = PLANE_ZX;
			server_write("OK\r\n");
			break;
		case 19:
			tool_state.current_plane = PLANE_YZ;
			server_write("OK\r\n");
			break;
		default:
			server_write("ERR\r\n");
			break;
		}
	}
	else if (line[0] == 'F') {
		double feedrate = parse_number(line, 'F', tool_state.current_feedrate);
	}
}

void StartDefaultTask(void const * argument)
{
	char *data;
	char line_buffer[64] = { 0 };
	int line_index = 0;
	int i;

	memset(line_buffer, 0, sizeof(line_buffer));

	MX_LWIP_Init();
	MX_USB_DEVICE_Init();

	serverTaskHandle = osThreadCreate(osThread(serverTask), NULL);

	for(;;)
	{
		if (incomingQueue != 0) {
			if (xQueueReceive(incomingQueue, (void *)&data, portMAX_DELAY) == pdTRUE)
			{
				for (i = 0; i < strlen(data); i++) {
					if (data[i] == '\r' && strlen(line_buffer) > 0) {
						// captured a line, process it
						process_line((char *)line_buffer);
						memset((char *)line_buffer, 0, sizeof(line_buffer));
						line_index = 0;
					}
					else if (data[i] != '\n') {
						if (line_index == (sizeof(line_buffer) - 1)) {
							line_index = 0;
							memset((char *)line_buffer, 0, sizeof(line_buffer));
						}
						line_buffer[line_index++] = data[i];
					}
				}

				vPortFree((void *)data);
			}
		}
		else {
			osDelay(1);
		}
	}
}
