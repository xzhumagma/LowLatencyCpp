#pragma once

#include <sstream>

#include "common/types.h"
#include "common/lf_queue.h"

//using namespace Common;

namespace Exchange {
#pragma pack(push, 1)
  enum class MarketUpdateType : uint8_t {
    INVALID = 0,
    ADD = 1,
    MODIFY = 2,
    CANCEL = 3,
    TRADE = 4
  };

  inline std::string marketUpdateTypeToString(MarketUpdateType type) {
    switch (type) {
      case MarketUpdateType::ADD:
        return "ADD";
      case MarketUpdateType::MODIFY:
        return "MODIFY";
      case MarketUpdateType::CANCEL:
        return "CANCEL";
      case MarketUpdateType::TRADE:
        return "TRADE";
      case MarketUpdateType::INVALID:
        return "INVALID";
    }
    return "UNKNOWN";
  }

  struct MEMarketUpdate {
    MarketUpdateType type_ = MarketUpdateType::INVALID;

    Common::OrderId order_id_ = OrderId_INVALID;
    Common::TickerId ticker_id_ = TickerId_INVALID;
    Common::Side side_ = Common::Side::INVALID;
    Common::Price price_ = Price_INVALID;
    Common::Qty qty_ = Qty_INVALID;
    Common::Priority priority_ = Priority_INVALID;

    auto toString() const {
      std::stringstream ss;
      ss << "MEMarketUpdate"
         << " ["
         << " type:" << marketUpdateTypeToString(type_)
         << " ticker:" << Common::tickerIdToString(ticker_id_)
         << " oid:" << Common::orderIdToString(order_id_)
         << " side:" << sideToString(side_)
         << " qty:" << Common::qtyToString(qty_)
         << " price:" << Common::priceToString(price_)
         << " priority:" << Common::priorityToString(priority_)
         << "]";
      return ss.str();
    }
  };

#pragma pack(pop)

typedef Common::LFQueue<Exchange::MEMarketUpdate> MEMarketUpdateLFQueue;
}