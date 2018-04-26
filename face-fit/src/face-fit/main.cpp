// CSIRO has filed various patents which cover the Software. 

// CSIRO grants to you a license to any patents granted for inventions
// implemented by the Software for academic, research and non-commercial
// use only.

// CSIRO hereby reserves all rights to its inventions implemented by the
// Software and any patents subsequently granted for those inventions
// that are not expressly granted to you.  Should you wish to license the
// patents relating to the Software for commercial use please contact
// CSIRO IP & Licensing, Gautam Tendulkar (gautam.tendulkar@csiro.au) or
// Nick Marsh (nick.marsh@csiro.au)

// This software is provided under the CSIRO OPEN SOURCE LICENSE
// (GPL2) which can be found in the LICENSE file located in the top
// most directory of the source code.

// Copyright CSIRO 2013

#include "utils/helpers.hpp"
#include "utils/command-line-arguments.hpp"
#include "utils/points.hpp"
#include "tracker/FaceTracker.hpp"
#include "matrix.h"
#include "rbf.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

using namespace FACETRACKER;

#pragma pack(push, 1) // 取消内存大小自动对齐
struct face_fit_msg{
unsigned char packages_head[2] ={0xff, 0xfe};
double face_fit_data[222];
unsigned char package_end[3] = {0xbe,0xef,'\0'};
};
#pragma pack(pop)

void
print_usage()
{
  std::string text =
    "Usage: [options] <image-argument> [landmarks-argument]\n"
    "\n"
    "Options:\n"
    "  --help                    This helpful message.\n"    
    "  --lists                   Switch to list processing mode. See below.\n"
    "  --video                   Switch to video processing mode. See below.\n"
    "  --wait-time <double>      How many seconds to wait when displaying the\n"
    "                            registration results. The default depends on the mode.\n"
    "  --model <pathname>        The pathname to the tracker model to use.\n"
    "  --params <pathname>       The pathname to the parameters to use.\n"
    "  --threshold <int>         The threshold of the error detector.\n"
    "                            Can range from 0 to 10 where 10 is extremely picky.\n"
    "                            The default is 5.\n"
    "  --title <string>          The window title to use.\n"                  
    "  --3d                      Save 3D shape instead of the 2D shape.\n"
    "  --verbose                 Display information whilst processing.\n"
    "\n"
    "Default mode:\n"
    "Perform fitting on an image located at <image-argument> and save\n"
    "the results to [landarks-argument] if specified, otherwise display\n"
    "the results.\n"
    "\n"
    "List mode:\n"
    "Perform fitting on the list of image pathnames contained in <image-argument>.\n"
    "If [landmarks-argument] is specified, then it must be a list with the same length as\n"
    "<image-argument> and contain pathnames to write the tracked points to.\n"
    "\n"
    "Video mode:\n"
    "Perform fitting on the video found at <image-pathname>. If\n"  
    "[landmarks-argument] is specified, then it represents a format string\n"
    "used by sprintf. The template must accept at most one unsigned integer\n"
    "value. If no [landmarks-argument] is given, then the tracking is displayed\n"
    "to the screen.\n"
    "\n";
  
  std::cout << text << std::endl;
}

class user_pressed_escape : public std::exception
{
public:
};

struct Configuration
{
  double wait_time;
  std::string model_pathname;
  std::string params_pathname;
  int tracking_threshold;
  std::string window_title;
  bool verbose;
  bool save_3d_points;

  int circle_radius;
  int circle_thickness;
  int circle_linetype;
  int circle_shift;
};

int run_lists_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

int run_video_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

int run_image_mode(const Configuration &cfg,
		   const CommandLineArgument<std::string> &image_argument,
		   const CommandLineArgument<std::string> &landmarks_argument);

void display_data(const Configuration &cfg,
		  const cv::Mat &image,
		  const std::vector<cv::Point_<double> > &points,
		  const Pose &pose);

int
run_program(int argc, char **argv)
{
  CommandLineArgument<std::string> image_argument;
  CommandLineArgument<std::string> landmarks_argument;

  bool lists_mode = false;
  bool video_mode = false;
  bool wait_time_specified = false;

  Configuration cfg;
  cfg.wait_time = 0;
  cfg.model_pathname = DefaultFaceTrackerModelPathname();
  cfg.params_pathname = DefaultFaceTrackerParamsPathname();
  cfg.tracking_threshold = 5;
  cfg.window_title = "CSIRO Face Fit";
  cfg.verbose = false;
  cfg.circle_radius = 2;
  cfg.circle_thickness = 1;
  cfg.circle_linetype = 8;
  cfg.circle_shift = 0;  
  cfg.save_3d_points = false;

  for (int i = 1; i < argc; i++) {
    std::string argument(argv[i]);

    if ((argument == "--help") || (argument == "-h")) {
      print_usage();
      return 0;
    } else if (argument == "--lists") {
      lists_mode = true;
    } else if (argument == "--video") {
      video_mode = true;
    } else if (argument == "--wait-time") {
      wait_time_specified = true;
      cfg.wait_time = get_argument<double>(&i, argc, argv);
    } else if (argument == "--model") {
      cfg.model_pathname = get_argument(&i, argc, argv);
    } else if (argument == "--params") {
      cfg.params_pathname = get_argument(&i, argc, argv);
    } else if (argument == "--title") {
      cfg.window_title = get_argument(&i, argc, argv);
    } else if (argument == "--threshold") {
      cfg.tracking_threshold = get_argument<int>(&i, argc, argv);
    } else if (argument == "--verbose") {
      cfg.verbose = true;
    } else if (argument == "--3d") {
      cfg.save_3d_points = true;
    } else if (!assign_argument(argument, image_argument, landmarks_argument)) {
      throw make_runtime_error("Unable to process argument '%s'", argument.c_str());
    }
  }

  if (!have_argument_p(image_argument)) {
    print_usage();
    return 0;
  }

  if (lists_mode && video_mode)
    throw make_runtime_error("The operator is confused as the switches --lists and --video are present on the command line.");

  if (lists_mode) {
    if (!wait_time_specified)
      cfg.wait_time = 1.0 / 30;
    return run_lists_mode(cfg, image_argument, landmarks_argument);
  } else if (video_mode) {
    if (!wait_time_specified)
      cfg.wait_time = 1.0 / 30;
    return run_video_mode(cfg, image_argument, landmarks_argument);
  } else {
    if (!wait_time_specified) 
      cfg.wait_time = 0;      
    return run_image_mode(cfg, image_argument, landmarks_argument);			  
  }

  return 0;
}

face_fit_msg test_face_msg;

int
main(int argc, char **argv)
{
  try {
    return run_program(argc, argv);
  } catch (user_pressed_escape) {
    std::cout << "Stopping prematurely." << std::endl;
    return 1;
  } catch (std::exception &e) {
    std::cerr << "Caught unhandled exception: " << e.what() << std::endl;
    return 2;
  }
}

// Helpers
int
run_lists_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{
  FaceTracker * tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params  = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  std::list<std::string> image_pathnames = read_list(image_argument->c_str());
  std::list<std::string> landmark_pathnames;
  if (have_argument_p(landmarks_argument)) {
    landmark_pathnames = read_list(landmarks_argument->c_str());
    if (landmark_pathnames.size() != image_pathnames.size())
      throw make_runtime_error("Number of pathnames in list '%s' does not match the number in '%s'",
			       image_argument->c_str(), landmarks_argument->c_str());
  }

  std::list<std::string>::const_iterator image_it     = image_pathnames.begin();
  std::list<std::string>::const_iterator landmarks_it = landmark_pathnames.begin();
  const int number_of_images = image_pathnames.size();
  int current_image_index = 1;

  for (; image_it != image_pathnames.end(); image_it++) {
    if (cfg.verbose) {
      printf(" Image %d/%d\r", current_image_index, number_of_images);    
      fflush(stdout);
    }
    current_image_index++;

    cv::Mat image;
    cv::Mat_<uint8_t> gray_image = load_grayscale_image(image_it->c_str(), &image);
    int result = tracker->NewFrame(gray_image, tracker_params);

    std::vector<cv::Point_<double> > shape;
    std::vector<cv::Point3_<double> > shape3D;
    Pose pose;
    if (result >= cfg.tracking_threshold) {
      shape = tracker->getShape();
      shape3D = tracker->get3DShape();
      pose = tracker->getPose();
    } else {
      tracker->Reset();
    }

    if (!have_argument_p(landmarks_argument)) {
      display_data(cfg, image, shape, pose);
    } else if (shape.size() > 0) {
      if (cfg.save_3d_points)	
	save_points3(landmarks_it->c_str(), shape3D);
      else
	save_points(landmarks_it->c_str(), shape);

      if (cfg.verbose)
	display_data(cfg, image, shape, pose);
    } else if (cfg.verbose) {
      display_data(cfg, image, shape, pose);
    }

    if (have_argument_p(landmarks_argument))
      landmarks_it++;
  }  

  delete tracker;
  delete tracker_params; 
  
  return 0;
}

int
run_video_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{
  FaceTracker *tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  assert(tracker);
  assert(tracker_params);
  std::string image_argument_string  = image_argument->c_str();
  cv::VideoCapture input;
 


	// initialize video_mode
  if(image_argument_string == "device") input.open(0); // if image_argument == 0 , open device 0.
	else input.open(image_argument->c_str());
  if (!input.isOpened())
    throw make_runtime_error("Unable to open video file '%s'", image_argument->c_str());

  cv::Mat image;

  std::vector<char> pathname_buffer;
  pathname_buffer.resize(1000);

  input >> image;
  int frame_number = 1;
  bool run_onec = 0; 
  double pre_frame_data[132] = {0.0};
  double curr_frame_data[132] = {0.0};
  while ((image.rows > 0) && (image.cols > 0)) {
    if (cfg.verbose) {
      printf(" Frame number %d\r", frame_number);
      fflush(stdout);
    }

 //initialize socket
 
  int  sockfd;
  
  struct sockaddr_in servaddr;

  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
    exit(0);
    }
   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(6666);
  if( inet_pton(AF_INET, "192.168.177.111", &servaddr.sin_addr) <= 0){
    printf("inet_pton error for server\n");
    exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }
    // struct linger so_linger;
    // so_linger.l_onoff = TRUE; 
    // so_linger.l_linger = 1;
    // if(setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof so_linger))    
    // perror("setsockopt(2)");
	
//
    cv::Mat_<uint8_t> gray_image;
    if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
      cv::cvtColor(image, gray_image, CV_BGR2GRAY);
    else if (image.type() == cv::DataType<uint8_t>::type)
      gray_image = image;
    else
      throw make_runtime_error("Do not know how to convert video frame to a grayscale image.");

    int result = tracker->Track(gray_image, tracker_params);

    std::vector<cv::Point_<double> > shape;
    std::vector<cv::Point3_<double> > shape3D;
    Pose pose;

    if (result >= cfg.tracking_threshold) {
      shape = tracker->getShape();
      shape3D = tracker->get3DShape();
      pose = tracker->getPose();
    } else {
      tracker->Reset();
    }
	//display point in terminal
	//std::cout << "shape size:" << shape.size()<<std::endl;
	//std::cout << shape[i].x << '\t' << shape[i].y << std::endl;
  size_t face_nodes_cnt = 66;
double centerX = 0.0, centerY = 0.0;
	for (size_t i = 0; i < shape.size(); ++i)
		{
			curr_frame_data[2*i] = shape[i].x;
			centerX += curr_frame_data[2*i];

			curr_frame_data[2*i + 1] = shape[i].y;
			centerY += curr_frame_data[2*i + 1];
//			test_face_msg.face_fit_data[2*i] = shape[i].x;
//			test_face_msg.face_fit_data[2*i+1] = shape[i].y;
    
		}
centerX /= face_nodes_cnt;
centerY /= face_nodes_cnt;
//  // intialiaze additional face points only run onec
//  // addtiems: total add nodes how many times
//  // index_pairs : the nodes' index to do linear interp
//  const int index_pairs[] = {7,9,6,10,5,11,3,31,35,13,2,30,30,14,1,29,29,15,0,28,28,16};
//  const size_t addNodes[] = {2,4,5,3,3,4,4,5,5,5,5}; // total 45 points
//  if((!run_onec) && (test_face_msg.face_fit_data[0] != 0) && (test_face_msg.face_fit_data[1] != 0))
//  {
//    size_t index_start = shape.size(), index_end = index_start+addNodes[0];
//    for (int addtiems = 0; addtiems < 11; ++addtiems)
//    {
//      for (size_t i = index_start; i < index_end; ++i)
//      {
//        // std::cout << index_pairs[2*addtiems]*2 << ' ' << index_pairs[2*addtiems+1]*2 << ' '
//        // << index_pairs[2*addtiems]*2+ 1 << ' ' << index_pairs[2*addtiems+1]*2 + 1<< std::endl;;
//        double x = test_face_msg.face_fit_data[index_pairs[2*addtiems]*2] 
//        + (test_face_msg.face_fit_data[index_pairs[2*addtiems+1]*2] 
//        - test_face_msg.face_fit_data[index_pairs[2*addtiems]*2])
//        /(addNodes[addtiems]+1)*(i - index_start + 1);
//        double y = test_face_msg.face_fit_data[index_pairs[2*addtiems]*2+ 1] 
//        + (test_face_msg.face_fit_data[index_pairs[2*addtiems+1]*2 + 1] 
//        - test_face_msg.face_fit_data[index_pairs[2*addtiems]*2 + 1])
//        / (addNodes[addtiems]+1)*(i - index_start + 1);

//        test_face_msg.face_fit_data[2*i] = x;
//        test_face_msg.face_fit_data[2*i+1] = y;
//      }
//    if(addtiems <= 10)
//      {
//        index_start = index_start+addNodes[addtiems];
//        index_end = index_end+addNodes[addtiems+1];
//      }
//    }
// save initialized points to face-fit-data.txt
//	std::ofstream out("face-fit-data.txt");
//	if(out.is_open())
//	for(size_t i =0; i<222;i=i+2)
//	{
//		out << test_face_msg.face_fit_data[i] << '\t'
//		<< test_face_msg.face_fit_data[i+1] << '\n';
//	}
//    out.close();
//// save the raw picture
//	imwrite("face-fit-pic.jpg",image);
//    run_onec = 1;
//  }
  
  if((curr_frame_data[0] != 0) && (pre_frame_data[0] != 0) ) // current and pre frame have data.
  {
	//computing scales ,depening on your unity3d *obj models.
	//x = 65.72 +66.25 y= 50.4+68.17
	double ScaleX = (131.97)/ abs(curr_frame_data[32] - curr_frame_data[0]);
	double ScaleY = (118.57)/abs(curr_frame_data[54+1] - curr_frame_data[16+1]);
	test_face_msg.face_fit_data[168] = ScaleX;
	test_face_msg.face_fit_data[169] = ScaleY;

	
     //std::cout << " rbfing..." << std::endl;
    //using pre_frame_data to predict current frame the additional points' X-position
    for (size_t i = 0,j = 0; i < face_nodes_cnt*2; i=i+2,++j)
    {

      X[j] = -(pre_frame_data[i] - centerX)*ScaleX ;
	  test_face_msg.face_fit_data[i] = curr_frame_data[i]- pre_frame_data[i];
      Y.put(j,0,test_face_msg.face_fit_data[i]*ScaleX);
      //std::cout << j << ' ' << X[j] << ' ' <<test_face_msg.face_fit_data[i] - pre_frame_data[i]<<std::endl;
    }
  	//train RBF
    Train_RBF();
    //std::cout << " rbfing..." << std::endl;
	for(size_t i = 132,  k = 0; i < 132+18; i = i+3){
	test_face_msg.face_fit_data[i] = Weight.get(k,0);
	test_face_msg.face_fit_data[i+1] = center[k];
	test_face_msg.face_fit_data[i+2] = delta[k];
    //std::cout << Weight.get(k,0) << ' ' << center[k] << ' ' << delta[k] << std::endl;
	++k;
	}	
 	//Use RBF to predict other points
//	std::cout << "test _rbf  : " << " output : " << getOutput(32.2) << "  "<< getOutput(53.2) <<std::endl;
//    for(size_t i = face_nodes_cnt*2; i< 222; i=i+2){
//      double temp = getOutput(pre_frame_data[i]);
//      test_face_msg.face_fit_data[i] = pre_frame_data[i] + temp;
//      //std::cout << " X: " << temp << " " << test_face_msg.face_fit_data[i] << std::endl; 
//    }
    //using pre_frame_data to predict current frame the additional points' Y-position
    for (size_t i = 1 ,j = 0; i < face_nodes_cnt*2; i=i+2,++j)
    {
      X[j] = -(pre_frame_data[i] - centerY)*ScaleY;
      test_face_msg.face_fit_data[i] = curr_frame_data[i]- pre_frame_data[i];
      Y.put(j,0,test_face_msg.face_fit_data[i]*ScaleY);
    }
    Train_RBF();
	for(size_t i = 132+18, k = 0; i < 132+36; i = i+3){
	test_face_msg.face_fit_data[i] = Weight.get(k,0);
	test_face_msg.face_fit_data[i+1] = center[k];
	test_face_msg.face_fit_data[i+2] = delta[k];
	++k;
	}

//    for(size_t i = face_nodes_cnt*2+1; i< 222; i=i+2){
//      double temp = getOutput(pre_frame_data[i]);
//      test_face_msg.face_fit_data[i] = pre_frame_data[i] +temp ;
      //std::cout << " Y: " << temp << " " << test_face_msg.face_fit_data[i] << std::endl; 
//}

  }


 
     
  //copy face-fit-data to pre_frame_data
  //std::cout << "copying data " <<std::endl;
  for (size_t i = 0; i < 132; ++i)
  {
    //pre_frame_data[i] = test_face_msg.face_fit_data[i];
	pre_frame_data[i] = curr_frame_data[i];
  }
/*  for (int i = 0; i < 222; ++i)
  {
    std::cout <<i <<' '<< pre_frame_data[i]  << std::endl;
  }*/

// for(size_t i = 0 ; i < 222; i = i+2){
//   std::cout << i<<':'<<' ' <<test_face_msg.face_fit_data[i] <<' '<< test_face_msg.face_fit_data[i+1]<< std::endl;
// }
  //   size_t add_nodes = 4;
  // for (size_t i = face_nodes_cnt; i < face_nodes_cnt + add_nodes; ++i)
  // {
  //   //std::cout << i << '\t' << (2*i+1) << std::endl;
  //   // double x = shape[6].x + (shape[10].x - shape[6].x)/ (add_nodes + 1 )*(i- face_nodes_cnt + 1);
  //   // double y = shape[6].y + (shape[10].y - shape[6].y)/ (add_nodes + 1 )*(i- face_nodes_cnt + 1);
  //   double x = test_face_msg.face_fit_data[12] + (test_face_msg.face_fit_data[20] - test_face_msg.face_fit_data[12])/
  //     (add_nodes+1)*(i - face_nodes_cnt + 1);
  //   double y = test_face_msg.face_fit_data[12 + 1] + (test_face_msg.face_fit_data[20 + 1] - test_face_msg.face_fit_data[12 + 1])/
  //     (add_nodes+1)*(i - face_nodes_cnt + 1); 

  //   //std::cout << x << '\t' << y << std::endl;
  //   test_face_msg.face_fit_data[2*i] = x;
  //   test_face_msg.face_fit_data[2*i+1] = y;
  // }

	//send face-fit result by socket

      if(send(sockfd, (char *)&test_face_msg, sizeof(face_fit_msg), 0) < 0)
		{
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			
			exit(0);
		} 
	//after send msg close socketfd
    if (!have_argument_p(landmarks_argument)) {
      display_data(cfg, image, shape, pose);
    } else if (shape.size() > 0) {
      snprintf(pathname_buffer.data(), pathname_buffer.size(), landmarks_argument->c_str(), frame_number);

      if (cfg.save_3d_points)	
	save_points3(pathname_buffer.data(), shape3D);
      else
	save_points(pathname_buffer.data(), shape);

      if (cfg.verbose)
	display_data(cfg, image, shape, pose);
    } else if (cfg.verbose) {
      display_data(cfg, image, shape, pose);
    }
    close(sockfd);
    input >> image;
    frame_number++;
  }

	// exit prog

  delete tracker;
  delete tracker_params; 
  return 0;
}

int
run_image_mode(const Configuration &cfg,
	       const CommandLineArgument<std::string> &image_argument,
	       const CommandLineArgument<std::string> &landmarks_argument)
{  
  FaceTracker * tracker = LoadFaceTracker(cfg.model_pathname.c_str());
  FaceTrackerParams *tracker_params  = LoadFaceTrackerParams(cfg.params_pathname.c_str());

  cv::Mat image;
  cv::Mat_<uint8_t> gray_image = load_grayscale_image(image_argument->c_str(), &image);

  int result = tracker->NewFrame(gray_image, tracker_params);

  std::vector<cv::Point_<double> > shape;
  std::vector<cv::Point3_<double> > shape3;
  Pose pose;
  
  if (result >= cfg.tracking_threshold) {
    shape = tracker->getShape();
    shape3 = tracker->get3DShape();
    pose = tracker->getPose();
  }

  if (!have_argument_p(landmarks_argument)) {
    display_data(cfg, image, shape, pose); 
  } else if (shape.size() > 0) {
    if (cfg.save_3d_points)
      save_points3(landmarks_argument->c_str(), shape3);
    else
      save_points(landmarks_argument->c_str(), shape);
  }
 
  delete tracker;
  delete tracker_params; 
  
  return 0;
}

cv::Mat
compute_pose_image(const Pose &pose, int height, int width)
{
  cv::Mat_<cv::Vec<uint8_t,3> > rv = cv::Mat_<cv::Vec<uint8_t,3> >::zeros(height,width);
  cv::Mat_<double> axes = pose_axes(pose);
  cv::Mat_<double> scaling = cv::Mat_<double>::eye(3,3);

  for (int i = 0; i < axes.cols; i++) {
    axes(0,i) = -0.5*double(width)*(axes(0,i) - 1);
    axes(1,i) = -0.5*double(height)*(axes(1,i) - 1);
  }
  
  cv::Point centre(width/2, height/2);
  // pitch
  cv::line(rv, centre, cv::Point((int)axes(0,0), (int)axes(1,0)), cv::Scalar(255,0,0));
  // yaw
  cv::line(rv, centre, cv::Point((int)axes(0,1), (int)axes(1,1)), cv::Scalar(0,255,0));
  // roll
  cv::line(rv, centre, cv::Point((int)axes(0,2), (int)axes(1,2)), cv::Scalar(0,0,255));

  return rv;
}

void
display_data(const Configuration &cfg,
	     const cv::Mat &image,
	     const std::vector<cv::Point_<double> > &points,
	     const Pose &pose)
{

  cv::Scalar colour;
  if (image.type() == cv::DataType<uint8_t>::type)
    colour = cv::Scalar(255);
  else if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
    colour = cv::Scalar(0,0,255);
  else
    colour = cv::Scalar(255);

  cv::Mat displayed_image;
  if (image.type() == cv::DataType<cv::Vec<uint8_t,3> >::type)
    displayed_image = image.clone();
  else if (image.type() == cv::DataType<uint8_t>::type)
    cv::cvtColor(image, displayed_image, CV_GRAY2BGR);
  else 
    throw make_runtime_error("Unsupported camera image type for display_data function.");

  for (size_t i = 0; i < points.size(); i++) {
    cv::circle(displayed_image, points[i], cfg.circle_radius, colour, cfg.circle_thickness, cfg.circle_linetype, cfg.circle_shift);
  }

  int pose_image_height = 100;
  int pose_image_width = 100;
  cv::Mat pose_image = compute_pose_image(pose, pose_image_height, pose_image_width);
  for (int i = 0; i < pose_image_height; i++) {
    for (int j = 0; j < pose_image_width; j++) {
      displayed_image.at<cv::Vec<uint8_t,3> >(displayed_image.rows - pose_image_height + i,
					      displayed_image.cols - pose_image_width + j)
			 
			 = pose_image.at<cv::Vec<uint8_t,3> >(i,j);
    }
  }

  cv::imshow(cfg.window_title, displayed_image);

  if (cfg.wait_time == 0)
    std::cout << "Press any key to continue." << std::endl;

  char ch = cv::waitKey((int) (cfg.wait_time * 1000));

  if (ch == 27) // escape
    throw user_pressed_escape();
}
