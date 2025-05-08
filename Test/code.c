#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#define ID_TIMER 1
#define ID_SETALARM 2

HWND hAlarmHour, hAlarmMin, hAlarmLabel;
int alarmHour = -1, alarmMin = -1;

void ShowAlarmMessage() {
    MessageBox(NULL, "알람 시간이 되었습니다!", "알람", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hSetBtn;
    char buf[64];

    switch (msg) {
    case WM_CREATE:
        CreateWindow("STATIC", "알람 시 (0-23):", WS_VISIBLE | WS_CHILD,
            20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hAlarmHour = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            120, 20, 40, 20, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "분 (0-59):", WS_VISIBLE | WS_CHILD,
            180, 20, 70, 20, hwnd, NULL, NULL, NULL);
        hAlarmMin = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            250, 20, 40, 20, hwnd, NULL, NULL, NULL);

        hSetBtn = CreateWindow("BUTTON", "알람 설정", WS_VISIBLE | WS_CHILD,
            310, 20, 80, 25, hwnd, (HMENU)ID_SETALARM, NULL, NULL);

        hAlarmLabel = CreateWindow("STATIC", "알람이 설정되지 않았습니다.", WS_VISIBLE | WS_CHILD,
            20, 60, 300, 20, hwnd, NULL, NULL, NULL);

        SetTimer(hwnd, ID_TIMER, 1000, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_SETALARM) {
            char hourStr[8], minStr[8];
            GetWindowText(hAlarmHour, hourStr, sizeof(hourStr));
            GetWindowText(hAlarmMin, minStr, sizeof(minStr));
            alarmHour = atoi(hourStr);
            alarmMin = atoi(minStr);
            if (alarmHour < 0 || alarmHour > 23 || alarmMin < 0 || alarmMin > 59) {
                MessageBox(hwnd, "올바른 시간을 입력하세요.", "오류", MB_OK | MB_ICONERROR);
                alarmHour = alarmMin = -1;
                SetWindowText(hAlarmLabel, "알람이 설정되지 않았습니다.");
            } else {
                sprintf(buf, "알람이 %02d:%02d 에 설정되었습니다.", alarmHour, alarmMin);
                SetWindowText(hAlarmLabel, buf);
            }
        }
        break;

    case WM_TIMER: {
        if (alarmHour != -1 && alarmMin != -1) {
            time_t t = time(NULL);
            struct tm *now = localtime(&t);
            if (now->tm_hour == alarmHour && now->tm_min == alarmMin && now->tm_sec == 0) {
                ShowAlarmMessage();
                alarmHour = alarmMin = -1;
                SetWindowText(hAlarmLabel, "알람이 설정되지 않았습니다.");
            }
        }
        break;
    }

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "AlarmApp";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("AlarmApp", "C 알람 시계", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        100, 100, 420, 150, NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}