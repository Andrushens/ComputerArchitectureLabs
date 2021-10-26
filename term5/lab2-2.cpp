#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <mutex>
#include <ctime>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

mutex mtx;
const int numTasks = 1024 * 1024;
vector<uint8_t> a(numTasks);
int mutexIdx;

void threadFuncAtomic(atomic<int>& sharedIdx, int threadId, bool doSleep)
{
    int idx;
    while (true)
    {
        idx = sharedIdx++;
        if (idx >= a.size()) break;
        a.at(idx)++;
        if (doSleep)
            this_thread::sleep_for(chrono::nanoseconds(10));
    }
}

void spawnThreadsAtomic(int numThreads, bool doSleep = false)
{
    vector<thread> threads(numThreads);
    atomic<int> sharedIdx(0);

    for (int i = 0; i < numThreads; i++)
    {
        threads[i] = thread(threadFuncAtomic, ref(sharedIdx), i + 1, doSleep);
    }
    for (auto& t : threads) {
        t.join();
    }
}

void threadFuncMutex(int threadId, bool doSleep = false)
{
    int idx;
    while (true)
    {
        mtx.lock();
        idx = mutexIdx++;
        mtx.unlock();
        if (idx >= a.size()) break;
        a.at(idx)++;
        if (doSleep)
            this_thread::sleep_for(chrono::nanoseconds(10));
    }
}

void spawnThreadsMutex(int numThreads, bool doSleep)
{
    vector<thread> threads(numThreads);

    for (int i = 0; i < numThreads; i++)
    {
        threads[i] = thread(threadFuncMutex, i + 1, doSleep);
    }
    for (auto& t : threads) {
        t.join();
    }
}

string checkVector()
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != 3)
        {
            cout << "wrong index: " << i << endl;
            return "incorrect";
        }
    }
    return "correct";
}

void solve(int numThreads, bool doSleep)
{
    mutexIdx = 0;
    a = vector<uint8_t>(numTasks);
    int start;

    start = clock();
    for (int i = 0; i < a.size(); i++)
    {
        a[i]++;
        if (doSleep)
            this_thread::sleep_for(chrono::nanoseconds(10));
    }
    cout << "sync:   " << clock() - start << endl;

    start = clock();
    spawnThreadsMutex(numThreads, doSleep);
    cout << "mutex:  " << clock() - start << endl;

    start = clock();
    spawnThreadsAtomic(numThreads, doSleep);
    cout << "atomic: " << clock() - start << endl;

    cout << checkVector() << endl << endl;
}

int main()
{
    cout << "4  threads no sleep:" << endl;
    solve(4, false);
    cout << "8  threads no sleep:" << endl;
    solve(8, false);
    cout << "16 threads no sleep:" << endl;
    solve(16, false);
    cout << "32 threads no sleep:" << endl;
    solve(32, false);

    cout << "4  threads with sleep:" << endl;
    solve(4, true);
    cout << "8  threads with sleep:" << endl;
    solve(8, true);
    cout << "16 threads with sleep:" << endl;
    solve(16, true);
    cout << "32 threads with sleep:" << endl;
    solve(32, true);

    return 0;
}