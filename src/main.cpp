/*
	Code written by: Nigel Thornewill von Essen
	Student Numbe: s304434

	This Code if for the Computer Vision assignemnt where we had to make a
	survalence system for a level crossing where we had to classify a image
	into 6 difference catagories
		1. OnTrack: railway is not clear from Cars and Padestrian
		2. Entering: Car is entering the level crossing
		3. Leaving: Car is Leaving the level crossing
		4. Barrier: barrier was deployed safly
		5. Train: Train is using level crossing

		if there are no events, then it returns a '0'

	The .exe takes a directory of images to classify, and a optional parameter
	to show the images.
		where not putting anything in doesnt show images and a 1 shows and stops at each image to
		check classification.

	there are 4 functions
	display_image:
		takes a image and a image name and displays the given image

	wait_delete:
		waits for user input then moves on deleteing the window of current file

	somthing_there_test:
		This returns a boolean value saying if a somthing is there in a given area.
			- takes reference image, image to check, area to check and thresholding values.

	Classification:
		this returns a vector of booleans that has a multiclass classification for each senerio.


	the main function goes through each image in a given directory and classifies the image based of a
	refereance image and displays the following
		filename: event N event N ...ect

	ways to use it
		1. program.exe filedirectory\*.png					this gies classification of each image set
		2. program.exe 1 filedirectory\*.png				this gets the classification and displays image set, waits for user
		2. program.exe 2 checkArea filedirectory\*.png      this gets the single accuracy accuraccy of check area and data set.

*/

//import libaries
#include <iostream>
#include <String>
#include <filesystem>
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

#include "funcs.h"

int main(int argc, char** argv) {
	
	//if not arguments passed then terminate the program
	if (argc < 2) {
		std::cout << "not the right number of arguments!" << std::endl;
		return -1;
	}

	//if the reference image couldnt be loaded terminate
	cv::Mat ref = cv::imread("..\\..\\CV_assignment\\reference.png", 0);
	if (ref.empty()) {
		std::cout << "[ERROR]: Referenc Image could not be loaded" << std::endl;
		return -1;
	}

	//sets the path to directory
	std::string path = argv[1];

	//where all the files will be stored for looping through
	std::vector<cv::String> filenames;
	cv::glob(path, filenames); //what puts paths into vector

	//for debugging
	if ((argc == 4) && (*argv[1] == '2')) {
		std::cout << argc << std::endl; // say how many arguments given
		std::cout << "Chosen to test out single Accuracy" << std::endl; //what type of test
		test_single(argv[2], argv[3], ref); //actual testing
		return -1;
	}

	//loops through all files in vector holding files
	for (unsigned int i = 0; i < filenames.size(); i++) {

		//reads image, if cant load -> terminate program
		cv::Mat check = cv::imread(filenames[i], 0);
		if (check.empty()) {
			std::cout << "[ERROR]: Check image could not be loaded" << std::endl;
			return -1;
		}

		//finds the cut point to just get the image name
		size_t cut_point = path.find('*');

		std::string img_name = "";

		//if cant find anything, just moves on
		if (cut_point != std::string::npos) {
			std::string img_name = filenames[i].c_str();
			img_name.erase(0, cut_point);
			std::cout << img_name << ": ";
		}

		//gives the classification of the image
		std::vector<bool> img_classifications = Classification(ref, check);

		//goes through the classification vector  and outputs the right result.
		//	see order up above
		for (int idx = 0; idx < img_classifications.size(); idx++) {
			switch (idx){
			case(0):
				//checkss train vlaue
				if (img_classifications[idx]) {
					std::cout << " Train 5 ";
				}
				break;
			case(1):
				//checks ontrack value
				if (img_classifications[idx]) {
					std::cout << " OnTrack 1 ";
				}
				break;
			case(2):
				//checks barrier value
				if (img_classifications[idx]) {
					std::cout << " Barrier 4 ";
				}
				break;
			case(3):
				//checks Entering value
				if (img_classifications[idx]) {
					std::cout << " Entering 2 ";
				}
				break;
			case(4):
				//checks leaving value
				if (img_classifications[idx]) {
					std::cout << " Leaving 3 ";
				}
				break;
			case(5):
				//checks empty value
				if (img_classifications[idx]) {
					std::cout << " 0 ";
				}
				break;

			default:
				//this should never run, but if it does, says somthign is wrong.
				std::cout << "Oops somthing went wrong!" << std::endl;
			}
		}
		std::cout << std::endl << std::endl;

		//if there is a second argument of 1 then show the immages and wait for user input
		if ((argc == 3) && (*argv[2] == '1')) {
			std::cout << "Chose to look at image and classify" << std::endl;
			display_img(check, img_name);
			wait_delete(0);
		}
	}

	return 0;
}
