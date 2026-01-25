#pragma once

#include "DiscSelectorScreen.h"
#include "MainControllerScreen.h"
#include "UiTypes.h"

class UiApp {
  public:
    UiApp() = default;

    // Build all LVGL objects. Call once after LVGL init + display init.
    void init();

    // Render/update widgets from the snapshot. Must be safe to call repeatedly.
    void render(const UiNowPlayingSnapshot& s);

    void setActionCallback(UiActionCb cb, void* user);
    void setKeypadError(bool on);
    void showNowPlaying();

  private:
    enum class ActiveScreen : uint8_t {
        Main = 0,
        DiscSelector
    };

    static void onScreenActionThunk_(UiAction action, void* user);
    void handleScreenAction_(UiAction action);
    void switchToMain_();
    void switchToDiscSelector_();

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;
    ActiveScreen active_ = ActiveScreen::Main;

    MainControllerScreen main_;
    DiscSelectorScreen disc_;
};
