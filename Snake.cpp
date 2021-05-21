#include <iostream>
#include <Windows.h>
#include <random>
#include <list>
#include <iterator>
#include <string>

const int screenWidth = 120, screenHeight = 40;

bool checkAtBorder(int i);
int generateApplePosition(std::list<int> snakePositions);

// class for snake character
class snake {
public:
    std::list <int> position;
    std::list <char> model;
    int snakeLength = 7;
    const int startingPosition = 525 + 10 * 120;
    char headMovements[screenWidth * screenHeight];
    int score = 0;

    snake() {
        model.push_front('H');
        position.push_front(startingPosition);
        for (int i = 1; i < snakeLength; i++) {
            model.push_back('0');
            position.push_back(startingPosition + i);
        }
        headMovements[position.front()] = 'S';
    }

    void drawSnake(char screen[screenWidth * screenHeight]) {
        std::list<char>::iterator modelIt = model.begin();
        std::list<int>::iterator positionIt = position.begin();
        for (; modelIt != model.end() && positionIt != position.end(); modelIt++, positionIt++) {
            screen[*positionIt] = *modelIt;
        }
    }

    void move(char movement) {
        if (headMovements[position.front()] == 'S') {
            std::list<int>::iterator positionIt = position.begin();
            positionIt++;
            for (; positionIt != position.end(); positionIt++) {
                headMovements[*positionIt] = 'L';
            }
        }
        headMovements[position.front()] = movement;
        std::list<int> positionTemp;
        std::list<int>::iterator positionIt = position.begin();
        for (; positionIt != position.end(); positionIt++) {
            if (headMovements[*positionIt] == 'U') {
                positionTemp.push_back(*positionIt - 120);
            }
            else if (headMovements[*positionIt] == 'D') {
                positionTemp.push_back(*positionIt + 120);
            }
            else if (headMovements[*positionIt] == 'R') {
                positionTemp.push_back(*positionIt + 1);
            }
            else if (headMovements[*positionIt] == 'L') {
                positionTemp.push_back(*positionIt - 1);
            }
        }
        position = positionTemp;
    }

    bool checkDeath() {
        // intersected with body 
        std::list<int>::iterator positionIt = position.begin();
        positionIt++;
        for (; positionIt != position.end(); positionIt++) {
            if (position.front() == *positionIt) {
                return true;
            }
        }

        // intersected with border
        return checkAtBorder(position.front());
    }

    void updateCharacter() {
        model.push_back('0');
        if (headMovements[position.back()] == 'U') {
            position.push_back(position.back() + 120);
        }
        else if (headMovements[position.back()] == 'D') {
            position.push_back(position.back() - 120);
        }
        else if (headMovements[position.back()] == 'L') {
            position.push_back(position.back() + 1);
        }
        else if (headMovements[position.back()] == 'R') {
            position.push_back(position.back() - 1);
        }
    }

};

class apple {
public:
    char model = 'A';
    int position;

    apple(std::list<int> snakePositions) {
        position = 525 + 10 * 120 - 5;
    }

    bool updatePosition(std::list<int> snakePositions) {
        if (snakePositions.front() == position) {
            position = generateApplePosition(snakePositions);
            return true;
        }
        return false;
    }

    void drawApple(char screen[screenWidth * screenHeight]) {
        screen[position] = model;
    }

};

void drawPlayingField(char screen[screenWidth * screenHeight]) {
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        if (checkAtBorder(i)) {
            screen[i] = '#';
        }
        else {
            screen[i] = ' ';
        }
    }
}

bool checkAtBorder(int i) {
    if (0 <= i && i <= screenWidth - 1) {                                                           // top border
        return true;
    }
    else if (screenWidth * (screenHeight - 11) <= i && i <= screenWidth * (screenHeight - 10) - 1) {     // bottom border
        return true;
    }
    else if (i % screenWidth == 0) {                                                                // left side border
        return true;
    }
    else if ((i - screenWidth + 1) % screenWidth == 0) {                                            // right side border
        return true;
    }
    else {
        return false;
    }
}

bool checkInSnakePositionArray(std::list<int> snakePositions, int position) {
    for (std::list<int>::iterator positionIt = snakePositions.begin(); positionIt != snakePositions.end(); positionIt++) {
        if (position == *positionIt)
            return true;
        return false;
    }
}

int generateApplePosition(std::list<int> snakePositions) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, screenWidth * (screenHeight - 10) - 1);
    int position = distr(gen);
    while (checkAtBorder(position) || checkInSnakePositionArray(snakePositions, position) 
        || position == snakePositions.back() + 120 || position == snakePositions.back() - 120 || position == snakePositions.back() + 1 || position == snakePositions.back() -1 ) {
        position = distr(gen);
    }
    return position;
}

int main() {
    bool isPlaying = true;

    char screen[screenWidth * screenHeight];
    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console);
    DWORD bytesWritten = 0;

    snake player;

    apple item(player.position);

    char movement = 'X';

    while (isPlaying) {

        Sleep(65);

        // Determines which movement was inputted
        if (GetAsyncKeyState(VK_UP) && movement != 'D') {
            movement = 'U';
        }
        else if (GetAsyncKeyState(VK_DOWN) && movement != 'U') {
            movement = 'D';
        }
        else if (GetAsyncKeyState(VK_LEFT) && movement != 'R') {
            movement = 'L';
        }
        else if (GetAsyncKeyState(VK_RIGHT) && movement != 'L') {
            movement = 'R';
        }

        if (movement != 'X') {
            if (player.headMovements[player.position.front()] == 'S' && movement != 'R')
                player.move(movement);
            else if (player.headMovements[player.position.front()] != 'S')
                player.move(movement);
            if (player.headMovements[player.position.front()] == 'S' && movement == 'R')
                movement = 'X';
        }

        if (player.checkDeath()) {
            isPlaying = false;
        }

        if (item.updatePosition(player.position)) {
            player.updateCharacter();
            player.score += 1;
        }

        drawPlayingField(screen);
        item.drawApple(screen);
        player.drawSnake(screen);

        std::string score = "Score: ";
        score.append(std::to_string(player.score));

        for (int i = 0; i < score.length(); i++) {
            screen[3480 + 10 + i] = score[i];
        }

        WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    drawPlayingField(screen);

    std::string gameOver = "GAME OVER! Final Score is ";
    gameOver.append(std::to_string(player.score));

    for (int i = 0; i < gameOver.length(); i++) {
        screen[500 + i] = gameOver[i];
    }

    while (true) {
        WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    return 0;
}