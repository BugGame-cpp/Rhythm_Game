#include "RhythmGame.h"

RhythmGame::RhythmGame()
    : gen(rd()), score(0), combo(0), maxCombo(0), missCount(0), gameRunning(false)
{
    init();
}

RhythmGame::~RhythmGame() {
    // 필요한 정리 작업
}

void RhythmGame::init() {
    notes.clear();
    score = 0;
    combo = 0;
    maxCombo = 0;
    missCount = 0;
    gameRunning = false;

    // 화면 초기화
    initScreen();
}

void RhythmGame::initScreen() {
    // 화면 배열 초기화
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            gameScreen[y][x] = ' ';
        }
    }

    // 레인 경계 그리기
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int i = 0; i <= NOTE_LANES; i++) {
            int x = (SCREEN_WIDTH / NOTE_LANES) * i;
            if (x < SCREEN_WIDTH) {
                gameScreen[y][x] = '|';
            }
        }
    }

    // 히트 존 그리기
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        gameScreen[HIT_ZONE_Y][x] = '-';
    }

    // 레인 키 표시
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

    // 콘솔 커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    // 콘솔 화면 지우기
    system("cls");

    std::cout << "리듬게임을 시작합니다! (종료: Q)" << std::endl;
    std::cout << "A, S, D, F 키를 사용하여 노트를 맞추세요." << std::endl;
    std::cout << std::endl;

    run();
}

void RhythmGame::stop() {
    gameRunning = false;

    // 콘솔 커서 보이기
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    system("cls");
    std::cout << "게임 종료!" << std::endl;
    std::cout << "최종 점수: " << score << std::endl;
    std::cout << "최대 콤보: " << maxCombo << std::endl;
    std::cout << "놓친 노트: " << missCount << std::endl;
}

void RhythmGame::run() {
    auto lastSpawnTime = std::chrono::steady_clock::now();
    auto lastUpdateTime = std::chrono::steady_clock::now();

    while (gameRunning) {
        auto currentTime = std::chrono::steady_clock::now();

        // 노트 생성
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSpawnTime).count() > SPAWN_RATE_MS) {
            spawnNote();
            lastSpawnTime = currentTime;
        }

        // 노트 업데이트
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count() > NOTE_SPEED_MS) {
            updateNotes();
            lastUpdateTime = currentTime;
        }

        // 키 입력 처리
        processInput();

        // 화면 그리기
        drawScreen();

        // CPU 사용량 줄이기
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void RhythmGame::spawnNote() {
    if (!gameRunning) return;

    // 랜덤 레인 선택
    std::uniform_int_distribution<> dis(0, NOTE_LANES - 1);
    int lane = dis(gen);

    // 새 노트 생성
    notes.emplace_back(lane);
}

void RhythmGame::updateNotes() {
    for (auto& note : notes) {
        if (note.active && !note.hit) {
            // 노트 이동
            note.y++;

            // 히트 영역 벗어남 체크
            if (note.y > SCREEN_HEIGHT) {
                note.active = false;
                combo = 0;
                missCount++;

                // 너무 많은 노트를 놓치면 게임 종료
                if (missCount >= 20) {
                    stop();
                }
            }
        }
    }
}

void RhythmGame::drawScreen() {
    // 화면 초기화
    initScreen();

    // 노트 그리기
    for (const auto& note : notes) {
        if (note.active && !note.hit && note.y < SCREEN_HEIGHT) {
            int laneWidth = SCREEN_WIDTH / NOTE_LANES;
            int noteX = note.lane * laneWidth + laneWidth / 2;
            gameScreen[note.y][noteX] = '*';
        }
    }

    // 화면 출력
    setCursorPosition(0, 3);

    // 점수와 콤보 표시
    std::cout << "점수: " << score << " | 콤보: " << combo << "   " << std::endl;

    // 게임 화면 출력
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

        // 소문자로 변환
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

    // 히트 영역 안에 있는 노트 찾기
    for (auto& note : notes) {
        if (note.active && !note.hit && note.lane == lane) {
            // 히트 영역에 있는지 확인 (오차 범위 +/- 1)
            if (abs(note.y - HIT_ZONE_Y) <= 1) {
                note.hit = true;
                hitSuccess = true;

                // 정확도에 따른 점수 계산
                int accuracy = abs(note.y - HIT_ZONE_Y);

                if (accuracy == 0) {
                    // 퍼펙트
                    addScore(100);
                }
                else {
                    // 좋음
                    addScore(50);
                }

                break;
            }
        }
    }

    // 히트 실패시 콤보 초기화
    if (!hitSuccess) {
        combo = 0;
    }
}

void RhythmGame::addScore(int points) {
    combo++;
    if (combo > maxCombo) {
        maxCombo = combo;
    }

    // 콤보 보너스
    int comboBonus = (combo / 10) * 10;
    score += points + comboBonus;
}