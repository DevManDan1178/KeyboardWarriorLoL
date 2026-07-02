#pragma once
enum class BindType {
    Keyboard, Mouse, None
};

enum Modifiers {
    None = 0,
    Shift = 1 << 0,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
};

struct Hotkey {
    int keyCode;
    BindType bindType;
    uint8_t modifiers; //Bitmask from enum Modifiers
    Hotkey()
        : keyCode(0), bindType(BindType::None), modifiers(0) {}
        
    Hotkey(int k, BindType b, uint8_t m)
        : keyCode(k), bindType(b), modifiers(m) {}
};
