#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CUBE 3

using namespace std;
using namespace cv;


// String which_color(Scalar s, map<char*, Scalar> colors) {
//
// }

int main(int argc, char const *argv[]) {

  VideoCapture cap(0);
  if (!cap.isOpened())
    return -1;

  Mat img;
  Mat cube;

  cap >> img;
  int w = img.size().width;
  int h = img.size().height;
  int space_between = 50;
  int square_side = 100;
  int space_x = (w - (CUBE * square_side + (CUBE - 1) * space_between)) / 2;
  int space_y = (h - (CUBE * square_side + (CUBE - 1) * space_between)) / 2;


  int side_small = 20;
  int space_small_1 = 6;
  int space_small_2 = 2;

  // map<char*, Scalar> std_colors = {
  //   { "red", Scalar(0, 0, 255) },
  //   { "green", Scalar(0, 255, 0) },
  //   { "blue", Scalar(255, 0, 0) },
  //   { "yellow", Scalar(0, 0, 255) },
  //   { "orange", Scalar(0, 0, 255) },
  //   { "white", Scalar(255, 255, 255) }
  // };




  Rect s[CUBE][CUBE];
  Scalar colors[CUBE][CUBE];
  Rect cube_small[6][CUBE][CUBE];
  Scalar cube_colors[6][CUBE][CUBE];
  Mat masks[CUBE][CUBE];
  for (int i = 0; i < CUBE; i++) {
    for (int j = 0; j < CUBE; j++) {
      s[i][j] = Rect(space_x + j * (square_side + space_between), space_y + i * (square_side + space_between), square_side, square_side);
      masks[i][j] = Mat::zeros(h, w, CV_8UC1);
      rectangle(masks[i][j], s[i][j], Scalar(255, 255, 255), -1);
    }
  }





  int offsets[4];


  for (int i = 0; i < 4; i++) {
    offsets[i] = space_small_1 + i * (space_small_1 + side_small * 3 + space_small_2 * 2);
  }

  for (int side = 0; side < 6; side++) {
    for (int i = 0; i < CUBE; i++) {
      for (int j = 0; j < CUBE; j++) {
        if (side <= 3) {
          cube_small[side][i][j] = Rect(
                offsets[side] + j * (space_small_2 + side_small),
                offsets[1] + i * (space_small_2 + side_small),
                side_small,
                side_small
              );

        } else {
          if (side == 4) {
            cube_small[side][i][j] = Rect(
                  offsets[1] + j * (space_small_2 + side_small),
                  offsets[0] + i * (space_small_2 + side_small),
                  side_small,
                  side_small
                );
          } else {
            cube_small[side][i][j] = Rect(
                  offsets[1] + j * (space_small_2 + side_small),
                  offsets[2] + i * (space_small_2 + side_small),
                  side_small,
                  side_small
                );
          }

        }

      }
    }
  }

  cube = Mat::zeros(space_small_1 + (offsets[1] - space_small_1) * 3, space_small_1 + (offsets[1] - space_small_1) * 4, CV_8UC3);









  //namedWindow("img", 1);
  //namedWindow("cube", 1);
  for (;; )
  {
    Mat frame;
    cap >> frame;       // get a new frame from camera



    for (int i = 0; i < CUBE; i++) {
      for (int j = 0; j < CUBE; j++) {


        colors[i][j] = mean(frame, masks[i][j]);

        rectangle(frame, s[i][j], colors[i][j], 4);
        //usleep(10000);
      }
    }

    for (int side = 0; side < 6; side++) {
      for (int i = 0; i < CUBE; i++) {
        for (int j = 0; j < CUBE; j++) {
          rectangle(cube, cube_small[side][i][j], Scalar(255, 255, 255), -1);
        }
      }
    }







    char key = (char) waitKey(30);
    if (key == 27) {
      cout << "BYE BYE!" << endl;
      break;
    } else if (key == ' ') {
      cout << "NEW CONF" << endl;
    }






    imshow("img", frame);
    imshow("cube", cube);
    // if (waitKey(30) >= 0) break;
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;

}
