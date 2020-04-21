﻿#include <iostream>
#define _WIN32_WINNT 0x0500 //RIGHT
#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <tlhelp32.h>
#include <vector>
#include <string.h>
#include "haze.h"
#include "Classes.h"

DWORD GetModuleBaseAddress(LPSTR ModuleName, DWORD pid)
{
    MODULEENTRY32 module32;

    std::string helper;

    module32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    Module32First(hSnap, &module32);
    if (hSnap == INVALID_HANDLE_VALUE)
        return 0;
    do
    {
        for (int i = 0; i < 256; i++)
        {
            if ((CHAR)module32.szModule[i] != 0)
            {
                helper = helper + (CHAR)module32.szModule[i];
            }
        }

        if (strcmp(helper.c_str(), ModuleName) == 0)
        {
            break;
        }
        helper = "";
    } while (Module32Next(hSnap, &module32));
    CloseHandle(hSnap);

    return (DWORD)module32.modBaseAddr;
}

using namespace std;
using namespace hazedumper;

int GetClosest(Player entity[64], Player myplayer, int maxplayer)
{
    float clo_dist = 100000;
    int enemy_return = 1;
    float dist;

    for (int i = 1; i <= maxplayer; i++)
    {
        dist = sqrt((myplayer.pos[0] - entity[i].pos[0]) * (myplayer.pos[0] - entity[i].pos[0]) + (myplayer.pos[1] - entity[i].pos[1]) * (myplayer.pos[1] - entity[i].pos[1]) + (myplayer.pos[2] - entity[i].pos[2]) * (myplayer.pos[2] - entity[i].pos[2]));
        if (dist < clo_dist && entity[i].hp > 0 && entity[i].team != myplayer.team)
        {
            clo_dist = dist;
            enemy_return = i;
        }
        dist = 0;
    }
    return enemy_return;
}

vector<float> Aim(Player entity[64], Player myplayer, int closest)
{
    float angles[3];
    double delta[3] = { (myplayer.pos[0] - entity[closest].pos[0]), (myplayer.pos[1] - entity[closest].pos[1]), (myplayer.pos[2] - entity[closest].pos[2]) };
    double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
    angles[0] = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
    angles[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
    angles[2] = 0.0f;
    if (delta[0] >= 0.0) { angles[1] += 180.0f; }
    if (angles[1] > 180) { angles[1] = angles[1] - 360; }

    vector<float> angles_vec;
    angles_vec.push_back(angles[0]);
    angles_vec.push_back(angles[1]);
    if (angles[1] > -180 && angles[1] <= 180 && angles[0] <= 89 && angles[0] > -89)
        return angles_vec;
    else
    {
        return { 10,0 };
    }
}

HANDLE proces;

int main()
{
    SYSTEMTIME st;
    int chwila = NULL;

    bool ifplay = false;
    int maxplayer = 0;
    Player entity[64];

    int tryb = 2;

    DWORD pid;
    HWND window;
    window = FindWindow(0, L"Counter-Strike: Global Offensive");
    if (window != NULL)
    {
        GetWindowThreadProcessId(window, &pid);
        proces = OpenProcess(PROCESS_VM_READ, false, pid);
        if (proces != NULL)
        {
            while (true)
            {
                if (GetAsyncKeyState(VK_UP))
                    ifplay = true;

                if (GetAsyncKeyState(VK_F2))
                {
                    tryb = 2;//ak
                }
                if (GetAsyncKeyState(VK_F4))
                {
                    tryb = 1;//sniper
                }

                if (ifplay == true)
                {
                    DWORD clientmodule = GetModuleBaseAddress((LPSTR)"client_panorama.dll", pid);
                    DWORD engine = GetModuleBaseAddress((LPSTR)"engine.dll", pid);
                    DWORD dwclient;

                    DWORD player;
                    ReadProcessMemory(proces, (LPCVOID)(clientmodule + signatures::dwLocalPlayer), (PVOID)&player, sizeof(player), 0);

                    Player myplayer;
                    ReadProcessMemory(proces, (LPCVOID)(engine + signatures::dwClientState), (PVOID)&dwclient, sizeof(dwclient), 0);

                    DWORD basebone;
                    ReadProcessMemory(proces, (LPCVOID)(player + netvars::m_dwBoneMatrix), (PVOID)&basebone, sizeof(basebone), 0);

                    while (true)
                    {

                        GetSystemTime(&st);
                        if (st.wSecond % 3 == 0 && chwila != st.wSecond)
                        {
                            chwila = st.wSecond;
                            for (int i = 1; i < 64; i++)
                            {
                                entity[i].strzelono = false;
                            }
                        }

                        if (GetAsyncKeyState(VK_DOWN))
                        {
                            ifplay = false;
                            break;
                        }

                        if (GetAsyncKeyState(VK_F2))
                        {
                            tryb = 2;//ak
                        }
                        if (GetAsyncKeyState(VK_F4))
                        {
                            tryb = 1;//sniper
                        }

                        float pos_add[3];

                        ReadProcessMemory(proces, (LPCVOID)(player + netvars::m_iHealth), (LPVOID)&myplayer.hp, sizeof(myplayer.hp), 0);
                        ReadProcessMemory(proces, (LPCVOID)(player + netvars::m_iTeamNum), (PVOID)&myplayer.team, sizeof(myplayer.team), 0);
                        ReadProcessMemory(proces, (LPCVOID)(player + netvars::m_vecOrigin), (PVOID)&myplayer.pos, sizeof(myplayer.pos), 0);
                        ReadProcessMemory(proces, (LPCVOID)(player + netvars::m_vecViewOffset), (PVOID)&pos_add, sizeof(pos_add), 0);

                        myplayer.pos[2] += pos_add[2];

                        ReadProcessMemory(proces, (LPCVOID)(dwclient + signatures::dwClientState_ViewAngles), (PVOID)&myplayer.ang, sizeof(myplayer.ang), 0);

                        maxplayer = 0;

                        for (int i = 1; i < 64; i++)
                        {
                            DWORD ent;
                            ReadProcessMemory(proces, (LPCVOID)(clientmodule + signatures::dwEntityList + (0x10 * i)), (PVOID)&ent, sizeof(ent), 0);
                            if (ent != NULL)
                            {
                                maxplayer++;

                                ReadProcessMemory(proces, (LPCVOID)(ent + netvars::m_iTeamNum), (PVOID)&entity[i].team, sizeof(entity[i].team), 0);
                                ReadProcessMemory(proces, (LPCVOID)(ent + netvars::m_iHealth), (PVOID)&entity[i].hp, sizeof(entity[i].hp), 0);
                                ReadProcessMemory(proces, (LPCVOID)(ent + netvars::m_bSpotted), (PVOID)&entity[i].isSpotted, sizeof(entity[i].isSpotted), 0);

                                ReadProcessMemory(proces, (LPCVOID)(ent + netvars::m_vecOrigin), (PVOID)&entity[i].pos, sizeof(entity[i].pos), 0);
                                DWORD basebone;
                                ReadProcessMemory(proces, (LPCVOID)(ent + netvars::m_dwBoneMatrix), (PVOID)&basebone, sizeof(basebone), 0);
                                ReadProcessMemory(proces, (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x0C), (PVOID)&entity[i].pos[0], sizeof(entity[i].pos[0]), 0);
                                ReadProcessMemory(proces, (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x1C), (PVOID)&entity[i].pos[1], sizeof(entity[i].pos[1]), 0);
                                ReadProcessMemory(proces, (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x2C), (PVOID)&entity[i].pos[2], sizeof(entity[i].pos[2]), 0);
                            }
                        }

                        int minchange = 100;
                        int closestentity = 0;

                        for (int i = 1; i <= maxplayer; i++)
                        {
                            vector<float> angle = Aim(entity, myplayer, i);
                            float angles[3];
                            angles[0] = angle[0];
                            angles[1] = angle[1];
                            angles[2] = 0;
                            float changex = (myplayer.ang[0] - angles[0]);
                            float changey = (myplayer.ang[1] - angles[1]);
                            if ((abs(changex) + abs(changey)) / 2 < minchange && entity[i].team != myplayer.team && entity[i].hp > 0 && entity[i].isSpotted == true && entity[i].strzelono == false)
                            {
                                minchange = (abs(changex) + abs(changey)) / 2;
                                closestentity = i;
                            }

                        }

                        if (closestentity != 0)
                        {
                            vector<float> angle = Aim(entity, myplayer, closestentity);
                            float angles[3];
                            angles[0] = angle[0];
                            angles[1] = angle[1];
                            angles[2] = 0;
                            float changex = (myplayer.ang[0] - angles[0]);
                            float changey = (myplayer.ang[1] - angles[1]);

                            if (GetAsyncKeyState(VK_LBUTTON))
                            {
                                if (abs(changex) < 3 * (3 - tryb) && abs(changey) < 3 * (3 - tryb) && entity[closestentity].team != myplayer.team && entity[closestentity].hp > 0 && entity[closestentity].isSpotted == true && entity[closestentity].strzelono == false)
                                {
                                    for (int i = 0; i < 5; i++)
                                    {
                                        myplayer.ang[0] -= changex / 5;
                                        myplayer.ang[1] -= changey / 5;

                                        CloseHandle(proces);
                                        proces = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid);
                                        WriteProcessMemory(proces, (LPVOID)(dwclient + signatures::dwClientState_ViewAngles), myplayer.ang, sizeof(angles), 0);
                                        CloseHandle(proces);
                                        proces = OpenProcess(PROCESS_VM_READ, false, pid);

                                        Sleep(2);
                                    }
                                    entity[closestentity].strzelono = true;
                                }
                                keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 0, 0);
                                Sleep(2);
                                keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 2, 0);
                                Sleep(20);
                            }
                        }
                        else
                        {
                            if (GetAsyncKeyState(VK_LBUTTON))
                            {
                                keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 0, 0);
                                Sleep(2);
                                keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 2, 0);
                                Sleep(20);
                            }
                        }
                        Sleep(1);
                    }
                }
                else
                {
                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 0, 0);
                        Sleep(2);
                        keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 2, 0);
                        Sleep(20);
                    }
                }
            }
        }
        else
        {
            cout << "Nie znaleziono procesu cs:go";
        }
    }
    else
    {
        cout << "Nie znaleziono okna cs:go";
    }
    CloseHandle(proces);
    return 0;
}
