# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'yolov4_live_detectorkpVnRE.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(560, 633)
        font = QFont()
        font.setPointSize(9)
        MainWindow.setFont(font)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.Prog_Title = QLabel(self.centralwidget)
        self.Prog_Title.setObjectName(u"Prog_Title")
        self.Prog_Title.setGeometry(QRect(90, 20, 381, 41))
        font1 = QFont()
        font1.setFamily(u"Agency FB")
        font1.setPointSize(20)
        self.Prog_Title.setFont(font1)
        self.Prog_Title.setLayoutDirection(Qt.LeftToRight)
        self.Prog_Title.setTextFormat(Qt.PlainText)
        self.Prog_Title.setAlignment(Qt.AlignCenter)
        self.index_input = QTextEdit(self.centralwidget)
        self.index_input.setObjectName(u"index_input")
        self.index_input.setGeometry(QRect(460, 180, 91, 31))
        self.index_label = QLabel(self.centralwidget)
        self.index_label.setObjectName(u"index_label")
        self.index_label.setGeometry(QRect(460, 160, 81, 21))
        font2 = QFont()
        font2.setPointSize(12)
        self.index_label.setFont(font2)
        self.SaveIndexBtn = QPushButton(self.centralwidget)
        self.SaveIndexBtn.setObjectName(u"SaveIndexBtn")
        self.SaveIndexBtn.setGeometry(QRect(460, 220, 91, 23))
        self.CapBtn = QPushButton(self.centralwidget)
        self.CapBtn.setObjectName(u"CapBtn")
        self.CapBtn.setGeometry(QRect(460, 280, 91, 23))
        self.CloseBtn = QPushButton(self.centralwidget)
        self.CloseBtn.setObjectName(u"CloseBtn")
        self.CloseBtn.setGeometry(QRect(460, 350, 91, 23))
        self.AboutBtn = QPushButton(self.centralwidget)
        self.AboutBtn.setObjectName(u"AboutBtn")
        self.AboutBtn.setGeometry(QRect(460, 380, 91, 23))
        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")
        self.label.setGeometry(QRect(30, 90, 416, 416))
        font3 = QFont()
        font3.setFamily(u"Agency FB")
        font3.setPointSize(26)
        font3.setBold(True)
        font3.setWeight(75)
        self.label.setFont(font3)
        self.label.setAutoFillBackground(True)
        self.label.setAlignment(Qt.AlignCenter)
        self.GetObjSegmentBtn = QPushButton(self.centralwidget)
        self.GetObjSegmentBtn.setObjectName(u"GetObjSegmentBtn")
        self.GetObjSegmentBtn.setGeometry(QRect(460, 250, 91, 23))
        self.SelcPathBtn = QPushButton(self.centralwidget)
        self.SelcPathBtn.setObjectName(u"SelcPathBtn")
        self.SelcPathBtn.setGeometry(QRect(360, 580, 75, 23))
        self.path_label = QLabel(self.centralwidget)
        self.path_label.setObjectName(u"path_label")
        self.path_label.setGeometry(QRect(10, 580, 341, 20))
        self.path_label.setFont(font)
        self.status_label = QLabel(self.centralwidget)
        self.status_label.setObjectName(u"status_label")
        self.status_label.setGeometry(QRect(450, 420, 101, 131))
        self.status_label.setFont(font2)
        self.status_label.setAlignment(Qt.AlignCenter)
        self.height_input = QTextEdit(self.centralwidget)
        self.height_input.setObjectName(u"height_input")
        self.height_input.setGeometry(QRect(460, 120, 91, 31))
        self.height_label = QLabel(self.centralwidget)
        self.height_label.setObjectName(u"height_label")
        self.height_label.setGeometry(QRect(460, 100, 81, 21))
        self.height_label.setFont(font2)
        self.width_input = QTextEdit(self.centralwidget)
        self.width_input.setObjectName(u"width_input")
        self.width_input.setGeometry(QRect(460, 70, 91, 31))
        self.width_label = QLabel(self.centralwidget)
        self.width_label.setObjectName(u"width_label")
        self.width_label.setGeometry(QRect(460, 50, 81, 21))
        self.width_label.setFont(font2)
        self.fromFileBtn = QPushButton(self.centralwidget)
        self.fromFileBtn.setObjectName(u"fromFileBtn")
        self.fromFileBtn.setGeometry(QRect(460, 310, 91, 31))
        self.SelcPathBtn_segment = QPushButton(self.centralwidget)
        self.SelcPathBtn_segment.setObjectName(u"SelcPathBtn_segment")
        self.SelcPathBtn_segment.setGeometry(QRect(360, 550, 75, 23))
        self.segmentPath_label = QLabel(self.centralwidget)
        self.segmentPath_label.setObjectName(u"segmentPath_label")
        self.segmentPath_label.setGeometry(QRect(10, 550, 341, 20))
        self.segmentPath_label.setFont(font)
        self.detect_status = QLabel(self.centralwidget)
        self.detect_status.setObjectName(u"detect_status")
        self.detect_status.setGeometry(QRect(10, 520, 431, 20))
        self.detect_status.setAlignment(Qt.AlignCenter)
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"YOLOv4-Tiny Real-Time Object Detection", None))
        self.Prog_Title.setText(QCoreApplication.translate("MainWindow", u"YOLOv4-Tiny Live Detector Ver 1.0", None))
        self.index_input.setHtml(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">0</span></p></body></html>", None))
        self.index_label.setText(QCoreApplication.translate("MainWindow", u"Index:", None))
        self.SaveIndexBtn.setText(QCoreApplication.translate("MainWindow", u"Save Pref.", None))
        self.CapBtn.setText(QCoreApplication.translate("MainWindow", u"Cap!", None))
        self.CloseBtn.setText(QCoreApplication.translate("MainWindow", u"Close", None))
        self.AboutBtn.setText(QCoreApplication.translate("MainWindow", u"About", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Video Stream", None))
        self.GetObjSegmentBtn.setText(QCoreApplication.translate("MainWindow", u"Segment", None))
        self.SelcPathBtn.setText(QCoreApplication.translate("MainWindow", u"Select", None))
        self.path_label.setText(QCoreApplication.translate("MainWindow", u"Select Cap Saving Path...", None))
        self.status_label.setText(QCoreApplication.translate("MainWindow", u"Status", None))
        self.height_input.setHtml(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">416</span></p></body></html>", None))
        self.height_label.setText(QCoreApplication.translate("MainWindow", u"Height:", None))
        self.width_input.setHtml(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">416</span></p></body></html>", None))
        self.width_label.setText(QCoreApplication.translate("MainWindow", u"Width:", None))
        self.fromFileBtn.setText(QCoreApplication.translate("MainWindow", u"From File", None))
        self.SelcPathBtn_segment.setText(QCoreApplication.translate("MainWindow", u"Select", None))
        self.segmentPath_label.setText(QCoreApplication.translate("MainWindow", u"Select Segment Saving Path...", None))
        self.detect_status.setText(QCoreApplication.translate("MainWindow", u"Detect Status", None))
    # retranslateUi

