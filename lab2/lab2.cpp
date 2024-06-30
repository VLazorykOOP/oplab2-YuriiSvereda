#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>

const int AREA_WIDTH = 50; // ширина області проживання
const int AREA_HEIGHT = 30; // висота області проживання
const int SPEED = 3; // швидкість руху
const char GOLDFISH_CHAR = 'G'; // символ для золотих рибок
const char GUPPY_CHAR = 'U'; // символ для гуппі

std::mutex cout_mutex; // для синхронізованого виводу
std::vector<std::vector<char>> area(AREA_HEIGHT, std::vector<char>(AREA_WIDTH, ' ')); // область проживання

std::atomic<bool> running(true); // змінна для контролю завершення програми

void drawArea() {
    while (running) {
        {
            std::lock_guard<std::mutex> guard(cout_mutex);
            // system("clear"); // для Linux/Unix систем
            system("cls"); // для Windows систем

            for (const auto& row : area) {
                for (char cell : row) {
                    std::cout << cell;
                }
                std::cout << '\n';
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void moveGoldfish(int id, int& positionX) {
    while (running) {
        {
            std::lock_guard<std::mutex> guard(cout_mutex);
            area[1 + id][positionX] = ' '; // видалити старе положення
            positionX += SPEED;
            if (positionX >= AREA_WIDTH) {
                positionX = 0; // якщо дійшла до кінця області, повертаємо на початок
            }
            area[1 + id][positionX] = GOLDFISH_CHAR; // намалювати нове положення
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // пауза для симуляції руху
    }
}

void moveGuppy(int id, int& positionY) {
    while (running) {
        {
            std::lock_guard<std::mutex> guard(cout_mutex);
            area[positionY][1 + id * 2] = ' '; // видалити старе положення
            positionY += SPEED;
            if (positionY >= AREA_HEIGHT) {
                positionY = 0; // якщо дійшла до кінця області, повертаємо на початок
            }
            area[positionY][1 + id * 2] = GUPPY_CHAR; // намалювати нове положення
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // пауза для симуляції руху
    }
}

int main() {
    int numGoldfish = 2; // кількість золотих рибок
    int numGuppies = 5;  // кількість гуппі

    std::vector<std::thread> threads;

    // Запуск потоку для малювання області
    threads.emplace_back(drawArea);

    // Запуск потоків для золотих рибок
    for (int i = 0; i < numGoldfish; ++i) {
        int* positionX = new int(0);
        threads.emplace_back(moveGoldfish, i, std::ref(*positionX));
    }

    // Запуск потоків для гуппі
    for (int i = 0; i < numGuppies; ++i) {
        int* positionY = new int(0);
        threads.emplace_back(moveGuppy, i, std::ref(*positionY));
    }

    // Очікуємо натискання клавіші для завершення програми
    std::cin.get();
    running = false;

    // Очікуємо завершення всіх потоків
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
