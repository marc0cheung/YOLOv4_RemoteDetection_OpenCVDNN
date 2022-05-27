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

void socketInitialization();
void writeFileJson(void* pObj, int nNum, int* pRectPoints);

void* g_Obj = NULL;

int main()
{
	// Socket Communication
	//定义长度变量
	int send_len = 0, recv_len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	socketInitialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(9999);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "接收器连接失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "接收器连接成功！" << endl;
	}

	
	// Get Object Name(s)
	vector<string> classes;  //for Object Name(s)
	String classesFile = "D:/Yolo/YOLO-CPP-Win32/QYoloVisionLib/1.names";

	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line))
		classes.push_back(line);

	// Set net argvs
	float confThreshold = 0.9; // Confidence threshold
	float nmsThreshold = 0.4;  // Non-maximum suppression threshold
	int inpWidth = 416;  // Width of network's input image
	int inpHeight = 416; // Height of network's input image

	//Define a YOLO net
	Net yolo_net;
	yolo_net = readNetFromDarknet("D:/Yolo/YOLO-CPP-Win32/QYoloVisionLib/1.cfg", "D:/Yolo/YOLO-CPP-Win32/QYoloVisionLib/1.weights");
	//yolo_net.setPreferableBackend(DNN_BACKEND_OPENCV);
	//yolo_net.setPreferableTarget(DNN_TARGET_CPU);
	yolo_net.setPreferableBackend(DNN_BACKEND_CUDA);
	yolo_net.setPreferableTarget(DNN_TARGET_CUDA);

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

	while (1)
	{
		cap >> frame;
		if (frame.empty()) break;
		Size dsize = Size(960, 540);  //Set Video Stream Resolution
		resize(frame, frame, dsize, 0, 0, INTER_AREA);

		//Start timer
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

		// Draw Predict Boxes and output coordinates
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
		}

		//Draw Predict Boxes
		if (indices.size() > 0)
		{
			Json::Value root;
			// Object_Count * 2 value per Coord * 5_Coordinates
			int* pRect = new int[indices.size() * 2 * 5];
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

				//指针偏移：动态创建数组
				//Point 1
				pRect[i * 10 + 0] = box.x;
				pRect[i * 10 + 1] = box.y;
				//Point 2
				pRect[i * 10 + 2] = box.x + box.width;
				pRect[i * 10 + 3] = box.y;
				//Point 3
				pRect[i * 10 + 4] = box.x;
				pRect[i * 10 + 5] = box.y + box.height;
				//Point 4
				pRect[i * 10 + 6] = box.x + box.width;
				pRect[i * 10 + 7] = box.y + box.height;
				//Point 5 (center point)
				pRect[i * 10 + 8] = (box.x + box.width) / 2;
				pRect[i * 10 + 9] = (box.y + box.height) / 2;

				
				// Write Coordinates to coordinates.json using JSONCPP Library
				writeFileJson(g_Obj, indices.size(), pRect);


				// Use String to send coordinates to receiver, but needs decoding process.
				String sendData;
				sendData += " Object_";  sendData += to_string(i + 1);
				sendData += "\n[(";
				sendData += to_string(box.x);  sendData += ",";  sendData += to_string(box.y);
				sendData += ")-(";
				sendData += to_string(box.x + box.width);  sendData += ",";   sendData += to_string(box.y);
				sendData += ")-(";
				sendData += to_string(box.x);  sendData += ",";   sendData += to_string(box.y + box.height);
				sendData += ")-(";
				sendData += to_string(box.x + box.width);  sendData += ",";   sendData += to_string(box.y + box.height);
				sendData += ")-(";
				sendData += to_string(box.x + (box.width / 2));  sendData += ",";   sendData += to_string(box.y + (box.height / 2));
				sendData += ")]\n";
				cout << sendData << endl;
				strcpy(send_buf, sendData.c_str());
				send_len = send(s_server, send_buf, 100, 0);

			}
			delete[]pRect;
		}

		// End Timer
		finish = clock();
		double  duration;
		duration = (double)(finish - start);

		// Put YOLO Processing Time and YOLO FPS to the frame
		putText(frame, to_string((int)duration)+" ms", Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
		putText(frame, "FPS: " + to_string((int)(1000 / (int)duration)), Point(20, 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 4);
		// Display Video Stream
		imshow("OpenCV DNN", frame);

		if (getWindowProperty("OpenCV DNN", WND_PROP_VISIBLE) < 1)
		{
			cap.release();
			break;
		}

		if (waitKey(33) >= 0) break;
		
	}

	//waitKey(0);
	//destroyAllWindows();
	//关闭套接字
	closesocket(s_server);
	//释放DLL资源
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
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	// check version
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}

}


void writeFileJson(void* pObj, int nNum, int* pRectPoints)
{
	//nNum: object count； pRectPoints：predict box coordinates
	// JSON root node
	Json::Value root;

	for (int i = 0; i < nNum; i++)
	{
		Json::Value Object[10];

		String Point1 = to_string(pRectPoints[i * 10 + 0]).append(",");
		Point1.append(to_string(pRectPoints[i * 10 + 1]));

		String Point2 = to_string(pRectPoints[i * 10 + 2]).append(",");
		Point2.append(to_string(pRectPoints[i * 10 + 3]));

		String Point3 = to_string(pRectPoints[i * 10 + 4]).append(",");
		Point3.append(to_string(pRectPoints[i * 10 + 5]));

		String Point4 = to_string(pRectPoints[i * 10 + 6]).append(",");
		Point4.append(to_string(pRectPoints[i * 10 + 7]));

		String Point5 = to_string(pRectPoints[i * 10 + 8]).append(",");
		Point5.append(to_string(pRectPoints[i * 10 + 9]));

		// child nodes, 5 coordinates
		Object[i]["Point_1"] = Json::Value(Point1);
		Object[i]["Point_2"] = Json::Value(Point2);
		Object[i]["Point_3"] = Json::Value(Point3);
		Object[i]["Point_4"] = Json::Value(Point4);
		Object[i]["Point_5"] = Json::Value(Point5);


		//load child nodes to root node
		String dst = "Object_";
		dst.append(to_string(i + 1));
		root[dst] = Json::Value(Object[i]);

	}
	

	//JSON Fast Writer output
	//cout << "FastWriter:" << endl;
	//Json::FastWriter fw;
	//cout << fw.write(root) << endl << endl;

	// JSON Styled Writer output
	cout << "StyledWriter:" << endl;
	Json::StyledWriter sw;
	cout << sw.write(root) << endl << endl;

	// generate JSON file
	ofstream os;
	os.open("coordinates.json", std::ios::out | std::ios::app);
	if (!os.is_open())
		cout << "error：can not find or create the file which named \" coordinates.json\"." << endl;
	os << sw.write(root);
	os.close();

	cout << "JSON DONE!" << endl;

}