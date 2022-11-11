
#define _CRT_SECURE_NO_WARNINGS // avoid : warning of localTime

#include <iostream>
#include <iomanip>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sys/stat.h> // stat

//--------------------------------------------------------------
// Windows
#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
#endif


//--------------------------------------------------------------
// OpenCV
#include <opencv2/opencv.hpp>
#if defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib, "opencv_world455.lib")
#endif

//--------------------------------------------------------------
// oscpack
#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>
#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>
#if defined(_WIN32) || defined(_WIN64)
	#ifdef _DEBUG
		#pragma comment(lib,"oscpack_1_1_0_d.lib")
	#else
		#pragma comment(lib,"oscpack_1_1_0.lib")
	#endif
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "winmm.lib")
#endif

//--------------------------------------------------------------
// set_camera (Windows only)
#if defined(_WIN32) || defined(_WIN64)
	#define _USE_SET_CAMERA
	#include <cam_lib.h>
#endif

using namespace std;


//--------------------------------------------------------------
// global
#define OSC_PACKET_BUFFER_SIZE  2048

static const double NOT_SET = 9999;
typedef struct {
	double FRAME_WIDTH = NOT_SET;
	double FRAME_HEIGHT = NOT_SET;
	double FPS = NOT_SET;
	double BRIGHTNESS = NOT_SET;
	double CONTRAST = NOT_SET;
	double HUE = NOT_SET;
	double SATURATION = NOT_SET;
	double SHARPNESS = NOT_SET;
	double GAMMA = NOT_SET;
	double WB_TEMPERATURE = NOT_SET;
	double AUTO_WB = NOT_SET;
	double GAIN = NOT_SET;
	double ZOOM = NOT_SET;
	double FOCUS = NOT_SET;
	double AUTOFOCUS = NOT_SET;
	double EXPOSURE = NOT_SET;
	double AUTO_EXPOSURE = NOT_SET;
	double IRIS = NOT_SET;
	double PAN = NOT_SET;
	double TILT = NOT_SET;
	double ROLL = NOT_SET;
} CAP_PROP_VALUES;
CAP_PROP_VALUES cap_prop_values;

string g_ip_address;
int g_port;
string g_address_pattern;
int g_is_video_file = false;
int g_camera_id;
int g_show_config_on_startup;
int g_threshold;
int g_min_size;
int g_max_size;
int g_tracker_range_max;
int g_max_points;

const char* win1 = "LuminousOSC";
const char* win2 = "Control";
cv::VideoCapture cap;
bool seeked = false;


inline char separator() {
#if defined(_WIN32) || defined(_WIN64)
	return '\\';
#else
	return '/';
#endif
}

int LoadConfigFile(const char* filename) {
	ifstream fin(filename);
	if (!fin) {
		cout << endl << "can't open " << filename << endl;
		return -1;
	}

	cout << endl << "*** Loaded Settings" << endl << endl;

	string word;
	while (fin >> word) {
		if (word == "CAMERA_ID") {
			fin >> g_camera_id;
			cout << "  CAMERA_ID = " << g_camera_id << endl;
		}
		else if (word == "SHOW_CONFIG_ON_STARTUP") {
			fin >> g_show_config_on_startup;
			cout << "  SHOW_CONFIG_ON_STARTUP = " << g_show_config_on_startup << endl;
		}
		else if (word == "THRESHOLD") {
			fin >> g_threshold;
			cout << "  THRESHOLD = " << g_threshold << endl;
		}
		else if (word == "MIN_SIZE") {
			fin >> g_min_size;
			cout << "  MIN_SIZE = " << g_min_size << endl;
		}
		else if (word == "MAX_SIZE") {
			fin >> g_max_size;
			cout << "  MAX_SIZE = " << g_max_size << endl;
		}
		else if (word == "IP_ADDRESS") {
			fin >> g_ip_address;
			cout << "  IP_ADDRESS = " << g_ip_address << endl;
		}
		else if (word == "PORT") {
			fin >> g_port;
			cout << "  PORT = " << g_port << endl;
		}
		else if (word == "ADDRESS_PATTERN") {
			fin >> g_address_pattern;
			cout << "  ADDRESS_PATTERN = " << g_address_pattern << endl;
		}
		else if (word == "RANGE_MAX") {
			fin >> g_tracker_range_max;
			cout << "  RANGE_MAX = " << g_tracker_range_max << endl;
		}
		else if (word == "MAX_POINTS") {
			fin >> g_max_points;
			cout << "  MAX_POINTS = " << g_max_points << endl;
		}
		else if (word == "[Note]") {
			break;
		}
		else if (word == "CAP_PROP_FRAME_WIDTH") {
			fin >> cap_prop_values.FRAME_WIDTH;
			cout << "  CAP_PROP_FRAME_WIDTH = " << cap_prop_values.FRAME_WIDTH << endl;
		}
		else if (word == "CAP_PROP_FRAME_HEIGHT") {
			fin >> cap_prop_values.FRAME_HEIGHT;
			cout << "  CAP_PROP_FRAME_HEIGHT = " << cap_prop_values.FRAME_HEIGHT << endl;
		}
		else if (word == "CAP_PROP_FPS") {
			fin >> cap_prop_values.FPS;
			cout << "  CAP_PROP_FPS = " << cap_prop_values.FPS << endl;
		}
		else if (word == "CAP_PROP_BRIGHTNESS") {
			fin >> cap_prop_values.BRIGHTNESS;
			cout << "  CAP_PROP_BRIGHTNESS = " << cap_prop_values.BRIGHTNESS << endl;
		}
		else if (word == "CAP_PROP_CONTRAST") {
			fin >> cap_prop_values.CONTRAST;
			cout << "  CAP_PROP_CONTRAST = " << cap_prop_values.CONTRAST << endl;
		}
		else if (word == "CAP_PROP_HUE") {
			fin >> cap_prop_values.HUE;
			cout << "  CAP_PROP_HUE = " << cap_prop_values.HUE << endl;
		}
		else if (word == "CAP_PROP_SATURATION") {
			fin >> cap_prop_values.SATURATION;
			cout << "  CAP_PROP_SATURATION = " << cap_prop_values.SATURATION << endl;
		}
		else if (word == "CAP_PROP_SHARPNESS") {
			fin >> cap_prop_values.SHARPNESS;
			cout << "  CAP_PROP_SHARPNESS = " << cap_prop_values.SHARPNESS << endl;
		}
		else if (word == "CAP_PROP_GAMMA") {
			fin >> cap_prop_values.GAMMA;
			cout << "  CAP_PROP_GAMMA = " << cap_prop_values.GAMMA << endl;
		}
		else if (word == "CAP_PROP_WB_TEMPERATURE") {
			fin >> cap_prop_values.WB_TEMPERATURE;
			cout << "  CAP_PROP_WB_TEMPERATURE = " << cap_prop_values.WB_TEMPERATURE << endl;
		}
		else if (word == "CAP_PROP_AUTO_WB") {
			fin >> cap_prop_values.AUTO_WB;
			cout << "  CAP_PROP_AUTO_WB = " << cap_prop_values.AUTO_WB << endl;
		}
		else if (word == "CAP_PROP_GAIN") {
			fin >> cap_prop_values.GAIN;
			cout << "  CAP_PROP_GAIN = " << cap_prop_values.GAIN << endl;
		}
		else if (word == "CAP_PROP_ZOOM") {
			fin >> cap_prop_values.ZOOM;
			cout << "  CAP_PROP_ZOOM = " << cap_prop_values.ZOOM << endl;
		}
		else if (word == "CAP_PROP_FOCUS") {
			fin >> cap_prop_values.FOCUS;
			cout << "  CAP_PROP_FOCUS = " << cap_prop_values.FOCUS << endl;
		}
		else if (word == "CAP_PROP_AUTOFOCUS") {
			fin >> cap_prop_values.AUTOFOCUS;
			cout << "  CAP_PROP_AUTOFOCUS = " << cap_prop_values.AUTOFOCUS << endl;
		}
		else if (word == "CAP_PROP_EXPOSURE") {
			fin >> cap_prop_values.EXPOSURE;
			cout << "  CAP_PROP_EXPOSURE = " << cap_prop_values.EXPOSURE << endl;
		}
		else if (word == "CAP_PROP_AUTO_EXPOSURE") {
			fin >> cap_prop_values.AUTO_EXPOSURE;
			cout << "  CAP_PROP_AUTO_EXPOSURE = " << cap_prop_values.AUTO_EXPOSURE << endl;
		}
		else if (word == "CAP_PROP_IRIS") {
			fin >> cap_prop_values.IRIS;
			cout << "  CAP_PROP_IRIS = " << cap_prop_values.IRIS << endl;
		}
		else if (word == "CAP_PROP_PAN") {
			fin >> cap_prop_values.PAN;
			cout << "  CAP_PROP_PAN = " << cap_prop_values.PAN << endl;
		}
		else if (word == "CAP_PROP_TILT") {
			fin >> cap_prop_values.TILT;
			cout << "  CAP_PROP_TILT = " << cap_prop_values.TILT << endl;
		}
		else if (word == "CAP_PROP_ROLL") {
			fin >> cap_prop_values.ROLL;
			cout << "  CAP_PROP_ROLL = " << cap_prop_values.ROLL << endl;
		}
	}
	fin.close();

	return 0;
}

void set_cap_props(CAP_PROP_VALUES values) {
	cout << endl << "*** Set Video Capture Properties" << endl << endl;

	if (values.FRAME_WIDTH != NOT_SET) {
		cap.set(cv::CAP_PROP_FRAME_WIDTH, values.FRAME_WIDTH);
		cout << "  applying : CAP_PROP_FRAME_WIDTH " << endl;
	}
	if (values.FRAME_HEIGHT != NOT_SET) {
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, values.FRAME_HEIGHT);
		cout << "  applying : CAP_PROP_FRAME_HEIGHT " << endl;
	}
	if (values.FPS != NOT_SET) {
		cap.set(cv::CAP_PROP_FPS, values.FPS);
		cout << "  applying : CAP_PROP_FPS " << endl;
	}
	if (values.BRIGHTNESS != NOT_SET) {
		cap.set(cv::CAP_PROP_BRIGHTNESS, values.BRIGHTNESS);
		cout << "  applying : CAP_PROP_BRIGHTNESS " << endl;
	}
	if (values.CONTRAST != NOT_SET) {
		cap.set(cv::CAP_PROP_CONTRAST, values.CONTRAST);
		cout << "  applying : CAP_PROP_CONTRAST " << endl;
	}
	if (values.HUE != NOT_SET) {
		cap.set(cv::CAP_PROP_HUE, values.HUE);
		cout << "  applying : CAP_PROP_HUE " << endl;
	}
	if (values.SATURATION != NOT_SET) {
		cap.set(cv::CAP_PROP_SATURATION, values.SATURATION);
		cout << "  applying : CAP_PROP_SATURATION " << endl;
	}
	if (values.SHARPNESS != NOT_SET) {
		cap.set(cv::CAP_PROP_SHARPNESS, values.SHARPNESS);
		cout << "  applying : CAP_PROP_SHARPNESS " << endl;
	}
	if (values.GAMMA != NOT_SET) {
		cap.set(cv::CAP_PROP_GAMMA, values.GAMMA);
		cout << "  applying : CAP_PROP_GAMMA " << endl;
	}
	if (values.WB_TEMPERATURE != NOT_SET) {
		cap.set(cv::CAP_PROP_WB_TEMPERATURE, values.WB_TEMPERATURE);
		cout << "  applying : CAP_PROP_WB_TEMPERATURE " << endl;
	}
	if (values.AUTO_WB != NOT_SET) {
		cap.set(cv::CAP_PROP_AUTO_WB, values.AUTO_WB);
		cout << "  applying : CAP_PROP_AUTO_WB " << endl;
	}
	if (values.GAIN != NOT_SET) {
		cap.set(cv::CAP_PROP_GAIN, values.GAIN);
		cout << "  applying : CAP_PROP_GAIN " << endl;
	}
	if (values.ZOOM != NOT_SET) {
		cap.set(cv::CAP_PROP_ZOOM, values.ZOOM);
		cout << "  applying : CAP_PROP_ZOOM " << endl;
	}
	if (values.FOCUS != NOT_SET) {
		cap.set(cv::CAP_PROP_FOCUS, values.FOCUS);
		cout << "  applying : CAP_PROP_FOCUS " << endl;
	}
	if (values.AUTOFOCUS != NOT_SET) {
		cap.set(cv::CAP_PROP_AUTOFOCUS, values.AUTOFOCUS);
		cout << "  applying : CAP_PROP_AUTOFOCUS " << endl;
	}
	if (values.EXPOSURE != NOT_SET) {
		cap.set(cv::CAP_PROP_EXPOSURE, values.EXPOSURE);
		cout << "  applying : CAP_PROP_EXPOSURE " << endl;
	}
	if (values.AUTO_EXPOSURE != NOT_SET) {
		cap.set(cv::CAP_PROP_AUTO_EXPOSURE, values.AUTO_EXPOSURE);
		cout << "  applying : CAP_PROP_AUTO_EXPOSURE " << endl;
	}
	if (values.IRIS != NOT_SET) {
		cap.set(cv::CAP_PROP_IRIS, values.IRIS);
		cout << "  applying : CAP_PROP_IRIS " << endl;
	}
	if (values.PAN != NOT_SET) {
		cap.set(cv::CAP_PROP_PAN, values.PAN);
		cout << "  applying : CAP_PROP_PAN " << endl;
	}
	if (values.TILT != NOT_SET) {
		cap.set(cv::CAP_PROP_TILT, values.TILT);
		cout << "  applying : CAP_PROP_TILT " << endl;
	}
	if (values.ROLL != NOT_SET) {
		cap.set(cv::CAP_PROP_ROLL, values.ROLL);
		cout << "  applying : CAP_PROP_ROLL " << endl;
	}
}


string getDatetimeStr() {
	time_t t = time(nullptr);
	const tm* localTime = localtime(&t);
	std::stringstream s;
	// setw(), setfill() : zero-filling
	s << localTime->tm_year + 1900 << "-";
	s << setw(2) << setfill('0') << localTime->tm_mon + 1 << "-";
	s << setw(2) << setfill('0') << localTime->tm_mday << "_";
	s << setw(2) << setfill('0') << localTime->tm_hour << "_";
	s << setw(2) << setfill('0') << localTime->tm_min << "_";
	s << setw(2) << setfill('0') << localTime->tm_sec;
	s << separator();

	return s.str();	// return as std::string
}

void trackbar_threshold(int value, void*) {
	g_threshold = value;
}

void trackbar_min_size(int value, void*) {
	g_min_size = value;

	if (g_min_size > g_max_size) {
		cv::setTrackbarPos("Min size", win2, value);
		cv::setTrackbarPos("Max size", win2, value);
	}
}

void trackbar_max_size(int value, void*) {
	g_max_size = value;

	if (g_max_size < g_min_size) {
		cv::setTrackbarPos("Min size", win2, value);
		cv::setTrackbarPos("Max size", win2, value);
	}
}

void trackbar_seek(int value, void*) {
	cap.set(cv::CAP_PROP_POS_FRAMES, value);
	seeked = true;
}

string GetFolderPath(const string& path) {

	size_t pos = path.rfind('\\');
	if (pos != string::npos) {
		return path.substr(0, pos + 1);
	}
	pos = path.rfind('/');
	if (pos != string::npos) {
		return path.substr(0, pos + 1);
	}
	return "";
}

int main(int argc, char* argv[]) {
	cout << "---------------------------------------------" << endl;
	cout << "  LuminousOSC - ver.0.1.5" << endl;
	cout << "---------------------------------------------" << endl;

	// get paths
	string local_path = GetFolderPath(argv[0]);
	string video_file_path = "";
	if (argc > 1) {
		video_file_path = argv[1];
		g_is_video_file = true;
	}

	// load setting file
	string config_path = local_path + "settings.txt";
	LoadConfigFile(config_path.c_str());

	// open video capture
	if (g_is_video_file) {
		cap.open(video_file_path);
	}
	else {
		if (g_camera_id < 0) {
			cout << "Input your camera id : ";
			cin >> g_camera_id;
			cout << endl;
		}
		cap.open(g_camera_id);
	}
	if (!cap.isOpened()) {
		cout << "can't open VideoCapture." << endl;
		return -1;
	}

	// capture settings (webcam only)
	if (!g_is_video_file) {
		set_cap_props(cap_prop_values);
	}

	// input information
	cout << endl << "*** Input Information" << endl << endl;
#ifdef _USE_SET_CAMERA
	camera_data* cam_data = NULL;
	if (!g_is_video_file) {
		scam_init();
		cam_data = init_camera_data(g_camera_id); // this shows "DEVICE:"
		if (g_show_config_on_startup) {
			disp_camera_property(cam_data);
		}
	}
#endif
	if (g_is_video_file) {
		cout << "  Video file : " << video_file_path << endl;
	}
	int width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	int frame_count = int(cap.get(cv::CAP_PROP_FRAME_COUNT));
	double fps = cap.get(cv::CAP_PROP_FPS);
	int mspf = (int)(1000.0 / fps);

	cout << "  Actual Image Size : " << width << "x" << height << endl;
	cout << "  FPS : " << fps << endl;

	// key info
	cout << endl << "*** Key operations on the image window" << endl << endl;
#ifdef _USE_SET_CAMERA
	if (!g_is_video_file) {
		cout << "  s     : open camera settings" << endl;
	}
#endif
	cout << "  c     : save image" << endl;
	cout << "  SPACE : pause / resume" << endl;
	cout << "  ESC   : quit" << endl;
	cout << endl << "*** Processing started" << endl;

	// setup trackbar
	cv::namedWindow(win1);
	cv::namedWindow(win2);
	cv::createTrackbar("Threshold", win2, NULL, 255, trackbar_threshold);
	cv::createTrackbar("Min size", win2, NULL, g_tracker_range_max, trackbar_min_size);
	cv::createTrackbar("Max size", win2, NULL, g_tracker_range_max, trackbar_max_size);
	cv::setTrackbarPos("Min size", win2, g_min_size);
	cv::setTrackbarPos("Max size", win2, g_max_size);
	cv::setTrackbarPos("Threshold", win2, g_threshold);
	if (g_is_video_file) {
		cv::createTrackbar("Frame", win2, NULL, frame_count - 1, trackbar_seek);
	}

	// OSC
	UdpTransmitSocket transmitSocket(IpEndpointName(g_ip_address.c_str(), g_port));
	char buffer[OSC_PACKET_BUFFER_SIZE];
	osc::OutboundPacketStream packet(buffer, OSC_PACKET_BUFFER_SIZE);

	// for image save
	string image_path = "";
	int image_index = 0;

	// images
	cv::Mat frame, gray, binary, label, result;
	bool video_playing = true;

	while (true) {
		// capture
		if (video_playing || seeked) {
			cap.read(frame);
			seeked = false;
		}
		std::chrono::system_clock::time_point time_start = std::chrono::system_clock::now();

		// seek
		if (video_playing && frame_count > 0) {
			int frame_index = (int)cap.get(cv::CAP_PROP_POS_FRAMES);
			cv::setTrackbarPos("Frame", win2, frame_index);
			if (frame_index == frame_count) {
				cap.set(cv::CAP_PROP_POS_FRAMES, 0);
			}
		}

		// image processing
		result = frame.clone();
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
		cv::threshold(gray, binary, g_threshold, 255, cv::THRESH_BINARY);
		cv::Mat stats, centroids;
		int n_label = cv::connectedComponentsWithStats(binary, label, stats, centroids);
		cv::cvtColor(binary, binary, cv::COLOR_GRAY2BGR);  // for color drawing

		// get points
		std::vector<cv::Point> points;
		for (int i = 1; i < n_label; i++) {
			// baunding box info
			int* param = stats.ptr<int>(i);
			int area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			// centroid info
			double* param_c = centroids.ptr<double>(i);
			int cx = static_cast<int>(param_c[0]);
			int cy = static_cast<int>(param_c[1]);

			if (g_min_size <= area && area <= g_max_size) {
				cv::rectangle(binary, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 255), 1);
				cv::circle(binary, cv::Point(cx, cy), 3, cv::Scalar(255, 255, 0), -1);
				cv::circle(binary, cv::Point(cx, cy), 3, cv::Scalar(0, 0, 0), 1);

				// upto max
				if (points.size() < g_max_points) {
					points.push_back(cv::Point(cx, cy));
				}
			}
		}

		// drawing
		for (int i = 0; i < points.size(); i++) {
			cv::Point p = points[i];
			int x = p.x;
			int y = p.y;
			cv::circle(binary, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), -1);
			cv::circle(binary, cv::Point(x, y), 3, cv::Scalar(0, 0, 0), 1);
			cv::circle(result, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), -1);
			cv::circle(result, cv::Point(x, y), 3, cv::Scalar(0, 0, 0), 1);

			std::stringstream ss;
			ss << i;
			cv::putText(binary, ss.str(), cv::Point(x + 15, y + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0), 5);
			cv::putText(binary, ss.str(), cv::Point(x + 15, y + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
			cv::putText(result, ss.str(), cv::Point(x + 15, y + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0), 5);
			cv::putText(result, ss.str(), cv::Point(x + 15, y + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
		}

		// OSC sending
		packet.Clear();
		packet << osc::BeginBundleImmediate << osc::BeginMessage(g_address_pattern.c_str());
		for (cv::Point pos : points) {
			packet << pos.x << pos.y;

		}
		packet << osc::EndMessage << osc::EndBundle;
		transmitSocket.Send(packet.Data(), packet.Size());

		// showing
		cv::imshow(win1, result);
		cv::imshow(win2, binary);

		// waiting
		std::chrono::system_clock::time_point time_end = std::chrono::system_clock::now();
		int elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
		int delay_ms = mspf - elapsed;
		if (delay_ms <= 0) {
			delay_ms = 1;
		}
		const int key = cv::waitKey(delay_ms);

		// key control
		if (key == 27) { // ESC
			break;
		}
		else if (key == ' ') {
			video_playing = !video_playing;
		}

#ifdef _USE_SET_CAMERA
		else if (key == 's' && !g_is_video_file) {
			if (cam_data != NULL) {
				disp_camera_property(cam_data);
			}
		}
#endif
		else if (key == 'c') {
			if (image_index == 0) {
				image_path = local_path + getDatetimeStr();
				cout << endl << "  The image is stored to: " << endl << "  " << image_path << endl << endl;
			}

			struct stat statBuf;
			if (stat(image_path.c_str(), &statBuf) != 0) {
				std::filesystem::create_directory(image_path);
			}

			string filename;
			stringstream ss;
			ss << setfill('0') << setw(4) << right << to_string(image_index) << ".jpg";
			ss >> filename;
			cv::imwrite(image_path + "input_" + filename, frame);
			cv::imwrite(image_path + "binary_" + filename, binary);
			cv::imwrite(image_path + "result_" + filename, result);
			cout << "  Image saved : " << "input_" + filename + ", " + "binary_" + filename + ", " + "result_" + filename << endl;
			image_index++;
		}
	}
	cv::destroyAllWindows();
	cap.release();

#ifdef _USE_SET_CAMERA
	if (cam_data != NULL) {
		free_camera_data(cam_data);
	}
#endif

	cout << endl << "*** Finished" << endl;
	return 0;
}
