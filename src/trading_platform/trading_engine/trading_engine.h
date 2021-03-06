// Copyright [2020] <Copyright Kevin, kevin.lau.gd@gmail.com>

#ifndef FT_SRC_TRADING_SYSTEM_TRADING_ENGINE_H_
#define FT_SRC_TRADING_SYSTEM_TRADING_ENGINE_H_

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "common/md_snapshot.h"
#include "common/order.h"
#include "common/portfolio.h"
#include "common/types.h"
#include "core/account.h"
#include "core/config.h"
#include "core/error_code.h"
#include "interface/gateway.h"
#include "interface/trading_engine_interface.h"
#include "ipc/redis_md_helper.h"
#include "risk_management/risk_manager.h"

namespace ft {

class TradingEngine : public TradingEngineInterface {
 public:
  TradingEngine();

  ~TradingEngine();

  bool login(const Config& config);

  void process_cmd();

  void close();

  static uint64_t version() { return 202006222311; }

 private:
  void process_cmd_from_redis();

  void process_cmd_from_queue();

  void execute_cmd(const TraderCommand& cmd);

  bool send_order(const TraderCommand& cmd);

  void cancel_order(uint64_t order_id);

  void cancel_for_ticker(uint32_t ticker_index);

  void cancel_all();

  void on_query_contract(Contract* contract) override;

  void on_query_account(Account* account) override;

  void on_query_position(Position* position) override;

  void on_query_trade(OrderTradedRsp* trade) override;

  void on_tick(TickData* tick) override;

  void on_order_accepted(OrderAcceptedRsp* rsp) override;

  void on_order_rejected(OrderRejectedRsp* rsp) override;

  void on_order_traded(OrderTradedRsp* rsp) override;

  void on_order_canceled(OrderCanceledRsp* rsp) override;

  void on_order_cancel_rejected(OrderCancelRejectedRsp* rsp) override;

 private:
  void on_primary_market_traded(OrderTradedRsp* rsp);  // ETF申赎

  void on_secondary_market_traded(OrderTradedRsp* rsp);  // 二级市场买卖

  uint64_t next_engine_order_id() { return next_engine_order_id_++; }

 private:
  std::unique_ptr<Gateway> gateway_{nullptr};

  volatile bool is_logon_{false};
  uint64_t next_engine_order_id_{1};

  Account account_;
  Portfolio portfolio_;
  OrderMap order_map_;
  std::unique_ptr<RiskManager> risk_mgr_{nullptr};
  RedisMdPusher md_pusher_;
  MdSnapshot md_snapshot_;
  std::mutex mutex_;

  int cmd_queue_key_ = 0;
};

}  // namespace ft

#endif  // FT_SRC_TRADING_SYSTEM_TRADING_ENGINE_H_
