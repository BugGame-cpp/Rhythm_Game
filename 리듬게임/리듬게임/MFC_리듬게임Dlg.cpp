// MFC_리듬게임Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC_리듬게임.h"
#include "MFC_리듬게임Dlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Note 클래스 구현
Note::Note(int lane, double timing)
    : m_Lane(lane), m_Timing(timing), m_Position(0), m_Hit(false)
{
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    // 구현입니다.
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_START, &CMFC리듬게임Dlg::OnBnClickedButtonStart)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SELECT_MUSIC, &CMFC리듬게임Dlg::OnBnClickedButtonSelectMusic)

END_MESSAGE_MAP()

// CMFC리듬게임Dlg 대화 상자

CMFC리듬게임Dlg::CMFC리듬게임Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MFC__DIALOG, pParent)
    , m_GameTime(0.0)
    , m_Score(0)
    , m_GameStarted(false)
    , m_LastJudgment(Judgment::NONE)
    , m_JudgmentShowTime(0.0)
    , m_Combo(0)
    , m_MaxCombo(0)
    , m_MusicPlaying(false)
    , m_MusicDuration(0.0)
    , m_ComboAnimTime(0.0)
    , m_ComboAnimDuration(0.3)
    , m_LastAnimatedCombo(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_MusicPath = _T("");
}



void CMFC리듬게임Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC리듬게임Dlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_START, &CMFC리듬게임Dlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CMFC리듬게임Dlg 메시지 처리기

BOOL CMFC리듬게임Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 대화상자 크기 설정
    SetWindowPos(NULL, 0, 0, 800, 600, SWP_NOMOVE | SWP_NOZORDER);

    // 시작 버튼 생성 및 스타일 설정
    CButton* pButton = new CButton();
    pButton->Create(_T("게임 시작"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        CRect(350, 500, 450, 530), this, IDC_BUTTON_START);


    // 버튼 폰트 설정
    CFont* buttonFont = new CFont();
    buttonFont->CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

    // 노래 선택 버튼 생성
    CComboBox* pCombo = new CComboBox();
    pCombo->Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        CRect(260, 0, 420, 495), this, IDC_COMBO_SONG_LIST);

    // 노래 목록 추가
    pCombo->AddString(_T("LOVE DIVE"));
    pCombo->AddString(_T("Whiplash"));
    pCombo->AddString(_T("HAPPY"));
    pCombo->AddString(_T("Carly Rae Jepsen - Call Me Maybe"));
    pCombo->AddString(_T("Dont Look Back In Anger"));
    pCombo->AddString(_T("Shape Of You"));
    pCombo->AddString(_T("Viva la Vida"));

    // 기본 선택
    pCombo->SetCurSel(0);
    CComboBox* pDiffCombo = new CComboBox();
    pDiffCombo->Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        CRect(440, 0, 560, 495), this, 1004);  // ID: 1003 (새로 부여)

    pDiffCombo->AddString(_T("Easy"));
    pDiffCombo->AddString(_T("Normal"));
    pDiffCombo->AddString(_T("Hard"));
    pDiffCombo->SetCurSel(1); // 기본 선택: Normal

    // 게임 초기화
    InitGame();

    return TRUE;
}


void CMFC리듬게임Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CMFC리듬게임Dlg::OnPaint()
{
    if (IsIconic())
    {
        // (기존 아이콘 그리기 코드 유지)
    }
    else
    {
        CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

        // 더블 버퍼링을 위한 메모리 DC 생성
        CDC memDC;
        memDC.CreateCompatibleDC(&dc);

        CRect rect;
        GetClientRect(&rect);

        CBitmap memBitmap;
        memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

        CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

        // 게임 화면 그리기
        DrawGame(&memDC);

        // 메모리 DC에서 화면으로 복사
        dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

        // 리소스 해제
        memDC.SelectObject(pOldBitmap);
    }
}

HCURSOR CMFC리듬게임Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// 키보드 입력 처리
BOOL CMFC리듬게임Dlg::PreTranslateMessage(MSG* pMsg)
{
    // 키보드 메시지 처리
    if (pMsg->message == WM_KEYDOWN && m_GameStarted)
    {
        switch (pMsg->wParam)
        {
        case 'A': ProcessKeyInput(0); return TRUE;
        case 'S': ProcessKeyInput(1); return TRUE;
        case 'D': ProcessKeyInput(2); return TRUE;
        case 'F': ProcessKeyInput(3); return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

// 타이머 처리
void CMFC리듬게임Dlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 && m_GameStarted)
    {
        // 음악 위치 확인
        if (m_MusicPlaying)
        {
            double position = GetMusicPosition();

            // 음악이 끝났는지 확인
            if (position >= m_MusicDuration - 0.5)  // 0.5초 버퍼
            {
                // 게임 종료
                KillTimer(1);
                StopMusic();
                m_GameStarted = false;

                // 버튼 텍스트 변경
                CButton* pButton = (CButton*)GetDlgItem(IDC_BUTTON_START);
                if (pButton)
                {
                    pButton->SetWindowText(_T("게임 시작"));
                }

                // 결과 메시지 표시
                CString resultMsg;
                resultMsg.Format(_T("게임 종료!\n최종 점수: %d\n최대 콤보: %d"), m_Score, m_MaxCombo);
                AfxMessageBox(resultMsg);

                // 화면 갱신
                Invalidate(FALSE);
                return;
            }

            // 게임 시간을 음악 위치와 동기화
            m_GameTime = position;
        }
        else
        {
            // 음악이 재생되지 않으면 직접 시간 계산
            m_GameTime += 0.016;  // 약 16ms
        }

        // 노트 위치 업데이트
        UpdateNotes();

        // 화면 다시 그리기
        Invalidate(FALSE);
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CMFC리듬게임Dlg::OnBnClickedButtonStart()
{
    // 콤보박스에서 현재 선택된 곡 가져오기
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SONG_LIST);
    int sel = pCombo->GetCurSel();
    CString songName;
    pCombo->GetLBText(sel, songName);

    // 선택된 노래에 따라 파일 경로 설정
    if (songName == _T("LOVE DIVE"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\LOVE DIVE.wav");
    else if (songName == _T("Whiplash"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\Whiplash.wav");
    else if (songName == _T("HAPPY"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\HAPPY.wav");
    else if (songName == _T("Call Me Maybe"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\Call Me Maybe.wav");
    else if (songName == _T("Dont Look Back In Anger"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\Dont Look Back In Anger.wav");
    else if (songName == _T("Shape Of You"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\Shape Of You.wav");
    else if (songName == _T("Viva la Vida"))
        m_MusicPath = _T("C:\\Users\\user\\Desktop\\MFC_RhythmGame\\Viva la Vida.wav");

    // 난이도 콤보박스에서 선택값 읽기
    CComboBox* pDiffCombo = (CComboBox*)GetDlgItem(1004);
    int diffSel = pDiffCombo->GetCurSel();

    if (diffSel == 0)      m_Difficulty = Difficulty::EASY;
    else if (diffSel == 1) m_Difficulty = Difficulty::NORMAL;
    else if (diffSel == 2) m_Difficulty = Difficulty::HARD;

    // 기존 로직 유지
    if (!m_GameStarted)
    {
        m_MusicDuration = GetMusicDuration(m_MusicPath);

        InitGame();
        if (PlayMusic(m_MusicPath))
        {
            SetTimer(1, 16, NULL);
            m_GameStarted = true;
        }
        else
        {
            AfxMessageBox(_T("음악 파일을 로드할 수 없습니다."));
        }
    }
    else
    {
        KillTimer(1);
        StopMusic();
        m_GameStarted = false;
    }

    // 버튼 텍스트 변경
    CButton* pButton = (CButton*)GetDlgItem(IDC_BUTTON_START);
    if (pButton)
        pButton->SetWindowText(m_GameStarted ? _T("게임 중지") : _T("게임 시작"));
}


// 게임 초기화
void CMFC리듬게임Dlg::InitGame()
{
    // 변수 초기화
    m_GameTime = 0.0;
    m_Score = 0;
    m_LastJudgment = Judgment::NONE;
    m_JudgmentShowTime = 0.0;
    m_Combo = 0;
    m_MaxCombo = 0;
    m_MusicPlaying = false;

    // 콤보 애니메이션 초기화 (이 부분 추가)
    m_ComboAnimTime = 0.0;
    m_LastAnimatedCombo = 0;

    // 배경 별 초기화
    CRect rect;
    GetClientRect(&rect);
    m_MaxStars = 100;       // 시작 시 별 100개
    m_StarColorPhase = 0;   // 색상 단계 초기화

    // 초기 별 생성
    m_Stars.clear();
    for (int i = 0; i < m_MaxStars; i++) {
        Star star;
        star.x = rand() % rect.Width();
        star.y = rand() % rect.Height();
        star.size = rand() % 2 + 1;
        star.brightness = rand() % 155 + 100;  // 100~255 사이 밝기
        star.color = RGB(255, 255, 255);       // 기본 흰색
        m_Stars.push_back(star);
    }
    // 노트 데이터 로드
    LoadNotes();
}
// 노트 데이터 로드
void CMFC리듬게임Dlg::LoadNotes()
{
    m_Notes.clear();
    double startOffset = 2.0; // 음악 시작 후 몇 초부터 노트 생성
    int laneCount = 4;

    double interval;
    int maxNotesPerBeat;

    // 난이도에 따라 생성 패턴 달리함
    switch (m_Difficulty)
    {
    case Difficulty::EASY:
        interval = 1.5;
        maxNotesPerBeat = 1;
        break;
    case Difficulty::NORMAL:
        interval = 1.0;
        maxNotesPerBeat = 2;
        break;
    case Difficulty::HARD:
        interval = 0.5;
        maxNotesPerBeat = 3;
        break;
    }

    for (double t = startOffset; t < m_MusicDuration - 1.0; t += interval)
    {
        int notesThisTime = rand() % maxNotesPerBeat + 1;

        // 중복 없는 레인 번호 무작위 선택
        std::vector<int> lanes = { 0, 1, 2, 3 };
        std::random_shuffle(lanes.begin(), lanes.end());

        for (int i = 0; i < notesThisTime; i++)
        {
            m_Notes.push_back(Note(lanes[i], t));
        }
    }
}


// 노트 위치 업데이트
void CMFC리듬게임Dlg::UpdateNotes()
{
    for (auto& note : m_Notes)
    {
        if (!note.IsHit())
        {
            // 노트의 Y 위치 계산 (시간에 따라 아래로 이동)
            double timeDiff = note.GetTiming() - m_GameTime;

            // 노트가 화면에 보이기 시작할 시간 차이
            double visibleTimeDiff = 3.0;  // 3초 전부터 노트 표시

            if (timeDiff <= visibleTimeDiff)
            {
                // 노트 위치 계산 (판정선까지의 거리를 시간으로 나누어 계산)
                double position = JUDGMENT_LINE_Y - (timeDiff / visibleTimeDiff) * (JUDGMENT_LINE_Y - 50);
                note.SetPosition(position);

                // 노트가 판정선을 지나쳤는지 확인
                if (timeDiff < -0.2)  // 0.2초 이상 지나면 자동으로 미스 처리
                {
                    note.SetHit(true);
                    m_LastJudgment = Judgment::MISS;
                    m_JudgmentShowTime = m_GameTime + 0.5;  // 0.5초간 판정 표시
                }
            }
        }
    }
    // 히트 이펙트 업데이트
    for (auto it = m_HitEffects.begin(); it != m_HitEffects.end(); ) {
        if (m_GameTime - it->startTime > it->duration) {
            it = m_HitEffects.erase(it);
        }
        else {
            ++it;
        }
    }
    // 콤보에 따라 별 개수와 색상 업데이트
    CRect rect;
    GetClientRect(&rect);
    int targetStars = 100 + (m_Combo / 10) * 50;  // 10콤보마다 50개씩 추가
    targetStars = min(targetStars, 500);          // 최대 500개로 제한

    // 별 개수 조정
    if (m_Stars.size() < targetStars) {
        // 별 추가
        for (int i = m_Stars.size(); i < targetStars; i++) {
            Star star;
            star.x = rand() % rect.Width();
            star.y = rand() % rect.Height();
            star.size = rand() % 2 + 1;
            star.brightness = rand() % 155 + 100;

            // 콤보 단계에 따른 색상 결정
            int colorPhase = m_Combo / 10;
            switch (colorPhase % 6) {
            case 0: star.color = RGB(255, 255, 255); break; // 흰색
            case 1: star.color = RGB(255, 200, 200); break; // 연한 빨강
            case 2: star.color = RGB(200, 255, 200); break; // 연한 초록
            case 3: star.color = RGB(200, 200, 255); break; // 연한 파랑
            case 4: star.color = RGB(255, 255, 200); break; // 연한 노랑
            case 5: star.color = RGB(255, 200, 255); break; // 연한 보라
            }

            m_Stars.push_back(star);
        }
    }

    // 콤보 단계가 변경되었을 때 색상 업데이트
    int currentColorPhase = m_Combo / 10;
    if (currentColorPhase != m_StarColorPhase) {
        m_StarColorPhase = currentColorPhase;

        // 모든 별의 색상을 새 단계에 맞게 서서히 변경
        COLORREF targetColor;
        switch (m_StarColorPhase % 6) {
        case 0: targetColor = RGB(255, 255, 255); break; // 흰색
        case 1: targetColor = RGB(255, 200, 200); break; // 연한 빨강
        case 2: targetColor = RGB(200, 255, 200); break; // 연한 초록
        case 3: targetColor = RGB(200, 200, 255); break; // 연한 파랑
        case 4: targetColor = RGB(255, 255, 200); break; // 연한 노랑
        case 5: targetColor = RGB(255, 200, 255); break; // 연한 보라
        }

        // 10%의 별만 한 번에 색상 변경 (점진적 변화 효과)
        int starsToChange = m_Stars.size() / 10;
        for (int i = 0; i < starsToChange; i++) {
            int idx = rand() % m_Stars.size();
            m_Stars[idx].color = targetColor;
        }
    }

    // 별 깜빡임 효과 (일부 별의 밝기 변경)
    for (int i = 0; i < m_Stars.size() / 20; i++) {  // 5%의 별만 업데이트
        int idx = rand() % m_Stars.size();
        m_Stars[idx].brightness = rand() % 155 + 100;  // 밝기 무작위 변경
    }
}

// 키 입력 처리
void CMFC리듬게임Dlg::ProcessKeyInput(int lane)
{
    // 해당 레인에서 가장 가까운 노트 찾기
    Note* closestNote = FindClosestNote(lane, m_GameTime);

    if (closestNote)
    {
        // 노트와 현재 시간의 차이 계산
        double timeDiff = std::abs(closestNote->GetTiming() - m_GameTime);

        // 판정
        Judgment judge = JudgeHit(closestNote->GetTiming(), m_GameTime);

        // 판정에 따른 점수 부여
        switch (judge)
        {
        case Judgment::PERFECT:
            m_Score += 100;
            m_Combo++;
            break;
        case Judgment::GREAT:
            m_Score += 80;
            m_Combo++;
            break;
        case Judgment::GOOD:
            m_Score += 50;
            m_Combo++;
            break;
        case Judgment::MISS:
            m_Combo = 0;  // 미스 시 콤보 초기화
            break;
        }
        // 콤보가 증가하면 애니메이션 시작 (이 부분 추가)
        if (m_Combo > m_LastAnimatedCombo) {
            m_ComboAnimTime = m_GameTime;
            m_LastAnimatedCombo = m_Combo;
        }

        // 최대 콤보 갱신
        if (m_Combo > m_MaxCombo)
            m_MaxCombo = m_Combo;

        // 노트 처리
        closestNote->SetHit(true);

        // 판정 결과 표시
        m_LastJudgment = judge;
        m_JudgmentShowTime = m_GameTime + 0.5;  // 0.5초간 판정 표시

        // 판정에 따른 점수 부여 부분 끝에 추가 (미스가 아닌 경우만 이펙트 추가)
        if (judge != Judgment::MISS) {
            // 히트 이펙트 추가
            HitEffect effect;
            effect.lane = lane;
            effect.startTime = m_GameTime;
            effect.duration = 0.2; // 0.2초로 짧게 설정
            effect.judgment = judge;
            m_HitEffects.push_back(effect);
        }
    }
}

// 판정 함수
Judgment CMFC리듬게임Dlg::JudgeHit(double noteTime, double pressTime)
{
    double diff = std::abs(noteTime - pressTime);

    if (diff < 0.05) return Judgment::PERFECT;
    else if (diff < 0.1) return Judgment::GREAT;
    else if (diff < 0.15) return Judgment::GOOD;
    else return Judgment::MISS;
}

// 가장 가까운 노트 찾기
Note* CMFC리듬게임Dlg::FindClosestNote(int lane, double time)
{
    Note* closestNote = nullptr;
    double minTimeDiff = 1.0;  // 판정 최대 시간 차이

    for (auto& note : m_Notes)
    {
        if (!note.IsHit() && note.GetLane() == lane)
        {
            double timeDiff = std::abs(note.GetTiming() - time);
            if (timeDiff < minTimeDiff)
            {
                minTimeDiff = timeDiff;
                closestNote = &note;
            }
        }
    }

    return closestNote;
}

// 게임 그리기
void CMFC리듬게임Dlg::DrawGame(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    // 배경 그리기
    DrawBackground(pDC, rect);

    // 레인 그리기
    DrawLanes(pDC, rect);

    // 판정선 그리기
    DrawJudgmentLine(pDC);

    // 노트 그리기
    DrawNotes(pDC);

    // 이펙트 그리기 (노트 위에 그려야 함)
    DrawHitEffects(pDC);

    // 점수 표시
    DrawScore(pDC);

    // 판정 결과 표시
    DrawJudgment(pDC);

    // 콤보 표시
    DrawCombo(pDC);
}

// 배경 그리기
void CMFC리듬게임Dlg::DrawBackground(CDC* pDC, const CRect& rect)
{
    // 그라데이션 배경 (어두운 파란색에서 검은색으로)
    // 콤보에 따라 배경 색상 약간 변경
    COLORREF bgTopColor, bgBottomColor;

    // 콤보 레벨에 따른 배경 색상 결정
    int comboLevel = m_Combo / 10;
    switch (comboLevel % 6) {
    case 0: // 기본
        bgTopColor = RGB(0, 0, 50);
        bgBottomColor = RGB(0, 0, 10);
        break;
    case 1: // 빨강 조금 추가
        bgTopColor = RGB(30, 0, 50);
        bgBottomColor = RGB(10, 0, 10);
        break;
    case 2: // 초록 조금 추가
        bgTopColor = RGB(0, 30, 50);
        bgBottomColor = RGB(0, 10, 10);
        break;
    case 3: // 파랑 강화
        bgTopColor = RGB(0, 0, 70);
        bgBottomColor = RGB(0, 0, 20);
        break;
    case 4: // 보라 느낌
        bgTopColor = RGB(20, 0, 60);
        bgBottomColor = RGB(10, 0, 15);
        break;
    case 5: // 청록 느낌
        bgTopColor = RGB(0, 20, 60);
        bgBottomColor = RGB(0, 10, 15);
        break;
    }

    // 그라데이션 그리기
    for (int y = 0; y < rect.Height(); y++) {
        double ratio = static_cast<double>(y) / rect.Height();
        int r = static_cast<int>(GetRValue(bgTopColor) * (1.0 - ratio) + GetRValue(bgBottomColor) * ratio);
        int g = static_cast<int>(GetGValue(bgTopColor) * (1.0 - ratio) + GetGValue(bgBottomColor) * ratio);
        int b = static_cast<int>(GetBValue(bgTopColor) * (1.0 - ratio) + GetBValue(bgBottomColor) * ratio);

        pDC->FillSolidRect(0, y, rect.Width(), 1, RGB(r, g, b));
    }

    // 별 그리기 - 기존의 랜덤 별 대신 저장된 별 사용
    for (const auto& star : m_Stars) {
        // 별 밝기에 따른 색상 조정
        COLORREF starColor = RGB(
            min(255, (GetRValue(star.color) * star.brightness) / 255),
            min(255, (GetGValue(star.color) * star.brightness) / 255),
            min(255, (GetBValue(star.color) * star.brightness) / 255)
        );

        // 별 크기에 따라 다르게 그리기
        if (star.size == 1) {
            pDC->SetPixel(star.x, star.y, starColor);
        }
        else {
            pDC->FillSolidRect(star.x, star.y, star.size, star.size, starColor);

            // 큰 별은 십자 형태로 빛을 추가
            if (star.size >= 2) {
                int glow = star.brightness / 2; // 약한 빛 효과
                COLORREF glowColor = RGB(
                    min(255, (GetRValue(star.color) * glow) / 255),
                    min(255, (GetGValue(star.color) * glow) / 255),
                    min(255, (GetBValue(star.color) * glow) / 255)
                );

                pDC->SetPixel(star.x - 1, star.y, glowColor);
                pDC->SetPixel(star.x + star.size, star.y, glowColor);
                pDC->SetPixel(star.x, star.y - 1, glowColor);
                pDC->SetPixel(star.x, star.y + star.size, glowColor);
            }
        }
    }
}

void CMFC리듬게임Dlg::OnBnClickedButtonSelectMusic()
{
    CFileDialog fileDlg(TRUE, _T("wav"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("음악 파일 (*.wav;*.mp3)|*.wav;*.mp3|모든 파일 (*.*)|*.*||"));

    if (fileDlg.DoModal() == IDOK)
    {
        m_MusicPath = fileDlg.GetPathName();
        AfxMessageBox(_T("선택된 노래: ") + m_MusicPath);
    }
}


// 레인 그리기
void CMFC리듬게임Dlg::DrawLanes(CDC* pDC, const CRect& rect)
{
    int centerX = rect.Width() / 2;
    int laneStartX = centerX - (LANE_COUNT * LANE_WIDTH) / 2;

    // 각 레인 그리기
    for (int i = 0; i < LANE_COUNT; i++)
    {
        int laneX = laneStartX + i * LANE_WIDTH;

        // 레인 배경 (그라데이션 효과)
        for (int y = 50; y <= JUDGMENT_LINE_Y; y++)
        {
            int intensity = 30 + (y - 50) * 20 / (JUDGMENT_LINE_Y - 50);
            COLORREF color = RGB(intensity / 3, intensity / 2, intensity);
            pDC->FillSolidRect(laneX, y, LANE_WIDTH, 1, color);
        }

        // 레인 구분선 (밝은 색상)
        pDC->FillSolidRect(laneX, 50, 1, JUDGMENT_LINE_Y - 50, RGB(80, 80, 100));
        pDC->FillSolidRect(laneX + LANE_WIDTH - 1, 50, 1, JUDGMENT_LINE_Y - 50, RGB(80, 80, 100));

        // 키 표시 (둥근 버튼 형태)
        CString keyText;
        switch (i)
        {
        case 0: keyText = _T("A"); break;
        case 1: keyText = _T("S"); break;
        case 2: keyText = _T("D"); break;
        case 3: keyText = _T("F"); break;
        }

        // 키 버튼 그리기
        CRect keyRect(laneX + 5, JUDGMENT_LINE_Y + 10, laneX + LANE_WIDTH - 5, JUDGMENT_LINE_Y + 40);
        pDC->FillSolidRect(keyRect, RGB(60, 60, 70));
        pDC->Draw3dRect(keyRect, RGB(120, 120, 140), RGB(40, 40, 60));

        // 텍스트 설정
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(255, 255, 255));

        // 키 글자 폰트 설정
        CFont font;
        font.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

        CFont* pOldFont = pDC->SelectObject(&font);
        pDC->DrawText(keyText, keyRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);
    }
}

// 판정선 그리기
void CMFC리듬게임Dlg::DrawJudgmentLine(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    int centerX = rect.Width() / 2;
    int laneStartX = centerX - (LANE_COUNT * LANE_WIDTH) / 2;
    int laneEndX = laneStartX + LANE_COUNT * LANE_WIDTH;

    // 판정선 그리기 (빛나는 효과)
    int glowSize = 3;
    for (int i = -glowSize; i <= glowSize; i++)
    {
        int alpha = 255 - abs(i) * 50;
        COLORREF color = RGB(alpha, alpha, 255);
        pDC->FillSolidRect(laneStartX, JUDGMENT_LINE_Y + i, laneEndX - laneStartX, 1, color);
    }

    // 중앙 판정선 (밝은 색상)
    pDC->FillSolidRect(laneStartX, JUDGMENT_LINE_Y, laneEndX - laneStartX, 2, RGB(255, 255, 255));
}

// 노트 그리기
void CMFC리듬게임Dlg::DrawNotes(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    int centerX = rect.Width() / 2;
    int laneStartX = centerX - (LANE_COUNT * LANE_WIDTH) / 2;

    for (const auto& note : m_Notes)
    {
        if (!note.IsHit())
        {
            double timeDiff = note.GetTiming() - m_GameTime;

            // 노트가 화면에 보이기 시작할 시간 차이
            double visibleTimeDiff = 3.0;

            if (timeDiff <= visibleTimeDiff && timeDiff > -0.2)
            {
                int laneX = laneStartX + note.GetLane() * LANE_WIDTH;
                int noteY = static_cast<int>(note.GetPosition());

                // 노트 모양 (둥근 형태)
                int cornerRadius = 5;
                CRect noteRect(laneX + 10, noteY - NOTE_HEIGHT / 2, laneX + LANE_WIDTH - 10, noteY + NOTE_HEIGHT / 2);

                // 노트 색상 결정 (각 레인마다 다른 색상)
                COLORREF noteColor;
                switch (note.GetLane())
                {
                case 0: noteColor = RGB(255, 100, 100); break; // 빨강
                case 1: noteColor = RGB(100, 255, 100); break; // 초록
                case 2: noteColor = RGB(100, 100, 255); break; // 파랑
                case 3: noteColor = RGB(255, 255, 100); break; // 노랑
                default: noteColor = RGB(200, 200, 200); break;
                }

                // 노트 그림자 효과
                CRect shadowRect = noteRect;
                shadowRect.OffsetRect(2, 2);
                pDC->FillSolidRect(shadowRect, RGB(0, 0, 0));

                // 노트 내부 채우기
                pDC->FillSolidRect(noteRect, noteColor);

                // 노트 밝은 부분 (하이라이트 효과)
                CRect highlightRect(noteRect.left, noteRect.top, noteRect.right, noteRect.top + 3);
                pDC->FillSolidRect(highlightRect, RGB(255, 255, 255));

                // 노트 테두리
                pDC->Draw3dRect(noteRect, RGB(255, 255, 255), RGB(150, 150, 150));
            }
        }
    }
}


// 점수 표시
void CMFC리듬게임Dlg::DrawScore(CDC* pDC)
{
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(255, 255, 255));

    // 점수 배경 (둥근 모서리 패널)
    CRect scorePanel(10, 10, 150, 50);
    pDC->FillSolidRect(scorePanel, RGB(50, 50, 70));
    pDC->Draw3dRect(scorePanel, RGB(100, 100, 120), RGB(30, 30, 50));

    // 점수 텍스트
    CString scoreStr;
    scoreStr.Format(_T("점수: %d"), m_Score);

    // 폰트 설정
    CFont font;
    font.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

    CFont* pOldFont = pDC->SelectObject(&font);
    pDC->DrawText(scoreStr, scorePanel, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pDC->SelectObject(pOldFont);
}

// 판정 결과 표시
void CMFC리듬게임Dlg::DrawJudgment(CDC* pDC)
{
    if (m_LastJudgment != Judgment::NONE && m_GameTime < m_JudgmentShowTime)
    {
        pDC->SetBkMode(TRANSPARENT);

        CString judgmentStr;
        COLORREF judgmentColor;

        switch (m_LastJudgment)
        {
        case Judgment::PERFECT:
            judgmentStr = _T("PERFECT");
            judgmentColor = RGB(255, 215, 0); // 골드
            break;
        case Judgment::GREAT:
            judgmentStr = _T("GREAT");
            judgmentColor = RGB(0, 255, 0); // 초록
            break;
        case Judgment::GOOD:
            judgmentStr = _T("GOOD");
            judgmentColor = RGB(0, 150, 255); // 파랑
            break;
        case Judgment::MISS:
            judgmentStr = _T("MISS");
            judgmentColor = RGB(255, 0, 0); // 빨강
            break;
        }

        CRect rect;
        GetClientRect(&rect);

        int centerX = rect.Width() / 2;
        CRect judgmentRect(centerX - 100, JUDGMENT_LINE_Y - 80, centerX + 100, JUDGMENT_LINE_Y - 40);

        // 판정 텍스트에 빛나는 효과 추가
        for (int i = -2; i <= 2; i++)
        {
            for (int j = -2; j <= 2; j++)
            {
                if (i != 0 || j != 0)
                {
                    CRect glowRect = judgmentRect;
                    glowRect.OffsetRect(i, j);

                    // 폰트 설정
                    CFont glowFont;
                    glowFont.CreateFont(35, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

                    CFont* pOldFont = pDC->SelectObject(&glowFont);
                    pDC->SetTextColor(RGB(50, 50, 50)); // 그림자 색상
                    pDC->DrawText(judgmentStr, glowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    pDC->SelectObject(pOldFont);
                }
            }
        }

        // 메인 텍스트
        CFont font;
        font.CreateFont(35, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

        CFont* pOldFont = pDC->SelectObject(&font);
        pDC->SetTextColor(judgmentColor);
        pDC->DrawText(judgmentStr, judgmentRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);
    }
}
void CMFC리듬게임Dlg::DrawCombo(CDC* pDC)
{
    if (m_Combo > 0)
    {
        CRect rect;
        GetClientRect(&rect);

        // 콤보 표시 위치
        int rightX = rect.Width() - 20;
        int middleY = rect.Height() / 2;

        // 패널 배경 (더 크게 설정하여 두 텍스트를 충분히 담음)
        CRect panelRect(rightX - 140, middleY - 30, rightX, middleY + 40);
        pDC->FillSolidRect(panelRect, RGB(30, 30, 50));
        pDC->Draw3dRect(panelRect, RGB(70, 70, 90), RGB(20, 20, 40));

        // 콤보 숫자 영역 (상단에 배치)
        CRect comboRect(rightX - 140, middleY - 30, rightX, middleY);

        // 콤보 텍스트
        CString comboStr;
        comboStr.Format(_T("%d COMBO"), m_Combo);

        // 애니메이션 진행 상태 계산
        double animProgress = 1.0;

        if (m_GameTime - m_ComboAnimTime < m_ComboAnimDuration) {
            double progress = (m_GameTime - m_ComboAnimTime) / m_ComboAnimDuration;
            animProgress = 1.0 + 0.5 * sin(progress * 3.14159);
        }

        // 콤보 값에 따라 색상 변화
        COLORREF comboColor;
        if (m_Combo >= 50)
            comboColor = RGB(255, 50, 50);
        else if (m_Combo >= 30)
            comboColor = RGB(255, 100, 0);
        else if (m_Combo >= 10)
            comboColor = RGB(255, 160, 0);
        else
            comboColor = RGB(255, 200, 100);

        // 폰트 설정 - 애니메이션에 따라 크기 조절
        int fontSize = static_cast<int>(25 * animProgress);

        CFont font;
        font.CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

        CFont* pOldFont = pDC->SelectObject(&font);
        pDC->SetTextColor(comboColor);
        pDC->SetBkMode(TRANSPARENT);

        // 움직임 효과
        if (m_GameTime - m_ComboAnimTime < m_ComboAnimDuration) {
            double offsetY = -3.0 * sin((m_GameTime - m_ComboAnimTime) / m_ComboAnimDuration * 3.14159);
            comboRect.OffsetRect(0, static_cast<int>(offsetY));
        }

        pDC->DrawText(comboStr, comboRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 추가 텍스트를 위한 구분선 추가 (선택 사항)
        if (m_Combo >= 10) {
            int lineY = middleY + 5;  // 중앙보다 약간 아래
            pDC->FillSolidRect(panelRect.left + 10, lineY, panelRect.Width() - 20, 1, RGB(60, 60, 80));
        }

        // 콤보 추가 텍스트 영역 (하단에 별도 배치)
        if (m_Combo >= 10) {
            CFont smallFont;
            smallFont.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

            pDC->SelectObject(&smallFont);

            CString bonusText;
            if (m_Combo >= 50)
                bonusText = _T("EXCELLENT!");
            else if (m_Combo >= 30)
                bonusText = _T("GREAT COMBO!");
            else if (m_Combo >= 10)
                bonusText = _T("NICE COMBO!");

            // 추가 텍스트 영역을 아래쪽에 별도 배치
            CRect bonusRect(rightX - 140, middleY + 10, rightX, middleY + 35);

            // 추가 텍스트 색상 (콤보 색상보다 약간 더 밝게)
            COLORREF bonusColor = RGB(
                min(255, GetRValue(comboColor) + 40),
                min(255, GetGValue(comboColor) + 40),
                min(255, GetBValue(comboColor) + 40)
            );

            pDC->SetTextColor(bonusColor);
            pDC->DrawText(bonusText, bonusRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        pDC->SelectObject(pOldFont);
    }
}

void CMFC리듬게임Dlg::DrawHitEffects(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    int centerX = rect.Width() / 2;
    int laneStartX = centerX - (LANE_COUNT * LANE_WIDTH) / 2;

    for (const auto& effect : m_HitEffects) {
        double progress = (m_GameTime - effect.startTime) / effect.duration;
        if (progress < 1.0) {
            int laneX = laneStartX + effect.lane * LANE_WIDTH;
            int laneCenter = laneX + LANE_WIDTH / 2;

            // 판정에 따른 색상 결정 - 흰색 성분 없이 순수 색상만 사용
            COLORREF color;
            switch (effect.judgment) {
            case Judgment::PERFECT: color = RGB(180, 80, 200); break;  // 보라색
            case Judgment::GREAT:   color = RGB(50, 180, 200); break;  // 청록색
            case Judgment::GOOD:    color = RGB(70, 120, 200); break;  // 파랑색
            default:                color = RGB(150, 150, 150); break; // 회색
            }

            // 효과 크기 계산
            double sizeProgress = sin(progress * 3.14159);
            int maxSize = LANE_WIDTH * 1.5;
            int size = static_cast<int>(maxSize * sizeProgress);

            // 원형 효과 - 밝은 흰색 효과 대신 색상 그라데이션 사용
            for (int i = 0; i < 3; i++) {
                int circleSize = size - i * 10;
                if (circleSize > 0) {
                    // 투명도 효과 (점점 사라지게)
                    int intensity = static_cast<int>((1.0 - progress) * 255);

                    // 각 원마다 색상 조절 (밝은 흰색 대신 색상의 밝기만 약간 조절)
                    int r = GetRValue(color) + (2 - i) * 20;
                    int g = GetGValue(color) + (2 - i) * 20;
                    int b = GetBValue(color) + (2 - i) * 20;

                    r = min(255, r);
                    g = min(255, g);
                    b = min(255, b);

                    COLORREF circleColor = RGB(r, g, b);

                    // 테두리와 내부 채우기 모두 동일 색상 사용
                    CPen pen(PS_SOLID, 1, circleColor);
                    CPen* pOldPen = pDC->SelectObject(&pen);

                    CBrush brush(circleColor);
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    // 원 그리기 (내부 채우기)
                    pDC->Ellipse(
                        laneCenter - circleSize / 2, JUDGMENT_LINE_Y - circleSize / 2,
                        laneCenter + circleSize / 2, JUDGMENT_LINE_Y + circleSize / 2
                    );

                    pDC->SelectObject(pOldPen);
                    pDC->SelectObject(pOldBrush);
                }
            }

            // 빛줄기 효과는 기존 유지하되 색상만 변경
            CPen lightPen(PS_SOLID, 2, color);
            CPen* pOldPen = pDC->SelectObject(&lightPen);

            int lightLength = static_cast<int>(LANE_WIDTH * 1.0 * sizeProgress);
            int lightWidth = static_cast<int>(LANE_WIDTH * 0.6 * (1.0 - progress));

            // 위쪽 빛줄기
            pDC->MoveTo(laneCenter - lightWidth / 2, JUDGMENT_LINE_Y);
            pDC->LineTo(laneCenter, JUDGMENT_LINE_Y - lightLength);
            pDC->LineTo(laneCenter + lightWidth / 2, JUDGMENT_LINE_Y);

            // 아래쪽 빛줄기
            pDC->MoveTo(laneCenter - lightWidth / 2, JUDGMENT_LINE_Y);
            pDC->LineTo(laneCenter, JUDGMENT_LINE_Y + lightLength);
            pDC->LineTo(laneCenter + lightWidth / 2, JUDGMENT_LINE_Y);

            pDC->SelectObject(pOldPen);
        }
    }
}


bool CMFC리듬게임Dlg::PlayMusic(const CString& filePath)
{
    // 이미 재생 중인 음악 중지
    StopMusic();

    // MCI 명령 형식으로 재생
    CString command;
    command.Format(_T("open \"%s\" type mpegvideo alias BGM"), filePath);

    if (mciSendString(command, NULL, 0, NULL) != 0)
        return false;

    if (mciSendString(_T("play BGM"), NULL, 0, NULL) != 0)
    {
        mciSendString(_T("close BGM"), NULL, 0, NULL);
        return false;
    }

    m_MusicPlaying = true;
    m_MusicPath = filePath;
    m_MusicDuration = GetMusicDuration(filePath);

    return true;
}

// 음악 중지 함수
void CMFC리듬게임Dlg::StopMusic()
{
    if (m_MusicPlaying)
    {
        mciSendString(_T("stop BGM"), NULL, 0, NULL);
        mciSendString(_T("close BGM"), NULL, 0, NULL);
        m_MusicPlaying = false;
    }
}

// 음악 길이 가져오기
double CMFC리듬게임Dlg::GetMusicDuration(const CString& filePath)
{
    // 임시로 파일을 열어 길이 확인
    CString command;
    command.Format(_T("open \"%s\" type mpegvideo alias TempBGM"), filePath);

    if (mciSendString(command, NULL, 0, NULL) != 0)
        return 0.0;

    TCHAR buffer[128] = { 0 };
    if (mciSendString(_T("status TempBGM length"), buffer, sizeof(buffer), NULL) != 0)
    {
        mciSendString(_T("close TempBGM"), NULL, 0, NULL);
        return 0.0;
    }

    double duration = _ttoi(buffer) / 1000.0; // ms를 초로 변환
    mciSendString(_T("close TempBGM"), NULL, 0, NULL);

    return duration;
}

// 현재 재생 위치 가져오기
double CMFC리듬게임Dlg::GetMusicPosition()
{
    if (!m_MusicPlaying)
        return 0.0;

    TCHAR buffer[128] = { 0 };
    if (mciSendString(_T("status BGM position"), buffer, sizeof(buffer), NULL) != 0)
        return 0.0;

    return _ttoi(buffer) / 1000.0; // ms를 초로 변환
}

// 대화 상자 종료 시 호출
void CMFC리듬게임Dlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // 음악 정리
    StopMusic();
}