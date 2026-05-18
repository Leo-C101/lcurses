#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace lc {

template <typename T>
struct Vector2 {
    T x, y;

    constexpr Vector2(const T& x = T(), const T& y = T()) {
        this->x = x;
        this->y = y;
    }

    constexpr bool operator==(const Vector2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    constexpr bool operator!=(const Vector2& rhs) const {
        return !(*this == rhs);
    }

    constexpr Vector2 operator+(const Vector2& rhs) const {
        return Vector2(x + rhs.x, y + rhs.y);
    }

    void operator+=(const Vector2& rhs) {
        x += rhs.x;
        y += rhs.y;
    }

    constexpr Vector2 operator-(const Vector2& rhs) const {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    void operator-=(const Vector2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
    }

    constexpr Vector2 operator*(const Vector2& rhs) const {
        return Vector2(x * rhs.x, y * rhs.y);
    }

    void operator*=(const Vector2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
    }

    constexpr Vector2 operator/(const Vector2& rhs) const {
        return Vector2(x / rhs.x, y / rhs.y);
    }

    void operator/=(const Vector2& rhs) {
        x /= rhs.x;
        y /= rhs.y;
    }
};

using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned int>;
using Vector2f = Vector2<float>;

[[nodiscard]] inline Vector2i term_size() {
    struct winsize w {};
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &w) == 0) {
        return Vector2i(w.ws_col, w.ws_row);
    }
    return Vector2i(-1, -1);
}

inline int getch(void) {
    struct termios oldattr, newattr;
    if (tcgetattr(STDIN_FILENO, &oldattr) != 0) {
        return std::getchar();
    }

    struct TermiosGuard {
        termios original;
        ~TermiosGuard() {
            tcsetattr(STDIN_FILENO, TCSANOW, &original);
        }
    } guard{oldattr};

    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newattr) != 0) {
        return std::getchar();
    }

    return std::getchar();
}

struct Color {
    std::uint8_t r, g, b;

    constexpr Color(std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    [[nodiscard]] constexpr bool operator==(const Color& rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }

    [[nodiscard]] constexpr bool operator!=(const Color& rhs) const {
        return !(*this == rhs);
    }
};

struct Theme {
    Color fg;
    Color bg;

    constexpr Theme(Color fg, Color bg) {
        this->fg = fg;
        this->bg = bg;
    }

    static constexpr Theme Default() {
        return Theme(Color(255, 255, 255), Color(0, 0, 0));
    }

    static constexpr Theme Light() {
        return Theme(Color(0, 0, 0), Color(255, 255, 255));
    }

    static constexpr Theme Dark() {
        return Theme(Color(255, 255, 255), Color(48, 48, 48));
    }

    static constexpr Theme Night() {
        return Theme(Color(87, 238, 255), Color(3, 46, 84));
    }

    static constexpr Theme Solar() {
        return Theme(Color(161, 153, 0), Color(246, 255, 150));
    }

    static constexpr Theme Monokai() {
        return Theme(Color(248, 248, 242), Color(39, 40, 34));
    }

    static constexpr Theme Dracula() {
        return Theme(Color(248, 248, 242), Color(45, 52, 64));
    }

    static constexpr Theme Gruvbox() {
        return Theme(Color(235, 219, 178), Color(40, 40, 40));
    }

    static constexpr Theme SolarizedLight() {
        return Theme(Color(101, 123, 131), Color(253, 246, 227));
    }

    static constexpr Theme Ocean() {
        return Theme(Color(173, 216, 230), Color(2, 60, 80));
    }

    static constexpr Theme Forest() {
        return Theme(Color(200, 230, 200), Color(10, 30, 10));
    }

    static constexpr Theme Pastel() {
        return Theme(Color(60, 60, 60), Color(255, 240, 245));
    }

    static constexpr Theme Matrix() {
        return Theme(Color(0, 255, 70), Color(0, 0, 0));
    }

    static constexpr Theme Ember() {
        return Theme(Color(255, 200, 150), Color(40, 10, 10));
    }
};

struct Cell {
    wchar_t ch;
    Theme theme;

    Cell(wchar_t ch = ' ', Theme theme = Theme::Default())
        : ch(ch), theme(theme) {}
};

inline std::string get_fg_change_esc(Color c) {
    return "\033[38;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
}

inline std::string get_bg_change_esc(Color c) {
    return "\033[48;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
}

inline std::string wchar_to_utf8(wchar_t wch) {
    std::string out;
    if (wch <= 0x7F) {
        out.push_back(static_cast<char>(wch));
    }
    else if (wch <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | ((wch >> 6) & 0x1F)));
        out.push_back(static_cast<char>(0x80 | (wch & 0x3F)));
    }
    else if (wch <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | ((wch >> 12) & 0x0F)));
        out.push_back(static_cast<char>(0x80 | ((wch >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (wch & 0x3F)));
    }
    else if (wch <= 0x10FFFF) {
        out.push_back(static_cast<char>(0xF0 | ((wch >> 18) & 0x07)));
        out.push_back(static_cast<char>(0x80 | ((wch >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((wch >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (wch & 0x3F)));
    }
    return out;
}

class Screen {
private:
    std::vector<std::vector<Cell>> m_Buffer;

public:
    Screen() {
        Vector2i termSize = term_size();

        if (termSize != Vector2i(-1, -1)) {
            m_Buffer = std::vector<std::vector<Cell>>(termSize.y, std::vector<Cell>(termSize.x, Cell()));
        }
        else {
            m_Buffer = std::vector<std::vector<Cell>>(24, std::vector<Cell>(80, Cell()));
        }
    }

    void set_char_at(int x, int y, wchar_t ch) {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size()) || x < 0 || x >= static_cast<int>(m_Buffer[0].size())) {
            return;
        }
        m_Buffer[y][x].ch = ch;
    }

    wchar_t get_char_at(int x, int y) const {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size()) || x < 0 || x >= static_cast<int>(m_Buffer[0].size())) {
            return '\0';
        }
        return m_Buffer[y][x].ch;
    }

    Cell get_cell_at(int x, int y) const {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size()) || x < 0 || x >= static_cast<int>(m_Buffer[0].size())) {
            return Cell();
        }
        return m_Buffer[y][x];
    }

    void set_fg_at(int x, int y, Color fg) {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size()) || x < 0 || x >= static_cast<int>(m_Buffer[0].size())) {
            return;
        }
        m_Buffer[y][x].theme.fg = fg;
    }

    void set_bg_at(int x, int y, Color bg) {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size()) || x < 0 || x >= static_cast<int>(m_Buffer[0].size())) {
            return;
        }
        m_Buffer[y][x].theme.bg = bg;
    }

    void add_str(int x, int y, const std::string& str) {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size())) {
            return;
        }

        int currentX = x; // Start writing from the requested x coordinate
        std::size_t i = 0;
        
        while (i < str.size()) {
            if (currentX >= static_cast<int>(m_Buffer[0].size())) break;
            wchar_t wch = 0;
            unsigned char c = str[i];

            if (c <= 0x7F) {
                wch = c;
                i += 1;
            }
            else if ((c & 0xE0) == 0xC0) {
                if (i + 1 >= str.size()) break;
                wch = ((c & 0x1F) << 6) | (static_cast<unsigned char>(str[i + 1]) & 0x3F);
                i += 2;
            }
            else if ((c & 0xF0) == 0xE0) {
                if (i + 2 >= str.size()) break;
                wch = ((c & 0x0F) << 12) | 
                    ((static_cast<unsigned char>(str[i + 1]) & 0x3F) << 6) | 
                    (static_cast<unsigned char>(str[i + 2]) & 0x3F);
                i += 3;
            }
            else if ((c & 0xF8) == 0xF0) {
                if (i + 3 >= str.size()) break;
                wch = ((c & 0x07) << 18) | 
                    ((static_cast<unsigned char>(str[i + 1]) & 0x3F) << 12) | 
                    ((static_cast<unsigned char>(str[i + 2]) & 0x3F) << 6) | 
                    (static_cast<unsigned char>(str[i + 3]) & 0x3F);
                i += 4;
            }
            else {
                i += 1;
                continue;
            }

            if (currentX >= 0) {
                set_char_at(currentX, y, wch);
            }
            currentX++;
        }
    }

    void add_wstr(int x, int y, const std::wstring& str) {
        if (y < 0 || y >= static_cast<int>(m_Buffer.size())) {
            return;
        }
        for (int i = 0; i < static_cast<int>(str.size()); ++i) {
            if (x + i >= 0) {
                set_char_at(x + i, y, str[i]);
            }
        }
    }

    void fill_fg(Color fg) {
        for (int y = 0; y < static_cast<int>(m_Buffer.size()); ++y) {
            for (int x = 0; x < static_cast<int>(m_Buffer[0].size()); ++x) {
                m_Buffer[y][x].theme.fg = fg;
            }
        }
    }

    void fill_bg(Color bg) {
        for (int y = 0; y < static_cast<int>(m_Buffer.size()); ++y) {
            for (int x = 0; x < static_cast<int>(m_Buffer[0].size()); ++x) {
                m_Buffer[y][x].theme.bg = bg;
            }
        }
    }

    void fill(Cell cell) {
        for (int y = 0; y < static_cast<int>(m_Buffer.size()); ++y) {
            for (int x = 0; x < static_cast<int>(m_Buffer[0].size()); ++x) {
                m_Buffer[y][x] = cell;
            }
        }
    }

    void fill(wchar_t ch) {
        for (int y = 0; y < static_cast<int>(m_Buffer.size()); ++y) {
            for (int x = 0; x < static_cast<int>(m_Buffer[0].size()); ++x) {
                m_Buffer[y][x].ch = ch;
            }
        }
    }

    void display() {
        std::string output;
        for (int y = 0; y < static_cast<int>(m_Buffer.size()); ++y) {
            Color currFg;
            Color currBg;
            bool hasCurrColors = false;
            for (int x = 0; x < static_cast<int>(m_Buffer[0].size()); ++x) {
                if (!hasCurrColors || currFg != m_Buffer[y][x].theme.fg) {
                    output += get_fg_change_esc(m_Buffer[y][x].theme.fg);
                    currFg = m_Buffer[y][x].theme.fg;
                }

                if (!hasCurrColors || currBg != m_Buffer[y][x].theme.bg) {
                    output += get_bg_change_esc(m_Buffer[y][x].theme.bg);
                    currBg = m_Buffer[y][x].theme.bg;
                }

                hasCurrColors = true;

                output += wchar_to_utf8(m_Buffer[y][x].ch);
            }
            if (y < static_cast<int>(m_Buffer.size()) - 1) {
                output += "\n";
            }
        }
        output += "\033[0m";
        std::cout << output << std::flush;
    }
};

inline Screen stdscr;

inline void move_cursor(int x, int y) {
    std::cout << "\033[" << (y + 1) << ';' << (x + 1) << 'H';
}

inline void clear() {
    stdscr.fill(' ');
}

inline void mvprint(int x, int y, const std::string& str) {
    stdscr.add_str(x, y, str);
}

inline void mvprintw(int x, int y, const std::wstring& str) {
    stdscr.add_wstr(x, y, str);
}

inline void mvprint_colored(int x, int y, Color fg, Color bg, const std::wstring& str) {
    for (int i = 0; i < static_cast<int>(str.size()); ++i) {
        stdscr.set_char_at(x + i, y, str[i]);
        stdscr.set_fg_at(x + i, y, fg);
        stdscr.set_bg_at(x + i, y, bg);
    }
}

inline void mvprint_colored(int x, int y, Color fg, const std::wstring& str) {
    for (int i = 0; i < static_cast<int>(str.size()); ++i) {
        stdscr.set_char_at(x + i, y, str[i]);
        stdscr.set_fg_at(x + i, y, fg);
    }
}

inline void set_theme(Theme t) {
    stdscr.fill_fg(t.fg);
    stdscr.fill_bg(t.bg);
}

inline void set_fill_fg_color(Color c) {
    stdscr.fill_fg(c);
}

inline void set_fill_bg_color(Color c) {
    stdscr.fill_bg(c);
}

inline void set_fg_at(int x, int y, Color c) {
    stdscr.set_fg_at(x, y, c);
}

inline void set_bg_at(int x, int y, Color c) {
    stdscr.set_bg_at(x, y, c);
}

inline void refresh() {
    move_cursor(0, 0);
    stdscr.display();
    stdscr.fill(' ');
    std::cout << std::flush;
}

struct Box {
protected:
    static constexpr wchar_t BOX_VERTICAL = L'│';
	static constexpr wchar_t BOX_HORIZONTAL = L'─';
	static constexpr wchar_t BOX_TOP_LEFT = L'┌';
	static constexpr wchar_t BOX_TOP_RIGHT = L'┐';
	static constexpr wchar_t BOX_BOTTOM_LEFT = L'└';
	static constexpr wchar_t BOX_BOTTOM_RIGHT = L'┘';
	static constexpr wchar_t BOX_CONNECT_LEFT = L'┤';
	static constexpr wchar_t BOX_CONNECT_RIGHT = L'├';

public:
    int x, y, width, height;

    Box(int x, int y, int w, int h) {
        this->x = x;
        this->y = y;
        this->width = std::max(2, w);
        this->height = std::max(2, h);
    }

    static std::wstring get_box_top(int w) {
		return BOX_TOP_LEFT + std::wstring(w - 2, BOX_HORIZONTAL) + BOX_TOP_RIGHT;
	}

	static std::wstring get_box_bottom(int w) {
		return BOX_BOTTOM_LEFT + std::wstring(w - 2, BOX_HORIZONTAL) + BOX_BOTTOM_RIGHT;
	}

    virtual void display() const {
		mvprintw(x, y, get_box_top(width));

		for (int dy = 1; dy < height - 1; ++dy) {
			mvprintw(x, y + dy, std::wstring(1, BOX_VERTICAL));
			mvprintw(x + width - 1, y + dy, std::wstring(1, BOX_VERTICAL));
		}

		mvprintw(x, y + height - 1, get_box_bottom(width));
	}
};

}