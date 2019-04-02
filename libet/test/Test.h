#pragma once
#include "../Logger.h"

#define FIVE_K_TXT "E:\\File\\5K.txt"
#define TWENTY_K_TXT "E:\\File\\20K.txt"
#define TWO_HUNDRED_K_TXT "E:\\File\\200K.txt"
#define ONEM_TXT "E:\\File\\1M.txt"
#define TWOM_TXT "E:\\File\\2M.txt"
#define BUF_SIZE 1024

// ����TCP������
void TestTcpServer();

//���Զ�ʱ��
void TestTimer();

//���Թر���������
void IdleCloseTest();

//����echo������
void HeartbeatTest();

//���Զ�ʱ����txt�ı�
void PeriodicallySendTest();

//����MultiBase����txt�ı�
void MultiEbPeriodicallySendTest();

//���Ե�EventBase�������̳߳�
void OneIoMultiWork();

//���Ե���ģʽ
void TestSingleton();

