#pragma once

#include "UiApp.h"
#include "../system/SLinkSystem.h"

class UiAdapter final : public SLinkUnitEventObserver {
public:
    UiAdapter(SLinkSystem& system, UiApp& app);

    // Call after LVGL + display init and after SLinkSystem is constructed/started (if applicable)
    void start();

    // Optional but recommended if removeUnitObserver exists
    void stop();

    // SLinkUnitEventObserver
    void onUnitEvent(const SLinkUnitEvent& e) override;

private:
    void refreshFromSnapshot_();   // pulls snapshot, converts, calls app.render()
    static void onUiActionThunk_(UiAction action, void* user);
    void onUiAction_(UiAction action);
    static size_t actionIndex_(UiAction action);
    bool shouldHandleAction_(UiAction action, uint32_t now_ms);

    SLinkSystem& system_;
    UiApp&       app_;

    // Cached SLink snapshot types
    SLinkDiscInfo disc_{};
    SLinkTrackInfo track_{};

    // Cached UI snapshot
    UiNowPlayingSnapshot ui_{};
    bool hasUi_ = false;

    // Throttle refresh to avoid spamming LVGL if events burst
    uint32_t lastRefreshMs_ = 0;
    static constexpr uint32_t MIN_REFRESH_MS = 50;  // keep modest

    static constexpr size_t kActionCount_ = 6;
    static constexpr uint32_t kActionDebounceMs_ = 200;
    uint32_t lastActionMs_[kActionCount_] = {};
    bool hasAction_[kActionCount_] = {};

    bool powerToggleKnown_ = false;
    bool powerIsOn_ = false;
};
