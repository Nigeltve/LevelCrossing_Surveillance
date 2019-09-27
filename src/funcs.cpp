#include "funcs.h"

void wait_delete(int delay) {
	cv::waitKey(delay);
	cv::destroyAllWindows();
}


void display_img(cv::Mat& img, std::string name) {
	cv::namedWindow(name);
	cv::imshow(name, img);
}


bool Somthing_There_Test(cv::Mat& ref, cv::Mat& check, std::vector<cv::Point> Area, int thresh, int min_Value) {
	//firsts blurs the image so that the differences are better to detect
	cv::Mat ref_blur, check_blur, ref_exp, check_exp;
	cv::GaussianBlur(ref, ref_blur, cv::Size(9, 9), 4);
	cv::GaussianBlur(check, check_blur, cv::Size(9, 9), 4);

	//takes the absolute differenc
	cv::Mat img_diff, img_thresh;
	cv::absdiff(ref_blur, check_blur, img_diff);
	cv::threshold(img_diff, img_thresh, min_Value, 255, CV_THRESH_BINARY); //theshold with min_value


	int dilation_size = 3;//sets the dilation size
	int loop = 1; //sets how many times itll dilate and erode
	for (int iter = 0; iter < loop; iter++) {
		//erodes fist and I want to get rid of small white spaces first
		//then dilates to get back to where it was.
		cv::erode(img_thresh, img_thresh, cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
			cv::Point(dilation_size, dilation_size)));

		cv::dilate(img_thresh, img_thresh, cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
			cv::Point(dilation_size, dilation_size)));
	}
	//sets the area for mask
	std::vector<std::vector<cv::Point>> pts_total = { Area };

	//makes empty mask the fills it with given area
	cv::Mat mask_Area = cv::Mat(img_diff.rows, img_diff.cols, img_diff.type(), cv::Scalar(0));
	cv::fillPoly(mask_Area, pts_total, cv::Scalar(255));

	//gets only the region of interst ie only differences big enough in a given area can show in immage
	cv::Mat mask_thresh;
	cv::bitwise_and(mask_Area, img_thresh, mask_thresh);

	//sets where teh contours and hierarchy will be saved
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	//findes controus
	cv::findContours(mask_thresh, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//get the area total
	double area = 0;
	for (int i = 0; i < contours.size(); i++) {
		area += cv::contourArea(contours[i]);
	}
	//calculates what the average is
	double avg_area = area / contours.size();

	//check if its big enough
	if (avg_area > thresh) {
		return true;
	}

	//if it hasnt returned true then return false.
	return false;

}


std::vector<bool> Classification(cv::Mat& ref, cv::Mat& check) {
	//sets whats states are true
	// Order - > { Train, OnTrack, Barrier, Entering, Leaving, Empty }
	std::vector<bool> states = { false, false, false, false, false , false };

	///////////////////// sets all the areas /////////////////////

	//sets on track area
	std::vector<cv::Point> OnTrack;
	OnTrack.push_back(cv::Point(171, 367));
	OnTrack.push_back(cv::Point(556, 137));
	OnTrack.push_back(cv::Point(694, 203));
	OnTrack.push_back(cv::Point(275, 446));

	//////////////////////////////////////////////////////////////

	//sets the Train areas
	std::vector<cv::Point > Train_Upper;
	Train_Upper.push_back(cv::Point(584, 156));
	Train_Upper.push_back(cv::Point(693, 217));
	Train_Upper.push_back(cv::Point(693, 92));

	std::vector<cv::Point> Train_Lower;
	Train_Lower.push_back(cv::Point(300, 462));
	Train_Lower.push_back(cv::Point(22, 462));
	Train_Lower.push_back(cv::Point(22, 438));
	Train_Lower.push_back(cv::Point(248, 336));
	Train_Lower.push_back(cv::Point(350, 440));

	//////////////////////////////////////////////////////////////

	//sets the barrier areas
	std::vector<cv::Point> Barrier_Area_Right;
	Barrier_Area_Right.push_back(cv::Point(234, 189));
	Barrier_Area_Right.push_back(cv::Point(340, 152));
	Barrier_Area_Right.push_back(cv::Point(353, 167));
	Barrier_Area_Right.push_back(cv::Point(247, 206));

	std::vector<cv::Point> Barrier_Area_Left;
	Barrier_Area_Left.push_back(cv::Point(86, 251));
	Barrier_Area_Left.push_back(cv::Point(94, 260));
	Barrier_Area_Left.push_back(cv::Point(32, 282));
	Barrier_Area_Left.push_back(cv::Point(28, 274));

	//////////////////////////////////////////////////////////////

	//sets the Leaving areas
	std::vector <cv::Point> Leaving_Upper;
	Leaving_Upper.push_back(cv::Point(125, 54));
	Leaving_Upper.push_back(cv::Point(261, 54));
	Leaving_Upper.push_back(cv::Point(518, 141));
	Leaving_Upper.push_back(cv::Point(416, 205));

	std::vector<cv::Point> Leaving_Lower;
	Leaving_Lower.push_back(cv::Point(348, 432));
	Leaving_Lower.push_back(cv::Point(518, 318));
	Leaving_Lower.push_back(cv::Point(692, 427));
	Leaving_Lower.push_back(cv::Point(692, 462));
	Leaving_Lower.push_back(cv::Point(382, 462));

	//////////////////////////////////////////////////////////////

	//sets the entering Areas
	std::vector <cv::Point> Entering_Upper;
	Entering_Upper.push_back(cv::Point(33, 55));
	Entering_Upper.push_back(cv::Point(87, 55));
	Entering_Upper.push_back(cv::Point(340, 186));
	Entering_Upper.push_back(cv::Point(152, 273));
	Entering_Upper.push_back(cv::Point(33, 185));

	std::vector<cv::Point> Entering_Lower;
	Entering_Lower.push_back(cv::Point(569, 283));
	Entering_Lower.push_back(cv::Point(674, 208));
	Entering_Lower.push_back(cv::Point(690, 218));
	Entering_Lower.push_back(cv::Point(690, 364));

	//////////////////////////////////////////////////////////////

	//Train Test
	if (Somthing_There_Test(ref, check, Train_Upper, 200, 40) && Somthing_There_Test(ref, check, Train_Lower, 200, 40)) {
		states[0] = true;

		// sets new points for Entering and leaving Upper Areas
		// if train is there
		Entering_Upper.clear();
		Entering_Upper.push_back(cv::Point(24, 55));
		Entering_Upper.push_back(cv::Point(75, 55));
		Entering_Upper.push_back(cv::Point(214, 139));
		Entering_Upper.push_back(cv::Point(24, 214));

		Leaving_Upper.clear();
		Leaving_Upper.push_back(cv::Point(77, 55));
		Leaving_Upper.push_back(cv::Point(259, 55));
		Leaving_Upper.push_back(cv::Point(355, 87));
		Leaving_Upper.push_back(cv::Point(222, 139));

	}
	//if train isnt there check if somthing is on the track andd if the barrier is deployed or not
	else {

		//checks onTrack
		if (Somthing_There_Test(ref, check, OnTrack, 100, 40)) {
			states[1] = true;
		}

		//checks if both left and right Barrier areas have somting in them.
		if (Somthing_There_Test(ref, check, Barrier_Area_Left, 20, 20) && Somthing_There_Test(ref, check, Barrier_Area_Right, 20, 20)) {
			states[2] = true;
		}
	}

	//check for changes in either Upper or Lower Entering Regions.
	if (Somthing_There_Test(ref, check, Entering_Upper, 200, 50) || Somthing_There_Test(ref, check, Entering_Lower, 200, 40)) {
		states[3] = true;
	}

	//check for changes in either Upper or Lower Leaving Regions.
	if (Somthing_There_Test(ref, check, Leaving_Upper, 200, 50) || Somthing_There_Test(ref, check, Leaving_Lower, 200, 40)) {
		states[4] = true;
	}

	//goes through the first 5 cases and checks if none of them are true
	for (unsigned int idx = 0; idx < states.size() - 1; idx++) {
		if (states[idx]) {
			//if one was true, breaks and sets emtpy to false
			states[5] = false;
			break;
		}
		else {
			//else sets to true (I know this is putting empty to true mulitple times)
			states[5] = true;
		}
	}

	//returns the classification vector
	return states;
}


void test_single(std::string area, std::string filepath, cv::Mat& ref) {
	auto start = std::chrono::high_resolution_clock::now();
	int index; //needs to know what part of the classification vector to look at

	//checks to see what area has been picked and sets the index accordingly
	if (area == "Ontrack") {
		std::cout << "You have Chosen: Ontrack" << std::endl;
		index = 1;
	}
	else if (area == "Train") {
		std::cout << "You have Chosen: Train" << std::endl;
		index = 0;
	}
	else if (area == "Barrier") {
		std::cout << "You have Chosen: Barrier" << std::endl;
		index = 2;
	}
	else if (area == "Leaving") {
		std::cout << "You have Chosen: Leaving" << std::endl;
		index = 4;
	}
	else if (area == "Entering") {
		std::cout << "You have Chosen: Entering" << std::endl;
		index = 3;
	}
	else if (area == "Empty") {
		std::cout << "You have Chosen: Empty" << std::endl;
		index = 4;
	}
	else {
		std::cout << "You have not selected an area" << std::endl;
		return;
	}


	std::vector<cv::String> filenames; //where it stores all file names
	cv::glob(filepath, filenames); //gets files

	double img_correct = 0; //see how many it got right
	double img_count = 0; //counts how many images are there

	for (unsigned int i = 0; i < filenames.size(); i++) {
		//goes through all the files, reads them in, and checks if they are right by the index
		// in the classification vector
		std::cout << filenames[i] << std::endl;
		cv::Mat check = imread(filenames[i], 0);
		img_count++;
		std::vector<bool> img_classification = Classification(ref, check);
		if (img_classification[index]) {
			img_correct++;
		}
	}
	//tries to find area in path
	size_t area_there = filepath.find(area);

	//sets the score
	double score = 0;
	if (area_there != std::string::npos) {
		//if area same as data set label, 100% is what the classification is
		score = (img_correct / img_count) * 100;
		std::cout << "Got " << score << "% right" << std::endl;
	}
	else {
		// if area same as data set lable, 100% is what classification is not.
		// meaning classification for Entering on a leaving set is meant to give 0%, but its 100% right
		std::cout << "looking at and set are not the same!" << std::endl;
		score = 100 - ((img_correct / img_count) * 100);
		std::cout << "Got " << score << "% right" << std::endl;
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> duration = end - start;
	std::cout << "number of files tested: " << img_count << std::endl;
	std::cout << "Single testing time Elasped: " << duration.count() << std::endl;
	std::cout << "time for single classification: " << duration.count() / img_count << std::endl;
}
