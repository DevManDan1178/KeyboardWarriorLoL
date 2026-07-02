#include "input/InputReader.hpp"
#include <iostream>
#include <thread>
#include <bit>
#include <windows.h>


// uiohook raw keycodes for modifier keys
#define KEY_LEFT_CTRL   0x001D
#define KEY_RIGHT_CTRL  0xE01D
#define KEY_LEFT_SHIFT  0x002A
#define KEY_RIGHT_SHIFT 0x0036
#define KEY_LEFT_ALT    0x0038
#define KEY_RIGHT_ALT   0xE038

std::vector<HotkeyBinding> pressedBindings;
std::vector<HotkeyBinding> releaseBindings;
std::unordered_map<int, bool> keyStates;

bool ctrlPressed  = false;
bool shiftPressed = false;
bool altPressed   = false;


// Modifiers
bool isModifierKey(int keycode)
{   
    return keycode == KEY_LEFT_CTRL   || keycode == KEY_RIGHT_CTRL ||
           keycode == KEY_LEFT_SHIFT  || keycode == KEY_RIGHT_SHIFT ||
           keycode == KEY_LEFT_ALT    || keycode == KEY_RIGHT_ALT;
}

void updateModifier(int keycode, bool pressed)
{
    if (keycode == KEY_LEFT_CTRL || keycode == KEY_RIGHT_CTRL)
        ctrlPressed = pressed;
    else if (keycode == KEY_LEFT_SHIFT || keycode == KEY_RIGHT_SHIFT)
        shiftPressed = pressed;
    else if (keycode == KEY_LEFT_ALT || keycode == KEY_RIGHT_ALT) 
        altPressed = pressed;
}


uint8_t InputReader::currentModifiers()
{
    uint8_t mods = Modifiers::None;

    bool ctrl  = ctrlPressed || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    bool shift = shiftPressed || (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    bool alt   = altPressed || (GetAsyncKeyState(VK_MENU) & 0x8000);

    if (shift) {
        mods |= Modifiers::Shift;
    } 

    if (alt) {
        mods |= Modifiers::Alt;
    }

    if (ctrl) {
        mods |= Modifiers::Ctrl;
    }

    return mods;
}

bool InputReader::matchesModifiers(uint8_t required)
{
    uint8_t current = currentModifiers();

    return current == required;
}

// Event handling
void InputReader::dispatch(uiohook_event* event) {
    switch (event->type)
    {
        case EVENT_KEY_PRESSED:
        {
            int code = event->data.keyboard.keycode;

            updateModifier(code, true);
            keyStates[code] = true;

            if (!isModifierKey(code))
            {
                for (auto& binding : pressedBindings)
                {
                    if (binding.hotkey.bindType == BindType::Keyboard &&
                        binding.hotkey.keyCode == code)
                    {
                        if (matchesModifiers(binding.hotkey.modifiers))
                        {
                            binding.callback();
                        }
                    }
                }
            }
            break;
        }


        case EVENT_KEY_RELEASED:
        {
            int code = event->data.keyboard.keycode;
            updateModifier(code, false);
            keyStates[code] = false;

            if (!isModifierKey(code))
            {
                for (auto& binding : releaseBindings)
                {
                    if (binding.hotkey.bindType == BindType::Keyboard &&
                        binding.hotkey.keyCode == code)
                    {
                        if (matchesModifiers(binding.hotkey.modifiers))
                        {
                            binding.callback();
                        }
                    }
                }
            }
            break;
        }


        case EVENT_MOUSE_PRESSED:
        {
            int btn = event->data.mouse.button;

            keyStates[-btn] = true;

            uint8_t mods = currentModifiers();

            HotkeyBinding* bestMatch = nullptr;
            int bestScore = -1;

            for (auto& binding : pressedBindings)
            {
                if (binding.hotkey.bindType != BindType::Mouse) {
                    continue;
                }
                    

                if (binding.hotkey.keyCode != btn) {
                    continue;
                }
                    

                if (!matchesModifiers(binding.hotkey.modifiers)) {
                    continue;
                }
                    

    
                int score = std::popcount(binding.hotkey.modifiers);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMatch = &binding;
                }
            }

            if (bestMatch) {
                bestMatch->callback();
            }
                

            break;
        }


       case EVENT_MOUSE_RELEASED:
        {
            int btn = event->data.mouse.button;

            keyStates[-btn] = false;

            HotkeyBinding* bestMatch = nullptr;
            int bestScore = -1;

            for (auto& binding : releaseBindings)
            {
                if (binding.hotkey.bindType != BindType::Mouse) {
                    continue;
                }
                    

                if (binding.hotkey.keyCode != btn) {
                    continue;
                }
                    

                if (!matchesModifiers(binding.hotkey.modifiers)) {
                    continue;
                }
                    

                int score = std::popcount(binding.hotkey.modifiers);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMatch = &binding;
                }
            }

            if (bestMatch) {
                bestMatch->callback();
            }
                

            break;
        }

        default:
            break;
    }
}

// Usage

bool InputReader::start()
{
    hook_set_dispatch_proc(dispatch);

    std::thread([]()
    {
        int result = hook_run();
        if (result != UIOHOOK_SUCCESS)
            std::cerr << "[InputReader] hook_run failed: " << result << "\n";
    }).detach();

    return true;
}

void InputReader::stop()
{
    hook_stop();
}

// Hotkeys

void InputReader::onHotkey(const Hotkey& hotkey, HotkeyCallback callback)
{
    pressedBindings.push_back({ hotkey, std::move(callback) });
}

void InputReader::onHotkeyRelease(const Hotkey& hotkey, HotkeyCallback callback)
{
    releaseBindings.push_back({ hotkey, std::move(callback) });
}

void InputReader::clearHotkeys()
{
    pressedBindings.clear();
    releaseBindings.clear();
}
                

bool InputReader::isKeyDown(int keycode)
{
    bool down = keyStates[keycode];

    return down;
}