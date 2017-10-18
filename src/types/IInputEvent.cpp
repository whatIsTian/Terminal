/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/

#include "precomp.h"
#include "inc/IInputEvent.hpp"

#include <unordered_map>

IInputEvent::~IInputEvent()
{
}

std::unique_ptr<IInputEvent> IInputEvent::Create(_In_ const INPUT_RECORD& record)
{
    switch (record.EventType)
    {
    case KEY_EVENT:
        return std::make_unique<KeyEvent>(record.Event.KeyEvent);
    case MOUSE_EVENT:
        return std::make_unique<MouseEvent>(record.Event.MouseEvent);
    case WINDOW_BUFFER_SIZE_EVENT:
        return std::make_unique<WindowBufferSizeEvent>(record.Event.WindowBufferSizeEvent);
    case MENU_EVENT:
        return std::make_unique<MenuEvent>(record.Event.MenuEvent);
    case FOCUS_EVENT:
        return std::make_unique<FocusEvent>(record.Event.FocusEvent);
    default:
        THROW_HR(E_INVALIDARG);
    }
}

std::deque<std::unique_ptr<IInputEvent>> IInputEvent::Create(_In_reads_(cRecords) const INPUT_RECORD* const pRecords,
                                                             _In_ const size_t cRecords)
{
    std::deque<std::unique_ptr<IInputEvent>> outEvents;
    for (size_t i = 0; i < cRecords; ++i)
    {
        outEvents.push_back(Create(pRecords[i]));
    }
    return outEvents;
}


// Routine Description:
// - Converts std::deque<INPUT_RECORD> to std::deque<std::unique_ptr<IInputEvent>>
// Arguments:
// - inRecords - records to convert
// Return Value:
// - std::deque of IInputEvents on success. Will throw exception on failure.
std::deque<std::unique_ptr<IInputEvent>> IInputEvent::Create(_In_ const std::deque<INPUT_RECORD>& records)
{
    std::deque<std::unique_ptr<IInputEvent>> outEvents;
    for (size_t i = 0; i < records.size(); ++i)
    {
        std::unique_ptr<IInputEvent> event = IInputEvent::Create(records[i]);
        outEvents.push_back(std::move(event));
    }
    return outEvents;
}

HRESULT IInputEvent::ToInputRecords(_In_ const std::deque<std::unique_ptr<IInputEvent>>& events,
                                    _Out_writes_(cRecords) INPUT_RECORD* const Buffer,
                                    _In_ const size_t cRecords)
{
    assert(events.size() <= cRecords);

    if (events.size() > cRecords)
    {
        return E_INVALIDARG;
    }

    for (size_t i = 0; i < cRecords; ++i)
    {
        Buffer[i] = events[i]->ToInputRecord();
    }
    return S_OK;
}

// Routine Description:
// - checks if flag is present in flags
// Arguments:
// - flags - bit battern to check for flag
// - flag - bit pattern to search for
// Return Value:
// - true if flag is present in flags
// Note:
// - The wil version of IsFlagSet is only to operate on values that
// are compile time constants. This will work with runtime calculated
// values.
static bool RuntimeIsFlagSet(_In_ const DWORD flags, _In_ const DWORD flag) noexcept
{
    return !!(flags & flag);
}

// Routine Description:
// - Expands legacy control keys bitsets into a stl set
// Arguments:
// - flags - legacy bitset to expand
// Return Value:
// - set of ModifierKeyState values that represent flags
std::unordered_set<ModifierKeyState> ExpandModifierKeyStateFlags(_In_ const DWORD flags)
{
    std::unordered_set<ModifierKeyState> keyStates;

    static const std::unordered_map<DWORD, ModifierKeyState> mapping =
    {
        { RIGHT_ALT_PRESSED, ModifierKeyState::RightAlt },
        { LEFT_ALT_PRESSED, ModifierKeyState::LeftAlt },
        { RIGHT_CTRL_PRESSED, ModifierKeyState::RightCtrl },
        { LEFT_CTRL_PRESSED, ModifierKeyState::LeftCtrl },
        { SHIFT_PRESSED, ModifierKeyState::Shift },
        { NUMLOCK_ON, ModifierKeyState::NumLock },
        { SCROLLLOCK_ON, ModifierKeyState::ScrollLock },
        { CAPSLOCK_ON, ModifierKeyState::CapsLock },
        { ENHANCED_KEY, ModifierKeyState::EnhancedKey },
        { NLS_DBCSCHAR, ModifierKeyState::NlsDbcsChar },
        { NLS_ALPHANUMERIC, ModifierKeyState::NlsAlphanumeric },
        { NLS_KATAKANA, ModifierKeyState::NlsKatakana },
        { NLS_HIRAGANA, ModifierKeyState::NlsHiragana },
        { NLS_ROMAN, ModifierKeyState::NlsRoman },
        { NLS_IME_CONVERSION, ModifierKeyState::NlsImeConversion },
        { NLS_IME_DISABLE, ModifierKeyState::NlsImeDisable }
    };

    for (auto it = mapping.begin(); it != mapping.end(); ++it)
    {
        if (RuntimeIsFlagSet(flags, it->first))
        {
            keyStates.insert(it->second);
        }
    }

    return keyStates;
}