/*
 * CAN_Hack.h
 *
 *  Created on: Nov 2, 2024
 *      Author: Leeo S.V
 */

#ifndef INC_CAN_HACK_H_
#define INC_CAN_HACK_H_

#include "usbd_cdc_if.h"
#include "main.h"
//********************************************************************************************************
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

extern TIM_HandleTypeDef htim5;

#define LED_TX_ON   HAL_GPIO_WritePin(GPIOA,TX_Pin, GPIO_PIN_SET);
#define LED_TX_OFF  HAL_GPIO_WritePin(GPIOA,TX_Pin, GPIO_PIN_RESET);
#define LED_RX_ON   HAL_GPIO_WritePin(GPIOA,RX_Pin, GPIO_PIN_SET);
#define LED_RX_OFF  HAL_GPIO_WritePin(GPIOA,RX_Pin, GPIO_PIN_RESET);

//********************************************************************************************************
void CAN_Init();
void CAN_Run();
void CDC_Receive(uint8_t* Buf, uint32_t *Len);
void CAN_SetBaud(CAN_HandleTypeDef *hcan, uint8_t Buf);
//********************************************************************************************************
#endif /* INC_CAN_HACK_H_ */
