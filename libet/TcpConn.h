#pragma once
#include <assert.h>
#include <atomic>

#include "Global.h"
#include "Buffer.h"
#include "Net.h"
#include "Util.h"

struct TcpConn : public std::enable_shared_from_this<TcpConn>, private noncopyable
{
	enum State {
		Invalid = 1,
		Handshaking,
		Connected,
		Closed,
		Failed,
	};
	// Tcp���캯����ʵ�ʿ��õ�����Ӧ��ͨ��createConnection����
	TcpConn();
	virtual ~TcpConn();

	EventBase* GetBase() { return m_base; }
	Buffer& GetInput() { return m_input; }
	Buffer& GetOutput() { return m_output; }
	Channel* GetChannel() { return m_channel; }
	std::atomic_int& GetState() { return m_state; }

	//���ݵ���ʱ�ص���HandleRead��
	void OnRead(const TcpCallBack &cb) {
		assert(!m_readCb);
		m_readCb = cb;
	};
	//��tcp��������дʱ�ص�
	void OnWrite(const TcpCallBack &cb) { m_writeCb = cb; }
	// tcp״̬�ı�ʱ�ص�
	void OnState(const TcpCallBack &cb) { m_stateCb = cb; }
	// tcp���лص�
	void AddIdleCB(int idle, const TcpCallBack &cb);

	//��Ϣ�ص����˻ص���OnRead�ص���ͻ��ֻ�ܹ�����һ��, ������OnRead��
	void OnMsg(CodecBase* codec, const MsgCallBack& cb);

	//��ʱ����
	void OnTimer(const TcpCallBack& cb) { m_timerCb = cb; }

	// conn�����¸��¼����ڽ��д���
	void Close();

	//��������ͨ��������ͨ��ע��read/write�¼�������ͨ����ӵ�PollerBase
	void Attach(EventBase* base, int fd, Ip4Addr local, Ip4Addr peer);

	//�������ݣ����̰߳�ȫ
	void Send(const char *buf, size_t len);
	void Send(const char *s) { Send(s, strlen(s)); }
	void Send(const std::string &s) { Send(s.data(), s.size()); }
	void Send(Buffer & buf);
	void SendOutput() { Send(m_output); }

	//������Ϣ���̰߳�ȫ
	//void SendMsg(Slice msg);
	void SendMsg(const Slice msg);
	//void SendMsg(Buffer buf);

public:
	int HandleHandshake(const TcpConnPtr &con);
	void HandleRead(const TcpConnPtr &con);
	/* �ڷ������ݵ�ʱ����ֱ��Send��ʣ��û���͵�������HandleWrite�з��ͣ�
	���û�������һֱ����HandleWrite���������ر�дͨ�� */
	void HandleWrite(const TcpConnPtr &con);

	//����m_spChannel
	void Cleanup(const TcpConnPtr &con);
	int SendBase(const char * buf, size_t len);

public:
	EventBase* m_base;
	Channel* m_channel;	//����ͨ��
	Buffer m_input, m_output;

	Ip4Addr m_local, m_peer;
	std::atomic_int m_state;
	TcpCallBack m_readCb, m_writeCb, m_stateCb;
	TcpCallBack m_timerCb;
	std::list<IdleId> m_idleIds;	//ά������ʱ����µ���������
	TimerId m_timeoutId;	//δʹ��
	std::list<TimerId> m_timerTasks; //ά�����ж�ʱ����
	std::unique_ptr<CodecBase> m_codec;
};