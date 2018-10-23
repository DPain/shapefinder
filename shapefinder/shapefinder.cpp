// shapefinder.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "shapefinder.h"
#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

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

void detect_shape() {
  string file_directory = "rsc/" + args["-s"] + ".ppm";
  cout << "Reading image: " << file_directory << endl;

  Mat img = imread(file_directory);

  namedWindow("image", WINDOW_NORMAL);
  imshow("image", img);
  waitKey(0);

  cout << "detected a shape!" << endl;
}
