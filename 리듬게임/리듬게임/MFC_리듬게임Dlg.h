// MFC_리듬게임Dlg.h: 헤더 파일
//

#pragma once
#include <vector>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 노트 클래스 정의
class Note {
private:
    int m_Lane;       // 노트가 떨어질 레인 (0~3)
    double m_Timing;  // 노트가 판정선에 도달하는 시간 (초)
    double m_Position; // 노트의 현재 Y 위치
    bool m_Hit;       // 노트가 처리되었는지 여부

public:
    Note(int lane, double timing);
    int GetLane() const { return m_Lane; }
    double GetTiming() const { return m_Timing; }
    double GetPosition() const { return m_Position; }
    void SetPosition(double pos) { m_Position = pos; }
    bool IsHit() const { return m_Hit; }
    void SetHit(bool value) { m_Hit = value; }
};

// 판정 열거형
enum class Judgment {
    NONE,
    MISS,
    GOOD,
    GREAT,
    PERFECT
};

// 노트 히트 이펙트를 위한 구조체
struct HitEffect {
    int lane;           // 이펙트가 표시될 레인
    double startTime;   // 이펙트 시작 시간
    double duration;    // 이펙트 지속 시간
    Judgment judgment;  // 판정 결과 (색상 결정에 사용)
};

// 배경 별을 위한 구조체
struct Star {
    int x, y;           // 별의 위치
    int size;           // 별의 크기
    int brightness;     // 별의 밝기
    COLORREF color;     // 별의 색상
};

// CMFC리듬게임Dlg 대화 상자
class CMFC리듬게임Dlg : public CDialogEx
{
    // 생성입니다.
public:
    CMFC리듬게임Dlg(CWnd* pParent = nullptr);    // 표준 생성자입니다.

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MFC__DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    // 구현입니다.
protected:
    HICON m_hIcon;

    // 생성된 메시지 맵 함수
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    // 키보드 입력 처리
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    // 게임 관련 변수
    std::vector<Note> m_Notes;      // 노트 벡터
    double m_GameTime;              // 게임 진행 시간
    int m_Score;                    // 현재 점수
    bool m_GameStarted;             // 게임 시작 여부
    Judgment m_LastJudgment;        // 마지막 판정 결과
    double m_JudgmentShowTime;      // 판정 표시 시간
    int m_Combo;                    // 현재 콤보 수
    int m_MaxCombo;                 // 최대 콤보 수


    //난이도
    enum class Difficulty {
        EASY,
        NORMAL,
        HARD
    };

    // 현재 난이도를 저장할 멤버 변수
    Difficulty m_Difficulty;

    // 그래픽 리소스
    CBitmap m_BackgroundBmp;        // 배경 이미지
    CBitmap m_NoteBmp;              // 노트 이미지
    CBitmap m_JudgmentLineBmp;      // 판정선 이미지

    // 게임 관련 상수
    const int LANE_COUNT = 4;       // 레인 수
    const int LANE_WIDTH = 80;      // 레인 너비
    const int NOTE_HEIGHT = 20;     // 노트 높이
    const double NOTE_SPEED = 200.0; // 노트 이동 속도 (픽셀/초)
    const int JUDGMENT_LINE_Y = 400; // 판정선 Y 위치

    // 음악 제어 함수
    bool PlayMusic(const CString& filePath);
    void StopMusic();
    double GetMusicDuration(const CString& filePath);
    double GetMusicPosition();

    // 그리기 관련 함수
    void DrawGame(CDC* pDC);
    void DrawBackground(CDC* pDC, const CRect& rect);
    void DrawLanes(CDC* pDC, const CRect& rect);
    void DrawJudgmentLine(CDC* pDC);
    void DrawNotes(CDC* pDC);
    void DrawScore(CDC* pDC);
    void DrawJudgment(CDC* pDC);
    void DrawCombo(CDC* pDC);       // 콤보 표시 함수 추가
    void DrawHitEffects(CDC* pDC); // 노트 히트 이펙트 그리기

    // 게임 로직 함수
    void InitGame();
    void LoadNotes();
    void UpdateNotes();
    void ProcessKeyInput(int lane);
    Judgment JudgeHit(double noteTime, double pressTime);
    Note* FindClosestNote(int lane, double time);

    // 음악 관련 변수 추가
    bool m_MusicPlaying;            // 음악 재생 여부
    double m_MusicDuration;         // 음악 총 길이
    CString m_MusicPath;            // 음악 파일 경로

    // 콤보 애니메이션 관련 변수 추가 (이 위치에 추가)
    double m_ComboAnimTime;       // 콤보 애니메이션 시작 시간
    double m_ComboAnimDuration;   // 콤보 애니메이션 지속 시간
    int m_LastAnimatedCombo;      // 마지막으로 애니메이션 된 콤보 값
    // 배경 효과 관련 변수
    std::vector<Star> m_Stars;      // 배경 별 목록
    int m_MaxStars;                 // 최대 별 개수
    int m_StarColorPhase;           // 별 색상 변화 단계
    // 이펙트 관련 변수
    std::vector<HitEffect> m_HitEffects;  // 히트 이펙트 목록

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonSelectMusic();


};