#ifndef FUNC_H
#define FUNC_H

#include <iostream>
#include <String>
#include <filesystem>
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

/**
* @brief: helper function to display images
*
* @param[in] img: the image to display
*
* @param[in] name: the name of the window
*
*/
void display_img(cv::Mat& img, std::string name);

/**
* @brief: helper functions for user input to close windows created by display_img
*
* @param[in] delay: if 
*
* @see display_img(cv::Mat& img, std::string name)
*
*/
void wait_delete(int delay);

/**
* @brief: tests if somthing is in area based on reference image.
*
* @param[in] ref: the reference image
*
* @param[in] check: the image to check
*
* @param[in] Area: the area of the image to check, given as pixel coords
*
* @param[in] Thresh: minimum abs difference value after absolute subtraction of reference image and check image
*
* @param[in] min_Value: minimum area of blob needed in the area to constitute something being there.
*
* @return: boolean value, true being something is there, false meaning something is not
*/
bool Somthing_There_Test(cv::Mat& ref, cv::Mat& check, std::vector<cv::Point> Area, int thresh, int min_Value);

/***
* @brief: does the classification of the image/images on a multi-classification.
*
* @param[in] ref: the referecne image.
*
* @param[in] check: the image to be checked.
*
* @return: a vector of booleans for each classification.
*
*/
std::vector<bool> Classification(cv::Mat& ref, cv::Mat& check);

/**
* @brief: checks if something is in a single area, does not care for multi-classification
*
* @param[in] area: the area that the user wants to check
*
* @param[in] filename: the file to where the images are stored
*
* @param[in] ref: the reference image.
*/
void test_single(std::string area, std::string filename, cv::Mat& ref);

#endif
