#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<limits.h>
#include "resource.h"

typedef struct
{
    int X;
    int Y;
    int Cluster;
} Point;

extern Point MyPoints[1024];
extern int PointNumber;
extern int ClusterNumber;

extern int KMeans_Algorithm(int P,int K);
extern int KMeans_GeneratePoints(int N);
extern void KMeans_Inital_RandomPartition(int P,int K);
extern void KMeans_Inital_Forgy(int P,int K);

/*  Declare Windows procedure  */

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
HWND hwnd1,hwnd2;
HMENU hMenu;

void (*InitialMethodPTR)(int P,int K);


int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) RGB(0,0,0);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;


    WNDCLASSEX wc = {0};
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.lpfnWndProc      = (WNDPROC) DialogProc;
    wc.hInstance        = hThisInstance;
    wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszClassName    = TEXT("DialogClass");
    RegisterClassEx(&wc);
    hwnd2=CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  TEXT("DialogClass"), TEXT("Set the value of K"),
                         WS_CAPTION , 100, 100, 200, 150,
                         NULL, NULL, hThisInstance,  NULL);
    /* The class is registered, let's create the program*/
    hwnd1 = CreateWindowEx (
                WS_EX_TOPMOST,                   /* Extended possibilites for variation */
                szClassName,         /* Classname */
                _T("K-Means Algorithm Demonstration Written By JSW"),       /* Title Text */
                WS_CAPTION|WS_SYSMENU, /* default window */
                CW_USEDEFAULT,       /* Windows decides the position */
                CW_USEDEFAULT,       /* where the window ends up on the screen */
                700,                 /* The programs width */
                700,                 /* and height in pixels */
                HWND_DESKTOP,        /* The window is a child-window to desktop */
                LoadMenu(hThisInstance,MAKEINTRESOURCE(IDR_MENU)),                /* No menu */
                hThisInstance,       /* Program Instance handler */
                NULL                 /* No Window Creation data */
            );

    /* Make the window visible on the screen */
    ShowWindow (hwnd1, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    HBRUSH hbr;
    RECT rt;
    char MyBuffer[8];
    static int I,J,K,L;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        InitialMethodPTR=KMeans_Inital_Forgy;
        hMenu=GetMenu(hwnd);
        break;
    case WM_PAINT:
        hdc=BeginPaint(hwnd,&ps);
        hbr=CreateSolidBrush(RGB(0,0,0));
        Rectangle(hdc,0,0,700,700);
        DeleteObject(hbr);
        for (I=0; I<PointNumber; I++)
        {
            if (MyPoints[I].Cluster!=INT_MAX)
            {
                hbr=CreateSolidBrush(RGB((MyPoints[I].Cluster*320+75645)%256,(MyPoints[I].Cluster*134+35434)%256,(MyPoints[I].Cluster*20+45538)%256));
                SetRect(&rt,MyPoints[I].X,MyPoints[I].Y,MyPoints[I].X+15,MyPoints[I].Y+15);
                sprintf(MyBuffer,"%d",MyPoints[I].Cluster);
                DrawText(hdc,MyBuffer,-1,&rt,DT_CENTER);
                SelectObject(hdc,hbr);
                Ellipse(hdc,MyPoints[I].X-5,MyPoints[I].Y-5,MyPoints[I].X+5,MyPoints[I].Y+5);
                DeleteObject(hbr);
            }
            else
            {
                Ellipse(hdc,MyPoints[I].X-5,MyPoints[I].Y-5,MyPoints[I].X+5,MyPoints[I].Y+5);
            }
        }
        EndPaint(hwnd,&ps);
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_AUTO:
            I=rand()%100+ClusterNumber;
            KMeans_GeneratePoints(I);
            InitialMethodPTR(I,ClusterNumber);
            KMeans_Algorithm(I,ClusterNumber);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_MANUAL:

            L=0;
            PointNumber=0;
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_SETK:
            ShowWindow (hwnd2, 5);
            break;
        case IDC_RandomPartition:
            InitialMethodPTR=KMeans_Inital_RandomPartition;
            CheckMenuItem(hMenu,IDC_Forgy,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_RandomPartition,MF_CHECKED);
            break;
        case IDC_Forgy:
            InitialMethodPTR=KMeans_Inital_Forgy;
            CheckMenuItem(hMenu,IDC_Forgy,MF_CHECKED);
            CheckMenuItem(hMenu,IDC_RandomPartition,MF_UNCHECKED);
            break;
        case IDC_HELP1:
            MessageBox(hwnd,"Click left-button on mouse to set a point.\nClick right-button on mouse to execute K-means Algorithm.","Help",0);
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        MyPoints[L].X=LOWORD(lParam);
        MyPoints[L].Y=HIWORD(lParam);
        MyPoints[L].Cluster=INT_MAX;
        L++;
        PointNumber=L;
        InvalidateRect(hwnd,NULL,1);
        UpdateWindow(hwnd);
        break;
    case WM_RBUTTONDOWN:
        InitialMethodPTR(PointNumber,ClusterNumber);
        KMeans_Algorithm(PointNumber,ClusterNumber);
        InvalidateRect(hwnd1,NULL,1);
        UpdateWindow(hwnd1);
        L=0;
        break;
    case WM_DESTROY:
        PostQuitMessage (0);      /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


LRESULT CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static HWND hwndEdit;
    char Text[8];
    int F,G;
    switch(msg)
    {
    case WM_CREATE:
        CreateWindow(TEXT("button"), TEXT("Ok"),
                     WS_VISIBLE | WS_CHILD ,
                     50, 80, 50, 25,
                     hwnd, (HMENU) 10001, NULL, NULL);
        hwndEdit = CreateWindowW(L"Edit", NULL,
                                 WS_CHILD | WS_VISIBLE | WS_BORDER,
                                 50, 50, 75, 25, hwnd, (HMENU) 10002,
                                 NULL, NULL);
        break;

    case WM_SHOWWINDOW:
        sprintf(Text,"%d",ClusterNumber);
        //itoa(ClusterNumber,Text,10);
        SetWindowTextA(hwndEdit,Text);
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case 10001:
            GetWindowTextA(hwndEdit, Text, 8);
            ClusterNumber=atoi(Text);
            ShowWindow (hwnd2, 0);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    }
    return (DefWindowProc(hwnd, msg, wParam, lParam));

}
