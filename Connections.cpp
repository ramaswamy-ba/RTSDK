OmmConsumerConfig config;
config.operationModel(OmmConsumerConfig::OperationModel::API_DISPATCH); // USER_DISPATCH

OmmConsumer consumer1(config.host("server1:14002").username("user1"));
OmmConsumer consumer2(config.host("server2:14002").username("user2"));

while (true) {
    consumer1.dispatch(1000);  // poll events for consumer1
    consumer2.dispatch(1000);  // poll events for consumer2
    // You can manage both from this single thread

  // OmmConsumerConfig::msgQueueSize or XML config - < Q Size
}


#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

struct MarketUpdate {
    std::string symbol;
    FieldList fields;
};

class AppClient : public OmmConsumerClient {
public:
    void decode(const FieldList& fl) {
        MarketUpdate update;
        update.symbol = fl.getName();   // example
        update.fields = fl;             // shallow copy

        {
            std::lock_guard<std::mutex> lock(mtx);
            workQueue.push(update);
        }
        cv.notify_one();  // wake worker thread
    }

private:
    std::queue<MarketUpdate> workQueue;
    std::mutex mtx;
    std::condition_variable cv;
};
-------------------
void worker(AppClient& client) {
    while (true) {
        std::unique_lock<std::mutex> lock(client.mtx);
        client.cv.wait(lock, [&]{ return !client.workQueue.empty(); });

        MarketUpdate update = client.workQueue.front();
        client.workQueue.pop();
        lock.unlock();

        // Heavy work here (safe, outside EMA dispatch)
        processUpdate(update);
    }
}
---------------------------------------------
V2
---------------------------------------------
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>

struct MarketUpdate {
    std::string symbol;
    FieldList fields;
};

class UpdateQueue {
public:
    void push(const MarketUpdate& update) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            workQueue.push(update);
        }
        cv.notify_one();
    }

    bool pop(MarketUpdate& update) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return !workQueue.empty() || stop; });
        if (workQueue.empty()) return false;
        update = workQueue.front();
        workQueue.pop();
        return true;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
    }

private:
    std::queue<MarketUpdate> workQueue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
};

  ------------------
    class AppClient : public OmmConsumerClient {
public:
    AppClient(UpdateQueue& q) : queue(q) {}

    void decode(const FieldList& fl) {
        MarketUpdate update;
        update.symbol = fl.getName();
        update.fields = fl;
        queue.push(update);  // enqueue for workers
    }

private:
    UpdateQueue& queue;
};

---------
  void worker(UpdateQueue& queue, int id) {
    MarketUpdate update;
    while (queue.pop(update)) {
        // Heavy work here
        processUpdate(update, id);
    }
}

UpdateQueue queue;
AppClient client(queue);

// Create 4 worker threads
std::vector<std::thread> workers;
for (int i = 0; i < 4; ++i) {
    workers.emplace_back(worker, std::ref(queue), i);
}

// EMA dispatch loop
while (true) {
    consumer.dispatch(0);  // non-blocking
    // other event loop work...
}

// Shutdown gracefully
queue.shutdown();
for (auto& w : workers) w.join();

