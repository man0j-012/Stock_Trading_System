#include "Order.hpp"

Order::Order(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty,
             double price, bool isMarket)
    : symbol_(std::move(symbol)), id_(id), side_(side), qty_(qty), price_(price),
      isMarket_(isMarket), ts_(std::chrono::steady_clock::now()) {}

MarketOrder::MarketOrder(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty)
    : Order(std::move(symbol), id, side, qty, 0.0, true) {}

LimitOrder::LimitOrder(std::string symbol, std::uint64_t id, OrderSide side, std::uint32_t qty, double price)
    : Order(std::move(symbol), id, side, qty, price, false) {}
