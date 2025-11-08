#pragma once
#include <string>
#include <chrono>
#include <cstdint>

enum class OrderSide { Buy, Sell };

class Order {
public:
    Order(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty,
          double price, bool isMarket);
    virtual ~Order() = default;

    const std::string& symbol() const noexcept { return symbol_; }
    std::uint64_t id() const noexcept { return id_; }
    OrderSide side() const noexcept { return side_; }
    std::uint32_t qty() const noexcept { return qty_; }
    double price() const noexcept { return price_; }
    bool isMarket() const noexcept { return isMarket_; }
    std::chrono::steady_clock::time_point timestamp() const noexcept { return ts_; }
    void reduceQty(std::uint32_t by) { qty_ -= by; }

private:
    std::string symbol_;
    std::uint64_t id_;
    OrderSide side_;
    std::uint32_t qty_;
    double price_;
    bool isMarket_;
    std::chrono::steady_clock::time_point ts_;
};

class MarketOrder : public Order {
public:
    MarketOrder(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty);
};

class LimitOrder : public Order {
public:
    LimitOrder(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty, double price);
};
