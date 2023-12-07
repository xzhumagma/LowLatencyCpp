#pragma once

#include <functional>
#include <map>

#include "common/thread_utils.h"
#include "common/lf_queue.h"
#include "common/macros.h"
#include "common/mcast_socket.h"

#include "exchange/market_data/market_update.h"

namespace Trading {
  class MarketDataConsumer {
  public:
    /* Constructor
      * @param *market_updates: pointer to the queue, where it will publish decoded and in-order market data updates.
      * @param iface: network interface name.
      * @param snapshot_ip, snapshot_port, incremental_ip and incremental_port : IP address of the snapshot adn incremental market data streams.
    */
    MarketDataConsumer(Common::ClientId client_id, Exchange::MEMarketUpdateLFQueue *market_updates, const std::string &iface,
                       const std::string &snapshot_ip, int snapshot_port,
                       const std::string &incremental_ip, int incremental_port);

    ~MarketDataConsumer() {
      stop();

      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(5s);
    }

    auto start() {
      run_ = true;
      ASSERT(Common::createAndStartThread(-1, "Trading/MarketDataConsumer", [this]() { run(); }) != nullptr, "Failed to start MarketData thread.");
    }

    auto stop() -> void {
      run_ = false;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MarketDataConsumer() = delete;

    MarketDataConsumer(const MarketDataConsumer &) = delete;

    MarketDataConsumer(const MarketDataConsumer &&) = delete;

    MarketDataConsumer &operator=(const MarketDataConsumer &) = delete;

    MarketDataConsumer &operator=(const MarketDataConsumer &&) = delete;

  private:
    // make sure that we process updates from incremental market data stream in the correct order and detect packet dromps on the 
    // incremental market data stream.
    size_t next_exp_inc_seq_num_ = 1;
    // To publish the MEMarketUpdate messages to the trading engine component.
    Exchange::MEMarketUpdateLFQueue *incoming_md_updates_ = nullptr;
    // control the execution of the thread and it is marked volatile since it is accessed from different threads.
    volatile bool run_ = false;

    std::string time_str_;
    Logger logger_;
    // multicast subscriber sockets. We will use to subsribe to and consume multicast data from the incremental
    // and the snapshot multicast streams, respectively.
    Common::McastSocket incremental_mcast_socket_, snapshot_mcast_socket_;
    // signify if MarketDataConsumer dected a packet drop and is currently tring to recover using the snapshot and incremental market data 
    // streams.
    bool in_recovery_ = false;
    // network interface to use, the IP address
    const std::string iface_, snapshot_ip_;
    // the port of the snapshot multicast stream.
    const int snapshot_port_;
    // queue up messages and order them by their corresponding sequence number.3
    typedef std::map<size_t, Exchange::MEMarketUpdate> QueuedMarketUpdates;
    QueuedMarketUpdates snapshot_queued_msgs_, incremental_queued_msgs_;

  private:
    auto run() noexcept -> void;

    auto recvCallback(McastSocket *socket) noexcept -> void;

    auto queueMessage(bool is_snapshot, const Exchange::MDPMarketUpdate *request);

    auto startSnapshotSync() -> void;
    auto checkSnapshotSync() -> void;
  };
}
