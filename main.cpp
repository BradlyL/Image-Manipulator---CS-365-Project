#include <stdio.h>
#include <iostream>
#include "main.h"
#pragma comment(lib,"gdiplus.lib")

int main()
{
    ImageProcessor imageProcessor;

    int programFlag = 1;

    while (programFlag == 1) {
        programFlag = imageProcessor.startMenu();
    }

    return 0;
}