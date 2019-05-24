#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {

  VideoCapture cap(0);
  if (!cap.isOpened())
    return -1;

  Mat img;

  cap >> img;
  int w = img.size().width;
  int h = img.size().height;

  int rect_side = 300;

  Rect roi = Rect(w / 2 - rect_side / 2, h / 2 - rect_side / 2, rect_side, rect_side);

  Mat mask = Mat::zeros(h, w, CV_8UC1);
  rectangle(mask, roi, Scalar(255, 255, 255), -1);

  Scalar color;
  int side = 0;
  char side_color[6] = {'r', 'g', 'b', 'y', 'o', 'w'};

  for (;; )
  {
    Mat frame;
    cap >> frame;

    color = mean(frame, mask);

    rectangle(frame, roi, color, 10);

    switch (side) {
    case 0:
      putText(frame, "RED", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;
    case 1:
      putText(frame, "GREEN", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;
    case 2:
      putText(frame, "BLUE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;
    case 3:
      putText(frame, "YELLOW", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;
    case 4:
      putText(frame, "ORANGE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;
    case 5:
      putText(frame, "WHITE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
      break;

    }


    char key = (char) waitKey(30);
    if (key == 27) {
      cout << "BYE BYE!" << endl;
      break;
    } else if (key == ' ') {

      cout << "{ \'" << side_color[side] << "', Scalar(" << color.val[0] << ", " << color.val[1] << ", " << color.val[2] << ") }," << endl;
      side++;

    }

    imshow("img", frame);

  }
  return 0;

}
