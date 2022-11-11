# LuminousOSC
An application that detects luminous points from camera images and sends their positions via OSC

![LuminousOSC_overview](https://user-images.githubusercontent.com/736387/200601487-351cacce-3ddb-4392-9f1e-56ff5bb7ac3a.png)

## Download

Please download the latest version from releases.  
https://github.com/kougaku/LuminousOSC/releases

## How to Use

1. Run LuminousOSC.exe. Then webcam is opened, detection and sending are started.
1. Adjust threshold for binarization. To successfully extract luminous blobs, it is important to adjust the exposure in the camera settings.
1. Adjust min size and max size for detecting luminous points.
1. Receive the OSC message that contains the coordinates of points in your application. 

Video file also can be used as input.  
(Windows) Drag-and-drop a video file onto the LuminousOSC.exe.  
(Mac) `./LuminousOSC your_video.mp4`

![example](https://user-images.githubusercontent.com/736387/200622713-89923d33-112c-4e76-9c95-97f9779ea7e4.png)

## OSC message

- Address Pattern: /points
  - This is default. You can change in settings.txt.
- Data (int values): x0, y0, x1, y1, x2, y2, ... xn, yn

Note:
LuminousOSC currently does not have a feature to track multiple points. If you want to track multiple points, make the correspondence based on the proximity of the coordinate values between frames. See P5_tracking_demo that implements a simple tracking.

## Receiving code examples (Processing)
Requires oscP5 library.

- P5_receive_points : Very simple example. Just receive OSC message and draw the points.
- P5_tracking_demo : Simple tracking demo. It matches points that are close in distance between frames.
  
https://user-images.githubusercontent.com/736387/201339470-30560083-9656-4e58-b376-af3f73c1f6a4.mp4



## Dependencies
- [oscpack](http://www.rossbencina.com/code/oscpack)
- [OpenCV](https://opencv.org/)
- [set_camera](https://saibara.sakura.ne.jp/program/directshow/)
