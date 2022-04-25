#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main() {

	VideoCapture cap("rtsp://192.168.10.12/live/ch00_1");
	Mat current_img , gray, thresh, firstimg, frameDelta;

	// 'contours' and 'hierarchy' are two variables we need for finding contours
	// 'contours' will contain all the contours points
	// 'hierarchy' value, we don't need here
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	// 'conPoly' variable will contains only neccessary contours points from 'contours' variable
	vector<Point>  conPoly;
	
	// boundRect will contains the points which will be required for drawing rectangle 
	Rect boundRect;

	
	
		
	cap.read(firstimg);
	resize(firstimg, firstimg, Size(600, 450));
	// converting it gray scale as colors as nothing to do with motion 
	cvtColor(firstimg, firstimg, COLOR_BGR2GRAY);
	GaussianBlur(firstimg, firstimg, Size(21, 21), 0);

		while (true) {
			cap.read(current_img);
			// resizing it we dont need to do processing on large image
			resize(current_img, current_img, Size(600, 450));
            // converting it gray scale as colors as nothing to do with motion 
			cvtColor(current_img, gray, COLOR_BGR2GRAY);
			

			GaussianBlur(gray, gray, Size(21, 21), 0);

			// compute the absolute difference between the current frameand first frame
			absdiff(firstimg, gray, frameDelta);

			// thresholding image so that the pixel with greater difference value will only be white
			threshold(frameDelta, thresh, 25, 255, THRESH_BINARY);

			// dilate the thresholded image to fill in holes, then find contours
			dilate(thresh, thresh, Mat(), Point(-1, -1), 2);

			// find contours
			findContours(thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

			// accessing contours of each moving object one by one
			for (int i = 0; i < contours.size(); i++) {
				// if contours area is too small we will not consider it
				if (contourArea(contours[i]) > 2000) {
					// finding perimeter
					float peri = arcLength(contours[i], true);
					// extracting only necessary contours point
					approxPolyDP(contours[i], conPoly, 0.02 * peri, true);

					// finding points, height and width for rectangle using neccessary contours in 'conPoly'
					boundRect = boundingRect(conPoly);

					// Now we will draw rectangle around every moving object
					// boundRect.tl() --> top left points of rectangle
					// boundRect.br() --> bottom right points of rectangle 
					rectangle(current_img, Point(boundRect.x,boundRect.y+50), 
						Point(boundRect.x+ boundRect.width, boundRect.y + boundRect.height- 10), Scalar(255, 0, 255), 5);
					
					putText(current_img, "Moving", {boundRect.x,boundRect.y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 0), 1);
				}
			}

			// displaying the image
			imshow("image", current_img);

			// if Esc is pressed loop will be  terminated
			if (waitKey(1) == 27) {
				break;
			}

		}
		destroyAllWindows();
}