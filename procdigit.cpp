//
//  procdigit.cpp
//  finalproject
//
//  Created by rantd on 2014/11/28.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#include "procdigit.h"

using namespace std;

// 画像から数字の部分のみを取り出す
void crop_digit(cv::Mat &input, cv::Mat &output) {
    int min_x = 10000, min_y = 10000, max_x = 0, max_y = 0;
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            if (input.at<uchar>(i, j) == 0) {
                min_x = min(min_x, j);
                max_x = max(max_x, j);
                min_y = min(min_y, i);
                max_y = max(max_y, i);
            }
        }
    }
    cv::Mat ans(input, cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1));
    output= ans.clone();
    return;
}

// 画像のピクセルデータを配列に変換する
// 黒のピクセルを1に, 白のピクセルを0に変える
void cvt_img2arr(cv::Mat &input, int output[]) {
    int ind = 0;
    for (int i = 0; i < INPUT_SIZE; ++i) {
        for (int j = 0; j < INPUT_SIZE; ++j) {
            output[ind++] = (input.at<uchar>(i, j) == 255) ? 0 : 1;
        }
    }
    return;
}

// 数値を文字列に変換する。長さlenとなるまで、頭に'0'を追加する
string cvt_int2str(int value, int len) {
    stringstream ss;
    ss << value;
    string ans = ss.str();
    assert(ans.size() <= len);
    while (ans.size() < len) {
        ans = '0' + ans;
    }
    return ans;
}

// 画像を処理する
void process_data(string data_path, int samples, char *output_file) {
    cv::Mat src, dst;
    int pixel[NUM_ATTRIBUTES];
	fstream file;
    cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
    file.open(output_file, ios::out);
    
    for (int d = 1; d <= 9; ++d) {
        for (int s = 1; s <= samples; ++s) {
            string img_path = data_path + cvt_int2str(d, 1) + "/img" + cvt_int2str(d + 1, 3) + "-" + cvt_int2str(s, 5) + ".png";
            cv::Mat src = cv::imread(img_path, 0);
            if (src.data == NULL) {
                fprintf(stderr, "空の画像であった\n");
                exit(1);
            }
            // ノイズを除去する
            cv::GaussianBlur(src, dst, cv::Size(5, 5), 0);
            
            // 閾値50で画像を2値にする
            cv::threshold(dst, dst, 50, 255, CV_THRESH_BINARY);
            
            cv::dilate(dst, dst, kernel);
            
            //最終的結果
            cv::Mat ans(INPUT_SIZE, INPUT_SIZE, CV_8U, cv::Scalar(0));
            
            crop_digit(dst, dst);
            cv::resize(dst, ans, cv::Size(INPUT_SIZE, INPUT_SIZE));
            cvt_img2arr(ans, pixel);
            
            for (int i = 0; i < INPUT_SIZE * INPUT_SIZE; ++i){
                file << pixel[i] << ",";
            }
            file << d << "\n";
        }
    }

    file.close();
    return;
}
