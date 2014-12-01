//
//  neural.cpp
//  finalproject
//
//  Created by rantd on 2014/11/28.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#include "neural.h"

using namespace std;

// 学習とテストのデータを読み取る
void read_dataset(char *filename, cv::Mat &data, cv::Mat &classes, int tot_samples) {
    int digit;
    float attribute;
    
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "ファイル %s を開けない\n", filename);
        exit(1);
    }
    
    for (int row = 0; row < tot_samples; ++row) {
        for (int col = 0; col < NUM_ATTRIBUTES; col++) {
            fscanf(input_file, "%f,", &attribute);
            data.at<float>(row, col) = attribute;
        }
        fscanf(input_file, "%d", &digit);
        classes.at<float>(row, digit - 1) = 1.0;
    }
    
    fclose(input_file);
}

// ニューラルネットワークを生成する
int training_nn() {
    cv::Mat training_set(TRAINING_SAMPLES, NUM_ATTRIBUTES, CV_32F);
    cv::Mat training_set_classifications(TRAINING_SAMPLES, NUM_CLASSES, CV_32F);
    cv::Mat test_set(TEST_SAMPLES, NUM_ATTRIBUTES,CV_32F);
    cv::Mat test_set_classifications(TEST_SAMPLES, NUM_CLASSES, CV_32F);
    
    cv::Mat classification_result(1, NUM_CLASSES, CV_32F);
    
    read_dataset((char *) "training_dataset.txt", training_set,
                 training_set_classifications, TRAINING_SAMPLES);
    read_dataset((char *) "test_dataset.txt", test_set,
                 test_set_classifications, TEST_SAMPLES);
    
    cv::Mat layers(3, 1, CV_32S);
    layers.at<int>(0, 0) = NUM_ATTRIBUTES;
    layers.at<int>(1, 0) = 20;
    layers.at<int>(2, 0) = NUM_CLASSES;
    
    CvANN_MLP ann(layers, CvANN_MLP::SIGMOID_SYM, 0.6, 1);
    CvANN_MLP_TrainParams params(cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 0.000001),
                                 CvANN_MLP_TrainParams::BACKPROP,
                                 0.1,
                                 0.1);
    
    int iters = ann.train(training_set, training_set_classifications, cv::Mat(), cv::Mat(), params);
    fprintf(stdout, "繰り返し回数: %d\n", iters);
    
    CvFileStorage *storage = cvOpenFileStorage("params.xml", 0, CV_STORAGE_WRITE);
    ann.write(storage, "OCR-digits");
    cvReleaseFileStorage(&storage);
    fprintf(stdout, "モデルをparams.xmlに保存した\n\n");
    
    cv::Mat test_sample;
    int correct_digits = 0;
    int wrong_digits = 0;
    
    int classification_matrix[NUM_CLASSES][NUM_CLASSES];
    memset(classification_matrix, 0, sizeof(classification_matrix));
    
    for (int sample = 0; sample < TEST_SAMPLES; sample++) {
        test_sample = test_set.row(sample);
        ann.predict(test_sample, classification_result);
        int ans = 1;
        float prob = 0.0;
        float max_prob = classification_result.at<float>(0, 0);
        for (int digit = 2; digit <= NUM_CLASSES; ++digit) {
            prob = classification_result.at<float>(0, digit - 1);
            if (prob > max_prob) {
                max_prob = prob;
                ans = digit;
            }
        }
        
        if (test_set_classifications.at<float>(sample, ans - 1) != 1.0) {
            wrong_digits++;
            for (int true_digit = 1; true_digit <= NUM_CLASSES; ++true_digit) {
                if (test_set_classifications.at<float>(sample, true_digit - 1) == 1.0) {
                    classification_matrix[true_digit - 1][ans - 1]++;
                    break;
                }
            }
        }
        else {
            correct_digits++;
            classification_matrix[ans - 1][ans - 1]++;
        }
    }
   
    fprintf(stdout, "学習結果\n"
           "\t正しい認識: %d (%f%%)\n"
           "\t間違い認識: %d (%f%%)\n",
           correct_digits, correct_digits * 100.0 / TEST_SAMPLES,
           wrong_digits, wrong_digits * 100.0 / TEST_SAMPLES);
    
    fprintf(stdout, "\n\t");
    for (int i = 1; i <= NUM_CLASSES; ++i) {
        fprintf(stdout, "%d\t", i);
    }
    fprintf(stdout, "\n");
    
    for (int row = 1; row <= NUM_CLASSES; ++row) {
        fprintf(stdout, "%d\t", row);
        for (int col = 1; col <= NUM_CLASSES; ++col) {
            fprintf(stdout, "%d\t", classification_matrix[row - 1][col - 1]);
        }
        fprintf(stdout, "\n");
    }

    return 0;
}
