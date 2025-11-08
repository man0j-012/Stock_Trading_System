#include "MatchingEngine.hpp"
#include <thread>
#include <cassert>

MatchingEngine::MatchingEngine() = default;
MatchingEngine::~MatchingEngine() { stop(); }

void MatchingEngine::start(std::size_t workers) {
    if (running_) return;
    running_ = true;
    for (std::size_t i = 0; i < workers; ++i) {
        workers_.emplace_back(&MatchingEngine::processLoop, this);
    }
}

void MatchingEngine::stop() {
    if (!running_) return;
    running_ = false;
    inbox_.stop();
    for (auto& t : workers_) if (t.joinable()) t.join();
    workers_.clear();
}

void MatchingEngine::submit(std::unique_ptr<Order> order) {
    inbox_.push(std::move(order));
}

std::vector<Trade> MatchingEngine::drainTrades() {
    std::scoped_lock lk(tradesMutex_);
    auto out = std::move(trades_);
    trades_.clear();
    return out;
}

void MatchingEngine::processLoop() {
    while (true) {
        auto item = inbox_.pop();
        if (!item.has_value()) break;
        auto order = std::move(*item);
        const std::string sym = order->symbol();

        {
            std::scoped_lock lk(booksMutex_);
            auto& book = books_[sym];
            std::scoped_lock blk(book.m);
            if (order->side() == OrderSide::Buy) {
                book.bids.push(order.release()); // transfer raw pointer into book container
            } else {
                book.asks.push(order.release());
            }
        }
        match(sym);
    }
}

void MatchingEngine::match(const std::string& symbol) {
    OrderBook* bookPtr = nullptr;
    {
        std::scoped_lock lk(booksMutex_);
        auto it = books_.find(symbol);
        if (it == books_.end()) return;
        bookPtr = &it->second;
    }

    auto& book = *bookPtr;
    std::scoped_lock blk(book.m);

    while (!book.bids.empty() && !book.asks.empty()) {
        Order* buy = book.bids.top();
        Order* sell = book.asks.top();

        double execPrice = 0.0;
        if (buy->isMarket() && sell->isMarket()) {
            break;
        } else if (buy->isMarket()) {
            execPrice = sell->price();
        } else if (sell->isMarket()) {
            execPrice = buy->price();
        } else {
            if (buy->price() < sell->price()) break;
            execPrice = sell->timestamp() <= buy->timestamp() ? sell->price() : buy->price();
        }

        std::uint32_t qty = std::min(buy->qty(), sell->qty());
        buy->reduceQty(qty);
        sell->reduceQty(qty);

        {
            std::scoped_lock tl(tradesMutex_);
            trades_.push_back(Trade{symbol, buy->id(), sell->id(), qty, execPrice});
        }

        if (buy->qty() == 0) {
            book.bids.pop();
            delete buy;
        }
        if (sell->qty() == 0) {
            book.asks.pop();
            delete sell;
        }
    }
}
