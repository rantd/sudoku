//
//  procdigit.h
//  finalproject
//
//  Created by rantd on 2014/11/28.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#ifndef _procdigit_h
#define _procdigit_h

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <cassert>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "marco.h"


using namespace std;

void crop_digit(cv::Mat &input, cv::Mat &output);
void cvt_img2arr(cv::Mat &input, int output[]);
string cvt_int2str(int value, int len);
void process_data(string data_path, int samples, char *output_file);

#endif
