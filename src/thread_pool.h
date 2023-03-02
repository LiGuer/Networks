#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

class ThreadPool {
private:
	vector<thread> m_threads;
	queue<function<void()>> m_tasks;
	vector<future<void>> taskResults;

	mutex m_mutex;
	condition_variable m_cv;

	bool is_terminate = false;

public:
	ThreadPool() = default;
	~ThreadPool();

	void threadLoop();
	void init();
	void addThread(function<void()> initalizer);

	template <class F, class... Args>
	auto addTask(F&& f, Args&&... args)
		-> future<typename result_of<F(Args...)>::type>;

	void queueTask(function<void()>& task);

	void wait_all() {
		for (auto&& result : taskResults) {
			result.get();
		}
	}

};

/*
 *  Initial
 */
inline void ThreadPool::init() {
	int numThreads = thread::hardware_concurrency();
	m_threads.resize(numThreads);

	for (int i = 0; i < numThreads; i++)
		addThread([]() {; });
}

/*
 *  add a thread
 */
inline void ThreadPool::addThread(function<void()> initalizer) {
	m_threads.emplace_back([this, initalizer] {
		initalizer();
		threadLoop();
	});
}

/*
 *  Loop of thread
 */
inline void ThreadPool::threadLoop() {
	while (true) {
		function<void()> task;
		{
			unique_lock<mutex> lock(m_mutex);
			m_cv.wait(lock, [this] {
				return is_terminate || !m_tasks.empty();
			});

			if (is_terminate)
				return;

			task = move(m_tasks.front());
			m_tasks.pop();
		}

		task();
	}
}

/*
 *  add a task
 */
template <class F, class... Args>
inline auto ThreadPool::addTask(F&& f, Args&&... args)
	-> future<typename result_of<F(Args...)>::type> {

	using return_type = typename result_of<F(Args...)>::type;

	auto task = make_shared<packaged_task<return_type()>> (
		bind(forward<F>(f), forward<Args>(args)...)
	);

	future<return_type> res = task->get_future();

	{
		unique_lock<mutex> lock(m_mutex);
		m_tasks.emplace([task]() { (*task)(); });
	}

	m_cv.notify_one();

	return res;
}

void ThreadPool::queueTask(function<void()>& task) {
	{
		unique_lock<mutex> lock(m_mutex);
		m_tasks.push(task);
	}
	m_cv.notify_one();
}

/*
 *  ~ThreadPool
 */
inline ThreadPool::~ThreadPool() {
	{
		unique_lock<mutex> lock(m_mutex);
		is_terminate = true;
	}

	m_cv.notify_all();

	for (thread& worker : m_threads)
		worker.join();
}

#endif