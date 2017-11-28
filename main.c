/*******************************************************************************************************************************************************
 * Copyright ¨Ï 2016 <WIZnet Co.,Ltd.> 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the ¡°Software¡±), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED ¡°AS IS¡±, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************************************************************************************************/
/**
 ******************************************************************************
 * @file    WZTOE/DHCPClient/main.c 
 * @author  IOP Team
 * @version V1.0.0
 * @date    01-May-2015
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2015 WIZnet Co.,Ltd.</center></h2>
 ******************************************************************************
 */ 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "W7500x_gpio.h"
#include "W7500x_uart.h"
#include "W7500x_crg.h"
#include "W7500x_wztoe.h"
#include "W7500x_miim.h"

//#include "W7500x_it.h"
#include "hx711.h"
#include "dhcp.h"
#include "socket.h"

#include "jiansensors.h"
/* Private typedef -----------------------------------------------------------*/
UART_InitTypeDef UART_InitStructure;

/* Private define ------------------------------------------------------------*/
#define __DEF_USED_MDIO__ 
#define __W7500P__ // for W7500

#ifndef __W7500P__ // for W7500
	#define __DEF_USED_IC101AG__ //for W7500 Test main Board V001
#endif

#define SOCK_TCPS	0
#define SOCK_UDPS	1
#define SOCK_DHCP	2

#define MY_MAX_DHCP_RETRY   3

#define ALLJSCNT 14
#define SENSORS_CNT 3
#define GAIN 1

/* Private function prototypes -----------------------------------------------*/
void delay(__IO uint32_t milliseconds); //Notice: used ioLibray
void TimingDelay_Decrement(void);

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint8_t test_buf[2048];
uint32_t my_dhcp_retry = 0;
uint8_t dest_ip[4];
uint16_t dest_port = 1234;
uint8_t mac_addr[6] = {0xEE, 0xEE, 0xEE, 0x01, 0x01, 0x01};
uint8_t sensors_used_jno[SENSORS_CNT] = {1,2,3};
uint32_t weight_vals[SENSORS_CNT];
uint32_t weight_vals_pre[SENSORS_CNT];
char udp_send_txt[256];


/**********||    |DAT  |SCK  ||****************/
/**********||----|-----|-----||****************/
/**********|| J1 |PC_08|PC_09||****************/
/**********|| J2 |PC_12|PC_13||****************/
/**********|| J3 |PC_14|PC_15||****************/
/**********|| J4 |PA_05|PA_06||****************/
/**********|| J5 |PA_07|PA_08||****************/
/**********|| J6 |PA_09|PA_10||****************/
/**********|| J7 |PA_00|PA_01||****************/
/**********|| J8 |PA_02|PA_11||****************/
/**********|| J9 |PA_12|PA_13||****************/
/**********|| J10|PA_14|PB_00||****************/
/**********|| J11|PB_01|PB_02||****************/
/**********|| J12|PB_03|PB_00||****************/
/**********|| J13|PC_01|PC_02||****************/
/**********|| J14|PC_03|PC_04||****************/
JIANBOARD_J ALLJS[ALLJSCNT] = {{GPIOC, GPIO_Pin_8, GPIOC, GPIO_Pin_9},        //J1
															 {GPIOC, GPIO_Pin_12, GPIOC, GPIO_Pin_13},      //J2
															 {GPIOC, GPIO_Pin_14, GPIOC, GPIO_Pin_15},      //J3
															 {GPIOA, GPIO_Pin_5, GPIOA, GPIO_Pin_6},        //J4
															 {GPIOA, GPIO_Pin_7, GPIOA, GPIO_Pin_8},        //J5
															 {GPIOA, GPIO_Pin_9, GPIOA, GPIO_Pin_10},       //J6
															 {GPIOA, GPIO_Pin_0, GPIOA, GPIO_Pin_1},        //J7
															 {GPIOA, GPIO_Pin_2, GPIOA, GPIO_Pin_11},       //J8
															 {GPIOA, GPIO_Pin_12, GPIOA, GPIO_Pin_13},      //J9
															 {GPIOA, GPIO_Pin_14, GPIOB, GPIO_Pin_0},       //J10
															 {GPIOB, GPIO_Pin_1, GPIOB, GPIO_Pin_2},        //J11
															 {GPIOB, GPIO_Pin_3, GPIOB, GPIO_Pin_0},        //J12
															 {GPIOC, GPIO_Pin_1, GPIOC, GPIO_Pin_2},        //J13
															 {GPIOC, GPIO_Pin_3, GPIOC, GPIO_Pin_4}};       //J14
/**
 * @brief   Main program
 * @param  None
 * @retval None
 */
int main()
{ 
    uint8_t tmp[8];
    uint32_t toggle = 1;
    int32_t ret;
		char tmp_udp_send_txt[10];
		int i;
		

    /* External Clock */
    //CRG_PLL_InputFrequencySelect(CRG_OCLK);

    /* Set Systme init */
    SystemInit();
		S_UART_Init(115200);
    /* SysTick_Config */
    SysTick_Config((GetSystemClock()/1000));
    /* Set WZ_100US Register */
    setTIC100US((GetSystemClock()/10000));
	  /* Sensors init*/
		for(i=0;i<SENSORS_CNT;i++)
    {
			JianSensor_Init(ALLJS[sensors_used_jno[i]-1], SENSORS_CNT);
		}
		
		for(i=0;i<SENSORS_CNT;i++)
		{
			weight_vals_pre[i] = JianSensor_Read(ALLJS[sensors_used_jno[i]-1], GAIN);
		}

#ifdef __DEF_USED_IC101AG__ //For using IC+101AG
    *(volatile uint32_t *)(0x41003068) = 0x64; //TXD0 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x4100306C) = 0x64; //TXD1 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003070) = 0x64; //TXD2 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003074) = 0x64; //TXD3 - set PAD strengh and pull-up
    *(volatile uint32_t *)(0x41003050) = 0x64; //TXE  - set PAD strengh and pull-up
#endif	

#ifdef __W7500P__
	*(volatile uint32_t *)(0x41003070) = 0x61;
	*(volatile uint32_t *)(0x41003054) = 0x61;
#endif


#ifdef __DEF_USED_MDIO__ 
    /* mdio Init */
    mdio_init(GPIOB, MDC, MDIO );
    //mdio_error_check(GPIOB, MDC, MDIO); //need verify...
    /* PHY Link Check via gpio mdio */
    while( link() == 0x0 )
    {
        printf(".");  
        delay(500);
    }
    printf("PHY is linked. \r\n");  
#else
    delay(1000);
#endif

    /* Network Configuration (Default setting) */
    setSHAR(mac_addr);

    /* Set Network Configuration */
    //wizchip_init(tx_size, rx_size);

    /* DHCP client Initialization */
    DHCP_init(SOCK_DHCP, test_buf);
    /* DHCP IP allocation and check the DHCP lease time (for IP renewal) */
    while(1)
    {
			  printf("> weight:");
			  for(i=0;i<SENSORS_CNT;i++)
        {
					weight_vals[i] = JianSensor_Read(ALLJS[sensors_used_jno[i]-1], GAIN);
				  printf(" %d", (int32_t)weight_vals[i]-18641);
				}
				printf("\r\n");
        switch(DHCP_run())
        {
            case DHCP_IP_ASSIGN:
            case DHCP_IP_CHANGED:
                toggle = 1;
                if(toggle)
                {
										getSIPR(tmp); printf("> DHCP IP : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                    getGAR(tmp);  printf("> DHCP GW : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                    getSUBR(tmp); printf("> DHCP SN : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);               
                    toggle = 0;
                    close(SOCK_TCPS); /* 
																				If renewal IP address was defferent previous IP address, 
																				socket becomes to disconnect or close for new connection.
																			*/
                }  						
                break;
            case DHCP_IP_LEASED:
                //
                if(toggle)
                {
										getSIPR(tmp); printf("> DHCP IP : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
									  memcpy(dest_ip, tmp ,sizeof(uint8_t)*4);
									  dest_ip[3] = 255;
                    getGAR(tmp);  printf("> DHCP GW : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                    getSUBR(tmp); printf("> DHCP SN : %d.%d.%d.%d\r\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                    toggle = 0;
                }
								sprintf(tmp_udp_send_txt,"%.2X%.2X",mac_addr[4],mac_addr[5]);
								strcpy(udp_send_txt, tmp_udp_send_txt);
								strcat(udp_send_txt," H");
								for(i=0;i<SENSORS_CNT;i++)
								{
									sprintf(tmp_udp_send_txt, " %d", (int32_t)weight_vals[i]-18641);
									strcat(udp_send_txt,tmp_udp_send_txt);
								}
								
                // TO DO YOUR NETWORK APPs.
								switch(getSn_SR(SOCK_UDPS))
								{
									case SOCK_CLOSE_WAIT:
										printf("%d:CloseWait\r\n",SOCK_UDPS);
										if((ret=disconnect(SOCK_UDPS)) != SOCK_OK){
										break;}
										printf("%d:Closed\r\n",SOCK_UDPS);
										break;
									case SOCK_CLOSED:
										ret = socket(SOCK_UDPS,Sn_MR_UDP,5000,0);
										if(ret != SOCK_UDPS){
										printf("%d:Socket Error\r\n",SOCK_UDPS);
										while(1);
										}else{
										printf("Socket OK\n\r");
										}
										break;
							  }
								ret = sendto(SOCK_UDPS, (uint8_t*)udp_send_txt, strlen(udp_send_txt), dest_ip, dest_port);
                break;

            case DHCP_FAILED:
                /* ===== Example pseudo code =====  */
                // The below code can be replaced your code or omitted.
                // if omitted, retry to process DHCP
                my_dhcp_retry++;
                if(my_dhcp_retry > MY_MAX_DHCP_RETRY)
                {
#if DEBUG_MODE != DEBUG_NO
                    printf(">> DHCP %d Failed\r\n",my_dhcp_retry);
#endif
                    my_dhcp_retry = 0;
                    DHCP_stop();      // if restart, recall DHCP_init()
                }
                break;
            default:
                break;
        }	

    memset(udp_send_txt,0,sizeof(udp_send_txt));
    }

}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in milliseconds.
 * @retval None
 */
void delay(__IO uint32_t milliseconds)
{
    TimingDelay = milliseconds;

    while(TimingDelay != 0);
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

