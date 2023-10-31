/**
 ****************************************************************************************************
 * @file        lwip_demo.h
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
 
#ifndef _LWIP_DEMO_H
#define _LWIP_DEMO_H
#include "./SYSTEM/sys/sys.h"

#define LWIP_DEMO_TX_BUFSIZE         200      /* ��������ݳ��� */
#define LWIP_DEMO_RX_BUFSIZE         200      /* ���������ݳ��� */
#define LWIP_SEND_DATA              0X80      /* ���������ݷ��� */
extern volatile uint8_t g_lwip_send_flag;              /* ���ݷ��ͱ�־λ */
extern int g_sock_conn;
void lwip_demo(void);

#endif /* _CLIENT_H */
