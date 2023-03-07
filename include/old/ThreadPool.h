#pragma once

#include <vector>
#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <queue>
#include <condition_variable>
#include <functional>

class ThreadPool{
public:
	void Start();
	void QueueJob(const std::function<void()>& job);
	void Stop();
	bool Busy();

private:
	void ThreadLoop();

	bool should_terminate = false; //stop looking for jobs
	std::mutex queue_mutex;        //prevents data race
	std::condition_variable mutex_condition; //allows threads to wait
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> jobs;
};
	
