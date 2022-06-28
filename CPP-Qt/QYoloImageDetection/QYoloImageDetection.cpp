#include "QYoloImageDetection.h"
#include <QPushButton>
#include <QFileDialog>

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/photo/photo.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <highgui/highgui_c.h>

#include <string.h>
#include <time.h>
#include <string>

#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

Mat SourceImg;
string SourceImgPath;
string ResultSavePath;

// Function Switch
int ShowObjNames_Conf = 0;
int ShowBBox = 1;
int ShowObjectIndex = 0;
float confThreshold = 0.7;
float nmsThreshold = 0.4;

// Bug Here
String cfgFile = "./network/button_v2.cfg";
String weightsFile = "./network/button_v2.weights";
String namesFile = "./network/button_v2.names";

QYoloImageDetection::QYoloImageDetection(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // CheckBox Signals and Slots
	connect(ui.showNames_checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_showNames_checkbox_Changed(int)));
	connect(ui.objIndex_checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_objIndex_checkbox_Changed(int)));
	connect(ui.showBBox_checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_showBBox_checkbox_Changed(int)));

    // Push Button Signals and Slots
    connect(ui.SelcPathBtn, SIGNAL(clicked()), this, SLOT(on_SelcPathBtn_Selected()));
	connect(ui.detect_btn, SIGNAL(clicked()), this, SLOT(on_detect_btn_Selected()));
	connect(ui.saveThresholdBtn, SIGNAL(clicked()), this, SLOT(on_saveThresholdBtn_Selected()));
	connect(ui.SavePathBtn, SIGNAL(clicked()), this, SLOT(on_SavePathBtn_Selected()));

	connect(ui.save_cap, SIGNAL(clicked()), this, SLOT(on_save_cap_Selected()));
	connect(ui.segment_save, SIGNAL(clicked()), this, SLOT(on_segment_save_Selected()));

	// Select Yolo Network Files
	connect(ui.cfgFileBtn, SIGNAL(clicked()), this, SLOT(on_cfgFileBtn_Selected()));
	connect(ui.namesFileBtn, SIGNAL(clicked()), this, SLOT(on_namesFileBtn_Selected()));
	connect(ui.weightsFileBtn, SIGNAL(clicked()), this, SLOT(on_weightsFileBtn_Selected()));

}

void QYoloImageDetection::on_SelcPathBtn_Selected()
{
	QString QSourceImagePath = QFileDialog::getOpenFileName(this, "Select Source Image", "./");

	if (QSourceImagePath != "")
	{
		SourceImgPath = QSourceImagePath.toStdString();
		ui.file_path_label->setText(QString::fromStdString(SourceImgPath));

		SourceImg = imread(QSourceImagePath.toStdString());
		imshow("Source Image", SourceImg);
	}
}

void QYoloImageDetection::on_detect_btn_Selected()
{
	Mat frame = imread(SourceImgPath);
	
	vector<string> classes;
	String classesFile = namesFile;

	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line))
		classes.push_back(line);

	Net yolo_net;
	// yolo_net = readNetFromDarknet("./network/button_v2.cfg", "./network/button_v2.weights");
	yolo_net = readNetFromDarknet(cfgFile, weightsFile);
	yolo_net.setPreferableBackend(DNN_BACKEND_OPENCV);
	yolo_net.setPreferableTarget(DNN_TARGET_CPU);

	// Use YOLOv4-Tiny Net to Detect Object
	Mat blob;
	blobFromImage(frame, blob, 1 / 255.0, Size(416, 416), Scalar(0, 0, 0), true, false);
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

	vector<int> indices;
	NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

	// Draw Predict Boxes
	if (indices.size() > 0)
	{
		string notify_msg = to_string(indices.size()) + " Object Detected.";
		ui.notification->setText(QString::fromStdString(notify_msg));
		
		// Object_Count * 2 value per Coord * 5_Coordinates
		//int* pRect = new int[indices.size() * 2 * 5];
		for (size_t i = 0; i < indices.size(); ++i)
		{
			int idx = indices[i];
			Rect box = boxes[idx];

			if (ShowBBox == 1)
			{
				rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(0, 0, 255), 1);
			}

			string label = format("%.2f", confidences[idx]);
			if (!classes.empty())
			{
				CV_Assert(classIds[idx] < (int)classes.size());
				label = classes[classIds[idx]] + ":" + label;
			}
			int baseLine;
			Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
			box.y = max(box.y, labelSize.height);

			if (ShowObjNames_Conf == 1)
			{
				putText(frame, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1);
			}

			std::string i_str = std::to_string(i + 1);

			if (ShowObjectIndex == 1)
			{
				putText(frame, i_str, Point(box.x + box.width, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);
			}

		}
	}

	// SHOW DETECTION RESULT
	imshow("Detection Result", frame);
}

void QYoloImageDetection::on_showNames_checkbox_Changed(int state)
{
	if (state == Qt::Checked)
	{
		ShowObjNames_Conf = 1;
		ui.notification->setText("Show Label: OPEN");
	}
	else if (state == Qt::Unchecked)
	{
		ShowObjNames_Conf = 0;
		ui.notification->setText("Show Label: CLOSED");
	}
}

void QYoloImageDetection::on_objIndex_checkbox_Changed(int state)
{
	if (state == Qt::Checked)
	{
		ShowObjectIndex = 1;
		ui.notification->setText("Show Index: OPEN");
	}
	else if (state == Qt::Unchecked)
	{
		ShowObjectIndex = 0;
		ui.notification->setText("Show Index: CLOSED");
	}
}

void QYoloImageDetection::on_showBBox_checkbox_Changed(int state)
{
	if (state == Qt::Checked)
	{
		ShowBBox = 1;
		ui.notification->setText("Show BBox: OPEN");
	}
	else if (state == Qt::Unchecked)
	{
		ShowBBox = 0;
		ui.notification->setText("Show BBox: CLOSED");
	}
}

void QYoloImageDetection::on_saveThresholdBtn_Selected()
{
	confThreshold = stof((ui.confThreshold_Input->toPlainText()).toStdString());
	nmsThreshold = stof((ui.NMSInput->toPlainText()).toStdString());
	
	string threshold_msg = "Conf: " + to_string(confThreshold) + ", NMS: " + to_string(nmsThreshold);
	ui.notification->setText(QString::fromStdString(threshold_msg));
}

void QYoloImageDetection::on_SavePathBtn_Selected()
{
	QString QSavePath = QFileDialog::getExistingDirectory();
	if (QSavePath != "")
	{
		ui.savepath_label->setText(QSavePath);
		ResultSavePath = QSavePath.toStdString();
	}
}

// Incomplete Function
void QYoloImageDetection::on_save_cap_Selected()
{
	// get detection result
	// imwrite
	ui.notification->setText("Detection Result Saved.");
}

// Incomplete Function
void QYoloImageDetection::on_segment_save_Selected()
{
	// get object segment
	// imwrite
	ui.notification->setText("Segment Saved!");
}

void QYoloImageDetection::on_cfgFileBtn_Selected()
{
	QString QcfgPath = QFileDialog::getOpenFileName();
	if (QcfgPath != "")
	{
		ui.cfgFilePath_Label->setText(QcfgPath);
		// set global cfg file path
		cfgFile = QcfgPath.toStdString();
	}
}

void QYoloImageDetection::on_namesFileBtn_Selected()
{
	QString QnamesPath = QFileDialog::getOpenFileName();
	if (QnamesPath != "")
	{
		ui.namesFilePath_Label->setText(QnamesPath);
		// set global .names file path
		namesFile = QnamesPath.toStdString();
	}
}

void QYoloImageDetection::on_weightsFileBtn_Selected()
{
	QString QweightsPath = QFileDialog::getOpenFileName();
	if (QweightsPath != "")
	{
		ui.weightsFilePath_Label->setText(QweightsPath);
		// set global .weights file path
		weightsFile = QweightsPath.toStdString();
	}
}


QYoloImageDetection::~QYoloImageDetection()
{}
