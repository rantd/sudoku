//
//  neural.h
//  finalproject
//
//  Created by rantd on 2014/11/28.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#ifndef _neural_h
#define _neural_h

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "marco.h"

using namespace std;

void read_dataset(char *filename, cv::Mat &data, cv::Mat &classes, int tot_samples);
int training_nn();

#endif
