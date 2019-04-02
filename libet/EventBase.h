#pragma once
#include <memory>
#include <WinSock2.h>

#include "Global.h"
#include "Threads.h"
#include "Buffer.h"

struct EventBases : private noncopyable 
{
	virtual EventBase* AllocBase() = 0;
};

//�¼�������
struct EventBase : public EventBases
{
	EventBase(int taskCapacity = 0);
	~EventBase();

	//�����¼�����ѭ��
	void Loop();
	void LoopOnce(int waitMs);
	//ȡ����ʱ������timer�Ѿ����ڣ������
	bool Cancel(TimerId timerid);
	//��ʱ����,������HandleTimeouts��ִ��
	TimerId RunAt(int64_t milli, const Task &task, int64_t interval = 0) { return RunAt(milli, Task(task), interval); }
	TimerId RunAt(int64_t milli, Task &&task, int64_t interval = 0);
	TimerId RunAfter(int64_t milli, const Task &task, int64_t interval = 0) { return RunAt(util::timeMilli() + milli, Task(task), interval); }
	TimerId RunAfter(int64_t milli, Task &&task, int64_t interval = 0) { return RunAt(util::timeMilli() + milli, std::move(task), interval); }
	TimerId RunEvery(const Task &task, int64_t interval = 0) { return RunAt(util::timeMilli(), Task(task), interval); }
	TimerId RunEvery(Task &&task, int64_t interval = 0) { return RunAt(util::timeMilli(), std::move(task), interval); }

	//�˳��¼�ѭ��
	EventBase& Exit();
	bool Exited();
	void Wakeup();
	void SafeCall(Task &&task);
	void SafeCall(const Task &task) { SafeCall(Task(task)); }
	virtual EventBase* AllocBase() override { return this; }	

	void AddConn(TcpConnPtr&& conn);
	void AddConn(const TcpConnPtr& conn);
	void RemoveConn(const TcpConnPtr& conn);

	void PostDataToOneConn(Buffer& buf, const std::weak_ptr<TcpConn>& wpConn);
	void PostDataToAllConns(Buffer& buf);


public:
	int CreatePipe(SOCKET fds[2]);
	void Init();
	//����ʱ�䵽�������ִ�лص�����,��HandleTimeouts��ִ��
	void CallIdles();

	IdleId RegisterIdle(int idle, const TcpConnPtr &con, const TcpCallBack &cb);
	void UnregisterIdle(const IdleId &id);
	void UpdateIdle(const IdleId &id);
	void HandleTimeouts();
	void RefreshNearest(const TimerId *tid = nullptr);
	void RepeatableTimeout(TimerRepeatable *tr);

	//void SendThreadProc();

public:
	PollerBase* m_poller;
	std::atomic<bool> m_exit;
	SOCKET m_wakeupFds[2];
	int m_nextTimeout;
	SafeQueue<Task> m_tasks;
	std::unique_ptr<ConnThreadPool> m_upConnThreadPool;

	std::map<TimerId, TimerRepeatable> m_timerReps;
	std::map<TimerId, Task> m_timers;
	std::atomic<int64_t> m_timerSeq;	//��ʱ�����
	// ��¼ÿ��idleʱ��㣨��λ�룩�����е����ӡ������е��������ӣ����µĲ��뵽����ĩβ���������л��������Ӵ��������Ƶ�����β���������ο�memcache
	std::map<int, std::list<IdleNode>> m_idleConns;
	bool m_idleEnabled;

	SafeList<TcpConnPtr> m_conns;
};

//���̵߳��¼��ɷ���
struct MultiBase : public EventBases 
{
	MultiBase(int sz) : m_id(0), m_bases(sz) {}
	virtual EventBase *AllocBase() override;
	void Loop();
	MultiBase &Exit();

private:
	std::atomic<int> m_id;
	std::vector<EventBase> m_bases;
};

struct Channel : private noncopyable
{
	Channel(EventBase* base, int fd);
	~Channel();

	EventBase* GetBase() { return m_base; }
	SOCKET fd() { return m_fd; }
	int64_t id() { return m_id; }

	/* 
	 * �����ر����ӣ�HandleRead->Cleanup->~Channel->Channel::Close->HandleRead->~TcpConn
	 * �����ر����ӣ�TcpConn::Close->Channel::Close->HandleRead->Cleanup->~Channel->Channel::Close->~TcpConn
	 * TryDecodeʧ�ܣ�Channel::Close->HandleRead->Cleanup->~Channel->Channel::Close->~TcpConn
	*/
	void Close();

	//�ҽ��¼�������
	void OnRead(const Task &readcb) { m_readCb = readcb; }
	void OnWrite(const Task &writecb) { m_writeCb = writecb; }
	void OnRead(Task &&readcb) { m_readCb = std::move(readcb); }
	void OnWrite(Task &&writecb) { m_writeCb = std::move(writecb); }

	//�����д�¼�
	void HandleRead() { m_readCb(); }
	void HandleWrite() { m_writeCb(); }

	void EnableRead(bool enable);
	void EnableWrite(bool enable);
	bool Readable() { return m_readable; }
	bool Writable() { return m_writable; }

protected:
	SOCKET m_fd;							
	EventBase* m_base;
	int64_t m_id;
	bool m_readable, m_writable;
	std::function<void()> m_readCb, m_writeCb, m_errorCb;
};