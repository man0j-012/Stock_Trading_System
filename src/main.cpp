#include "OrderFactory.hpp"
#include "MatchingEngine.hpp"
#include <thread>
#include <random>
#include <iostream>
#include <vector>
#include <atomic>

int main() {
    MatchingEngine engine;
    engine.start(4);

    std::atomic<std::uint64_t> id{1};
    auto producer = [&](const std::string& sym) {
        std::mt19937_64 rng{std::random_device{}()};
        std::uniform_int_distribution<int> sideDist(0,1);
        std::uniform_int_distribution<int> qtyDist(1, 100);
        std::uniform_real_distribution<double> pxDist(95.0, 105.0);

        for (int i = 0; i < 2000; ++i) {
            OrderSide side = sideDist(rng) == 0 ? OrderSide::Buy : OrderSide::Sell;
            std::uint32_t qty = static_cast<std::uint32_t>(qtyDist(rng));
            double px = pxDist(rng);

            auto order = OrderFactory::makeLimit(sym, id.fetch_add(1), side, qty, px);
            engine.submit(std::move(order));
        }
    };

    std::vector<std::thread> producers;
    producers.emplace_back(producer, "AAPL");
    producers.emplace_back(producer, "AAPL");
    producers.emplace_back(producer, "GOOG");
    producers.emplace_back(producer, "MSFT");

    for (auto& t : producers) t.join();
    engine.stop();

    auto trades = engine.drainTrades();
    std::cout << "Total trades executed: " << trades.size() << "\n";
    std::size_t sumQty = 0;
    for (auto& tr : trades) sumQty += tr.qty;
    std::cout << "Total matched quantity: " << sumQty << "\n";
    return 0;
}
