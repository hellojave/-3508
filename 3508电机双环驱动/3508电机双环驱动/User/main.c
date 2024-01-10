#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "CAN.h"
#include "pid.h"
__IO uint32_t flag = 0;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ

CanRxMsg RxMessage;				 //���ջ�����
pid_t speed_pid,angle_pid;
INIT_STATUS init_status=DONE;
int16_t encoder,speed,eletric,c,i,angle_pidout=0;
moter_angle angle_init;
int16_t targrt_angle;
int main(void)
{		

  /*��ʼ��USART1*/

	Serial_Init();
	
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	RECIVE_CAN(&RxMessage);
	angle_init.encoder_init=0;
	angle_init.round_cnt=0;
	/* ��ʼ��pid */
    PID_Struct_Init(&angle_pid,2.0f, 0, 0.3,500,500,init_status);//(,p,i,d,int32_t max_out,int32_t integral_limit,INIT_STATUS init_status)
	PID_Struct_Init(&speed_pid,5.0f, 0.0f, 0.3f,2000,500,init_status);//(,p,i,d,int32_t max_out,int32_t integral_limit,INIT_STATUS init_status)
    printf("ready");

  while(1)
	{
	
		c++;		
		if(flag==1)
	 {		
		  
          update_angle(&angle_init,encoder);
		  
			if(c%3==0)
		{
			angle_pidout=pid_calc(&angle_pid,angle_init.angle,50);
		 //SEND_CAN(pid_calc(&angle_pid,encoder,30));
		  SEND_CAN(pid_calc(&speed_pid,speed,angle_pidout));
			
			encoder=(RxMessage.Data[0]<<8)|RxMessage.Data[1];
	     	printf("\r\n���յ�ת���ٶ����ݣ�%d \r\n",speed);
			speed=(RxMessage.Data[2]<<8)|RxMessage.Data[3];
			printf("\r\n���յ�ת�ص������ݣ�%d \r\n",eletric);
			eletric=(RxMessage.Data[4]<<8)+RxMessage.Data[5];
			c=0;
			printf("\r\n����¶�:%d��\r\n",RxMessage.Data[6]);
			printf("\r\n�յ��ĽǶȵ����ݣ�%d\r\n",encoder);
		}
			
			flag=0;
		}

		
		
	}
     
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	/*�������ж�������*/
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    //  update_angle(&angle_init,encoder);
	/* �Ƚ�ID�Ƿ�Ϊ0x1314 */ 
//	if((RxMessage.StdId==0x200) && (RxMessage.IDE==CAN_ID_STD) && (RxMessage.DLC==8) )
//	{
	flag = 1; 					       //���ճɹ�  
//	}
//	else
//	{
//	flag = 0; 					   //����ʧ��
//	}
}
