#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/photo/photo.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <highgui/highgui_c.h>
#include <string.h>
#include <json/json.h>
#include <time.h>

#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<WINSOCK2.H>
#include<STDIO.H>
#include<cstring>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace cv;
using namespace dnn;

// Function Switch, 1 = on / true, 0 = off / false
// ===============================================
// JSON Configuration
#define WriteJSON           1
#define JSONOnlyOne         1  // need to keep this open.
#define outputJsonWriter    0
// Socket Configuration
#define OpenSocket          1
#define SocketServerAddress "127.0.0.1"
#define SocketPort          9999
#define outputSocketMessage 0
// Video Stream UI Configuration
#define DisplayBoxX         0
#define DisplayRunTime      0 
// Video Configuration
#define VideoStream         1
#define VideoFlip           1
#define VideoSize           960, 540
// YOLO network Configuration
#define Backend             "GPU"
#define cfgFile             "./network/1.cfg"
#define weightsFile         "./network/1.weights"
#define namesFile           "./network/1.names"

void socketInitialization();

void* g_Obj = NULL;

Json::Value root;
Json::Value Detected;
Json::Value Objects;

int main()
{
	// Socket Communication
	// Define the length of the message
	int send_len = 0;
	// Defining the send buffer
	char send_buf[100];
	// Define server-side sockets and accept request sockets
	SOCKET s_server;
	// server address
	SOCKADDR_IN server_addr;
	socketInitialization();
	// server-side information
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SocketServerAddress);
	server_addr.sin_port = htons(SocketPort);
	// Create sockets
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "Fail to establish connection to receiver." << endl;
		WSACleanup();
	}
	else {
		cout << "Successfully connect to receiver." << endl;
	}

	if (OpenSocket == 0)
	{
		cout << "\n[!!!] Socket initialized but Socket is set to CLOSED status.\n" << endl;
	}
	else
		cout << "Socket initialized and Socket Communication is OPEN." << endl;

	
	// Get Object Name(s)
	vector<string> classes;  //for Object Name(s)
	String classesFile = namesFile;

	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line))
		classes.push_back(line);

	// Set net argvs
	float confThreshold = 0.9; // Confidence threshold
	float nmsThreshold = 0.4;  // Non-maximum suppression threshold
	int inpWidth = 416;        // Width of network's input image
	int inpHeight = 416;       // Height of network's input image

	// Define a YOLO net
	Net yolo_net;
	yolo_net = readNetFromDarknet(cfgFile, weightsFile);

	// Set OpenCV dnn Backend and Target
	if (Backend == "CPU")
	{
		yolo_net.setPreferableBackend(DNN_BACKEND_OPENCV);
		yolo_net.setPreferableTarget(DNN_TARGET_CPU);
	}
	else if (Backend == "GPU")
	{
		yolo_net.setPreferableBackend(DNN_BACKEND_CUDA);
		yolo_net.setPreferableTarget(DNN_TARGET_CUDA);
	}

	//Open USB Camera
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cerr << "Couldn't open Camera." << endl;
		cap.release();
		return false;
	}
	Mat frame;

	clock_t start, finish; // Use these vars to record the YOLO processing time
	double tick_ori = static_cast<double>(getTickCount());  // Get Original Tick Count 
	int loopCount = 0;

	while (1)
	{
		// Calculate Programme Running Time
		double timex = static_cast<double>(getTickCount());
		timex = (timex - tick_ori) / getTickFrequency();  // now_tickCount - Original_Tick_Count = Program running time
		
		cap >> frame;
		if (frame.empty()) break;
		Size dsize = Size(VideoSize);  //Set Video Stream Resolution
		resize(frame, frame, dsize, 0, 0, INTER_AREA);
		if (VideoFlip == 1)
		{
			flip(frame, frame, -1);
		}

		//Start timer, use to calculate how long does YOLOv4-Tiny process a frame
		start = clock();

		// Use YOLOv4-Tiny Net to Detect Object
		Mat blob;
		blobFromImage(frame, blob, 1 / 255.0, Size(inpWidth, inpHeight), Scalar(0, 0, 0), true, false);
		yolo_net.setInput(blob);

		vector<Mat> outs;
		yolo_net.forward(outs, yolo_net.getUnconnectedOutLayersNames());

		// Postprocess(frame, outs)
		vector<int> classIds;
		vector<float> confidences;
		vector<Rect> boxes;

		for (size_t i = 0; i < outs.size(); ++i)
		{
			// Scan through all the bounding boxes output from the network and keep only the
			// ones with high confidence scores. Assign the box's class label as the class
			// with the highest score for the box.
			float* data = (float*)outs[i].data;
			for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
			{
				Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
				Point classIdPoint;
				double confidence;
				// Get the value and location of the maximum score
				minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
				if (confidence > confThreshold)
				{
					int centerX = (int)(data[0] * frame.cols);
					int centerY = (int)(data[1] * frame.rows);
					int width = (int)(data[2] * frame.cols);
					int height = (int)(data[3] * frame.rows);
					int left = centerX - width / 2;
					int top = centerY - height / 2;

					classIds.push_back(classIdPoint.x);
					confidences.push_back((float)confidence);
					boxes.push_back(Rect(left, top, width, height));
				}
			}
		}

		// Perform non maximum suppression to eliminate redundant overlapping boxes with
		// lower confidences
		vector<int> indices;
		NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

		// End Timer
		finish = clock();
		double  duration;
		duration = (double)(finish - start);

		//Draw Predict Boxes
		if (indices.size() > 0)
		{
			// Object_Count * 2 value per Coord * 5_Coordinates
			//int* pRect = new int[indices.size() * 2 * 5];
			for (size_t i = 0; i < indices.size(); ++i)
			{
				int idx = indices[i];
				Rect box = boxes[idx];
				rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(0, 0, 255), 1);
				string label = format("%.2f", confidences[idx]);
				if (!classes.empty())
				{
					CV_Assert(classIds[idx] < (int)classes.size());
					label = classes[classIds[idx]] + ":" + label;
				}
				int baseLine;
				Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
				box.y = max(box.y, labelSize.height);
				putText(frame, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1);
				std::string i_str = std::to_string(i + 1);
				putText(frame, i_str, Point(box.x + box.width, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);
				
				if (DisplayBoxX == 1)
				{
					// Output box.x coordinates on the frame
					putText(frame, "box.x: " + to_string(boxes[0].x), Point(20, 160), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
				}

				//Pointer offsets: dynamic creation of arrays
				//Point 1
				//pRect[i * 10 + 0] = box.x;   
				//pRect[i * 10 + 1] = box.y;   
				////Point 2
				//pRect[i * 10 + 2] = box.x + box.width;
				//pRect[i * 10 + 3] = box.y;
				////Point 3
				//pRect[i * 10 + 4] = box.x;
				//pRect[i * 10 + 5] = box.y + box.height;
				////Point 4
				//pRect[i * 10 + 6] = box.x + box.width;
				//pRect[i * 10 + 7] = box.y + box.height;
				////Point 5 (center point)
				//pRect[i * 10 + 8] = (box.x + box.width) / 2;
				//pRect[i * 10 + 9] = (box.y + box.height) / 2;

				
				if (WriteJSON == 1)
				{
					// Write Coordinates to coordinates.json using JSONCPP Library
					//writeFileJson(g_Obj, indices.size(), duration, pRect);
					//Objects["Point1"] = Json::Value(to_string(box.x) + "," + to_string(box.y));
					//Objects["Point2"] = Json::Value(to_string(box.x + box.width) + "," + to_string(box.y));
					//Objects["Point3"] = Json::Value(to_string(box.x) + ", " + to_string(box.y + box.height));
					//Objects["Point4"] = Json::Value(to_string(box.x + box.width) + "," + to_string(box.y + box.height));
					//Objects["Point5"] = Json::Value(to_string(box.x + (box.width / 2)) + "," + to_string(box.y + (box.height / 2)));

					int coordinatesArray[10] = { box.x, box.y , (box.x + box.width) , box.y , box.x , (box.y + box.height) ,
					(box.x + box.width) , (box.y + box.height) , (box.x + (box.width / 2)) , (box.y + (box.height / 2)) };

					for (int arrayIndx = 0; arrayIndx < 10; arrayIndx++)
					{
						Objects.append(coordinatesArray[arrayIndx]);
					}

					string detected_dst = "Object_" + to_string(i + 1);
					Detected[detected_dst] = Json::Value(Objects);
					Objects.clear();
				}

				if (OpenSocket == 1)
				{
					// Use String to send coordinates to receiver, but needs decoding process.
					String sendData;
					sendData += "Object_" + to_string(i + 1) + "\n[(";                                              // Object Count
					sendData += to_string(box.x) + "," + to_string(box.y) + ")-(";                                 // Point 1
					sendData += to_string(box.x + box.width) + "," + to_string(box.y) + ")-(";                    // Point 2
					sendData += to_string(box.x) + ", " + to_string(box.y + box.height) + ")-(";                 // Point 3
					sendData += to_string(box.x + box.width)+ "," + to_string(box.y + box.height) + ")-(";      // Point 4
					sendData += to_string(box.x + (box.width / 2)) + "," + to_string(box.y + (box.height / 2)) + ")-";  // Point 5
					sendData += to_string((int)duration) + "]";      // Processing Time
				
					if (outputSocketMessage == 1)
						cout << sendData << endl;
					strcpy(send_buf, sendData.c_str());
					send_len = send(s_server, send_buf, 100, 0);
				}

			}
			
			if (WriteJSON == 1) {
				string root_dst = "Detected_at_" + to_string(loopCount + 1);
				root[root_dst] = Detected;

				// JSON Styled Writer output
				if (outputJsonWriter == 1)
				{
					Json::StyledWriter sw;
					cout << "StyledWriter:" << endl;
					cout << sw.write(root) << endl << endl;
				}
			}

			//delete[]pRect;
		}

		// Put YOLO Processing Time and YOLO FPS to the frame
		putText(frame, "Delay: " + to_string((int)duration) + " ms", Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
		putText(frame, "FPS: " + to_string((int)(1000 / (int)duration)), Point(20, 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
		loopCount++;
		
		if (DisplayRunTime == 1)
		{
			putText(frame, "RunTime: " + to_string((int)timex) + " s", Point(20, 120), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
		}

		if (VideoStream == 1)
		{
			// Display Video Stream
			imshow("OpenCV DNN", frame);

			if (getWindowProperty("OpenCV DNN", WND_PROP_VISIBLE) < 1)
			{
				cap.release();
				break;
			}
		}

		if (WriteJSON == 1) 
		{
			ofstream os;
			Json::StyledWriter sw;

			if (JSONOnlyOne == 1)
			{
				os.open("coordinates.json");  // Output coordinates of the last frame to json file only
			}
			else
			{
				os.open("coordinates.json", std::ios::out | std::ios::app);  // Output all coordinates in one .json file
			}
			if (!os.is_open())
				cout << "error: can not find or create the file which named \" coordinates.json\"." << endl;
			os << sw.write(root);
			os.close();

			//cout << "JSON DONE!" << endl;
		}

		if (waitKey(33) >= 0) break;
		
	}

	//waitKey(0);
	//destroyAllWindows();

	// Close Socket Communication
	closesocket(s_server);
	// Release DLL Resources
	WSACleanup();
	
	return 0;
}


void socketInitialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "Failed to initialize the socket library!" << endl;
	}
	else {
		cout << "Initialization of socket library successful!" << endl;
	}
	// check version
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "The socket library version number does not match!" << endl;
		WSACleanup();
	}
	else {
		cout << "The socket library version is correct! " << endl;
	}

}