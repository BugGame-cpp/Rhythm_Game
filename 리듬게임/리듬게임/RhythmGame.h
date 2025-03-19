#pragma once

#include <vector>
#include <random>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>
#include <windows.h>

// ���� ���
const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 20;
const int NOTE_LANES = 4;
const int HIT_ZONE_Y = SCREEN_HEIGHT - 2;
const int NOTE_SPEED_MS = 200; // ��Ʈ �̵� �ӵ�(�и���)
const int SPAWN_RATE_MS = 1000; // ��Ʈ ���� �ӵ�(�и���)

// Ű �ڵ�
const char KEY_A = 'a';
const char KEY_S = 's';
const char KEY_D = 'd';
const char KEY_F = 'f';
const char KEY_Q = 'q'; // ���� Ű

// ��Ʈ Ŭ����
struct Note {
    int lane;
    int y;
    bool active;
    bool hit;

    Note(int _lane) : lane(_lane), y(0), active(true), hit(false) {}
};

// ���� Ŭ����
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

    // ��Ʈ ���� �Լ�
    void spawnNote();

    // ��Ʈ ������Ʈ �Լ�
    void updateNotes();

    // ȭ�� �ʱ�ȭ �Լ�
    void initScreen();

    // ȭ�� �׸��� �Լ�
    void drawScreen();

    // Ű �Է� ó�� �Լ�
    void processInput();

    // ��Ʈ üũ �Լ�
    void checkHit(int lane);

    // ���� �߰� �Լ�
    void addScore(int points);

    // �ܼ� Ŀ�� �̵� �Լ�
    void setCursorPosition(int x, int y);

public:
    RhythmGame();
    ~RhythmGame();

    // ���� ���� �Լ�
    void start();

    // ���� ���� �Լ�
    void stop();

    // ���� �ʱ�ȭ �Լ�
    void init();

    // ���� ���� �Լ�
    void run();
};