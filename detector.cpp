//
//  detector.cpp
//  finalproject
//
//  Created by rantd on 2014/11/29.
//  Copyright (c) 2014年 Vu Van Tan. All rights reserved.
//

#include "detector.h"

using namespace std;

// 二つのピクセルの距離を求める
inline double cal_dist(cv::Point2f p1, cv::Point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return (dx * dx + dy * dy);
}

// ノイズ除去などの前処理
void pre_process(cv::Mat &input, cv::Mat &output) {
    output = cv::Mat(input.size(), CV_8UC1);
    cv::cvtColor(input, output, CV_BGR2GRAY);
    cv::GaussianBlur(output, output, cv::Size(11, 11), 0);
    cv::adaptiveThreshold(output, output, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 5, 2);
    cv::bitwise_not(output, output);
}

// 数独のボードを検出する
int find_board(cv::Mat &src, cv::Mat &input, cv::Mat &output) {
    
    double area, max_area = -1;
    cv::Point max_pt;
    
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            if (input.at<uchar>(y, x) >= 128) {
                area = cv::floodFill(input, cv::Point(x, y), CV_RGB(0, 0, 64));
                if (area > max_area) {
                    max_pt = cv::Point(x, y);
                    max_area = area;
                }
            }
        }
    }
    
    cv::floodFill(input, max_pt, CV_RGB(255, 255, 255));
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            if (input.at<uchar>(y, x) == 64) {
                cv::floodFill(input, cv::Point(x, y), CV_RGB(0, 0, 0));
            }
        }
    }
    vector<cv::Point> points, convex_hull, board;
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            if (input.at<uchar>(y, x) == 255) {
                points.push_back(cv::Point(x, y));
            }
        }
    }
    
    cv::convexHull(points, convex_hull);
    
    
    // 数独の枠の角を検出
    cv::RotatedRect min_rect = cv::minAreaRect(convex_hull);
    cv::Size2f rect_size = min_rect.size;
    if (min_rect.angle < -45) {
        min_rect.angle += 90;
        swap(rect_size.width, rect_size.height);
    }
    double rotated = min_rect.angle * M_PI / 180.0;
    double width = rect_size.width;
    double height = rect_size.height;
    double dist = 0.5 * sqrt(width * width + height * height);
    double theta1 = asin(height / 2 / dist) + rotated;
    double theta2 = asin(height / 2 / dist) - rotated;
    
    cv::Point2f tmp_pt[4];
    tmp_pt[0] = cv::Point2f(min_rect.center.x - dist * cos(theta1),
                                     min_rect.center.y - dist * sin(theta1));
    int ind_min = 0;
    double min_dist = cal_dist(tmp_pt[0], convex_hull[0]);
    for (int i = 1; i < convex_hull.size(); ++i) {
        double tmp_dist = cal_dist(tmp_pt[0], convex_hull[i]);
        if (min_dist > tmp_dist) {
            min_dist = tmp_dist;
            ind_min = i;
        }
    }
    board.push_back(convex_hull[ind_min]);
    
    tmp_pt[1] = cv::Point2f(min_rect.center.x + dist * cos(theta2),
                                      min_rect.center.y - dist * sin(theta2));
    ind_min = 0;
    min_dist = cal_dist(tmp_pt[1], convex_hull[0]);
    for (int i = 1; i < convex_hull.size(); ++i) {
        double tmp_dist = cal_dist(tmp_pt[1], convex_hull[i]);
        if (min_dist > tmp_dist) {
            min_dist = tmp_dist;
            ind_min = i;
        }
    }
    board.push_back(convex_hull[ind_min]);
    
    tmp_pt[2] = cv::Point2f(2 * min_rect.center.x - tmp_pt[0].x,
                                      2 * min_rect.center.y - tmp_pt[0].y);
    ind_min = 0;
    min_dist = cal_dist(tmp_pt[2], convex_hull[0]);
    for (int i = 1; i < convex_hull.size(); ++i) {
        double tmp_dist = cal_dist(tmp_pt[2], convex_hull[i]);
        if (min_dist > tmp_dist) {
            min_dist = tmp_dist;
            ind_min = i;
        }
    }
    board.push_back(convex_hull[ind_min]);
    
    tmp_pt[3] = cv::Point2f(2 * min_rect.center.x - tmp_pt[1].x,
                                      2 * min_rect.center.y - tmp_pt[1].y);
    ind_min = 0;
    min_dist = cal_dist(tmp_pt[3], convex_hull[0]);
    for (int i = 1; i < convex_hull.size(); ++i) {
        double tmp_dist = cal_dist(tmp_pt[3], convex_hull[i]);
        if (min_dist > tmp_dist) {
            min_dist = tmp_dist;
            ind_min = i;
        }
    }
    board.push_back(convex_hull[ind_min]);
    
    cv::Point2f in_quad[4], out_quad[4];
    
    for (int i = 0; i < 4; ++i) {
        in_quad[i] = cv::Point2f(board[i].x, board[i].y);
    }
    out_quad[0] = cv::Point2f(0, 0);
    out_quad[1] = cv::Point2f(540, 0);
    out_quad[2] = cv::Point2f(540, 540);
    out_quad[3] = cv::Point2f(0, 540);
    
    cv::Mat map = cv::getPerspectiveTransform(in_quad, out_quad);
    cv::warpPerspective(src, output, map, cv::Size(540, 540));
    
    return 0;
}

// セルの余分の黒い領域を除去する
void remove_noise(cv::Mat &input) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            if (input.at<uchar>(i, j) < 128) {
                cv::floodFill(input, cv::Point(j, i), CV_RGB(255, 255, 255));
            }
            if (input.at<uchar>(input.rows - i - 1, j) < 128) {
                cv::floodFill(input, cv::Point(j, input.rows - i - 1), CV_RGB(255, 255, 255));
            }
        }
        for (int j = 0; j < input.rows; ++j) {
            if (input.at<uchar>(j, i) < 128) {
                cv::floodFill(input, cv::Point(i, j), CV_RGB(255, 255, 255));
            }
            if (input.at<uchar>(j, input.cols - i - 1) < 128) {
                cv::floodFill(input, cv::Point(input.cols - i - 1, j), CV_RGB(255, 255, 255));
            }
        }
    }
    cv::threshold(input, input, 55, 255, CV_THRESH_BINARY);
}

// そのセルは空白であるかをチェックする
bool is_blank(cv::Mat &input) {
    int cnt = 0;
    for (int i = 2; i < input.rows - 2; ++i) {
        for (int j = 2; j < input.cols - 2; ++j) {
            if (input.at<uchar>(i, j) == 0) {
                cnt++;
                int area = cv::floodFill(input, cv::Point(j, i), CV_RGB(0, 0, 0));
                if (area < 150) {
                    cv::floodFill(input, cv::Point(j, i), CV_RGB(255, 255, 255));
                    cnt--;
                }
            }
        }
    }
    return (cnt < 150);
}
