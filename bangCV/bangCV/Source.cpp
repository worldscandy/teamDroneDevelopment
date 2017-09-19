#include <opencv2/opencv.hpp>
#include <iostream> //追加
#include <vector> //追加

cv::Mat colorExtraction(cv::Mat* src, cv::Mat* dst,
	int code,
	int ch1Lower, int ch1Upper,
	int ch2Lower, int ch2Upper,
	int ch3Lower, int ch3Upper
);

int main() {
	using namespace cv;
	using namespace std; //追加
	int CornCounter = 0;
	Mat input;
	input = imread("origin.png", IMREAD_UNCHANGED);
	Mat gray;
	cvtColor(input, gray, CV_BGR2GRAY);
	Mat bin;
	//二値化はしているが使用していない by kamada
	threshold(gray, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);
	bin = ~bin;
	//resize(bin, bin, cv::Size(), 0.2, 0.2);
	//resize(input, input, cv::Size(),0.5 , 0.5);
	//imshow("before", input);
	Mat hsv;
	cvtColor(input,hsv,CV_BGR2HSV);
	resize(hsv, hsv, cv::Size(), 0.3, 0.3);
	//imshow("HSV", hsv);
	Mat extract;
	Mat extract2;
	extract2 = colorExtraction(&input,&extract,CV_BGR2HSV, 0,30, 0, 255,0, 255);
	imwrite("ex.png", extract2);
	//resize(extract, extract,cv::Size(), 0.3, 0.3);
	//imshow("broun", extract);
	//uchar hue, sat, val;
	////for (int y = 0; y < height; y++) {
	////	for (int x = 0; x < width; x++) {
	////		hue = hsv.at<Vec3b>(y ,x)[0];
	////		sat = hsv.at<Vec3b>(y, x)[1];
	////		val = hsv.at<Vec3b>(y, x)[2];
	////		if ((hue < 10 || hue > 150) && sat > 80) broun.at<uchar>(y, x) = 0;
	////		else broun.at<uchar>(y, x) = 255;
	////	}
	////	imshow("段ボール検出",broun);
	////}
	// 輪郭抽出
	int counter = 0;
	std::vector< std::vector< cv::Point > > contours;
	cv::findContours(extract2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// 輪郭線の描画
	for (auto contour = contours.begin(); contour != contours.end(); contour++) {
		cv::polylines(input, *contour, true, cv::Scalar(0, 255, 0), 2);
	}

	// 四角形判定
	for (auto contour = contours.begin(); contour != contours.end(); contour++) {
		// 輪郭を直線近似
		std::vector< cv::Point > approx;
		cv::approxPolyDP(cv::Mat(*contour), approx, 100.0, true);
		// 近似が4線かつ面積が100以上なら四角形
		double area = cv::contourArea(approx);
		if (approx.size() == 4 && area > 100.0 && area < 100000) {
			counter++;
			cv::polylines(input, approx, true, cv::Scalar(255, 0, 0), 2);
			std::stringstream sst;
			sst << "area : " << area;
			cv::putText(input, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 128, 0));
		}
	}



	cv::imwrite("result.png", input);
	resize(input, input, cv::Size(), 0.4, 0.4);
	cv::imshow("squerecheck", input);
	cout << counter << "squeres\n";
	
	//imwrite("after.png", input);
	cv:waitKey(0);
}
//	findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//	for (auto contour = contours.begin(); contour != contours.end(); contour++) {
//		cv::polylines(bin, *contour, true, cv::Scalar(0, 255, 0), 2);
//	}
//	for (auto contour = contours.begin(); contour != contours.end(); contour++) {
//		// 輪郭を直線近似
//		std::vector< cv::Point > approx;
//		cv::approxPolyDP(cv::Mat(*contour), approx, 50.0, true);
//		// 近似が4線かつ面積が一定以上なら四角形
//		double area = cv::contourArea(approx);
//		if (approx.size() == 4 && area > 100.0) {
//			CornCounter++;
//			cv::polylines(bin, approx, true, cv::Scalar(255, 0, 0), 2);
//			std::stringstream sst;
//			sst << "area : " << area;
//			cv::putText(bin, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 128, 0));
//		}
//	}
//	for (int i = 0; i<contours.size(); i++) {
//		int count = contours.at(i).size();
//		double x = 0; double y = 0;
//		for (int j = 0; j<count; j++) {
//			x += contours.at(i).at(j).x;
//			y += contours.at(i).at(j).y;
//		}
//		x /= count;
//		y /= count;
//		circle(input, Point(x, y), 5, Scalar(0, 0, 255), 2, 4);
//	}
//	cout << contours.size() << " boals" << endl;
//	cout << CornCounter << " squeres" << endl;
//	imshow("IMAGE",input);
//	cv:waitKey(0);
//}


cv::Mat colorExtraction(cv::Mat* src, cv::Mat* dst,
	int code,
	int ch1Lower, int ch1Upper,
	int ch2Lower, int ch2Upper,
	int ch3Lower, int ch3Upper
)
{
	cv::Mat colorImage;
	int lower[3];
	int upper[3];

	cv::Mat lut = cv::Mat(256, 1, CV_8UC3);

	cv::cvtColor(*src, colorImage, code);

	lower[0] = ch1Lower;
	lower[1] = ch2Lower;
	lower[2] = ch3Lower;

	upper[0] = ch1Upper;
	upper[1] = ch2Upper;
	upper[2] = ch3Upper;

	for (int i = 0; i < 256; i++) {
		for (int k = 0; k < 3; k++) {
			if (lower[k] <= upper[k]) {
				if ((lower[k] <= i) && (i <= upper[k])) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
			else {
				if ((i <= upper[k]) || (lower[k] <= i)) {
					lut.data[i*lut.step + k] = 255;
				}
				else {
					lut.data[i*lut.step + k] = 0;
				}
			}
		}
	}

	//LUTを使用して二値化
	cv::LUT(colorImage, lut, colorImage);

	//Channel毎に分解
	std::vector<cv::Mat> planes;
	cv::split(colorImage, planes);

	//マスクを作成
	cv::Mat maskImage;
	cv::bitwise_and(planes[0], planes[1], maskImage);
	cv::bitwise_and(maskImage, planes[2], maskImage);
	
	//cv::imshow("mask", maskImage);


	//出力
	cv::Mat maskedImage;
	src->copyTo(maskedImage, maskImage);
	*dst = maskedImage;

	return maskImage;
}