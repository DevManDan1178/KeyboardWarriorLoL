#include <windows.h>
#include "app.hpp"

int main() {
    App application;
    return application.initialize();
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    int result = main();
    return result;
}