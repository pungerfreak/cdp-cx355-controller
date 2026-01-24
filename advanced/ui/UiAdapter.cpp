#include "UiAdapter.h"

#include "UiApp.h"

#include <string.h>

namespace {
bool str_equal_ui_adapter(const char* a, const char* b)
{
    if (a == b) {
        return true;
    }
    if (a == nullptr || b == nullptr) {
        return false;
    }
    return strcmp(a, b) == 0;
}
}  // namespace

UiAdapter::UiAdapter(SLinkSystem& system, UiApp& app)
    : system_(system), app_(app)
{
}

void UiAdapter::start()
{
    system_.addUnitObserver(*this);
    system_.getUnitStateSnapshot(disc_, track_);
    refreshFromSnapshot_();
    app_.setActionCallback(&UiAdapter::onUiActionThunk_, this);
}

void UiAdapter::stop()
{
    system_.removeUnitObserver(*this);
}

void UiAdapter::requestStatus()
{
    system_.intentSource().getStatus();
}

void UiAdapter::onUnitEvent(const SLinkUnitEvent& e)
{
    switch (e.type) {
        case SLinkUnitEventType::DiscChanged:
        case SLinkUnitEventType::TrackChanged:
        case SLinkUnitEventType::CurrentDiscInfo:
        case SLinkUnitEventType::Status:
        case SLinkUnitEventType::TransportStateChanged:
            refreshFromSnapshot_();
            if (e.type == SLinkUnitEventType::Status && bootStatusPending_) {
                if (e.transport == SLinkTransportState::PowerOff) {
                    if (e.disc.present && e.disc.valid) {
                        system_.applyInitialState(e.disc.disc, 0);
                    }
                    system_.intentSource().powerOn();
                }
                bootStatusPending_ = false;
            }
            break;
        case SLinkUnitEventType::CurrentDiscBankSwitchNeeded:
        case SLinkUnitEventType::Unknown: {
            uint32_t now_ms = millis();
            if (now_ms - lastRefreshMs_ >= MIN_REFRESH_MS) {
                refreshFromSnapshot_();
            }
            break;
        }
    }
}

void UiAdapter::refreshFromSnapshot_()
{
    system_.getUnitStateSnapshot(disc_, track_);

    UiNowPlayingSnapshot next{};
    next.disc = (disc_.present && disc_.valid) ? disc_.disc : 0;
    next.track = (track_.present && track_.valid) ? track_.track : 0;
    next.elapsed_sec = 0;
    next.title = nullptr;
    next.artist = nullptr;
    next.album = nullptr;
    next.transport = UiTransportState::Unknown;

    switch (track_.transport) {
        case SLinkTransportState::Playing:
            next.transport = UiTransportState::Playing;
            break;
        case SLinkTransportState::Paused:
            next.transport = UiTransportState::Paused;
            break;
        case SLinkTransportState::Stopped:
            next.transport = UiTransportState::Stopped;
            break;
        case SLinkTransportState::PowerOff:
            next.transport = UiTransportState::Stopped;
            break;
        case SLinkTransportState::Unchanged:
        case SLinkTransportState::Unknown:
        default:
            next.transport = UiTransportState::Unknown;
            break;
    }

    bool changed = !hasUi_;
    if (!changed) {
        changed = next.disc != ui_.disc || next.track != ui_.track
                  || next.elapsed_sec != ui_.elapsed_sec || next.transport != ui_.transport
                  || !str_equal_ui_adapter(next.title, ui_.title) || !str_equal_ui_adapter(next.artist, ui_.artist)
                  || !str_equal_ui_adapter(next.album, ui_.album);
    }

    lastRefreshMs_ = millis();
    if (!changed) {
        return;
    }

    ui_ = next;
    hasUi_ = true;
    app_.render(ui_);
}

void UiAdapter::onUiActionThunk_(UiAction action, void* user)
{
    if (user == nullptr) {
        return;
    }
    static_cast<UiAdapter*>(user)->onUiAction_(action);
}

size_t UiAdapter::actionIndex_(UiAction action)
{
    switch (action) {
        case UiAction::PrevTrack:
            return 0;
        case UiAction::NextTrack:
            return 1;
        case UiAction::Play:
            return 2;
        case UiAction::Pause:
            return 3;
        case UiAction::Stop:
            return 4;
        case UiAction::Power:
            return 5;
        default:
            return 0;
    }
}

bool UiAdapter::shouldHandleAction_(UiAction action, uint32_t now_ms)
{
    if (!isDebouncedAction_(action)) {
        return true;
    }
    size_t index = actionIndex_(action);
    if (hasAction_[index] && now_ms - lastActionMs_[index] < kActionDebounceMs_) {
        return false;
    }
    lastActionMs_[index] = now_ms;
    hasAction_[index] = true;
    return true;
}

bool UiAdapter::isDebouncedAction_(UiAction action)
{
    switch (action) {
        case UiAction::PrevTrack:
        case UiAction::NextTrack:
        case UiAction::Play:
        case UiAction::Pause:
        case UiAction::Stop:
        case UiAction::Power:
            return true;
        default:
            return false;
    }
}

void UiAdapter::onUiAction_(UiAction action)
{
    uint32_t now_ms = millis();
    if (!shouldHandleAction_(action, now_ms)) {
        return;
    }

    SLinkCommandIntentSource& intents = system_.intentSource();
    switch (action) {
        case UiAction::Play: {
            bool ok = intents.play();
            Serial.println(ok ? "intent: play" : "intent: play (rejected)");
            break;
        }
        case UiAction::Pause: {
            bool ok = intents.pause();
            Serial.println(ok ? "intent: pause" : "intent: pause (rejected)");
            break;
        }
        case UiAction::Stop: {
            bool ok = intents.stop();
            Serial.println(ok ? "intent: stop" : "intent: stop (rejected)");
            break;
        }
        case UiAction::NextTrack:
        case UiAction::PrevTrack: {
            if (!hasUi_ || ui_.track == 0) {
                Serial.println(action == UiAction::NextTrack
                                   ? "intent: nextTrack ignored (track unknown)"
                                   : "intent: prevTrack ignored (track unknown)");
                break;
            }
            int delta = (action == UiAction::NextTrack) ? 1 : -1;
            int next = static_cast<int>(ui_.track) + delta;
            if (next < 1 || next > 255) {
                Serial.println(action == UiAction::NextTrack
                                   ? "intent: nextTrack ignored (out of range)"
                                   : "intent: prevTrack ignored (out of range)");
                break;
            }
            bool ok = intents.changeTrack(static_cast<uint8_t>(next));
            if (action == UiAction::NextTrack) {
                Serial.println(ok ? "intent: nextTrack -> changeTrack"
                                  : "intent: nextTrack rejected");
            } else {
                Serial.println(ok ? "intent: prevTrack -> changeTrack"
                                  : "intent: prevTrack rejected");
            }
            break;
        }
        case UiAction::Power: {
            if (!powerToggleKnown_) {
                powerIsOn_ = hasUi_
                             && (ui_.transport != UiTransportState::Unknown
                                 || ui_.disc != 0
                                 || ui_.track != 0);
                powerToggleKnown_ = true;
            }
            bool sendOff = powerIsOn_;
            bool ok = sendOff ? intents.powerOff() : intents.powerOn();
            if (sendOff) {
                Serial.println(ok ? "intent: powerOff" : "intent: powerOff (rejected)");
            } else {
                Serial.println(ok ? "intent: powerOn" : "intent: powerOn (rejected)");
            }
            if (ok) {
                powerIsOn_ = !powerIsOn_;
            }
            break;
        }
        case UiAction::OpenDiscKeypad:
            discEntryValue_ = 0;
            discEntryLen_ = 0;
            app_.setKeypadError(false);
            break;
        case UiAction::KeypadDigit0:
        case UiAction::KeypadDigit1:
        case UiAction::KeypadDigit2:
        case UiAction::KeypadDigit3:
        case UiAction::KeypadDigit4:
        case UiAction::KeypadDigit5:
        case UiAction::KeypadDigit6:
        case UiAction::KeypadDigit7:
        case UiAction::KeypadDigit8:
        case UiAction::KeypadDigit9: {
            if (discEntryLen_ < 3) {
                uint8_t digit = 0;
                switch (action) {
                    case UiAction::KeypadDigit1:
                        digit = 1;
                        break;
                    case UiAction::KeypadDigit2:
                        digit = 2;
                        break;
                    case UiAction::KeypadDigit3:
                        digit = 3;
                        break;
                    case UiAction::KeypadDigit4:
                        digit = 4;
                        break;
                    case UiAction::KeypadDigit5:
                        digit = 5;
                        break;
                    case UiAction::KeypadDigit6:
                        digit = 6;
                        break;
                    case UiAction::KeypadDigit7:
                        digit = 7;
                        break;
                    case UiAction::KeypadDigit8:
                        digit = 8;
                        break;
                    case UiAction::KeypadDigit9:
                        digit = 9;
                        break;
                    case UiAction::KeypadDigit0:
                    default:
                        digit = 0;
                        break;
                }
                discEntryValue_ = static_cast<uint16_t>(discEntryValue_ * 10 + digit);
                discEntryLen_++;
            }
            break;
        }
        case UiAction::KeypadBackspace:
            if (discEntryLen_ > 0) {
                discEntryValue_ = static_cast<uint16_t>(discEntryValue_ / 10);
                discEntryLen_--;
            }
            break;
        case UiAction::KeypadClear:
            discEntryValue_ = 0;
            discEntryLen_ = 0;
            break;
        case UiAction::KeypadGo: {
            if (discEntryLen_ == 0 || discEntryValue_ < 1 || discEntryValue_ > 300) {
                app_.setKeypadError(true);
                Serial.println("intent: changeDisc ignored (invalid entry)");
                break;
            }
            bool ok = intents.changeDisc(discEntryValue_);
            Serial.println(ok ? "intent: changeDisc" : "intent: changeDisc (rejected)");
            discEntryValue_ = 0;
            discEntryLen_ = 0;
            app_.setKeypadError(false);
            app_.showNowPlaying();
            break;
        }
        case UiAction::KeypadCancel:
            discEntryValue_ = 0;
            discEntryLen_ = 0;
            break;
        default:
            break;
    }
}
