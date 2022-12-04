# LuminousOSC
An application that detects luminous points from camera images and sends their positions via OSC

![LuminousOSC_overview_v100](https://user-images.githubusercontent.com/736387/205011797-a971f181-f9f6-4f87-8a80-c9c010369a19.png)


## Download

Please download the latest version from [releases](https://github.com/kougaku/LuminousOSC/releases).  

## How to Use
1. Run LuminousOSC. Then webcam is opened, detection and sending are started.
1. Set the maximum number of points you wish to send in GUI.
1. Adjust threshold for binarization. To successfully extract luminous blobs, it is important to adjust the exposure in the camera settings.
1. Adjust max area and min area for detecting luminous points.
1. Adjust matching distance for good tracking.
1. In your application, receive the OSC message that contains the position and ID number of the detected points.

Video file also can be used as input. Select "Load Video File" in the GUI. On Windows, to support QuickTime video files or files like MP4 video you will need to install the video codecs onto your machine. K-Lite Codec Pack is recommended.

![v100-camera](https://user-images.githubusercontent.com/736387/205016249-a9174901-62d3-4838-80c1-324f155a6adf.png)

## OSC message
LuminousOSC sends position and ID number of luminous point via OSC.

- Address Pattern: /points
  - This is default. You can change on GUI.
- Data (int values): x0, y0, id0, x1, y1, id1, x2, y2, id2 ... xn, yn, idn

## Receiving code example
P5_receiver_demo is very simple example written in Processing.  
Just receive OSC message and draw the points. It requires oscP5 library. 
  
  
https://user-images.githubusercontent.com/736387/205017269-5cb456cc-97db-430f-a8a0-12c54011cba8.mp4

## Dependencies
openFrameworks (v0.11.2), addons : ofxCv, ofxGui, ofxOpenCV, ofxOsc, ofxXmlSettings
