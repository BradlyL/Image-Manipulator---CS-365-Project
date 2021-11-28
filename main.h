#pragma once
#pragma comment(lib,"gdiplus.lib")
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <gdiplus.h>
#include "GdiplusHelperFunctions.h"
#include <iostream>
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <fstream>
#include <sstream>

#include "EasyBMP.h"

class ImageProcessor {
public:
	void FindFormat(char* file);
	wchar_t ConvertInput(char* file);
	void ConvertFile(wchar_t format);
	void ImageInput(char* file);
	void ParseInput(std::string input);
	void Grayscale(char* file);
	void Resize();
	void callSubProcess();
	int startMenu();
private:
	int fileE;
	char charFilePath[1024];
	int resizeAmount;
	enum codecs
	{
		PNG = 0,
		JPG = 1,
	};
};

void ImageProcessor::FindFormat(char* file) {
	char* fileExtension = strstr(file, ".");
	if (fileExtension == NULL) {
		printf("Incomplete file name.\n");
		fileE = -1;
		return;
	}
	int charSize = strlen(fileExtension);

	for (int i = 0; i < charSize; i++) {
		fileExtension[i] = fileExtension[i + 1];
	}

	if (strcmp(fileExtension, "png") == 0) {
		fileE = PNG;
	}
	if (strcmp(fileExtension, "jpg") == 0) {
		fileE = JPG;
	}
}

//Convert char input to type of wchar_t
wchar_t ImageProcessor::ConvertInput(char* file) {
	FindFormat(file);

	int fileSize = strlen(file);
	wchar_t* wc = new wchar_t[fileSize] + 1;
	std::mbstowcs(wc, file, fileSize);

	return *wc;
}

void ImageProcessor::ConvertFile(wchar_t format) {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Gdiplus::Image* image = NULL;
	CLSID   encoderClsid;
	Gdiplus::Status  status;
	if (fileE == PNG) {
		image = new Gdiplus::Image(L"image.png");
		GetEncoderClsid(L"image/png", &encoderClsid);
	}
	if (fileE == JPG) {
		image = new Gdiplus::Image(L"image.jpeg");
		GetEncoderClsid(L"image/jpeg", &encoderClsid);
	}

	if (image == NULL) {
		printf("Invalid codec.\n");
		return;
	}


	//Save to new file
	status = image->Save(L"image.bmp", &encoderClsid, NULL);

	if (status == Gdiplus::Ok)
		printf("Image.bmp was saved successfully\n");
	else
		printf("Failure: status = %d\n", status);

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

//Find file format
void ImageProcessor::ImageInput(char* file) {
	wchar_t ImagePath = ConvertInput(file);
	ConvertFile(ImagePath);
}

void ImageProcessor::ParseInput(std::string input) {
	std::ifstream inputCSV;
	std::string filePath;
	std::string rescaleVal;

	inputCSV.open(input);

	getline(inputCSV, filePath);
	getline(inputCSV, rescaleVal);
	//Convert string to char array
	strcpy(charFilePath, filePath.c_str());
	//Convert second string to int
	resizeAmount = stoi(rescaleVal);
}

void ImageProcessor::Grayscale(char* file) {
	BMP Image;
	RGBApixel Temp;
	ebmpBYTE TempBYTE;
	Image.ReadFromFile("image.bmp");

	for (int i = 0; i < Image.TellWidth(); i++)
	{
		for (int j = 0; j < Image.TellHeight(); j++)
		{
			Temp = Image.GetPixel(i, j);
			int TempVal = (Temp.Red * 0.299) + (Temp.Green * 0.587) + (Temp.Blue * 0.114);
			TempBYTE = (ebmpBYTE)TempVal;
			Image(i, j)->Red = TempBYTE;
			Image(i, j)->Blue = TempBYTE;
			Image(i, j)->Green = TempBYTE;
		}
	}

	Image.WriteToFile("image_grayscale.bmp");
}

void ImageProcessor::Resize() {
	BMP Image;
	RGBApixel Temp;
	ebmpBYTE TempBYTE;
	Image.ReadFromFile(charFilePath);
	Rescale(Image, 'p', resizeAmount);
	Image.WriteToFile("image_resized.bmp");
}

void ImageProcessor::callSubProcess()
{
	wchar_t path[] = L"C:/Users/bradl/source/repos/Image Manipulator/CS365 Project.exe";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		path,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		DWORD lastError = GetLastError();
		printf("CreateProcess failed (%d).\n", lastError);
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


//Menu
int ImageProcessor::startMenu() {
	char imageInput[1024];
	std::string fileInput;
	int choice;
	int menuFlag = 1;

	while (menuFlag == 1) {
		std::cout << "\n1 Convert JPG/PNG to BMP\n2 Grayscale\n3 Resize\n4 Add image label\n5 Exit\n";
		std::cin >> choice;
		if (!std::cin.good()) {
			std::cout << "Only numbers allowed, please restart program.\n";
			break;
		}
		else {
			switch (choice) {
			case 1:
				std::cout << "Enter image path: ";
				std::cin >> imageInput;
				ImageInput(imageInput);
				menuFlag = 0;
				break;
			case 2:
				//Grayscale();
				menuFlag = 0;
				break;
			case 3:
				std::cout << "Enter a .csv file with resize guidelines.\n";
				std::cin >> fileInput;
				ParseInput(fileInput);
				Resize();
				menuFlag = 0;
				break;
			case 4:
				callSubProcess();
				break;
			case 5:
				std::cout << "Exiting...\n";
				menuFlag = 0;
				break;
			default:
				std::cout << "Try again. No input.\n";
				break;
			}
			std::cin.clear();
			fflush(stdin);
		}
	}
	return 0;
}