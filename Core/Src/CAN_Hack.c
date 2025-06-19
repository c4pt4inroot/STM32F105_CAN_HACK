/*
 * CAN_Hack.c
 *
 *  Created on: Nov 2, 2024
 *      Author: Leeo S.V
 */
#include "CAN_Hack.h"
#include "usb_device.h"
#include "strings.h"
#include "main.h"

//********************************************************************************************************
//uint8_t bufferTextCan[256];

extern TIM_HandleTypeDef htim5;

CAN_TxHeaderTypeDef TxHeader1;
CAN_TxHeaderTypeDef TxHeader2;

CAN_RxHeaderTypeDef RxHeader1;
CAN_RxHeaderTypeDef RxHeader2;

CAN_FilterTypeDef canfil1;
CAN_FilterTypeDef canfil2;

uint8_t RxData[8];
uint8_t bufferTextCan[256];

uint8_t alertValue = 0;

//********************************************************************************************************
void CAN_Init(){

	// CAN1 Settings
	canfil1.FilterBank = 0;
	canfil1.FilterMode = CAN_FILTERMODE_IDMASK;
	canfil1.FilterFIFOAssignment = CAN_RX_FIFO0;
	canfil1.FilterIdHigh = 0;
	canfil1.FilterIdLow = 0;
	canfil1.FilterMaskIdHigh = 0;
	canfil1.FilterMaskIdLow = 0;
	canfil1.FilterScale = CAN_FILTERSCALE_32BIT;
	canfil1.FilterActivation = ENABLE;
	canfil1.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1,&canfil1);

	TxHeader1.DLC = 8;
    TxHeader1.IDE = CAN_ID_STD;
    TxHeader1.RTR = CAN_RTR_DATA;
    TxHeader1.StdId = 0x030;
	TxHeader1.ExtId = 0x02;
	TxHeader1.TransmitGlobalTime = DISABLE;

	// CAN2 Settings
	canfil2.FilterBank = 14;
	canfil2.FilterMode = CAN_FILTERMODE_IDMASK;
	canfil2.FilterFIFOAssignment = CAN_RX_FIFO0;
	canfil2.FilterIdHigh = 0;
	canfil2.FilterIdLow = 0;
	canfil2.FilterMaskIdHigh = 0;
	canfil2.FilterMaskIdLow = 0;
	canfil2.FilterScale = CAN_FILTERSCALE_32BIT;
	canfil2.FilterActivation = ENABLE;
	canfil2.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2,&canfil2);

	TxHeader2.DLC = 8;
	TxHeader2.IDE = CAN_ID_STD;
	TxHeader2.RTR = CAN_RTR_DATA;
	TxHeader2.StdId = 0x030;
	TxHeader2.ExtId = 0x02;
	TxHeader2.TransmitGlobalTime = DISABLE;
}

//********************************************************************************************************
void CDC_Receive(uint8_t* Buf, uint32_t *Len){

	if (Len && *Len > 0)
	    {
		switch(Buf[0])
		{
		   case 'V':

		     switch(Buf[1])
		     {
				case 'S':
			    // Envia a resposta de volta ao PC
				CDC_Transmit_FS((uint8_t*)"SFFFFFFFFFFFFFFFF\r", 18);
				break;
			 }

		     // Envia a resposta de volta ao PC
		     CDC_Transmit_FS((uint8_t*)"VF_26_03_2019\r", 15);
		   break;

		   case 'v':
		   // Envia a resposta de volta ao PC
		   alertValue = 0x07;
		   CDC_Transmit_FS(&alertValue, 1);
		   break;

		   case 'S':

		   	   switch(Buf[1])
		   	   {
		   		   case '1':
		   		   CAN1_SetBaud (&hcan1,Buf[2]);
		   		   break;

		   		   case '2':
		   		   CAN2_SetBaud (&hcan2,Buf[2]);
		   		   break;

		   	   }
		   // Transmite a resposta de confirmação
		   CDC_Transmit_FS((uint8_t*)"\r", 1);
		   break;

		   case 'T':
			   SendCANMessageEXT(Buf, Len);
		   break;

		   case 't':
			   SendCANMessageSTD(Buf, Len);
		   break;

		   case 'O':

			   switch(Buf[1])
			   {
			   		case '1':
		   	        //ticStartLog = HAL_GetTick();
			        HAL_CAN_Init(&hcan1);
			        HAL_CAN_Start(&hcan1);
                    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
		   	        //htim5.Instance->CNT = 0;
		   	        //HAL_TIM_Base_Start(&htim5);
                    break;

			   		case '2':
			   		//ticStartLog = HAL_GetTick();
			   		HAL_CAN_Init(&hcan2);
			   		HAL_CAN_Start(&hcan2);
			   		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
			   		//htim5.Instance->CNT = 0;
			   		//HAL_TIM_Base_Start(&htim5);
			   		break;
			   }
		   // Transmite a resposta de confirmação
		   CDC_Transmit_FS((uint8_t*)"\r", 1);
		   break;
		   }
	    }
}

//********************************************************************************************************
void CAN1_SetBaud(CAN_HandleTypeDef *hcan, uint8_t Buf) {

    HAL_CAN_Stop(&hcan1);  // Para a interface CAN1
   // HAL_CAN_Stop(&hcan2);  // Para a interface CAN2

    switch (Buf) {
        // 10k
        case '1':
            hcan1.Init.Prescaler = 13;
            hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
            hcan1.Init.TimeSeg1 = CAN_BS1_16TQ;
            hcan1.Init.TimeSeg2 = CAN_BS2_8TQ;
            break;

        // 33.333k
        case '2':
        	hcan1.Init.Prescaler = 72;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_8TQ;
        break;

        // 50k
        case '3':
        	hcan1.Init.Prescaler = 72;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_5TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 62.500k
        case '4':
        	hcan1.Init.Prescaler = 144;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

        break;

        // 83.333k
        case '5':
        	hcan1.Init.Prescaler = 54;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
        break;

        // 95.238k
        case 'D':
        	hcan1.Init.Prescaler = 42;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_7TQ;
        break;

        // 100k
        case '6':
        	hcan1.Init.Prescaler = 36;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_5TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 125k
        case '7':
        	hcan1.Init.Prescaler = 32;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 250k
        case '8':
        	hcan1.Init.Prescaler = 16;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 400k
        case '9':
        	hcan1.Init.Prescaler = 18;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
        break;

        // 500k
        case 'A':
        	hcan1.Init.Prescaler = 8;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 800k
        case 'B':
        	hcan1.Init.Prescaler = 5;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 1000k
        case 'C':
        	hcan1.Init.Prescaler = 4;
        	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;
    }

    // Atualiza os novos parâmetros e reinicia a interface CAN
    HAL_CAN_Init(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

//********************************************************************************************************
void CAN2_SetBaud(CAN_HandleTypeDef *hcan, uint8_t Buf) {

   // HAL_CAN_Stop(&hcan1);  // Para a interface CAN1
    HAL_CAN_Stop(&hcan2);  // Para a interface CAN2

    switch (Buf) {
        // 10k
        case '1':
            hcan2.Init.Prescaler = 13;
            hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
            hcan2.Init.TimeSeg1 = CAN_BS1_16TQ;
            hcan2.Init.TimeSeg2 = CAN_BS2_8TQ;
            break;

        // 33.333k
        case '2':
        	hcan2.Init.Prescaler = 72;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
            hcan2.Init.TimeSeg2 = CAN_BS2_8TQ;
        break;

        // 50k
        case '3':
        	hcan2.Init.Prescaler = 72;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_5TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 62.500k
        case '4':
        	hcan2.Init.Prescaler = 144;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
        break;

        // 83.333k
        case '5':
        	hcan2.Init.Prescaler = 54;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
        break;

        // 95.238k
        case 'D':
        	hcan2.Init.Prescaler = 42;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_1TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_7TQ;
        break;

        // 100k
        case '6':
        	hcan2.Init.Prescaler = 36;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_5TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 125k
        case '7':
        	hcan2.Init.Prescaler = 32;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 250k
        case '8':
        	hcan2.Init.Prescaler = 16;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 400k
        case '9':
        	hcan2.Init.Prescaler = 18;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
        break;

        // 500k
        case 'A':
        	hcan2.Init.Prescaler = 8;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 800k
        case 'B':
        	hcan2.Init.Prescaler = 5;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;

        // 1000k
        case 'C':
        	hcan2.Init.Prescaler = 4;
        	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
        	hcan2.Init.TimeSeg1 = CAN_BS1_4TQ;
        	hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
            break;
    }

    // Atualiza os novos parâmetros e reinicia a interface CAN
    HAL_CAN_Init(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

//********************************************************************************************************
void SendCANMessageEXT(uint8_t* Buf, uint32_t *Len) {

	 LED_TX_ON;
	 // Valida o comprimento da mensagem
	 if (*Len < 24) {
	 // Mensagem inválida, encerra
		 return;
	 }

	 // Indentifica se é canal 1 ou 2
	 uint8_t canal = Buf[1] - '0'; // 1 para CAN1 e 2 para CAN2

	 // Extrai o ID
	 char idStr[9];
	 strncpy(idStr, (char*)(Buf + 2), 8);
	 idStr[8] = '\0';
	 uint32_t id = (uint32_t)strtol(idStr, NULL, 16); // Converte para hexadecimal

	 // Extrai o DLC
	 uint8_t dlc = Buf[10] - '0';

	 // Extrai os dados
	 uint8_t TxData[8] = {0}; // Inicializa o array para evitar valores indesejados
	 for (int i = 0; i < dlc; i++) {
	 char dataStr[3] = { Buf[11 + i*2], Buf[12 + i*2], '\0' };
	 TxData[i] = (uint8_t)strtol(dataStr, NULL, 16); // Converte cada byte de dados
	 }

	  // Configuração do cabeçalho CAN
	  CAN_TxHeaderTypeDef TxHeader1;
	  uint32_t TxMailbox;
	  TxHeader1.StdId = id;
	  TxHeader1.ExtId = id;
	  TxHeader1.RTR = CAN_RTR_DATA;
	  TxHeader1.IDE = CAN_ID_EXT; // Para IDs de 29 bits
	  TxHeader1.DLC = dlc;
	  TxHeader1.TransmitGlobalTime = DISABLE;

	  CAN_TxHeaderTypeDef TxHeader2;
	  //uint32_t TxMailbox;
	  TxHeader2.StdId = id;
	  TxHeader2.ExtId = id;
	  TxHeader2.RTR = CAN_RTR_DATA;
	  TxHeader2.IDE = CAN_ID_EXT; // Para IDs de 29 bits
	  TxHeader2.DLC = dlc;
	  TxHeader2.TransmitGlobalTime = DISABLE;

	  // Envia pelo canal selecionado
	  if (canal == 1) {
		 HAL_CAN_Stop(&hcan1);
		 HAL_CAN_Start(&hcan1);
		 HAL_CAN_AddTxMessage(&hcan1, &TxHeader1, TxData, &TxMailbox);

	  } else if (canal == 2) {
		 HAL_CAN_Stop(&hcan2);
		 HAL_CAN_Start(&hcan2);
	     HAL_CAN_AddTxMessage(&hcan2, &TxHeader2, TxData, &TxMailbox);

	  }

	  LED_TX_OFF;
}

//********************************************************************************************************
void SendCANMessageSTD(uint8_t* Buf, uint32_t *Len) {

    LED_TX_ON;
    // Valida o comprimento da mensagem
    if (*Len < 16) {
        // Mensagem inválida, encerra
        return;
    }

    // Identifica se é canal 1 ou 2
    uint8_t canal = Buf[1] - '0'; // 1 para CAN1 e 2 para CAN2

    // Extrai o ID (deve ter até 3 dígitos hexadecimais para ID de 11 bits)
    char idStr[4];
    strncpy(idStr, (char*)(Buf + 2), 3);
    idStr[3] = '\0';
    uint32_t id = (uint32_t)strtol(idStr, NULL, 16); // Converte para hexadecimal

    // Extrai o DLC
    uint8_t dlc = Buf[5] - '0';

    // Extrai os dados
    uint8_t TxData[8] = {0}; // Inicializa o array para evitar valores indesejados
    for (int i = 0; i < dlc; i++) {
        char dataStr[3] = { Buf[6 + i*2], Buf[7 + i*2], '\0' };
        TxData[i] = (uint8_t)strtol(dataStr, NULL, 16); // Converte cada byte de dados
    }

    // Configuração do cabeçalho CAN para ID padrão
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    TxHeader.StdId = id;         // ID padrão (não estendido)
    TxHeader.ExtId = 0;          // Não usado para IDs de 11 bits
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_STD;   // Para IDs de 11 bits
    TxHeader.DLC = dlc;
    TxHeader.TransmitGlobalTime = DISABLE;

    // Envia pelo canal selecionado
    if (canal == 1) {
        HAL_CAN_Stop(&hcan1);
        HAL_CAN_Start(&hcan1);
        HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
    } else if (canal == 2) {
        HAL_CAN_Stop(&hcan2);
        HAL_CAN_Start(&hcan2);
        HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox);
    }

    LED_TX_OFF;
}

//********************************************************************************************************
void CAN_Run(){

	uint8_t sendLen;

	// CAN1
	if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0){
		LED_RX_ON;
	    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader1, RxData) == HAL_OK){
	    	if (RxHeader1.IDE == CAN_ID_EXT){
	    		sendLen = sprintf((char*)bufferTextCan, "T1%8.8X%1.1X", \
		        (unsigned int)RxHeader1.ExtId, \
		        (unsigned int)RxHeader1.DLC);
		    }
		    else{
		        sendLen = sprintf((char*)bufferTextCan, "t1%3.3X%1.1X", \
		        (unsigned int)RxHeader1.StdId, \
		        (unsigned int)RxHeader1.DLC);
		    }

		    for (uint8_t cnt = 0; cnt < RxHeader1.DLC; cnt ++){
		    	sendLen += sprintf((char*)bufferTextCan + sendLen, "%2.2X", RxData[cnt]);
		    }

		    sendLen += sprintf((char*)bufferTextCan + sendLen, "%4.4X\r", (unsigned int)htim5.Instance->CNT);
		    //sendLen += sprintf((char*)bufferTextCan + sendLen, "%4.4X\r", 200);
		    //tim5cnt = htim5.Instance->CNT;
		    CDC_Transmit_FS(bufferTextCan, sendLen);
	    }
	    LED_RX_OFF;
	}
    // CAN2
	if (HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0) > 0){
		LED_RX_ON;
		if (HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, RxData) == HAL_OK){
			if (RxHeader2.IDE == CAN_ID_EXT){
			    sendLen = sprintf((char*)bufferTextCan, "T2%8.8X%1.1X", \
			    (unsigned int)RxHeader2.ExtId, \
			    (unsigned int)RxHeader2.DLC);
			}
			else{
			     sendLen = sprintf((char*)bufferTextCan, "t2%3.3X%1.1X", \
			     (unsigned int)RxHeader2.StdId, \
			     (unsigned int)RxHeader2.DLC);
			}

		for (uint8_t cnt = 0; cnt < RxHeader2.DLC; cnt ++){
			sendLen += sprintf((char*)bufferTextCan + sendLen, "%2.2X", RxData[cnt]);
		}

		sendLen += sprintf((char*)bufferTextCan + sendLen, "%4.4X\r", (unsigned int)htim5.Instance->CNT);
		//sendLen += sprintf((char*)bufferTextCan + sendLen, "%4.4X\r", 200);
		//tim5cnt = htim5.Instance->CNT;
		CDC_Transmit_FS(bufferTextCan, sendLen);
		}
		LED_RX_OFF;
	}


}
