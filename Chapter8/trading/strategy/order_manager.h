#pragma once

#include "common/macros.h"
#include "common/logging.h"

#include "exchange/order_server/client_response.h"

#include "om_order.h"
#include "risk_manager.h"

using namespace Common;

namespace Trading {
  class TradeEngine;

  class OrderManager {
  public:
    OrderManager(Common::Logger *logger, TradeEngine *trade_engine, RiskManager& risk_manager)
        : trade_engine_(trade_engine), risk_manager_(risk_manager), logger_(logger) {
    }

    auto onOrderUpdate(const Exchange::MEClientResponse *client_response) noexcept -> void {
      logger_->log("%:% %() % %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_),
                   client_response->toString().c_str());
      auto order = &(ticker_side_order_.at(client_response->ticker_id_).at(sideToIndex(client_response->side_)));
      logger_->log("%:% %() % %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_),
                   order->toString().c_str());

      switch (client_response->type_) {
        case Exchange::ClientResponseType::ACCEPTED: {
          // mark it as accepted and active in the market.
          order->order_state_ = OMOrderState::LIVE;
        }
          break;
        case Exchange::ClientResponseType::CANCELED: {
          // the order is no longer active in th market.
          order->order_state_ = OMOrderState::DEAD;
        }
          break;
        case Exchange::ClientResponseType::FILLED: {
          // update the qty_field on OMOrder to be the new leaves_qty_.
          // This reflects the live quantity that still exists in the market.
          order->qty_ = client_response->leaves_qty_;
          if(!order->qty_)
            order->order_state_ = OMOrderState::DEAD;
        }
          break;
        case Exchange::ClientResponseType::CANCEL_REJECTED:
        case Exchange::ClientResponseType::INVALID: {
        }
          break;
      }
    }

    auto newOrder(OMOrder *order, TickerId ticker_id, Price price, Side side, Qty qty) noexcept -> void;

    auto cancelOrder(OMOrder *order) noexcept -> void;
    // manages a single order and either sends a new order or cancels an existing order.
    auto moveOrder(OMOrder *order, TickerId ticker_id, Price price, Side side, Qty qty) noexcept {
      switch (order->order_state_) {
        case OMOrderState::LIVE: {
          if(order->price_ != price)
            cancelOrder(order);
        }
          break;
        case OMOrderState::INVALID:
        case OMOrderState::DEAD: {
          if(LIKELY(price != Price_INVALID)) {
            const auto risk_result = risk_manager_.checkPreTradeRisk(ticker_id, side, qty);
            if(LIKELY(risk_result == RiskCheckResult::ALLOWED))
              newOrder(order, ticker_id, price, side, qty);
            else
              // log the risk check result if the order is not allowed.
              logger_->log("%:% %() % Ticker:% Side:% Qty:% RiskCheckResult:%\n", __FILE__, __LINE__, __FUNCTION__,
                           Common::getCurrentTimeStr(&time_str_),
                           tickerIdToString(ticker_id), sideToString(side), qtyToString(qty),
                           riskCheckResultToString(risk_result));
          }
        }
          break;
        case OMOrderState::PENDING_NEW:
        case OMOrderState::PENDING_CANCEL:
          break;
      }
    }

    auto moveOrders(TickerId ticker_id, Price bid_price, Price ask_price, Qty clip) noexcept {
      // we can do multiple clip values for buy or sell orders.  
      auto bid_order = &(ticker_side_order_.at(ticker_id).at(sideToIndex(Side::BUY)));
      moveOrder(bid_order, ticker_id, bid_price, Side::BUY, clip);

      auto ask_order = &(ticker_side_order_.at(ticker_id).at(sideToIndex(Side::SELL)));
      moveOrder(ask_order, ticker_id, ask_price, Side::SELL, clip);
    }

    auto getOMOrderSideHashMap(TickerId ticker_id) const {
      return &(ticker_side_order_.at(ticker_id));
    }

    // Deleted default, copy & move constructors and assignment-operators.
    OrderManager() = delete;

    OrderManager(const OrderManager &) = delete;

    OrderManager(OrderManager &&) = delete;

    OrderManager &operator=(const OrderManager &) = delete;

    OrderManager &operator=(OrderManager &&) = delete;

  private:
    // We will use this to store the parent TradeEngine instance that is using this order manager.
    TradeEngine *trade_engine_ = nullptr;
    // This will be used to perform pre-trade risk checks.
    const RiskManager& risk_manager_;

    std::string time_str_;
    Common::Logger *logger_ = nullptr;
    // hold a pair (a buy or sell) of OMOrder objects for each trading instrument.
    // This will be used as a hash map that's indexed first by the TickerId value of the instrument we want to send an order for and then indexed by the 
    // buy or sell side values to manange the buy or sell order.
    OMOrderTickerSideHashMap ticker_side_order_;
    OrderId next_order_id_ = 1;
  };
}
