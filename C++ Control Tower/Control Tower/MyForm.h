#pragma once
#include <Windows.h>
#include <iostream>
#include <msclr\marshal_cppstd.h> //for String to std::string conversion 

extern float Rthrottle;
extern float RleftFlap;
extern float RrightFlap;
extern bool connected;
extern bool updateSignal;
bool threadruns;

namespace Control_Tower {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;
	using namespace System::Threading;

	/// <summary>
	/// Opens a user interface: allows the user to visualize the XBOX stick signals, and set up a serial port to the arduino
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)//Constructor: called once (upon object creation)
		{
			InitializeComponent();

			findPorts(); //finds the available ports & updates the port selection combobox
			try {
				this->comboBox1->SelectedIndex = 0; //if usable port(s) were detected, automatically select the first one
			}
			catch (...) {}//if findPorts() cant find anything at startup, try/catch will prevent a crash
			this->comboBox2->SelectedItem = "9600";
			
			////start update loop (keeps progressbars/labels up to date, sends data over serial)
			::threadruns = true;
			Thread^ thread3 = gcnew Thread(gcnew ThreadStart(this,&MyForm::updateLoop));
			thread3->Name = "thread3";
			thread3->Start();


		}//end of constructor


	protected:

		~MyForm()//destructor: called when form is closed: used to prevent any memory leaks
		{
			bool threadruns = false;//breaks the background thread's infinite loop:

			if (components)
			{
				delete components;
			}
		}//end of destructor

		 //generated automatically by the winforms designer
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label8;
	public: System::Windows::Forms::ProgressBar^  progressBar1;
	public: System::Windows::Forms::ProgressBar^  progressBar2;
	public: System::Windows::Forms::ProgressBar^  progressBar3;
	private: System::IO::Ports::SerialPort^  serialPort1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::ComboBox^  comboBox2;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: System::ComponentModel::IContainer^  components;




#pragma region Windows Form Designer generated code 
			 //generated automatically by the winforms designer
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->progressBar2 = (gcnew System::Windows::Forms::ProgressBar());
			this->progressBar3 = (gcnew System::Windows::Forms::ProgressBar());
			this->serialPort1 = (gcnew System::IO::Ports::SerialPort(this->components));
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(101, 31);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(257, 17);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Please connect an Xbox 360 controller";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(55, 76);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 17);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Throttle: ";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(51, 106);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(68, 17);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Left Flap:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(41, 137);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(78, 17);
			this->label4->TabIndex = 3;
			this->label4->Text = L"Right Flap:";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(314, 137);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(31, 17);
			this->label8->TabIndex = 7;
			this->label8->Text = L"N/A";
			// 
			// progressBar1
			// 
			this->progressBar1->BackColor = System::Drawing::SystemColors::Control;
			this->progressBar1->Location = System::Drawing::Point(126, 69);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->progressBar1->Size = System::Drawing::Size(182, 24);
			this->progressBar1->Step = 100;
			this->progressBar1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar1->TabIndex = 9;
			// 
			// progressBar2
			// 
			this->progressBar2->BackColor = System::Drawing::SystemColors::Control;
			this->progressBar2->Location = System::Drawing::Point(126, 99);
			this->progressBar2->Name = L"progressBar2";
			this->progressBar2->Size = System::Drawing::Size(182, 24);
			this->progressBar2->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar2->TabIndex = 11;
			// 
			// progressBar3
			// 
			this->progressBar3->BackColor = System::Drawing::SystemColors::Control;
			this->progressBar3->Location = System::Drawing::Point(126, 130);
			this->progressBar3->Name = L"progressBar3";
			this->progressBar3->Size = System::Drawing::Size(182, 24);
			this->progressBar3->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar3->TabIndex = 19;
			// 
			// serialPort1
			// 
			this->serialPort1->ReadTimeout = 500;
			this->serialPort1->WriteTimeout = 500;
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(126, 190);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(127, 25);
			this->comboBox1->TabIndex = 12;
			this->comboBox1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::comboBox1_MouseClick);
			// 
			// comboBox2
			// 
			this->comboBox2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(1) { L"9600" });
			this->comboBox2->Location = System::Drawing::Point(126, 222);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(127, 25);
			this->comboBox2->TabIndex = 13;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(39, 230);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(81, 17);
			this->label5->TabIndex = 14;
			this->label5->Text = L"Baud Rate:";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(261, 190);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(97, 57);
			this->button1->TabIndex = 15;
			this->button1->Text = L"Open Port";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// button2
			// 
			this->button2->Enabled = false;
			this->button2->Location = System::Drawing::Point(158, 360);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(95, 38);
			this->button2->TabIndex = 16;
			this->button2->Text = L"Close Port";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// textBox1
			// 
			this->textBox1->Enabled = false;
			this->textBox1->Location = System::Drawing::Point(126, 280);
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(232, 25);
			this->textBox1->TabIndex = 20;
			this->textBox1->Text = L"Receiver";
			this->textBox1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(314, 76);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(31, 17);
			this->label6->TabIndex = 21;
			this->label6->Text = L"N/A";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(314, 106);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(31, 17);
			this->label7->TabIndex = 22;
			this->label7->Text = L"N/A";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(49, 198);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(71, 17);
			this->label10->TabIndex = 23;
			this->label10->Text = L"USB port:";
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(259, 360);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(112, 38);
			this->button3->TabIndex = 24;
			this->button3->Text = L"Controls Help";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(126, 309);
			this->textBox2->Name = L"textBox2";
			this->textBox2->ReadOnly = true;
			this->textBox2->Size = System::Drawing::Size(232, 25);
			this->textBox2->TabIndex = 25;
			this->textBox2->Text = L"Transmittor";
			this->textBox2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// button4
			// 
			this->button4->Enabled = false;
			this->button4->Location = System::Drawing::Point(45, 311);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(75, 23);
			this->button4->TabIndex = 26;
			this->button4->Text = L"Send";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			// 
			// button5
			// 
			this->button5->Enabled = false;
			this->button5->Location = System::Drawing::Point(45, 282);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(75, 23);
			this->button5->TabIndex = 27;
			this->button5->Text = L"Read";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &MyForm::button5_Click);
			// 
			// MyForm
			// 
			this->AcceptButton = this->button4;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 17);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(394, 403);
			this->ControlBox = false;
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->comboBox2);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->progressBar3);
			this->Controls->Add(this->progressBar2);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Font = (gcnew System::Drawing::Font(L"Arial", 9));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Control Tower";
			this->TopMost = true;
			this->ResumeLayout(false);
			this->PerformLayout();

		}//end of InitializeComponent

#pragma endregion
		/*******************************************************************
								END REGION : USER CODE
		********************************************************************/

delegate void updateDelegate(void); //delegates are required to call functions from a background thread
private: void update(void)
{
	try {
		//tests if updateInput was called from this main thread or some worker thread
		//if called from worker thread: must use delegate
		if (this->progressBar1->InvokeRequired)
		{
			updateDelegate^ d =
				gcnew updateDelegate(this, &MyForm::update);
			this->Invoke(d, gcnew array<Object^> {});
		}
		else {//delegate not required
			if (::updateSignal) {
				::updateSignal = false;

				if (this->serialPort1->IsOpen) {
					String^ throttleMSG = "!" + String::Format("{0:0.000}", ::Rthrottle);
					String^ leftFlapMSG = "@" + String::Format("{0:0.000}", ::RleftFlap);
					String^ rightFlapMSG = "#" + String::Format("{0:0.000}", ::RrightFlap);
					this->serialPort1->Write(throttleMSG);
					this->serialPort1->Write(leftFlapMSG);
					this->serialPort1->Write(rightFlapMSG);

				}//end of if/else (serialport.isopen)
			}//end of if(::updatesignal)
				if(::connected){
				this->label1->Text = L"             Xbox 360 controller detected";
				//this->progressBar2->Value = ::Rthrottle * 50 + 50;
				//problem with old code: built in animation feature leads to laggy response.
				//SetProgressNoAnimation makes use of a bug which removes the animation
				this->SetProgressNoAnimation(progressBar1, ::Rthrottle * 50 + 50);
				this->SetProgressNoAnimation(progressBar2, ::RleftFlap * 50 + 50);
				this->SetProgressNoAnimation(progressBar3, ::RrightFlap * 50 + 50);
				this->label6->Text = String::Format("{0:0.00}", ::Rthrottle);
				this->label7->Text = String::Format("{0:0.00}", ::RleftFlap);
				this->label8->Text = String::Format("{0:0.00}", ::RrightFlap);
			}
				else {
					this->label1->Text = L"Please connect an Xbox 360 controller";
					this->label6->Text = L"NA";
					this->label7->Text = L"NA";
					this->label8->Text = L"NA";
				}//end of if/else(::connected)
		}//end of if/else(delegate required)
	}catch(...){}//end of try/catch
}//end of update definition
void updateLoop() {
	while (::threadruns) {
		update();
		Sleep(15);
	}
}//end updateLoop

private: void findPorts(void)
		{
			//gets port names
			this->comboBox1->Items->Clear();//clear old values
			array<Object^>^ objectArray = SerialPort::GetPortNames();
			this->comboBox1->Items->AddRange(objectArray);//adds to combo box
		}

public: static void SetProgressNoAnimation(ProgressBar^ pb, float value)
		{
			// To get around this animation, we need to move the progress bar backwards.
			if (value == pb->Maximum) {
				// Special case (can't set value > Maximum).
				pb->Value = value;           // Set the value
				pb->Value = value - 1;       // Move it backwards
			}
			else {
				pb->Value = value + 1;       // Move past
			}
			pb->Value = value;               // Move to correct value
		}

//help button
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
	//help button is pressed: display the help dialog

	std::cout << std::endl << "HELP" << std::endl << std::endl
		<< "CONTROLS" << std::endl
		<< "throttle with left stick" << std::endl
		<< "steer with right stick" << std::endl << std::endl
		<< "TRIMMING" << std::endl
		<< "Throttle Trim control: dpad up and down" << std::endl
		<< "Reset throttle trim: dpad left" << std::endl
		<< "Flap Trim control: 'X' and 'B' for roll  (turning)" << std::endl
		<< "Flap Trim control: 'Y' and 'A' for pitch (up/down)" << std::endl
		<< "Reset flap trims: dpad right" << std::endl
		<< "DISARM MOTOR: R & L bumpers pressed simulataneously" << std::endl << std::endl
		<< "SERIAL COMMUNICATION" << std::endl
		<< "debug throttle:  send a float preceded by '!' eg: \"!0.51\" -> Send" << std::endl
		<< "debug leftFlap:  send a float preceded by '@' eg: \"@-0.2101\" -> Send" << std::endl
		<< "debug rightFlap: send a float preceded by '#' eg: \"#-1\" -> Send" << std::endl
		<< "values outside of [-1,1] will be reduced" << std::endl;
}//end of help button pressed event handler

//combo box 1: choose COM port
private: System::Void comboBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			//when the usb dropbox is clicked - search for new connections
			findPorts();
}

//open button
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
	if (this->comboBox1->Text == String::Empty || this->comboBox2->Text == String::Empty) {
		this->textBox1->Text = "Check port settings";
	}
	else {
		try {
			//make sure port isnt already open
			if (!this->serialPort1->IsOpen) {
				this->serialPort1->PortName = this->comboBox1->Text;
				this->serialPort1->BaudRate = Int32::Parse(this->comboBox2->Text);
				this->serialPort1->Open();
				this->textBox1->Text = "Port opened";
				this->textBox2->Text = "Enter_debug_message";
				this->textBox2->ReadOnly = false;
				this->button1->Enabled = false;
				this->button2->Enabled = true;
				this->button4->Enabled = true;
				this->button5->Enabled = true;
			}
			else {
				this->textBox1->Text = "Port already opened";
				}
		}
		catch (UnauthorizedAccessException^) {
			this->textBox1->Text = "Unauthorized Access";
		}
	}
}

//close button
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
	this->serialPort1->Close();
	this->button1->Enabled = true;
	this->button2->Enabled = false;
	this->button4->Enabled = false;
	this->button5->Enabled = false;
	this->textBox1->Text = "Port closed";
	this->textBox2->ReadOnly = true;
}

//read button
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
	if (this->serialPort1->IsOpen) {
		try {
			this->textBox1->Text = this->serialPort1->ReadLine();
		}
		catch (TimeoutException^) {
			this->textBox1->Text = "Timeout: nothing here";
		}
	}
}
	
//send button
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
	String^ message = this->textBox2->Text;
	if (this->serialPort1->IsOpen) {
		this->serialPort1->WriteLine(message);
	}
	else {
		this->textBox2->Text = "port not opened";
	}
}

};//end of MyForm declaration
}//end of namespace Control_Tower