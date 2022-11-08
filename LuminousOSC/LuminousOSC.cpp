
#define _CRT_SECURE_NO_WARNINGS // localTimeの警告避け

#include <iostream>
#include <iomanip>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <filesystem>
#include <sys/stat.h> // stat


// set_camera (Windows only)
#if _MSC_VER >=1500 // VC2008以降
    #define _USE_SET_CAMERA
#endif
#ifdef _USE_SET_CAMERA
    #include <cam_lib.h> // set_camera
#endif


// OpenCV
#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif


// oscpack
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

#ifdef _DEBUG
#pragma comment(lib,"oscpack_1_1_0_d.lib")
#else 
#pragma comment(lib,"oscpack_1_1_0.lib")
#endif
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")


using namespace std;

#define OSC_PACKET_BUFFER_SIZE  2048

string g_ip_address;
int g_port;
string g_address_pattern;
int camera_id;
int show_config_on_startup;
int g_threshold;
int g_min_size;
int g_max_size;
int g_tracker_range_max;
int g_max_points;

const char* win1 = "LuminousOSC";
const char* win2 = "Control";

static const double NOT_USED = 9999;
static const int CAP_PROPS_SIZE = 53;
double cap_prop_values[CAP_PROPS_SIZE];
string cap_prop_names[CAP_PROPS_SIZE];


void init_cap_props() {
    cap_prop_names[cv::CAP_PROP_POS_MSEC] = "CAP_PROP_POS_MSEC";
    cap_prop_names[cv::CAP_PROP_POS_FRAMES] = "CAP_PROP_POS_FRAMES";
    cap_prop_names[cv::CAP_PROP_POS_AVI_RATIO] = "CAP_PROP_POS_AVI_RATIO";
    cap_prop_names[cv::CAP_PROP_FRAME_WIDTH] = "CAP_PROP_FRAME_WIDTH";
    cap_prop_names[cv::CAP_PROP_FRAME_HEIGHT] = "CAP_PROP_FRAME_HEIGHT";
    cap_prop_names[cv::CAP_PROP_FPS] = "CAP_PROP_FPS";
    cap_prop_names[cv::CAP_PROP_FOURCC] = "CAP_PROP_FOURCC";
    cap_prop_names[cv::CAP_PROP_FRAME_COUNT] = "CAP_PROP_FRAME_COUNT";
    cap_prop_names[cv::CAP_PROP_FORMAT] = "CAP_PROP_FORMAT";
    cap_prop_names[cv::CAP_PROP_MODE] = "CAP_PROP_MODE";
    cap_prop_names[cv::CAP_PROP_BRIGHTNESS] = "CAP_PROP_BRIGHTNESS";
    cap_prop_names[cv::CAP_PROP_CONTRAST] = "CAP_PROP_CONTRAST";
    cap_prop_names[cv::CAP_PROP_SATURATION] = "CAP_PROP_SATURATION";
    cap_prop_names[cv::CAP_PROP_HUE] = "CAP_PROP_HUE";
    cap_prop_names[cv::CAP_PROP_GAIN] = "CAP_PROP_GAIN";
    cap_prop_names[cv::CAP_PROP_EXPOSURE] = "CAP_PROP_EXPOSURE";
    cap_prop_names[cv::CAP_PROP_CONVERT_RGB] = "CAP_PROP_CONVERT_RGB";
    cap_prop_names[cv::CAP_PROP_WHITE_BALANCE_BLUE_U] = "CAP_PROP_WHITE_BALANCE_BLUE_U";
    cap_prop_names[cv::CAP_PROP_RECTIFICATION] = "CAP_PROP_RECTIFICATION";
    cap_prop_names[cv::CAP_PROP_MONOCHROME] = "CAP_PROP_MONOCHROME";
    cap_prop_names[cv::CAP_PROP_SHARPNESS] = "CAP_PROP_SHARPNESS";
    cap_prop_names[cv::CAP_PROP_AUTO_EXPOSURE] = "CAP_PROP_AUTO_EXPOSURE";
    cap_prop_names[cv::CAP_PROP_GAMMA] = "CAP_PROP_GAMMA";
    cap_prop_names[cv::CAP_PROP_TEMPERATURE] = "CAP_PROP_TEMPERATURE";
    cap_prop_names[cv::CAP_PROP_TRIGGER] = "CAP_PROP_TRIGGER";
    cap_prop_names[cv::CAP_PROP_TRIGGER_DELAY] = "CAP_PROP_TRIGGER_DELAY";
    cap_prop_names[cv::CAP_PROP_WHITE_BALANCE_RED_V] = "CAP_PROP_WHITE_BALANCE_RED_V";
    cap_prop_names[cv::CAP_PROP_ZOOM] = "CAP_PROP_ZOOM";
    cap_prop_names[cv::CAP_PROP_FOCUS] = "CAP_PROP_FOCUS";
    cap_prop_names[cv::CAP_PROP_GUID] = "CAP_PROP_GUID";
    cap_prop_names[cv::CAP_PROP_ISO_SPEED] = "CAP_PROP_ISO_SPEED";
    cap_prop_names[cv::CAP_PROP_BACKLIGHT] = "CAP_PROP_BACKLIGHT";
    cap_prop_names[cv::CAP_PROP_PAN] = "CAP_PROP_PAN";
    cap_prop_names[cv::CAP_PROP_TILT] = "CAP_PROP_TILT";
    cap_prop_names[cv::CAP_PROP_ROLL] = "CAP_PROP_ROLL";
    cap_prop_names[cv::CAP_PROP_IRIS] = "CAP_PROP_IRIS";
    cap_prop_names[cv::CAP_PROP_SETTINGS] = "CAP_PROP_SETTINGS";
    cap_prop_names[cv::CAP_PROP_BUFFERSIZE] = "CAP_PROP_BUFFERSIZE";
    cap_prop_names[cv::CAP_PROP_AUTOFOCUS] = "CAP_PROP_AUTOFOCUS";
    cap_prop_names[cv::CAP_PROP_SAR_NUM] = "CAP_PROP_SAR_NUM";
    cap_prop_names[cv::CAP_PROP_SAR_DEN] = "CAP_PROP_SAR_DEN";
    cap_prop_names[cv::CAP_PROP_BACKEND] = "CAP_PROP_BACKEND";
    cap_prop_names[cv::CAP_PROP_CHANNEL] = "CAP_PROP_CHANNEL";
    cap_prop_names[cv::CAP_PROP_AUTO_WB] = "CAP_PROP_AUTO_WB";
    cap_prop_names[cv::CAP_PROP_WB_TEMPERATURE] = "CAP_PROP_WB_TEMPERATURE";
    cap_prop_names[cv::CAP_PROP_CODEC_PIXEL_FORMAT] = "CAP_PROP_CODEC_PIXEL_FORMAT";
    cap_prop_names[cv::CAP_PROP_BITRATE] = "CAP_PROP_BITRATE";
    cap_prop_names[cv::CAP_PROP_ORIENTATION_META] = "CAP_PROP_ORIENTATION_META";
    cap_prop_names[cv::CAP_PROP_ORIENTATION_AUTO] = "CAP_PROP_ORIENTATION_AUTO";
    cap_prop_names[cv::CAP_PROP_HW_ACCELERATION] = "CAP_PROP_HW_ACCELERATION";
    cap_prop_names[cv::CAP_PROP_HW_DEVICE] = "CAP_PROP_HW_DEVICE";
    cap_prop_names[cv::CAP_PROP_HW_ACCELERATION_USE_OPENCL] = "CAP_PROP_HW_ACCELERATION_USE_OPENCL";

    for (int i = 0; i < CAP_PROPS_SIZE; i++) {
        cap_prop_values[i] = NOT_USED;
    }
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
            fin >> camera_id;
            cout << "  CAMERA_ID = " << camera_id << endl;
        }
        else if (word == "SHOW_CONFIG_ON_STARTUP") {
            fin >> show_config_on_startup;
            cout << "  SHOW_CONFIG_ON_STARTUP = " << show_config_on_startup << endl;
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
        else {
            for (int i = 0; i < CAP_PROPS_SIZE; i++) {
                if (word == cap_prop_names[i]) {
                    fin >> cap_prop_values[i];
                    cout << "  " << cap_prop_names[i] << " = " << cap_prop_values[i] << endl;
                }
            }
        }
    }
	fin.close();

    return 0;
}


string getDatetimeStr() {
    time_t t = time(nullptr);
    const tm* localTime = localtime(&t);
    std::stringstream s;
    // setw(),setfill()で0詰め

    /*
    s << localTime->tm_year + 1900 << "年";
    s << setw(2) << setfill('0') << localTime->tm_mon + 1 << "月"; 
    s << setw(2) << setfill('0') << localTime->tm_mday << "日";
    s << setw(2) << setfill('0') << localTime->tm_hour << "時";
    s << setw(2) << setfill('0') << localTime->tm_min << "分";
    s << setw(2) << setfill('0') << localTime->tm_sec << "秒";
    s << "\\";
    */

    s << localTime->tm_year + 1900 << "-";
    s << setw(2) << setfill('0') << localTime->tm_mon + 1 << "-";
    s << setw(2) << setfill('0') << localTime->tm_mday << "_";
    s << setw(2) << setfill('0') << localTime->tm_hour << "_";
    s << setw(2) << setfill('0') << localTime->tm_min << "_";
    s << setw(2) << setfill('0') << localTime->tm_sec;
    s << "\\";

    // std::stringにして値を返す
    return s.str();
}

void trackbar_threshold(int value, void *) {
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

cv::VideoCapture cap;
bool seeked = false;

void trackbar_seek(int value, void*) {
    cap.set(cv::CAP_PROP_POS_FRAMES, value);
    seeked = true;
}

// シークバーの操作に合わせて表示画像が切り替わるようにしたかったが、
// 現在位置表示のためにやっている setTrackbarPos でもイベントが発火してしまうため
// ユーザによる操作と区別が付かない

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

    string local_path = GetFolderPath(argv[0]);
    string video_file_path = "";

    if (argc > 1) {
        video_file_path = argv[1];
    }

    cout << "---------------------------------------------" << endl;
    cout << "  LuminousOSC - ver.0.1.0" << endl;
    cout << "---------------------------------------------" << endl;

    // キャプチャのプロパティを格納した配列（と名前の配列）を初期化
    init_cap_props();

    // 設定ファイル読み込み
    string config_path = local_path + "settings.txt";
    LoadConfigFile(config_path.c_str());
    // cout << config_path << endl;

    // ビデオキャプチャの準備
    //cv::VideoCapture cap;

    if (video_file_path != "") {
        cap.open(video_file_path);
    }
    else {
        if (camera_id < 0) {    // 0未満なら入力を受け付ける
            cout << "Input your camera id : ";
            cin >> camera_id;
            cout << endl;
        }
        cap.open(camera_id);
    }

    if (!cap.isOpened()) {
        cout << "can't open VideoCapture." << endl;
        return -1;
    }

    // キャプチャ設定
    for (int i = 0; i < CAP_PROPS_SIZE; i++) {
        if (cap_prop_values[i] != NOT_USED) {
            cap.set(i, cap_prop_values[i]);
            // cout << "applying : " << cap_prop_names[i] << endl;
        }
    }

    cout << endl << "*** Input Information" << endl << endl;
 
    // set_cameraのコード
#ifdef _USE_SET_CAMERA
    scam_init();
    camera_data* cam_data = init_camera_data(camera_id);    // この関数の中で「使用カメラ : ***」と表示される
    if (show_config_on_startup) {
        disp_camera_property(cam_data);
    }
#endif

    int width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int frame_count = int(cap.get(cv::CAP_PROP_FRAME_COUNT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    int mspf = (int)(1000.0 / fps);

    cout << "  Actual Image Size : " << width << "x" << height << endl;
    cout << "  FPS : " << fps << endl;

    cout << endl << "*** Key operations on the image window" << endl << endl;
#ifdef _USE_SET_CAMERA
    cout << "  s     : open camera settings" << endl;
#endif
    cout << "  c     : save image" << endl;
    cout << "  SPACE : pause / resume" << endl;
    cout << "  ESC   : quit" << endl;

    cout << endl << "*** Processing started" << endl;

    cv::namedWindow(win1);
    cv::namedWindow(win2);
    cv::createTrackbar("Threshold", win2, NULL, 255, trackbar_threshold);
    cv::createTrackbar("Min size",  win2, NULL, g_tracker_range_max, trackbar_min_size);
    cv::createTrackbar("Max size",  win2, NULL, g_tracker_range_max, trackbar_max_size);
    cv::setTrackbarPos("Min size",  win2, g_min_size);
    cv::setTrackbarPos("Max size",  win2, g_max_size);
    cv::setTrackbarPos("Threshold", win2, g_threshold);

    // 動画であればシークバーを追加
    if (frame_count > 0) {
        cv::createTrackbar("Frame", win2, NULL, frame_count-1, trackbar_seek);
    }

    // OSC関係
    UdpTransmitSocket transmitSocket(IpEndpointName(g_ip_address.c_str(), g_port));
    char buffer[OSC_PACKET_BUFFER_SIZE];
    osc::OutboundPacketStream packet(buffer, OSC_PACKET_BUFFER_SIZE);

    // 画像保存関係
    string image_path = "";
    int image_index = 0;

    bool video_playing = true;
    cv::Mat frame, gray, binary, label, result;

    while (true) {
        if (video_playing || seeked) {
            cap.read(frame);
            seeked = false;
        }
        DWORD time_start = timeGetTime();

        // 動画であれば再生位置表示＆ループ再生制御
        if (video_playing && frame_count > 0) {
            int frame_index = (int)cap.get(cv::CAP_PROP_POS_FRAMES);
            cv::setTrackbarPos("Frame", win2, frame_index);
            if (frame_index == frame_count) {
                cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            }
        }

        // 画像処理
        result = frame.clone();
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::threshold(gray, binary, g_threshold, 255, cv::THRESH_BINARY);
        cv::Mat stats, centroids;
        int n_label = cv::connectedComponentsWithStats(binary, label, stats, centroids);
        cv::cvtColor(binary, binary, cv::COLOR_GRAY2BGR);   // カラーで結果表示するため

        // ラベル0番は背景なので1番から
        std::vector<cv::Point> points;
        for (int i = 1; i < n_label; i++) {
            // バウンディングボックス
            int* param = stats.ptr<int>(i);
            int area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
            int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
            int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
            int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
            int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
            // 重心
            double* param_c = centroids.ptr<double>(i);
            int cx = static_cast<int>(param_c[0]);
            int cy = static_cast<int>(param_c[1]);

            // サイズに収まっていれば
            if ( g_min_size <= area  && area <= g_max_size ) {

                // 候補領域にはバウンディングボックスと水色点を描く
                cv::rectangle(binary, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 255), 1);
                cv::circle(binary, cv::Point(cx, cy), 3, cv::Scalar(255, 255, 0), -1);
                cv::circle(binary, cv::Point(cx, cy), 3, cv::Scalar(0, 0, 0), 1);
                //cv::circle(result, cv::Point(cx, cy), 3, cv::Scalar(255, 255, 255), -1);
                //cv::circle(result, cv::Point(cx, cy), 3, cv::Scalar(0, 0, 0), 1);

                // リストに追加（上限あり）
                if ( points.size() < g_max_points ){
                    points.push_back(cv::Point(cx, cy));
                }
            }
        }

        // 規定内の個数の点に対して赤点と番号を描画
        for (int i = 0; i < points.size(); i++ ) {
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

        // OSC
        packet.Clear();
        packet << osc::BeginBundleImmediate << osc::BeginMessage(g_address_pattern.c_str());
        for (cv::Point pos : points) {
            packet << pos.x << pos.y;
            
        }
        packet << osc::EndMessage << osc::EndBundle;
        transmitSocket.Send(packet.Data(), packet.Size());

        cv::imshow(win1, result);
        cv::imshow(win2, binary);

        DWORD time_end = timeGetTime();
        int delay_ms = mspf - (time_end - time_start);
        if (delay_ms <= 0) {
            delay_ms = 1;
        }
        const int key = cv::waitKey(delay_ms);

        if (key == 27) { // ESC
            break;
        }
        else if (key == ' ') {
            video_playing = !video_playing;
        }
#ifdef _USE_SET_CAMERA
        else if (key == 's') {
            disp_camera_property(cam_data);
        }
#endif
        else if (key == 'c') {
            if (image_index == 0) {
                image_path = local_path + getDatetimeStr();
                cout << endl << "  The image is stored to: " << endl << "  " << image_path << endl << endl;
            }

            // フォルダが存在しなかったら作る（途中で消される場合もあるのでここで処理）
            struct stat statBuf;
            if ( stat(image_path.c_str(), &statBuf) != 0) {
                std::filesystem::create_directory(image_path);
            }

            string filename;
            stringstream ss;
            ss << setfill('0') << setw(4) << right << to_string(image_index) << ".jpg";
            ss >> filename;
            cv::imwrite(image_path + "input_"  + filename, frame);
            cv::imwrite(image_path + "result_" + filename, result);
            cout << "  Image saved : " << "input_" + filename + ", " + "result_" + filename << endl;
            image_index++;
        }
    }
    cv::destroyAllWindows();
    cap.release();

#ifdef _USE_SET_CAMERA
    // set_cameraの後始末
    free_camera_data(cam_data);
#endif

    cout << endl << "*** Finished" << endl;
	return 0;
}