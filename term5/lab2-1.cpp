#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <mutex>
#include <ctime>
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <condition_variable>

using namespace std;

class Producer
{
public:
    Producer(int size)
    {
        for (int i = 0; i < size; i++)
        {
            values.push_back(1);
        }
    }

    uint8_t getValue()
    {
        uint8_t res = values.back();
        values.pop_back();
        return res;
    }

    vector<uint8_t> values;
};

class Consumer
{
public:
    void add()
    {
        sum += 1;
    }

    int sum = 0;
};

class ThreadSafeMutexQueue
{
public:
    void push(uint8_t val) {
        mtx.lock();
        rawQueue.push(val);
        mtx.unlock();
    }

    bool pop(uint8_t& val) {
        mtx.lock();
        if (rawQueue.empty())
        {
            mtx.unlock();
            this_thread::sleep_for(chrono::milliseconds(1));
            mtx.lock();
            if (!rawQueue.empty())
            {
                val = rawQueue.front();
                rawQueue.pop();
                mtx.unlock();
                return true;
            }
            mtx.unlock();
            return false;
        }
        else
        {
            val = rawQueue.front();
            rawQueue.pop();
            mtx.unlock();
            return true;
        }
    }

    bool empty()
    {
        return rawQueue.empty();
    }

private:
    queue<uint8_t> rawQueue;
    mutex mtx;
};


class ThreadSafeMutexSizeQueue
{
public:
    ThreadSafeMutexSizeQueue(int size)
    {
        maxSize = size;
    }

    void push(uint8_t val) {
        unique_lock<mutex> unique_l(mtx);
        cv.wait(unique_l, [this]
        {
            return (rawQueue.size() < maxSize);
        });
        rawQueue.push(val);
    }

    bool pop(uint8_t& val) {
        mtx.lock();
        if (rawQueue.empty())
        {
            mtx.unlock();
            this_thread::sleep_for(chrono::milliseconds(1));
            mtx.lock();
            if (!rawQueue.empty())
            {
                val = rawQueue.front();
                rawQueue.pop();
                mtx.unlock();
                cv.notify_all();
                return true;
            }
            mtx.unlock();
            return false;
        }
        else
        {
            val = rawQueue.front();
            rawQueue.pop();
            mtx.unlock();
            cv.notify_all();
            return true;
        }
    }

    bool empty()
    {
        return rawQueue.empty();
    }

private:
    queue<uint8_t> rawQueue;
    mutex mtx;
    int maxSize;
    condition_variable cv;
};

const int taskNum = 1024;
ThreadSafeMutexSizeQueue *q;

void writeQueue(Producer producer)
{
    while (!producer.values.empty())
    {
        q->push(producer.getValue());
    }
}

void readQueue(Consumer* consumer)
{
    uint8_t val;
    while (q->pop(val))
    {
        consumer->add();
    }
}

void spawnThreads(int consumersNum, int producersNum, Consumer *consumers)
{
    vector<thread> threadsConsumers(consumersNum);
    vector<thread> threadsProducers(producersNum);
    int start1, start2;

    start1 = clock();
    for (int i = 0; i < producersNum; i++)
    {
        threadsProducers[i] = thread(writeQueue, Producer(taskNum/producersNum));
    }

    start2 = clock();
    for (int i = 0; i < consumersNum; i++)
    {
        threadsConsumers[i] = thread(readQueue, &consumers[i]);
    }

    for (auto& t : threadsProducers) {
        t.join();
    }
    cout << "producers: " << clock() - start1 << endl;

    for (auto& t : threadsConsumers) {
        t.join();
    }
    cout << "consumers: " << clock() - start2 << endl;
}

string checkSum(Consumer *consumers, int consumersNum)
{
    int res = 0;
    for (int i = 0; i < consumersNum; i++)
    {
        res += consumers[i].sum;
        //cout << "consumer " << i + 1 << " got " << consumers[i].sum << endl;
    }
    cout << "correct answer: " << taskNum << endl;
    cout << "my answer:      " << res << endl;
    return res == taskNum ? "correct" : "incorrect";
}

void solve(int consumersNum, int producersNum)
{
    Consumer* consumers = new Consumer[consumersNum];
    q = new ThreadSafeMutexSizeQueue(8);
    spawnThreads(consumersNum, producersNum, consumers);
    cout << checkSum(consumers, consumersNum) << endl;
}

int main()
{
    srand(time(0));

    cout << "4 consumers and 1 producers:\n";
    solve(4, 1);
    cout << endl;

    cout << "1 consumer and 1 producer:\n";
    solve(1, 1);
    cout << endl;
    
    cout << "1 consumers and 4 producers:\n";
    solve(1, 4);
    cout << endl;

    return 0;
}