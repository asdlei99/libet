#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <limits>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>

#include "Global.h"
#include "Util.h"


template <typename T>
struct SafeQueue : private std::mutex, private noncopyable 
{
	static const int wait_infinite = (std::numeric_limits<int>::max)();
	// 0 �����ƶ����е�������
	SafeQueue(size_t capacity = 0) : capacity_(capacity), exit_(false) {}
	//�������򷵻�false
	bool push(T &&v);
	//��ʱ�򷵻�T()
	T pop_wait(int waitMs = wait_infinite);
	//��ʱ����false
	bool pop_wait(T *v, int waitMs = wait_infinite);

	size_t size();
	void exit();
	bool exited() { return exit_; }

private:
	std::list<T> items_;
	std::condition_variable ready_;
	size_t capacity_;
	std::atomic<bool> exit_;
	void wait_ready(std::unique_lock<std::mutex> &lk, int waitMs);
};

//typedef std::function<void()> Task;
// reduce compile time and object file size.
//extern template class SafeQueue<Task>;

struct ThreadPool : private noncopyable 
{
	//�����̳߳�
	ThreadPool(int threads, int taskCapacity = 0, bool start = true);
	~ThreadPool();
	void start();
	ThreadPool &exit() {
		tasks_.exit();
		return *this;
	}
	void join();

	//����������false
	bool addTask(Task &&task);
	bool addTask(Task &task) { return addTask(Task(task)); }
	size_t taskSize() { return tasks_.size(); }

private:
	SafeQueue<Task> tasks_;
	std::vector<std::thread> threads_;
};

struct WorkerImp
{
	WorkerImp(size_t capacity = 0) : 
		m_tasks(new SafeQueue<Task>(capacity)) {}
	~WorkerImp() {
		delete m_tasks;
	}

	std::thread m_thread;
	SafeQueue<Task>* m_tasks;
};

struct ConnThreadPool : private noncopyable
{
	ConnThreadPool(int threads, int maxTaskNum = 0, bool start = true);
	~ConnThreadPool();
	void Start();
	ConnThreadPool&	 Exit();
	void Join();

	bool AddTask(int64_t id, Task&& task);
	bool AddTask(int64_t id, Task& task) { return AddTask(id, Task(task)); }

private:
	//sizeҪ>0
	int Index(int64_t id) { return id % m_workers.size(); }	

private:
	std::vector<WorkerImp> m_workers;
	std::atomic_bool m_exited;
};

template<typename T>
struct SafeList
{
	void EmplaceBack(T&& e) {
		lock_guard<mutex> lk(m_mtx);
		m_list.emplace_back(e);
	}
	void EmplaceBack(const T& e) {
		lock_guard<mutex> lk(m_mtx);
		m_list.emplace_back(e);
	}
	void Remove(const T& e) {
		lock_guard<mutex> lk(m_mtx);
		m_list.remove(e);
	}
	void Clear() { m_list.clear(); }

	template<typename U>
	void ForEach(const U& cb) {
		lock_guard<mutex> lk(m_mtx);
		for_each(m_list.begin(), m_list.end(), cb);
	}

private:
	std::list<T> m_list;
	std::mutex m_mtx;
};
