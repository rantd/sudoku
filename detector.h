//
//  detector.h
//  finalproject
//
//  Created by rantd on 2014/11/29.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#ifndef _detector_h
#define _detector_h

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

inline double cal_dist(cv::Point2f p1, cv::Point p2);
void pre_process(cv::Mat &input, cv::Mat &output);
int find_board(cv::Mat &src, cv::Mat &input, cv::Mat &output);
void remove_noise(cv::Mat &input);
bool is_blank(cv::Mat &input);

#endif
