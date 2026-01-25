#include "UiApp.h"

void UiApp::init()
{
    lv_obj_t* screen = lv_screen_active();
    main_.init(screen, UiApp::onScreenActionThunk_, this);
    disc_.init(screen, UiApp::onScreenActionThunk_, this);
    switchToMain_();
}

void UiApp::render(const UiNowPlayingSnapshot& s)
{
    main_.render(s);
}

void UiApp::setActionCallback(UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;
}

void UiApp::setKeypadError(bool on)
{
    disc_.setError(on);
}

void UiApp::showNowPlaying()
{
    switchToMain_();
}

void UiApp::onScreenActionThunk_(UiAction action, void* user)
{
    if (user == nullptr) {
        return;
    }
    UiApp* app = static_cast<UiApp*>(user);
    app->handleScreenAction_(action);
}

void UiApp::handleScreenAction_(UiAction action)
{
    switch (action) {
        case UiAction::OpenDiscKeypad:
            switchToDiscSelector_();
            break;
        case UiAction::KeypadCancel:
            switchToMain_();
            break;
        default:
            break;
    }

    if (cb_ != nullptr) {
        cb_(action, user_);
    }
}

void UiApp::switchToMain_()
{
    active_ = ActiveScreen::Main;
    disc_.reset();
    disc_.hide();
    main_.show();
}

void UiApp::switchToDiscSelector_()
{
    active_ = ActiveScreen::DiscSelector;
    main_.hide();
    disc_.show();
}
