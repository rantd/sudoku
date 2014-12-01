//
//  main.cpp
//  finalproject
//
//  Created by rantd on 2014/11/20.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "procdigit.h"
#include "neural.h"
#include "sudoku.h"
#include "detector.h"

#define NEED_PROCESSING 0   // 画像のデータを処理するフラッグ
#define NEED_TRAINING   0   // 画像のデータを学習するフラッグ
#define IMAGE_MODE      1   // 画像を認識するモード。そうでなければカメラから認識を始める

using namespace std;

// データへのリンク
string DATA_PATH = "database/";
CvANN_MLP ann;  // 用いるニューラルネットワーク

void recognize(cv::Mat org) {
    // 数独ボードを抽出する
    int pixel[NUM_ATTRIBUTES];
    cv::Mat org_gray, dst;
    pre_process(org, org_gray);
    find_board(org, org_gray, dst);
    int row = dst.rows, col = dst.cols;
    assert(row == SCALED_SIZE && col == SCALED_SIZE);
    int sub_size = SCALED_SIZE / 9;
    cv::Mat dst_gray;
    cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
    
    cv::cvtColor(dst, dst_gray, CV_BGR2GRAY);
    cv::GaussianBlur(dst_gray, dst_gray, cv::Size(11, 11), 0);
    cv::adaptiveThreshold(dst_gray, dst_gray, 255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
    
    // ボードを割り切り、各セルを処理する
    vector<vector<int> > board(9, vector<int>(9, 0));
    for (int i = 0; i < SCALED_SIZE; i += sub_size) {
        for (int j = 0; j < SCALED_SIZE; j += sub_size) {
            cv::Mat sub_board = dst_gray.colRange(j, j + sub_size).rowRange(i, i + sub_size).clone();
            
            remove_noise(sub_board);
            
            // 数字が入っている場合
            if (!is_blank(sub_board)) {
                cv::dilate(sub_board, sub_board, kernel);
                
                // データを取り出す
                crop_digit(sub_board, sub_board);
                
                cv::Mat digit(INPUT_SIZE, INPUT_SIZE, CV_32FC1);
                cv::resize(sub_board, digit, cv::Size(INPUT_SIZE, INPUT_SIZE));
                
                cvt_img2arr(digit, pixel);
                cv::Mat data(1, NUM_ATTRIBUTES, CV_32FC1);
                for (int k = 0; k < NUM_ATTRIBUTES; ++k) {
                    data.at<float>(0, k) = pixel[k];
                }
                // 認識を行う
                int ans = 1;
                cv::Mat classification_result(1, NUM_CLASSES, CV_32F);
                ann.predict(data, classification_result);
                float prob;
                float max_prob = classification_result.at<float>(0, 0);
                for (int d = 2; d <= NUM_CLASSES; ++d) {
                    prob = classification_result.at<float>(0, d - 1);
                    if (prob > max_prob) {
                        max_prob = prob;
                        ans = d;
                    }
                }
                fprintf(stdout, "認識結果 %d スコア %f\n", ans, max_prob);
                board[i / sub_size][j / sub_size] = ans;
            }
        }
    }
    
    fprintf(stdout, "\n認識した結果\n");
    
    for (int i = 0; i < 9; ++i) {
        if (i == 3 || i == 6) {
            fprintf(stdout, "---+---+---\n");
        }
        for (int j = 0; j < 9; ++j) {
            if (j == 3 || j == 6) {
                fprintf(stdout, "|");
            }
            if (board[i][j] != 0) fprintf(stdout, "%d", board[i][j]);
            else fprintf(stdout, " ");
        }
        fprintf(stdout, "\n");
    }
    
    fprintf(stdout, "\n数独を解いた結果\n");
    Sudoku *sdk = solve(new Sudoku(board));
    vector<vector<int> > final_board = sdk->get_result();
    
    for (int i = 0; i < 9; ++i) {
        if (i == 3 || i == 6) {
            fprintf(stdout, "---+---+---\n");
        }
        for (int j = 0; j < 9; ++j) {
            if (j == 3 || j == 6) {
                fprintf(stdout, "|");
            }
            fprintf(stdout, "%d", final_board[i][j]);
        }
        fprintf(stdout, "\n");
    }
    
    // 数独を解いた結果を画像に貼り付ける
    for (int i = 0; i < SCALED_SIZE; i += sub_size) {
        for (int j = 0; j < SCALED_SIZE; j += sub_size) {
            cv::Mat sub_board(dst_gray, cv::Rect(j + 15, i + 15, 30, 30));
            int x = i / 60, y = j / 60;
            if (board[x][y] == 0) {
                // 事前に用意したテンプレートを用いて貼り付ける
                string img_file = cvt_int2str(final_board[x][y], 1) + ".png";
                cv::Mat temp = cv::imread(img_file, 0);
                temp.copyTo(sub_board);
            }
        }
    }
    
    // 最終的な結果を表示する
    cv::cvtColor(org, org, CV_BGR2GRAY);
    cv::resize(org, org, cv::Size(SCALED_SIZE, SCALED_SIZE));
    cv::Mat canvas(SCALED_SIZE, SCALED_SIZE * 2, org.type());
    org.copyTo(canvas(cv::Rect(0, 0, SCALED_SIZE, SCALED_SIZE)));
    dst_gray.copyTo(canvas(cv::Rect(SCALED_SIZE, 0, SCALED_SIZE, SCALED_SIZE)));
    cv::namedWindow("sudokuMan");
    cv::imshow("sudokuMan", canvas);
    cv::waitKey(0);
    cv::destroyWindow("sudokuMan");
}

int main(int argc, char * argv[]) {
    
#if NEED_PROCESSING

    // 学習データ用とテストデータを作成する
    fprintf(stdout, "画像を処理中\n");
    process_data(DATA_PATH, TRAINING_SAMPLES / NUM_CLASSES, (char *) "training_dataset.txt");
    process_data(DATA_PATH, TEST_SAMPLES / NUM_CLASSES, (char *) "test_dataset.txt");
    fprintf(stdout, "処理終了\n\n");
    
#endif
    
#if NEED_TRAINING
    
    // 学習によってモデルを生成し、params.xmlに保存する
    fprintf(stdout, "ニューラルネットワークを生成\n");
    training_nn();
    
#endif
    
    // 保存されたANNモデルを読み取る
    fprintf(stdout, "\nparams.xmlからモデルを読み込む\n");
    CvFileStorage *storage = cvOpenFileStorage("params.xml", 0, CV_STORAGE_READ);
    CvFileNode *file_node = cvGetFileNodeByName(storage, 0, "OCR-digits");
    ann.read(storage, file_node);
    cvReleaseFileStorage(&storage);
    
#if IMAGE_MODE
    // 画像から認識する
    char *img_filename = (argc > 1) ? argv[1] : (char *) "test1.jpg";
    cv::Mat org;
    org = cv::imread(img_filename, 3);
    recognize(org);

#else
    // カメラから数独を認識する
    cv::VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened()) {
        fprintf(stderr, "カメラの入力を見つからない\n");
        exit(1);
    }
    cv::Mat frame;
    cv::namedWindow("input");
    bool loop_flag = true;
    while (loop_flag) {
        bool is_record = false;
        cap >> frame;
        cv::imshow("input", frame);
        int key = cv::waitKey(33);
        switch (key) {
            case 'q':
                loop_flag = false;
                break;
            case 'r':
                is_record = true;
                break;
            default:
                break;
        }
        if (is_record) {
            recognize(frame);
        }
    }
    
#endif
    return 0;
}
