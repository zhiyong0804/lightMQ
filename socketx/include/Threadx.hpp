#ifndef THREADX_HPP
#define THREADX_HPP

#include "utilx.hpp"

namespace socketx
{

    /********** Semaphore ************/
    class semaphore{
        private:
            std::mutex mut;
            std::condition_variable cv;
            ssize_t count;
        public:
            semaphore(ssize_t c);
            void P();
            void V();
    };

    /********* Thread safe queue *********/
    template<typename T>
    class squeue{
        private:
            mutable std::mutex mut;
            std::queue<std::shared_ptr<T>> data_queue;
            std::condition_variable cond;
        public:
            squeue()=default;
            void wait_pop(T &value);
            bool try_pop(T &value);
            std::shared_ptr<T> wait_pop();
            std::shared_ptr<T> try_pop();
            void push(T value);
            bool empty() const;
    };

    template<typename T>
    void squeue<T>::wait_pop(T &value){
        std::unique_lock<std::mutex> lk(mut);
        cond.wait(lk,[this]{return !data_queue.empty();});
        value = std::move(*data_queue.front());
        data_queue.pop();
    }

    template<typename T>
    bool squeue<T>::try_pop(T &value){
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }

    template<typename T>
    std::shared_ptr<T> squeue<T>::wait_pop(){
        std::unique_lock<std::mutex> lk(mut);
        cond.wait(lk,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    template<typename T>
    std::shared_ptr<T> squeue<T>::try_pop(){
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    template<typename T>
    void squeue<T>::push(T value){
        std::shared_ptr<T> data = std::make_shared<T>(std::move(value));
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        cond.notify_one();
    }

    template<typename T>
    bool squeue<T>::empty() const{
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

    /********* Circular queue **********/
    template<typename T>
    class cirqueue{
        private:
            size_t num;
            size_t front;
            size_t rear;
            semaphore *slots;
            semaphore *items;
            semaphore *mut;
            std::vector<std::shared_ptr<T>> data;
        public:
            cirqueue(size_t n=0);
            void wait_pop(T &value);
            void wait_push(T value);
            bool empty();
    };

    template<typename T>
    cirqueue<T>::cirqueue(size_t n){
        num = n;
        front = rear = 0;
        slots = new semaphore(num);
        items = new semaphore(0);
        mut = new semaphore(num);
        data = std::vector<std::shared_ptr<T>>(num);
    }
    
    template<typename T>
    void cirqueue<T>::wait_pop(T &value){
        items->P();
        mut->P();
        value = *data[front++%num];
        mut->V();
        slots->V();
    }

    template<typename T>
    void cirqueue<T>::wait_push(T value){
        slots->P();
        mut->P();
        data[rear++%num] = std::make_shared<T>(value);
        mut->V();
        items->V();
    }

    template<typename T>
    bool cirqueue<T>::empty(){
        mut->P();
        bool ret = data.empty();
        mut->V();
        return ret;
    }

    /********* Thread pool ************/
    class ThreadPool{
        private:
            std::vector<std::thread> workers;
            socketx::squeue<std::function<void()>> tasks;
            size_t thread_num;
            std::atomic<bool> done;
            std::atomic<int> idleThreadNum;

            /*Get a task from tasks queue*/
            void worker();
        public:
            ThreadPool(size_t num=std::thread::hardware_concurrency());
            virtual ~ThreadPool();

            int getThreadNum(){
                return thread_num;
            }
            int getIdleThreadNum(){
                return idleThreadNum;
            }

            void addThread();
            /*Submit a task f to tasks queue, then wait for the return value.*/
            template<typename F>
            auto submit(F f) -> std::future<typename std::result_of<F()>::type>;
    };

    template<typename F>
    auto ThreadPool::submit(F f) -> std::future<typename std::result_of<F()>::type>{
        using return_type = typename std::result_of<F()>::type;

        /*Since packaged_task is non-copyable, we cannot pass it to std::function<>.
        * Instead, making a shared_ptr point to it and invoking it by a lambda function.
        */
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::move(f));
        std::function<void()> func = [task]{(*task)();};
        tasks.push(func);

        std::future<return_type> res = task->get_future();
        return res;
    }
}

#endif
