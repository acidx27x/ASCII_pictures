#include <SFML/Graphics.hpp>
#define NOMINMAX
#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

void setConsoleFont(short width, short height);

class ASCII_picture {
private:
    unsigned short width;
    unsigned short height;
    const double K_RATIO = 1.75;
    const unsigned short MAX_WIDTH = 470;

    sf::Image image;

    bool check_file(const std::string& file_name);
    sf::Image open_file();
    void set_picture_size();
    sf::Image to_gray_scale();
    std::vector<std::vector<char>> convert_to_ascii();
    std::vector<std::vector<char>> convert_to_ascii_to_file();
    void print_ascii_picture(const std::vector<std::vector<char>>& converted) const;
    bool out_file(const std::vector<std::vector<char>>& converted);

    float map_value(const float& value_to_map, const float& start1, const float& stop1, const float& start2, const float& stop2);

public:
    ASCII_picture() { width = 0, height = 0; sf::Image image = sf::Image(); };
    bool working_with_picture();
};

bool ASCII_picture::out_file(const std::vector<std::vector<char>>& converted) {
    std::cout << "\n Do you want to save picture in file? (Y/N): ";
    char command = '0';
    while (true) {
        std::cin >> command;
        if (toupper(command) == 'Y') {
            std::cout << "\n Enter the file_name(|.txt|.text|): ";
            std::string file_name = "";
            std::cin >> file_name;
            std::ofstream file_out(file_name);
            if (file_out.is_open()) {
                std::cout << "\n The file is OK";

                for (unsigned short y = 0; y < height; ++y) {
                    for (unsigned short x = 0; x < width; ++x) {
                        file_out << converted[y][x];
                    }
                    file_out << '\n';
                }
                file_out.close();
                return true;
            }
        }
        else if (toupper(command) == 'N') {
            std::cout << "\n You won`t have file with picture.";
            return false;
        }
        else {
            std::cout << "\n Wrong command. Try again (Y/N): ";
            continue;
        }
    }
}

void ASCII_picture::print_ascii_picture(const std::vector<std::vector<char>>& converted) const {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);
    std::ostringstream out;
    for (auto y = 0; y < height; ++y) {
        for (auto x = 0; x < width; ++x) {
            out << converted[y][x];
        }
        out << "\n";
    }
    std::cout << out.str();
}

float ASCII_picture::map_value(const float& value_to_map, const float& start1, const float& stop1, const float& start2, const float& stop2) {
    return ((value_to_map - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

std::vector<std::vector<char>> ASCII_picture::convert_to_ascii() {
    std::vector<std::vector<char>> converted;
    converted.resize(height, std::vector<char>(width));

    char acsii_gradient[] = { '.', '~', ',', ':', ';', '!', '+', '?', 'x', 'v', 'z', 'X', 'J', 'Q', 'Z', '\0'};
    //$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~i!lI;:,\"^`".

    for (auto y = 0; y < height; ++y) {
        for (auto x = 0; x < width; ++x) {
            unsigned short index = (unsigned short)(map_value(image.getPixel(x, y).r, 0, 255, 0, (float)strlen(acsii_gradient) - 1));
            converted[y][x] = acsii_gradient[index];
        }
    }
    return converted;
}

std::vector<std::vector<char>> ASCII_picture::convert_to_ascii_to_file() {
    std::vector<std::vector<char>> converted_to_file;
    converted_to_file.resize(height, std::vector<char>(width));

    char acsii_gradient_resersed[] = { 'Z', 'Q', 'J', 'X', 'z', 'v', 'x', '?', '+', '!', ';', ':', ',', '~', '.', '\0' };
    //$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~i!lI;:,\"^`".

    for (auto y = 0; y < height; ++y) {
        for (auto x = 0; x < width; ++x) {
            unsigned short index = (unsigned short)(map_value(image.getPixel(x, y).r, 0, 255, 0, (float)strlen(acsii_gradient_resersed) - 1));
            converted_to_file[y][x] = acsii_gradient_resersed[index];
        }
    }
    return converted_to_file;
}

void ASCII_picture::set_picture_size() {
    width = image.getSize().x;
    height = image.getSize().y;
    //MAX_WIDTH = 470
    //K_RATIO = 1.75
    unsigned short max_height = (unsigned short)((float)height / K_RATIO * (float)MAX_WIDTH / (float)width);

    for (unsigned short y = 0; y < max_height; ++y)
    {
        for (unsigned short x = 0; x < MAX_WIDTH; ++x)
        {
            unsigned origX = (unsigned)((float)x / MAX_WIDTH * width);
            unsigned origY =(unsigned)((float)y / max_height * height);
            image.setPixel(x, y, image.getPixel(origX, origY));
        }
    }
    std::cout << "\n Picture width  = " << width;
    std::cout << "\n Picture height = " << height << std::endl;
    width = MAX_WIDTH;
    height = max_height;
    std::cout << "\n Resized picture width  = " << width;
    std::cout << "\n Resized picture height = " << height << std::endl;
}

sf::Image ASCII_picture::to_gray_scale() {
    for (unsigned short y = 0; y < height; ++y) {
        for (unsigned short x = 0; x < width; ++x) {
            auto pixel = image.getPixel(x, y);
            auto avg_value = (pixel.r + pixel.g + pixel.b) / 3;
            image.setPixel(x, y, sf::Color(avg_value, avg_value, avg_value));
        }
    }
    return image;
}

sf::Image ASCII_picture::open_file() {
    std::string file_name = "";
    while (true) {
        std::cout << "\n Enter file_name(|.tga|.jpg|.bmp|.png|.gif|.psd|.hdr|.pic|): ";
        std::cin >> file_name;
        if (check_file(file_name)) {
            if (!image.loadFromFile(file_name)) {
                std::cout << "\n Failed to open from " << file_name << ". Try again.";
            }
            else {
                std::cout << "\n The file is OK";
                return image;
            }
        }
        else {
            std::cout << "\n Wrong file_format. Try again.";
        }
    }
}

bool ASCII_picture::check_file(const std::string& file_name) {
    std::vector<std::string> allowed_files = { ".tga", ".jpg", ".bmp", ".png", ".gif", ".psd", ".hdr", ".pic" };
    for (const auto& file : allowed_files) {
        size_t pos = file_name.find(file);
        if (pos != std::string::npos)
            return true;
        else
            continue;
    }
    return false;
}

bool ASCII_picture::working_with_picture() {
    HWND hWnd = GetForegroundWindow();
    ShowWindow(hWnd, SW_MAXIMIZE);

    setConsoleFont(10, 24);
    open_file();
    set_picture_size();
    to_gray_scale();
    auto converted = convert_to_ascii();
    auto converted_to_file = convert_to_ascii_to_file();
    out_file(converted_to_file);
    std::cout << "\n There will be picture from ASCII symbols in 5 seconds...";
    Sleep(5000);
    system("cls");

    setConsoleFont(3, 5);

    print_ascii_picture(converted);
    
    ShowWindow(hWnd, SW_MINIMIZE);
    ShowWindow(hWnd, SW_MAXIMIZE);

    return true;
}

void setConsoleFont(short width, short height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = {};
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.dwFontSize = { width, height };
    fontInfo.cbSize = sizeof(fontInfo);
    wcscpy_s(fontInfo.FaceName, L"ISOCTEUR");
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
}

DWORD WINAPI CheckEscape() {
    while (GetAsyncKeyState(VK_ESCAPE) == 0);
    std::cout << "\x1B[2J\x1B[H";
    std::exit(EXIT_SUCCESS);
}

int main() {
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckEscape, 0, 0, 0);

    HWND hWnd = GetForegroundWindow();
    ShowWindow(hWnd, SW_MAXIMIZE);
    setConsoleFont(10, 24);

    ASCII_picture picture = ASCII_picture();
    while (true) {
        std::cout << "\n Press two times any key to continue... ESC to stop the programm...";
        std::cout << "\n To load a new picture, press any key after ASCII art..............";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        std::cout << "\x1B[2J\x1B[H";
        if (picture.working_with_picture())
            continue;
    }
    
    return 0;
}