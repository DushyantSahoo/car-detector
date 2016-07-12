
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <opencv2\core\core.hpp> 
#include <opencv2\highgui\highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\ocl\ocl.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <omp.h>
#include <cstdio>
#include <io.h>
#include <conio.h>
#include <sstream>
#include <sys/stat.h>
#include <fstream>
using namespace cv;
using namespace std;
using std::vector;
using std::string;
using cv::Mat;
using std::cout;
using std::cerr;
RNG rng(12345);
int threshold_value = 147;
int threshold_type = 3;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst,imgThresholded;
char* window_name = "Threshold Demo";

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

int edgeThresh = 1;
int lowThreshold=99;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;

vector<float> seperatehue(vector<float> inputhue){                         // sepearting contours which lie on the same car ( hue seperation)
	int lengthhue=inputhue.size();
	for(int k=0;k<lengthhue;k++){
		for(int l =k+1;l<lengthhue;l++){
			if(inputhue.at(k)<=inputhue.at(l)+1 && inputhue.at(k)>=inputhue.at(l)-1 )
			{
				inputhue.erase (inputhue.begin()+l-1);
				lengthhue=lengthhue-1;
			}
		}
	}
	return inputhue;
}

// function to find the contour with maximum size

float averagecolor(Mat &input){                            //   finding the average hue in a given contour
	float average=0;
	Mat newimage;
	int timer=0;
	cvtColor(input, newimage, COLOR_BGR2HSV);
	int c = input.cols;
	int r = input.rows;
	vector<Mat> hsv_planes;
	split( newimage, hsv_planes );
	for(int i=0;i<r;i++)
	{
		for(int j=0;j<c;j++)
		{
			if(hsv_planes[0].at<unsigned char>(i,j)!=0){
			 //cout<<"average"<<average<<"columns"<<c<<"rows"<<r;
				average=average+(hsv_planes[0].at<unsigned char>(i,j));
				timer++;
			}
			
		}
	}
	return average/timer;
}

int main(int argc, char* argv[])
{
	
		Mat src=imread("cars.png",1);
	
	imshow("original",src);
	// Create Polygon from vertices


	cvtColor( src, src_gray, CV_RGB2GRAY );

	 /// Create a window to display results
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar to choose type of Threshold
  createTrackbar( trackbar_type,
                  window_name, &threshold_type,
                  max_type );

  createTrackbar( trackbar_value,
                  window_name, &threshold_value,
                  max_value );
  // Create a trackbar to threshold the canny edge detector
   createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold );

   
   //loop to implement any change in the HSV value to make the output more accurate
   //while(true){
		threshold( src_gray, imgThresholded, threshold_value, max_BINARY_value,threshold_type );
      
		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );	//erosion using ellipse
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); //dilation using ellipse

		////morphological closing (fill small holes in the foreground)
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );		//erosion using ellipse
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );		//dilation using ellipse
		

		//high pass filtering (Canny)
		imshow("newx",imgThresholded);
		
		Mat canny_output;
	   
		Canny( imgThresholded, canny_output, lowThreshold, lowThreshold*ratio, kernel_size );				//canny filter for edge detection			
		imshow("canny",canny_output);
	    vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );	//find all the ontours

		// Draw contours
		
		Mat src1=imread("cars.png",1);
		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		int length=contours.size();
		vector<float> colorspace;
		//cout<<"contour size"<<contours.size();
		for( int i = 0; i< contours.size(); i++ )
			{
				//cout<<"number"<<i;
				Mat mask = Mat::zeros(src.size(), CV_8UC3);
				Mat imageROI;

				Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
				drawContours( mask, contours, i, Scalar(255, 255,255), CV_FILLED );
				src1.copyTo(imageROI, mask);
				colorspace.push_back(averagecolor(imageROI));
				cout<<"hue value"<<averagecolor(imageROI)<<endl;
			 }
		vector<float> newcolorspace=seperatehue(colorspace);	
		int pink=0;
		int green=0;
		int blue=0;// seperating hue 
		int yellow=0;
		cout<<"total number of vehicle"<<newcolorspace.size()<<endl;
		for (int g=0;g<newcolorspace.size();g++)					//printing the hue of the detected cars
			
		{
			cout<<"new hue"<<newcolorspace.at(g)<<endl;
			if (newcolorspace.at(g)<15 && newcolorspace.at(g)>0){
				pink=pink+1;
			}
			else if(newcolorspace.at(g)<115 && newcolorspace.at(g)>105){
				green++;
			}
			else if(newcolorspace.at(g)<180 && newcolorspace.at(g)>165){
				blue++;
			}
			else if
				(newcolorspace.at(g)<45 && newcolorspace.at(g)>15){
				yellow++;
			}
			else{
				;
			}
		}
		/// Show in a window
		cout<<pink<<"pink"<<green<<"green"<<blue<<"blue"<<yellow<<"yellow";
		namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
		imshow( "Contours", src1 );
		waitKey(0);
		//imwrite("detected.jpg",imageROI);				//save the image in detected.jpg
		//imshow("drawing only rectangle",contourRegion);
	//	if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
	//	{
 //           cout << "esc key is pressed by user" << endl;
 //           break; 
	//	}
	//}
	return 0;
}
