#pragma once

#include "common/thread_utils.h"
#include "common/lf_queue.h"
#include "common/macros.h"

#include "order_server/client_request.h"
#include "order_server/client_response.h"
#include "market_data/market_update.h"

#include "me_order_book.h"

namespace Exchange {
  class MatchingEngine final {
  public:
    MatchingEngine(ClientRequestLFQueue *client_requests,
                   ClientResponseLFQueue *client_responses,
                   MEMarketUpdateLFQueue *market_updates);

    ~MatchingEngine();

    auto start() -> void;

    auto stop() -> void;

    auto processClientRequest(const MEClientRequest *client_request) noexcept {
      auto order_book = ticker_order_book_[client_request->ticker_id_];
      switch (client_request->type_) {
        case ClientRequestType::NEW: {
          order_book->add(client_request->client_id_, client_request->order_id_, client_request->ticker_id_,
                           client_request->side_, client_request->price_, client_request->qty_);
        }
          break;

        case ClientRequestType::CANCEL: {
          order_book->cancel(client_request->client_id_, client_request->order_id_, client_request->ticker_id_);
        }
          break;

        default: {
          FATAL("Received invalid client-request-type:" + clientRequestTypeToString(client_request->type_));
        }
          break;
      }
    }

    auto sendClientResponse(const MEClientResponse *client_response) noexcept {
      logger_.log("%:% %() % Sending %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), client_response->toString());
      auto next_write = outgoing_ogw_responses_->getNextToWriteTo();
      *next_write = std::move(*client_response);
      outgoing_ogw_responses_->updateWriteIndex();
    }

    auto sendMarketUpdate(const MEMarketUpdate *market_update) noexcept {
      logger_.log("%:% %() % Sending %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), market_update->toString());
      auto next_write = outgoing_md_updates_->getNextToWriteTo();
      *next_write = *market_update;
      outgoing_md_updates_->updateWriteIndex();
    }

    auto run() noexcept {
      // __FILE__ is a macro that expands to the name of the current file as a string literal.
      // __LINE__ is a macro that expands to the current line number in the source file as a decimal constant.
      // __FUNCTION__ is a macro that expands to the name of the current function as a string literal.
      // this function is to log the file name, line number, function name and current time.
      logger_.log("%:% %() %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_));
      // If the MatchingEngine is not running, the thread will exit.
      while (run_) {
        // the next to read in the queue of incoming_requests.
        const auto me_client_request = incoming_requests_->getNextToRead();
        // If me_client_request is not a nullptr, then process the client request.
        if (LIKELY(me_client_request)) {
          // log the client requesnt to the file.
          logger_.log("%:% %() % Processing %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_),
                      me_client_request->toString());
          // process the client request.
          processClientRequest(me_client_request);
          incoming_requests_->updateReadIndex();
        }
      }
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MatchingEngine() = delete;

    MatchingEngine(const MatchingEngine &) = delete;

    MatchingEngine(MatchingEngine &&) = delete;

    MatchingEngine &operator=(const MatchingEngine &) = delete;

    MatchingEngine &operator=(MatchingEngine &&) = delete;

  private:
    OrderBookHashMap ticker_order_book_;

    ClientRequestLFQueue *incoming_requests_ = nullptr;
    ClientResponseLFQueue *outgoing_ogw_responses_ = nullptr;
    MEMarketUpdateLFQueue *outgoing_md_updates_ = nullptr;
    // The primary purpose of marking a variable as 'volatile' is to prevent the compiler from applying any optimizations on it that based 
    // on the assumption that the variable can not by changed by external factors.
    volatile bool run_ = false;

    std::string time_str_;
    Logger logger_;
  };
}
