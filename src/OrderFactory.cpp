#include "OrderFactory.hpp"

std::unique_ptr<Order> OrderFactory::makeMarket(const std::string& symbol, std::uint64_t id,
                                                OrderSide side, std::uint32_t qty) {
    return std::make_unique<MarketOrder>(symbol, id, side, qty);
}

std::unique_ptr<Order> OrderFactory::makeLimit(const std::string& symbol, std::uint64_t id,
                                               OrderSide side, std::uint32_t qty, double price) {
    return std::make_unique<LimitOrder>(symbol, id, side, qty, price);
}
