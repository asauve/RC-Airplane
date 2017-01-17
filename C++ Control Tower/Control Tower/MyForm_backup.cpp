#include "MyForm.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
using namespace System;
using namespace System::Windows::Forms;



[STAThread]
void Main(array<String^>^ args)
{
	//creates a console window!
	AllocConsole();
	std::freopen("conin$", "r", stdin);
	std::freopen("conout$", "w", stdout);
	std::freopen("conout$", "w", stderr);
	std::printf("Debugging Window:\n");



	//opens the windows form
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Control_Tower::MyForm form;
	Application::Run(%form);



}
