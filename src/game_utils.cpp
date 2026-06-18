#include "game_utils.h"

#include <iostream>
#include <random>
#include <stdexcept>

#ifdef _WIN32

static HANDLE hStdin = INVALID_HANDLE_VALUE;
static DWORD oldMode = 0;

char getch_nonblock() {
    if (hStdin == INVALID_HANDLE_VALUE) return 0;

    DWORD events = 0;
    GetNumberOfConsoleInputEvents(hStdin, &events);

    if (events == 0) return 0;

    INPUT_RECORD inputRecord;
    DWORD numRead = 0;

    while (events > 0) {
        if (!ReadConsoleInput(hStdin, &inputRecord, 1, &numRead)) break;

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
            WORD vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
            CHAR ch = inputRecord.Event.KeyEvent.uChar.AsciiChar;

            if (vk == VK_LEFT || ch == 'a' || ch == 'A') return 'a';
            if (vk == VK_RIGHT || ch == 'd' || ch == 'D') return 'd';
            if (vk == VK_SPACE || ch == ' ') return ' ';
            if (ch == 'y' || ch == 'Y') return 'y';
            if (ch == 'n' || ch == 'N') return 'n';

            if (ch != 0) return ch;
        }

        GetNumberOfConsoleInputEvents(hStdin, &events);
    }

    return 0;
}

void clear_screen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);
}

void setup_console() {
    SetConsoleOutputCP(CP_UTF8);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD newSize = {(SHORT)(csbi.srWindow.Right - csbi.srWindow.Left + 1),
                     (SHORT)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1)};
    SetConsoleScreenBufferSize(hConsole, newSize);

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &oldMode);
    // отключаю буферизацию строк и эхо, чтобы читать клавиши без Enter
    DWORD newMode = oldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(hStdin, newMode);

    FlushConsoleInputBuffer(hStdin);
}

#else

// BEGIN BORROWED — паттерн неблокирующего чтения символа из терминала
// источник: https://gist.github.com/whyrusleeping/3983293
char getch_nonblock() {
    struct termios oldt, newt;
    char ch = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // отключить канонический режим и эхо для неблокирующего чтения символов
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return ch;
}
// END BORROWED

void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void setup_console() {}

#endif

// BEGIN BORROWED — паттерн генерации случайного числа с mt19937
// источник:
// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
int generate_random(int min, int max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max in generate_random()");
    }
    // static, чтобы инициализировать генератор один раз за время жизни программы
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
// END BORROWED