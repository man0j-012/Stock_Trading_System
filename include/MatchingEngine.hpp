#pragma once
#include "Order.hpp"
#include "ThreadSafeQueue.hpp"
#include <map>
#include <queue>
#include <mutex>
#include <vector>
#include <optional>
#include <iostream>

struct Trade {
    std::string symbol;
    std::uint64_t buyId;
    std::uint64_t sellId;
    std::uint32_t qty;
    double price;
};

struct BidCmp {
    bool operator()(const Order* a, const Order* b) const {
        if (a->price() == b->price()) return a->timestamp() > b->timestamp();
        return a->price() < b->price();
    }
};

struct AskCmp {
    bool operator()(const Order* a, const Order* b) const {
        if (a->price() == b->price()) return a->timestamp() > b->timestamp();
        return a->price() > b->price();
    }
};

class OrderBook {
public:
    void addBuy(Order* o) { bids.push(o); }
    void addSell(Order* o) { asks.push(o); }

    std::priority_queue<Order*, std::vector<Order*>, BidCmp> bids;
    std::priority_queue<Order*, std::vector<Order*>, AskCmp> asks;
    std::mutex m;
};

class MatchingEngine {
public:
    MatchingEngine();
    ~MatchingEngine();

    void submit(std::unique_ptr<Order> order);
    void start(std::size_t workers = std::thread::hardware_concurrency());
    void stop();

    std::vector<Trade> drainTrades();

private:
    void processLoop();
    void match(const std::string& symbol);

    ThreadSafeQueue<std::unique_ptr<Order>> inbox_;
    std::map<std::string, OrderBook> books_;
    std::mutex booksMutex_;
    std::vector<std::thread> workers_;
    std::mutex tradesMutex_;
    std::vector<Trade> trades_;
    bool running_{false};
};
