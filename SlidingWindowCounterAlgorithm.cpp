#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

class SlidingWindowCounter
{
private:
    int prevCount;
    int currCount;
    chrono::steady_clock::time_point windowStart;
    int windowSize;
    int maxReqPerWindow;
    void (*drop_callback)(int);
    void (*allow_callback)(int);

public:
    SlidingWindowCounter(int maxReqPerWindow, int windowSize, void (*drop_callback)(int), void (*allow_callback)(int))
    {
        this->maxReqPerWindow = maxReqPerWindow;
        this->windowSize = windowSize;
        this->drop_callback = drop_callback;
        this->allow_callback = allow_callback;
        this->windowStart = chrono::steady_clock::now();
        this->prevCount = 0;
        this->currCount = 0;
    }

    void handleRequest(int req)
    {
        chrono::steady_clock::time_point currTime = chrono::steady_clock::now();

        if (currTime - windowStart >= chrono::seconds(windowSize))
        {
            windowStart = currTime;
            prevCount = currCount;
            currCount = 0;
        }

        int currTimeInSec = chrono::duration_cast<chrono::seconds>(currTime.time_since_epoch()).count();
        int windowStartInSec = chrono::duration_cast<chrono::seconds>(windowStart.time_since_epoch()).count();

        int elapsedCount = prevCount * ((windowSize - (currTimeInSec - windowStartInSec)) / windowSize) + currCount;

        cout << "currTimeInSec: " << currTimeInSec << " windowStartInSec: " << windowStartInSec << " prevCount: " << prevCount << " currCount: " << currCount << " elapsedCount: " << elapsedCount << endl;

        if (elapsedCount < maxReqPerWindow)
        {
            currCount++;
            allow_callback(req);
        }
        else
        {
            drop_callback(req);
        }
    }
};

void allowPacketCb(int req)
{
    cout << "Allow this packet : " << req << endl;
}

void dropPacketCb(int req)
{
    cout << "Drop this packet : " << req << endl;
}

int main()
{
    SlidingWindowCounter slidingWindowCounter(100, 5, dropPacketCb, allowPacketCb);

    int count = 0;

    while (count < 400)
    {
        if ((count != 0) && (count % 200 == 0))
        {
            cout << "Sleep for 5sec" << endl;
            std::this_thread::sleep_for(chrono::seconds(5));
        }
        slidingWindowCounter.handleRequest(count);
        count++;
    }

    return 0;
}