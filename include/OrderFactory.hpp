#pragma once
#include "Order.hpp"
#include <memory>
#include <string>

class OrderFactory {
public:
    static std::unique_ptr<Order> makeMarket(const std::string& symbol, std::uint64_t id,
                                             OrderSide side, std::uint32_t qty);
    static std::unique_ptr<Order> makeLimit(const std::string& symbol, std::uint64_t id,
                                            OrderSide side, std::uint32_t qty, double price);
};
