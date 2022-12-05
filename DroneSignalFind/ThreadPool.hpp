#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <mutex>
#include <vector>
#include <queue>

template<typename T_in, typename T_out>
class ThreadPool
{
public:
    typedef void thread_func_type(T_in*, T_out*);

    void Start();
    void QueueJob(const std::function<thread_func_type>& job, T_in* ptr, T_out* res);
    void Stop();
    bool busy();

private:
    void ThreadLoop();

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::mutex result_mutex;                 // Prevents data races to result data
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<thread_func_type>> jobs;
    std::queue<T_in*> jobs_ptrs;            // job parameters
    std::queue<T_out*> jobs_res;            // job results
    bool common_result = false;             // result of all jobs
};

template<typename T_in, typename T_out>
void ThreadPool<T_in, T_out>::Start()
{
    const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    threads.resize(num_threads);
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
    }
}

template<typename T_in, typename T_out>
void ThreadPool<T_in, T_out>::QueueJob(const std::function<thread_func_type>& job, T_in* ptr, T_out* res)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
        jobs_ptrs.push(ptr);
        jobs_res.push(res);
    }
    mutex_condition.notify_one();
}

template<typename T_in, typename T_out>
void ThreadPool<T_in, T_out>::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}

template<typename T_in, typename T_out>
bool ThreadPool<T_in, T_out>::busy()
{
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = jobs.empty();
    }
    return poolbusy;
}

template<typename T_in, typename T_out>
void ThreadPool<T_in, T_out>::ThreadLoop()
{
    while (true) {
        std::function<thread_func_type> job;
        T_in* ptr;
        T_out* res;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
                });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
            ptr = jobs_ptrs.front();
            jobs_ptrs.pop();
            res = jobs_res.front();
            jobs_res.pop();
        }
        T_out tmp_res;
        job(ptr, &tmp_res);
        //{
        //    std::unique_lock<std::mutex> lock(result_mutex);
        //    *res = tmp_res;
        //    if (result)
        //        common_result = true;
        //}
    }
}

#endif // THREADPOOL_H