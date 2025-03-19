#pragma once

#include <vector>
#include <random>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>
#include <windows.h>

// 게임 상수
const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 20;
const int NOTE_LANES = 4;
const int HIT_ZONE_Y = SCREEN_HEIGHT - 2;
const int NOTE_SPEED_MS = 200; // 노트 이동 속도(밀리초)
const int SPAWN_RATE_MS = 1000; // 노트 생성 속도(밀리초)

// 키 코드
const char KEY_A = 'a';
const char KEY_S = 's';
const char KEY_D = 'd';
const char KEY_F = 'f';
const char KEY_Q = 'q'; // 종료 키

// 노트 클래스
struct Note {
    int lane;
    int y;
    bool active;
    bool hit;

    Note(int _lane) : lane(_lane), y(0), active(true), hit(false) {}
};

// 게임 클래스
class RhythmGame {
private:
    std::vector<Note> notes;
    std::random_device rd;
    std::mt19937 gen;

    int score;
    int combo;
    int maxCombo;
    int missCount;
    bool gameRunning;

    char gameScreen[SCREEN_HEIGHT][SCREEN_WIDTH];

    // 노트 생성 함수
    void spawnNote();

    // 노트 업데이트 함수
    void updateNotes();

    // 화면 초기화 함수
    void initScreen();

    // 화면 그리기 함수
    void drawScreen();

    // 키 입력 처리 함수
    void processInput();

    // 히트 체크 함수
    void checkHit(int lane);

    // 점수 추가 함수
    void addScore(int points);

    // 콘솔 커서 이동 함수
    void setCursorPosition(int x, int y);

public:
    RhythmGame();
    ~RhythmGame();

    // 게임 시작 함수
    void start();

    // 게임 중지 함수
    void stop();

    // 게임 초기화 함수
    void init();

    // 게임 실행 함수
    void run();
};