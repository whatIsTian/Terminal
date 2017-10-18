/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/

#include "precomp.h"
#include "inc/IInputEvent.hpp"

MouseEvent::MouseEvent(_In_ const MOUSE_EVENT_RECORD& record) :
    _mousePosition{ record.dwMousePosition },
    _buttonState{ record.dwButtonState },
    _activeModifierKeys{ record.dwControlKeyState },
    _eventFlags{ record.dwEventFlags }
{
}

MouseEvent::MouseEvent(_In_ const COORD mousePosition,
                       _In_ const DWORD buttonState,
                       _In_ const DWORD activeModifierKeys,
                       _In_ const DWORD eventFlags) :
    _mousePosition{ mousePosition },
    _buttonState{ buttonState },
    _activeModifierKeys{ activeModifierKeys },
    _eventFlags{ eventFlags }
{
}

MouseEvent::~MouseEvent()
{
}

INPUT_RECORD MouseEvent::ToInputRecord() const noexcept
{
    INPUT_RECORD record{ 0 };
    record.EventType = MOUSE_EVENT;
    record.Event.MouseEvent.dwMousePosition = _mousePosition;
    record.Event.MouseEvent.dwButtonState = _buttonState;
    record.Event.MouseEvent.dwControlKeyState = _activeModifierKeys;
    record.Event.MouseEvent.dwEventFlags = _eventFlags;
    return record;
}

InputEventType MouseEvent::EventType() const noexcept
{
    return InputEventType::MouseEvent;
}

COORD MouseEvent::GetMousePosition() const noexcept
{
    return _mousePosition;
}

void MouseEvent::SetMousePosition(_In_ const COORD mousePosition) noexcept
{
    _mousePosition = mousePosition;
}

DWORD MouseEvent::GetButtonState() const noexcept
{
    return _buttonState;
}

void MouseEvent::SetButtonState(_In_ const DWORD buttonState) noexcept
{
    _buttonState = buttonState;
}

DWORD MouseEvent::GetActiveModifierKeys() const noexcept
{
    return _activeModifierKeys;
}

void MouseEvent::SetActiveModifierKeys(_In_ const DWORD activeModifierKeys) noexcept
{
    _activeModifierKeys = activeModifierKeys;
}

DWORD MouseEvent::GetEventFlags() const noexcept
{
    return _eventFlags;
}

void MouseEvent::SetEventFlags(_In_ const DWORD eventFlags) noexcept
{
    _eventFlags = eventFlags;
}
