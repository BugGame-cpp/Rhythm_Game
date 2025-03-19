#include "RhythmGame.h"

RhythmGame::RhythmGame()
    : gen(rd()), score(0), combo(0), maxCombo(0), missCount(0), gameRunning(false)
{
    init();
}

RhythmGame::~RhythmGame() {
    // �ʿ��� ���� �۾�
}

void RhythmGame::init() {
    notes.clear();
    score = 0;
    combo = 0;
    maxCombo = 0;
    missCount = 0;
    gameRunning = false;

    // ȭ�� �ʱ�ȭ
    initScreen();
}

void RhythmGame::initScreen() {
    // ȭ�� �迭 �ʱ�ȭ
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            gameScreen[y][x] = ' ';
        }
    }

    // ���� ��� �׸���
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int i = 0; i <= NOTE_LANES; i++) {
            int x = (SCREEN_WIDTH / NOTE_LANES) * i;
            if (x < SCREEN_WIDTH) {
                gameScreen[y][x] = '|';
            }
        }
    }

    // ��Ʈ �� �׸���
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        gameScreen[HIT_ZONE_Y][x] = '-';
    }

    // ���� Ű ǥ��
    gameScreen[SCREEN_HEIGHT - 1][SCREEN_WIDTH / NOTE_LANES / 2] = KEY_A;
    gameScreen[SCREEN_HEIGHT - 1][SCREEN_WIDTH / NOTE_LANES + SCREEN_WIDTH / NOTE_LANES / 2] = KEY_S;
    gameScreen[SCREEN_HEIGHT - 1][2 * SCREEN_WIDTH / NOTE_LANES + SCREEN_WIDTH / NOTE_LANES / 2] = KEY_D;
    gameScreen[SCREEN_HEIGHT - 1][3 * SCREEN_WIDTH / NOTE_LANES + SCREEN_WIDTH / NOTE_LANES / 2] = KEY_F;
}

void RhythmGame::setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void RhythmGame::start() {
    init();
    gameRunning = true;

    // �ܼ� Ŀ�� �����
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    // �ܼ� ȭ�� �����
    system("cls");

    std::cout << "��������� �����մϴ�! (����: Q)" << std::endl;
    std::cout << "A, S, D, F Ű�� ����Ͽ� ��Ʈ�� ���߼���." << std::endl;
    std::cout << std::endl;

    run();
}

void RhythmGame::stop() {
    gameRunning = false;

    // �ܼ� Ŀ�� ���̱�
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    system("cls");
    std::cout << "���� ����!" << std::endl;
    std::cout << "���� ����: " << score << std::endl;
    std::cout << "�ִ� �޺�: " << maxCombo << std::endl;
    std::cout << "��ģ ��Ʈ: " << missCount << std::endl;
}

void RhythmGame::run() {
    auto lastSpawnTime = std::chrono::steady_clock::now();
    auto lastUpdateTime = std::chrono::steady_clock::now();

    while (gameRunning) {
        auto currentTime = std::chrono::steady_clock::now();

        // ��Ʈ ����
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnTime).count() > SPAWN_RATE_MS) {
            spawnNote();
            lastSpawnTime = currentTime;
        }

        // ��Ʈ ������Ʈ
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count() > NOTE_SPEED_MS) {
            updateNotes();
            lastUpdateTime = currentTime;
        }

        // Ű �Է� ó��
        processInput();

        // ȭ�� �׸���
        drawScreen();

        // CPU ��뷮 ���̱�
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void RhythmGame::spawnNote() {
    if (!gameRunning) return;

    // ���� ���� ����
    std::uniform_int_distribution<> dis(0, NOTE_LANES - 1);
    int lane = dis(gen);

    // �� ��Ʈ ����
    notes.emplace_back(lane);
}

void RhythmGame::updateNotes() {
    for (auto& note : notes) {
        if (note.active && !note.hit) {
            // ��Ʈ �̵�
            note.y++;

            // ��Ʈ ���� ��� üũ
            if (note.y > SCREEN_HEIGHT) {
                note.active = false;
                combo = 0;
                missCount++;

                // �ʹ� ���� ��Ʈ�� ��ġ�� ���� ����
                if (missCount >= 20) {
                    stop();
                }
            }
        }
    }
}

void RhythmGame::drawScreen() {
    // ȭ�� �ʱ�ȭ
    initScreen();

    // ��Ʈ �׸���
    for (const auto& note : notes) {
        if (note.active && !note.hit && note.y < SCREEN_HEIGHT) {
            int laneWidth = SCREEN_WIDTH / NOTE_LANES;
            int noteX = note.lane * laneWidth + laneWidth / 2;
            gameScreen[note.y][noteX] = '*';
        }
    }

    // ȭ�� ���
    setCursorPosition(0, 3);

    // ������ �޺� ǥ��
    std::cout << "����: " << score << " | �޺�: " << combo << "   " << std::endl;

    // ���� ȭ�� ���
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            std::cout << gameScreen[y][x];
        }
        std::cout << std::endl;
    }
}

void RhythmGame::processInput() {
    if (_kbhit()) {
        char key = _getch();

        // �ҹ��ڷ� ��ȯ
        key = tolower(key);

        switch (key) {
        case KEY_A:
            checkHit(0);
            break;
        case KEY_S:
            checkHit(1);
            break;
        case KEY_D:
            checkHit(2);
            break;
        case KEY_F:
            checkHit(3);
            break;
        case KEY_Q:
            stop();
            break;
        }
    }
}

void RhythmGame::checkHit(int lane) {
    bool hitSuccess = false;

    // ��Ʈ ���� �ȿ� �ִ� ��Ʈ ã��
    for (auto& note : notes) {
        if (note.active && !note.hit && note.lane == lane) {
            // ��Ʈ ������ �ִ��� Ȯ�� (���� ���� +/- 1)
            if (abs(note.y - HIT_ZONE_Y) <= 1) {
                note.hit = true;
                hitSuccess = true;

                // ��Ȯ���� ���� ���� ���
                int accuracy = abs(note.y - HIT_ZONE_Y);

                if (accuracy == 0) {
                    // ����Ʈ
                    addScore(100);
                }
                else {
                    // ����
                    addScore(50);
                }

                break;
            }
        }
    }

    // ��Ʈ ���н� �޺� �ʱ�ȭ
    if (!hitSuccess) {
        combo = 0;
    }
}

void RhythmGame::addScore(int points) {
    combo++;
    if (combo > maxCombo) {
        maxCombo = combo;
    }

    // �޺� ���ʽ�
    int comboBonus = (combo / 10) * 10;
    score += points + comboBonus;
}