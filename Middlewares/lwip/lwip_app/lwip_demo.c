/**
 ****************************************************************************************************
 * @file        lwip_demo
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       lwIP SOCKET TCPServer ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */
 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>
#include <stdio.h>
#include <lwip/sockets.h>
#include "./BSP/LCD/lcd.h"
#include "./BSP/CAN/can.h"
#include "./MALLOC/malloc.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip_demo.h"


/* ����Զ��IP��ַ */
//#define DEST_IP_ADDR0               192
//#define DEST_IP_ADDR1               168
//#define DEST_IP_ADDR2                 1
//#define DEST_IP_ADDR3               167

/* ��Ҫ�Լ�����Զ��IP��ַ */
//#define IP_ADDR   "192.168.1.167"
#define LWIP_DEMO_PORT               8088                       /* ���ӵı��ض˿ں� */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )    /* ���������߳����ȼ� */

/* �������ݻ����� */
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 
/* ������������ */
uint8_t g_lwip_demo_sendbuf[LWIP_DEMO_TX_BUFSIZE] = {0};

/* ���ݷ��ͱ�־λ */
volatile uint8_t g_lwip_send_flag;
int g_sock_conn;                          /* ����� socked */
int g_lwip_connect_state = 0;
static void lwip_send_thread(void *arg);
extern QueueHandle_t g_display_queue;     /* ��ʾ��Ϣ���о�� */

/**
 * @brief       ���������߳�
 * @param       ��
 * @retval      ��
 */
void lwip_data_send(void)
{
    sys_thread_new("lwip_send_thread", lwip_send_thread, NULL, 512, LWIP_SEND_THREAD_PRIO );
}

/**
 * @brief       lwip_demoʵ�����
 * @param       ��
 * @retval      ��
 */
void lwip_demo(void)
{
    struct sockaddr_in server_addr; /* ��������ַ */
    struct sockaddr_in conn_addr;   /* ���ӵ�ַ */
    socklen_t addr_len;             /* ��ַ���� */
    int err;
    int length;
    int sock_fd;
    char *tbuf;
    BaseType_t lwip_err;
    lwip_data_send();                                    /* ����һ�������߳� */
	  
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* ����һ���µ�socket���� */
    memset(&server_addr, 0, sizeof(server_addr));        /* ����������ַ��� */
    server_addr.sin_family = AF_INET;                    /* ��ַ���� */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);     /* ע��ת��Ϊ�����ֽ��� */
    server_addr.sin_port = htons(LWIP_DEMO_PORT);        /* ʹ��SERVER_PORTָ��Ϊ����ͷ�趨�Ķ˿ں� */

    tbuf = mymalloc(SRAMIN, 200); /* �����ڴ� */
    sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT); /* �ͻ��˶˿ں� */
    //lcd_show_string(5, 150, 200, 16, 16, tbuf, BLUE);
	printf("%s\r\n", tbuf);
    
    err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); /* ������ */

    if (err < 0)                /* �����ʧ����ر��׽��� */
    {
        closesocket(sock_fd);   /* �ر��׽��� */
        myfree(SRAMIN, tbuf);
    }

    err = listen(sock_fd, 4);   /* ������������ */

    if (err < 0)                /* �������ʧ����ر��׽��� */
    {
        closesocket(sock_fd);   /* �ر��׽��� */
    }
    
    while(1)
    {
        g_lwip_connect_state = 0;
        addr_len = sizeof(struct sockaddr_in); /* �����ӵ�ַ��ֵ��addr_len */

        g_sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len); /* �Լ�����������������ӣ�״̬��ֵ��sock_conn */

        if (g_sock_conn < 0) /* ״̬С��0�������ӹ��ϣ���ʱ�ر��׽��� */
        {
            closesocket(sock_fd);
        }
        else 
        {
            //lcd_show_string(5, 90, 200, 16, 16, "State:Connection Successful", BLUE);
			printf("%s\r\n", "State:Connection Successful");
            g_lwip_connect_state = 1;
        }

        while (1)
        {
            memset(g_lwip_demo_recvbuf,0,LWIP_DEMO_RX_BUFSIZE);
            length = recv(g_sock_conn, (unsigned int *)g_lwip_demo_recvbuf, sizeof(g_lwip_demo_recvbuf), 0); /* ���յ������ݷŵ�����Buff */
            
            if (length <= 0)
            {
                goto atk_exit;
            }
            
//            printf("%s",g_lwip_demo_recvbuf);
            lwip_err = xQueueSend(g_display_queue,&g_lwip_demo_recvbuf,0);

            if (lwip_err == errQUEUE_FULL)
            {
                printf("����Key_Queue���������ݷ���ʧ��!\r\n");
            }
        }
atk_exit:
        if (g_sock_conn >= 0)
        {          
            closesocket(g_sock_conn);
            g_sock_conn = -1;
            //lcd_fill(5, 89, lcddev.width,110, WHITE);
            //lcd_show_string(5, 90, 200, 16, 16, "State:Disconnect", BLUE);
			printf("%s\r\n", "State:Disconnect");
            myfree(SRAMIN, tbuf);
        }
        
    }
}

extern CanPack can_pack;
/**
 * @brief       ���������̺߳���
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
static void lwip_send_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    
    while (1)
    {
        if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1)) /* ������Ҫ���� */
        {
            send(g_sock_conn, &can_pack, sizeof(can_pack), 0); /* �������� */
            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }
        vTaskDelay(1);
    }
}
