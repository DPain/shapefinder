// shapefinder.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "shapefinder.h"

map<string, string> args;

int main(int argc, char* argv[]) {
  load_arguments(argc, argv);

  detect_shape();

  cin.get();
  return 0;
}

void load_arguments(int argc, char* argv[]) {
  int cnt = (argc - 1) / 2;

  cout << "num_arguments: " << cnt << endl;

  for (int i = 0; i < cnt; i++) {
    args[argv[(2 * i) + 1]] = argv[(2 * i) + 2];
  }

  for (auto it = args.cbegin(); it != args.cend(); it++) {
    cout << "args[\"" << it->first << "\"] = \"" << it->second << "\"\n";
  }
}

short* get_rgb(string hex_string) {
  if (hex_string.at(0) == '#') {
    hex_string.erase(0, 1);
  }
  short* output = (short*)malloc(sizeof(short) * 3);
  output[0] = stoi(hex_string.substr(0, 2), nullptr, 16);
  output[1] = stoi(hex_string.substr(2, 2), nullptr, 16);
  output[2] = stoi(hex_string.substr(4, 2), nullptr, 16);
  return output;
}

void detect_shape() {
  short* rgb_color = get_rgb(args["-c"]);

  string file_directory = "rsc/" + args["-i"] + ".ppm";
  cout << "Reading image: " << file_directory << endl;

  cv::Mat img = cv::imread(file_directory);
  if (img.empty()) {
    return;
  }

  // cout << "Image data: " << img << endl;

  // Creates a grayscale version of the image.
  cv::Mat gray;
  cv::cvtColor(img, gray, CV_BGR2GRAY);

  cv::Mat edges;

  // Using otsu thresholding to get low & high value.
  double high =
      cv::threshold(gray, edges, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  double low = 0.5 * high;
  // Creates an image that contains edges based on the grayscale image.
  cv::Canny(gray, edges, low, high);

  // Find contours
  vector<vector<cv::Point> > contours;
  cv::findContours(edges.clone(), contours, CV_RETR_EXTERNAL,
                   CV_CHAIN_APPROX_SIMPLE);

  cv::Mat output = img.clone();

  for (int i = 0; i < contours.size(); i++) {
    vector<cv::Point> approx;
    // Approximate contour with accuracy proportional
    // to the contour perimeter
    cv::approxPolyDP(cv::Mat(contours[i]), approx,
                     cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

    // Skip small or non-convex objects
    if (fabs(cv::contourArea(contours[i])) <
            (img.size().height * img.size().width) / 2500 ||
        !cv::isContourConvex(approx)) {
      continue;
    }

    if (approx.size() == 4) {
      /// Approximate contours to polygons + get bounding rects and circles
      vector<vector<cv::Point> > contours_poly(contours.size());
      vector<cv::Rect> boundRect(contours.size());
      vector<cv::Point2f> center(contours.size());
      vector<float> radius(contours.size());

      for (int i = 0; i < contours.size(); i++) {
        bool isSquare = false;

        approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
        cv::minEnclosingCircle((cv::Mat)contours_poly[i], center[i], radius[i]);

        double bottom_x = approx[1].x - (fabs(approx[3].x - approx[1].x) / 20);
        double bottom_y = approx[1].y + (fabs(approx[3].y - approx[1].y) / 20);

        double top_x = approx[3].x + (fabs(approx[3].x - approx[0].x) / 20);
        double top_y = approx[3].y - (fabs(approx[1].y - approx[0].y) / 20);

        for (int i = 0; i < boundRect.size(); i++) {
          if (boundRect[i].height - boundRect[i].width == 0) {
            // Square
            isSquare = true;
          } else {
            // Rectangle
            isSquare = false;
          }

          if (isSquare && args["-s"] == "square" ||
              !isSquare && args["-s"] == "rectangle") {
            cv::rectangle(
                output, cv::Point(bottom_x, bottom_y), cv::Point(top_x, top_y),
                cv::Scalar(rgb_color[2], rgb_color[1], rgb_color[0]), 3, 8, 0);
          }
        }
      }

    } else {
      vector<cv::Vec3f> circles;

      /// Apply the Hough Transform to find the circles
      cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows / 8, 200,
                       100, 0, 0);

      /// Draw the circles detected
      for (size_t i = 0; i < circles.size(); i++) {
        cv::Point center =
            cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]) + (cvRound(circles[i][2]) / 10);

        if (args["-s"] == "circle") {
          // circle outline
          cv::circle(output, center, radius,
                     cv::Scalar(rgb_color[2], rgb_color[1], rgb_color[0]), 3, 8,
                     0);
        }
      }
    }
  }

  cv::namedWindow("output", cv::WINDOW_NORMAL);
  cv::imshow("output", output);
  cv::namedWindow("edges", cv::WINDOW_NORMAL);
  cv::imshow("edges", edges);

  cv::imwrite(args["-o"], output);

  cv::waitKey(0);
  cv::destroyAllWindows();

  cout << "detected a shape!" << endl;

  free(rgb_color);
}
