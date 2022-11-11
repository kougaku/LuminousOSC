#include <dshow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"

// マルチバッファの数
#define NBUF	8

// カメラキャプチャに関するセット
typedef struct {
  IGraphBuilder *pGraph;
  IBaseFilter *pSrc;
  IPin *pPin;
  IAMCameraControl *pCamCtrl;
  IAMVideoProcAmp *pVidProc;
  IAMVideoControl *pVidCtrl;
  HANDLE hThread;	// プロパティページ表示のためのスレッド
#ifdef SHOWGRAPH
  DWORD dwGraphRegister;
#endif
} camera_data;

#include "s_value.h"
#include "s_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif

int scam_init(void);
int scam_search_camera(char *camera_name,int disp_mode);
camera_data *init_camera_data(int ncam);
camera_data *free_camera_data(camera_data *cam_data);

int set_all_param(camera_data *cam_data,char id[],long value,long flag);
void list_all_param(camera_data *cam_data);

void disp_camera_property(camera_data *cam_data);
void wait_camera_property(camera_data *cam_data);

#ifdef __cplusplus
}

#pragma comment(lib,"winmm.lib")
#endif
