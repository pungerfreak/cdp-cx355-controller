# Milestone F - Disc Selection Keypad

## UI flow
- Now Playing adds a "Disc" button.
- Tap "Disc" to open the keypad screen.
- Enter 1-3 digits and press "Go" to request a disc change.
- "Cancel" returns to Now Playing without sending an intent.
- Keypad state (digits + error) is UI-local; playback state remains snapshot-driven.

## UiAction additions
- OpenDiscKeypad
- KeypadDigit0 .. KeypadDigit9
- KeypadBackspace
- KeypadClear
- KeypadGo
- KeypadCancel

## Validation rules
- Up to 3 digits.
- Leading zeros are allowed while typing; normalization happens on Go.
- Valid range: 1..300.
- Invalid Go shows inline error text "Enter 1-300" and stays on the keypad.

## Notes
- Scrubbing is intentionally skipped in this milestone.
