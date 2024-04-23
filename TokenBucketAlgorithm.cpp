#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

class TokenBucket
{
private:
    int capacity;
    int tokensPerSec;
    int tokensAvail;
    mutex mtx;

public:
    TokenBucket(int capacity, int tokensPerSec) : capacity{capacity}, tokensPerSec{tokensPerSec}, tokensAvail{tokensPerSec}
    {
        cout << "Token Bucket : capacity : " << capacity << " tokens per sec : " << tokensPerSec << endl;
    }

    bool canReqBeProccessed()
    {
        lock_guard<mutex> lock(mtx);

        if (tokensAvail > 0)
        {
            tokensAvail--;
            cout << "Tokens remaining are : " << tokensAvail << endl;
            return true;
        }
        else
        {
            return false;
        }
    }

    void refillTokens()
    {
        lock_guard<mutex> lock(mtx);

        tokensAvail = std::min(capacity, tokensAvail + tokensPerSec);

        cout << "tokensAvail updated as : " << tokensAvail << endl;

        // If required queue can be maintained for requests crossed the throttle limit
        // and handled here once tokens are refilled
    }
};

int main()
{
    TokenBucket bucket(10, 8);

    thread th([&]()
              {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            bucket.refillTokens();
        } });

    int count(100);

    while (count)
    {
        if (count % 20 == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            cout << "waited.." << endl;
        }
        if (bucket.canReqBeProccessed())
        {
            cout << "This request can be accepted" << endl;
        }
        else
        {
            cout << "Tokens are not available, cannot accept the request" << endl;
        }

        count--;
    }

    th.detach();

    return 0;
}