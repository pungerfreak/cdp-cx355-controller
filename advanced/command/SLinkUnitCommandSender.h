#pragma once
#include <Arduino.h>
#include "command/SLinkCommandIntentSource.h"
#include "transport/SLinkTx.h"
#include "frame/SLinkFrameBuilder.h"
#include "command/SLinkUnitCommand.h"

class SLinkUnitCommandSender : public SLinkCommandIntentSource {
public:
  using TxCallback = void (*)(const uint8_t* data, uint16_t len, void* context);

  explicit SLinkUnitCommandSender(SLinkTx& tx,
                                  uint8_t unit = SLINK_ADDR_CONTROLLER);

  bool canSendNow(uint32_t nowUs) const;
  uint32_t nextSendUs(uint32_t nowUs) const;
  void setCurrentDisc(uint16_t disc);
  void setTxCallback(TxCallback cb, void* context);
  bool send(const SLinkUnitCommand& cmd);
  bool play() override;
  bool stop() override;
  bool pause() override;
  bool powerOn() override;
  bool powerOff() override;
  bool changeDisc(uint16_t disc) override;
  bool changeTrack(uint8_t track) override;
  bool getCurrentDisc() override;

  void requestCurrentDiscBankB();
  void completeCurrentDiscRequest();

private:
  bool sendSimple(SLinkUnitCommandType type);
  bool resolveChange(SLinkUnitCommand& cmd) const;
  bool sendGetCurrentDisc(uint8_t unit);

  enum class CurrentDiscStage : uint8_t {
    Idle = 0,
    BankARequested,
    BankBRequested
  };

  void setStage(CurrentDiscStage stage);
  CurrentDiscStage currentDiscStage() const;

  CurrentDiscStage _currentDiscStage = CurrentDiscStage::Idle;
  SLinkTx& _tx;
  SLinkFrameBuilder _frameBuilder;
  uint8_t _unit;
  uint16_t _currentDisc = 0;
  bool _hasDisc = false;
  TxCallback _txCallback = nullptr;
  void* _txCallbackCtx = nullptr;
};
