#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstdlib>
#include <map>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "search.h"

#define CUBE 3

using namespace std;
using namespace cv;

int compute_score(Scalar base, Scalar color) {
  int score = 0;
  for (int i = 0; i < 3; i++) {
    score += abs(base.val[i] - color.val[i]);
  }
  return score;
}

char color_to_position(char c) {

  switch (c) {
  case 'y':
    return 'U';
    break;
  case 'o':
    return 'F';
    break;
  case 'g':
    return 'L';
    break;
  case 'b':
    return 'R';
    break;
  case 'r':
    return 'B';
    break;
  case 'w':
    return 'D';
    break;
  }

  return 'X';
}


char detect_color(Scalar color, map<char, Scalar> colors) {
  char c = 0;
  int score = 1000, new_score;
  for (map<char, Scalar>::iterator it = colors.begin(); it != colors.end(); ++it) {
    new_score = compute_score(it->second, color);
    if (new_score < score) {
      score = new_score;
      c = it->first;
    }
  }

  return c;
}

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

  map<char, Scalar> std_colors = {
    { 'r', Scalar(93.4559, 56.6308, 195.836) },
{ 'g', Scalar(101.331, 192.6, 99.5844) },
{ 'b', Scalar(199.217, 129.722, 33.9943) },
{ 'y', Scalar(114.743, 166.014, 160.418) },
{ 'o', Scalar(69.1428, 108.128, 214.683) },
{ 'w', Scalar(149.273, 148.158, 146.645) },
  };

  Rect s[CUBE][CUBE];
  Scalar colors[CUBE][CUBE];
  Rect cube_small[6][CUBE][CUBE];
  Scalar cube_colors[6][CUBE][CUBE];
  char cube_codes[6][CUBE][CUBE];
  Mat masks[CUBE][CUBE];
  for (int i = 0; i < CUBE; i++) {
    for (int j = 0; j < CUBE; j++) {
      s[i][j] = Rect(space_x + j * (square_side + space_between), space_y + i * (square_side + space_between), square_side, square_side);
      masks[i][j] = Mat::zeros(h, w, CV_8UC1);
      rectangle(masks[i][j], s[i][j], Scalar(255, 255, 255), -1);
    }
  }

  int offsets[4];
  char kociemba_input[54];

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

  ofstream myfile;
  myfile.open("state.txt");

  cube = Mat::zeros(space_small_1 + (offsets[1] - space_small_1) * 3, space_small_1 + (offsets[1] - space_small_1) * 4, CV_8UC3);
////U R F D B L
  char side_order_color[6] = {'y', 'b', 'o', 'w', 'g', 'r'};

  int side_order[6] = {4, 2, 1, 5, 0, 3};

  int side_color = 0;
  int kociemba_flag = 0;

  for (;; )
  {
    Mat frame;
    cap >> frame;


    if (side_color < 6) {
      for (int i = 0; i < CUBE; i++) {
        for (int j = 0; j < CUBE; j++) {
          if (i != 1 || j != 1) {
            colors[i][j] = mean(frame, masks[i][j]);

            char clr = detect_color(colors[i][j], std_colors);
            colors[i][j] = std_colors.at(clr);
            cube_colors[side_order[side_color]][i][j] = colors[i][j];

            rectangle(frame, s[i][j], colors[i][j], 4);

            cube_codes[side_order[side_color]][i][j] = color_to_position(clr);

          } else {
            colors[i][j] = std_colors.at(side_order_color[side_color]);
            cube_colors[side_order[side_color]][i][j] = colors[i][j];

            rectangle(frame, s[i][j], colors[i][j], 4);

            cube_codes[side_order[side_color]][i][j] = color_to_position(side_order_color[side_color]);
          }

        }
      }

      for (int side = 0; side < 6; side++) {
        for (int i = 0; i < CUBE; i++) {
          for (int j = 0; j < CUBE; j++) {

            rectangle(cube, cube_small[side][i][j], cube_colors[side][i][j], -1);
          }
        }
      }


      switch (side_order[side_color]) {
      case 0:
        putText(frame, "GREEN", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;
      case 1:
        putText(frame, "ORANGE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;
      case 2:
        putText(frame, "BLUE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;
      case 3:
        putText(frame, "RED", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;
      case 4:
        putText(frame, "YELLOW", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;
      case 5:
        putText(frame, "WHITE", cvPoint(20, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
        break;

      }
    }

    char key = (char) waitKey(30);
    if (key == 27) {
      cout << "BYE BYE!" << endl;
      break;
    } else if (key == ' ') {

      side_color++;
      //cout << "NEW CONF" << endl;


    } else if (key == 'x') {
      side_color--;
      kociemba_flag = 0;
    } else if (key == 's' && kociemba_flag == 1) {
      cout << "SENDING TO KOCIEMBA" << endl;
      for (int i = 0; i < 54; i++) {
        printf("%c", kociemba_input[i]);
        myfile << kociemba_input[i];
      }
      printf("\n");
      myfile << endl;
    }

    if (side_color == 6 && kociemba_flag == 0) {
      for (int side = 0; side < 6; side++) {
        for (int i = 0; i < CUBE; i++) {
          for (int j = 0; j < CUBE; j++) {
            //printf("%d ", side * 9 + i * 3 + j);

            kociemba_input[side * 9 + i * 3 + j] = cube_codes[side_order[side]][i][j];
          }
          //printf("\n");

        }
        //printf("\n\n\n");

      }
      kociemba_flag = 1;

    }

    imshow("img", frame);
    imshow("cube", cube);
    // if (waitKey(30) >= 0) break;
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;

}
