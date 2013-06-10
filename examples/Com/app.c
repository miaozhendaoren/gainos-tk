/* Copyright(C) 2013, GaInOS-TK by Fan Wang. All rights reserved.
 * Generated by GaInOS-TK Studio at 2013-06-01:15-55-19.
 * Don't Modify it by hand.
 * Email: parai@foxmail.com
 * URL: https://github.com/parai/gainos-tk  && http://hi.baidu.com/parai
 */
#include "Os.h"
#include <stdio.h>

#include "Com.h"
#include "Can.h"
#include "CanIf.h"
#include "J1939Tp.h"
#include "PduR.h"

void onSignal0Receive(void)
{
    static uint8 signal[8] = {0,0,0};
    printf("onSignal0Receive().\r\n");
    Com_ReceiveSignal(vCom_IPdu1_RX_Signal0,signal);
    printf("Signal = [0x%x, 0x%x,0x%x].\r\n",signal[0],signal[1],signal[2]);
}
void onSignal1Receive(void)
{
    static uint8 signal = 0;
    printf("onSignal1Receive().\r\n");
    Com_ReceiveSignal(vCom_IPdu1_RX_Signal1,&signal);
    printf("Signal = 0x%x.\r\n",signal);
}
TASK(vTaskSender)
{
    /* Add your task special code here, but Don't delete this Task declaration.*/
    static uint8 callcnt = 0;
    static uint8 signal[8] = {0,1,2,0,4,0,0,0};
    static PduInfoType pduinfo = {signal,8};
    signal[0]++; signal[1]++; signal[2]++; signal[4]++;
    callcnt ++;
    if(callcnt%5 == 0)
    {
        signal[3] = 1;//set update bit
    }
    else if((callcnt+2)%5 == 0)
    {
        signal[5] = 1;//set update bit
    }
    CanIf_Transmit(vCanIf_Channel_0, &pduinfo);
    signal[5] = signal[3] = 0;//clear update bit
    if((callcnt)%0x0E == 0)
    {
        Com_SendSignal(vCom_IPdu1_TX_Signal0,signal);
        //Com_TriggerIPduSend(vCom_IPdu1_TX);
    }
    (void)TerminateTask();
}

TASK(vTaskReceiver)
{
    /* Add your task special code here, but Don't delete this Task declaration.*/
    (void)TerminateTask();
}

TASK(vTaskMainFunction)
{
    /* Add your task special code here, but Don't delete this Task declaration.*/
    //(void)ActivateTask(ID_vTaskInit);
    Com_MainFunctionRx();
    Com_MainFunctionTx();
    (void)TerminateTask();
}
ALARM(vAlarmSender)
{
    /* Alarm Type: Task, you still can add your special code here.*/
    (void)ActivateTask(ID_vTaskSender);
}
ALARM(vAlarmReceiver)
{
    /* Alarm Type: Task, you still can add your special code here.*/
    (void)ActivateTask(ID_vTaskReceiver);
}
ALARM(vAlarmMainFunction)
{
    /* Alarm Type: Task, you still can add your special code here.*/
    (void)ActivateTask(ID_vTaskMainFunction);
}

void StartupHook(void)
{
    /* Add Code Here */
    (void)SetRelAlarm(ID_vAlarmReceiver,50,10);
	(void)SetRelAlarm(ID_vAlarmSender,100,200);
	(void)SetRelAlarm(ID_vAlarmMainFunction,200,1); //so cyclic 1 Ticks = 4ms
	Can_Init(&Can_ConfigData); 
    CanIf_Init(&CanIf_Config);
    PduR_Init(&PduR_Config);
    Com_Init(&ComConfiguration);
    Com_IpduGroupStart(vCom_IPduGrp0,TRUE);
   
    CanIf_SetControllerMode(vCanIf_Channel_0,CANIF_CS_STARTED);
    CanIf_SetControllerMode(vCanIf_Channel_1,CANIF_CS_STARTED);
}

void CanIf_UserRxIndication(uint8 channel, PduIdType pduId, const uint8 *sduPtr,
                           uint8 dlc, Can_IdType canId)
{
    int len = dlc;
    char* ptr= sduPtr;
    static int fcnt = 0;
    printf("channel=%d,pduId=%d,canId=%d.\r\r\n",(int)channel,(int)pduId,(int)canId);
    printf("[");      
    while(len > 0)
    {
        printf("0x%x,",(uint8)(*ptr));
        ptr++;
        len--;
    }
    printf("]\r\n");
}    
