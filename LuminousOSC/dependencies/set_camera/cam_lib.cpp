/*
  Windows専用 カメラ利用サブルーチン Ver.set1.11

  カメラパラメータ設定用に特化。

  Ver.1.00 scapterから作成。サブルーチン多すぎて断念。
  Ver.1.01 DirectShowのヘルプを基に作成。とりあえず動いた。
  Ver.1.02 コールバック方式に変更、GraphEdit対応
  Ver.1.03 一通り動くようになったので余計なものを削除した
  Ver.1.04 シャッタースピード指定を追加
  jpeg分岐
  Ver.1.05 MJPEG専用(可変長)に変更
  Ver.1.06 マルチバッファ化
  Ver.1.07 間引きモード追加
  Ver.1.08 2011/08/05 Intencity用定義の追加
  set分岐
  Ver.1.09 2011/08/09 カメラ設定専用にして作成、サンプルグラバ削除
  Ver.1.10 2011/08/11 カメラプロパティ画面表示サブルーチン追加
  Ver.1.11 2011/11/09 カメラ名検索サブルーチン追加
*/
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------
// 各種設定
// デバグ用grapheditに表示されるようにする定義
#undef SHOWGRAPH

extern "C" {
#include "s_malloc.h"
// 宣言する関数を全てC言語互換にする(Cから呼び出し可能にする)
}
#include "cam_lib.h"
#include "disp_mediatype.h"
#include <Dvdmedia.h>

#pragma comment (lib,"strmiids.lib")

// --------------------------------------------------------------
// サンプルにあるサブルーチン
#ifdef SHOWGRAPH
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;

    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
 
   StringCchPrintfW(
        wsz, STRING_LENGTH, 
        L"FilterGraph %08x pid %08x", 
        (DWORD_PTR)pUnkGraph, 
        GetCurrentProcessId()
        );
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    
    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
  IRunningObjectTable *pROT;
  if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
    {
      pROT->Revoke(pdwRegister);
      pROT->Release();
    }
}
#endif

static IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
  BOOL       bFound = FALSE;
  IEnumPins  *pEnum;
  IPin       *pPin;
  
  if (pFilter->EnumPins(&pEnum) != S_OK)
    return NULL ;
  while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
      PIN_DIRECTION PinDirThis;
      pPin->QueryDirection(&PinDirThis);
      if (bFound = (PinDir == PinDirThis))
        break;
      pPin->Release();
    }
  pEnum->Release();
  return (bFound ? pPin : 0);
}

HRESULT GetUnconnectedPin(
    IBaseFilter *pFilter,   // フィルタへのポインタ。
    PIN_DIRECTION PinDir,   // 検索するピンの方向。
    IPin **ppPin)           // ピンへのポインタを受け取る。
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // 既に接続済み、必要なピンではない。
            {
                pTmp->Release();
            }
            else  // 未接続、これが必要なピンである。
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // 一致するピンが見つからなかった。
    return E_FAIL;
}

HRESULT ConnectFilters(
    IGraphBuilder *pGraph, // フィルタ グラフ マネージャ。
    IPin *pOut,            // アップストリーム フィルタの出力ピン。
    IBaseFilter *pDest)    // ダウンストリーム フィルタ。
{
    if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }
#ifdef debug
        PIN_DIRECTION PinDir;
        pOut->QueryDirection(&PinDir);
        _ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

    // ダウンストリーム フィルタの入力ピンを検索する。
    IPin *pIn = 0;
    HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (FAILED(hr))
    {
        return hr;
    }
    // 接続を試す。
    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    return hr;
}

HRESULT ConnectFilters(
    IGraphBuilder *pGraph, 
    IBaseFilter *pSrc, 
    IBaseFilter *pDest)
{
    if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // 最初のフィルタの出力ピンを検索する。
    IPin *pOut = 0;
    HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (FAILED(hr)) 
    {
        return hr;
    }
    hr = ConnectFilters(pGraph, pOut, pDest);
    pOut->Release();
    return hr;
}

// --------------------------------------------------------------
// ワイド文字列関係のサブルーチン
// --------------------------------------------------------------
// ワイド文字列をマルチバイト文字に変換して表示する
void print_wchar(char head[],LPCWSTR value)
{
  char buf[1024];

  WideCharToMultiByte(CP_ACP, 0, value,-1, buf, 1023, 0, 0); 
  printf("%s : %s\n",head,buf);
}
// ワイド文字列中に指定したマルチバイト文字列が存在するか検索する
// 存在する：TRUEを返す / 存在しない：FALSEを返す
int is_exist_str_in_wstr(char key[],LPCWSTR target)
{
  WCHAR buf[1024];

  MultiByteToWideChar(CP_ACP,0,key,-1,buf,1024);
  if(wcsstr(target,buf) != 0) return(TRUE);
  return(FALSE);
}
// --------------------------------------------------------------
// グラフにフィルタを追加する
int add_graph_filter(IGraphBuilder *pGraph,IBaseFilter *pSrc,WCHAR name[])
{
  WCHAR buf[512];
  int i;
  HRESULT hr;

  hr = pGraph->AddFilter(pSrc, name); // まずそのままやってみる
  i = 0;
  while(hr != S_OK)
  {
    switch(hr)
	{
      case VFW_S_DUPLICATE_NAME:
        StringCbPrintfW(buf,512,L"%s %d",name,i);
	    hr = pGraph->AddFilter(pSrc, buf);
	  default:
	    return(TRUE);
	}
  }
  return(FALSE); // 成功したとして返す
}

// DirectShow全体の初期化
int scam_init(void)
{
  HRESULT hr;

  hr = CoInitialize(NULL); // COMオブジェクトの初期化
  if(FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return(TRUE);
    }
  return(FALSE); // 初期化成功
}


// 数字のみの文字列かどうかを判定
// 数字のみである場合:TRUEを返す
int is_number_string(char *str)
{
  if(str == NULL) return(FALSE);
  while(*str != '\0')
    {
      switch(*str)
      {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
	  str++;
	  break;
      default:
	  return(FALSE);
	  break;
      }
    }
  return(TRUE);
}

// カメラの名前もしくは数字からカメラの番号を返す
int scam_search_camera(char *camera_name,int disp_mode)
{
  int no,ret_code;
  HRESULT hr;
  ICreateDevEnum *pDevEnum;
  IMoniker * pMoniker;
  IEnumMoniker *pClassEnum = NULL;
  IPropertyBag *pPropBag;
  VARIANT varName;

  ret_code = -1;
  if(camera_name == NULL)
    {
      if(disp_mode) printf("ERROR - No camera name.");
      return(ret_code);
    }

  // 数字のみで構成されていた時は、単純にatoiで変換
  if(is_number_string(camera_name))
    {
      ret_code = atoi(camera_name);
    }

  if(ret_code == -1)
    {
      hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, // カメラの列挙子の初期化
	      IID_ICreateDevEnum, (void **)&pDevEnum);
  if(FAILED(hr))
    {
      if(disp_mode) printf("ERROR - Could not create the Device Enum");
      return(ret_code);
    }
  hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
	  &pClassEnum, 0);
  if(hr != S_OK)
    {
      pDevEnum->Release() ;
      return(ret_code);
    }
  ret_code = -1;
  no = 0;
  while((ret_code == -1) && (pClassEnum->Next(1, &pMoniker, NULL) == S_OK))
    {
      hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,(void**)(&pPropBag));
      if(SUCCEEDED(hr))
	{
	  // 説明またはフレンドリ名を検索する。
	  VariantInit(&varName);
	  hr = pPropBag->Read(L"Description", &varName, 0);
	  if(SUCCEEDED(hr))
	    {
	      if(is_exist_str_in_wstr(camera_name,varName.bstrVal))
	        {
		  // カメラ見つかった
		  ret_code = no;
	        }
	      VariantClear(&varName); 
	    }
	  hr = pPropBag->Read(L"FriendlyName", &varName, 0);
	  if(SUCCEEDED(hr))
	    {
	      if(is_exist_str_in_wstr(camera_name,varName.bstrVal))
	        {
		  // カメラ見つかった
		  ret_code = no;
	        }
	      VariantClear(&varName); 
	    }
	  pPropBag->Release();
	}
      pMoniker->Release();
      no++;
    }
  pClassEnum->Release();
  pDevEnum->Release();
  }
  return(ret_code);
}

// --------------------------------------------------------------
// カメラを初期化設定する
// --------------------------------------------------------------
camera_data *init_camera_data(int ncam)
{
  camera_data *ret_code;
  HRESULT hr;
  IGraphBuilder *pGraph;
  ICreateDevEnum *pDevEnum;
  ULONG cFetched;
  IMoniker * pMoniker = NULL;   // NULLを追加した
  IEnumMoniker *pClassEnum = NULL;
  IBaseFilter *pSrc;
#ifdef SHOWGRAPH
  DWORD dwGraphRegister=0;
#endif
 
  ret_code = NULL;

  hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, // カメラの列挙子の初期化
	      IID_ICreateDevEnum, (void **)&pDevEnum);
  if(FAILED(hr))
    {
      printf("ERROR - Could not create the Device Enum");
      return (NULL);
    }
  hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
	  &pClassEnum, 0);
  if(hr != S_OK)
    {
      pDevEnum->Release() ;
      return(NULL);
    }
  //ncam番目のカメラを使う
  if(ncam < 0) ncam = 0;
  while(ncam >= 0)
    {
	  if(pClassEnum->Next(1, &pMoniker, &cFetched) != S_OK)
	  {
	    return(NULL);
	  }
	  ncam--;
    }
  hr = pClassEnum->Reset();
  if(hr != S_OK)
    {
      return(NULL);
    }
  if(pMoniker->BindToObject(0,0,IID_IBaseFilter,(void **)&pSrc)
	 != S_OK)
	{
	  return(NULL);
	}
#if 01
	  IPropertyBag *pPropBag;
	  VARIANT varName;

	  // カメラ情報の表示
	  hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				       (void**)(&pPropBag));
	  if(SUCCEEDED(hr))
	    {
	  // 説明またはフレンドリ名を検索する。
	  VariantInit(&varName);
	  hr = pPropBag->Read(L"Description", &varName, 0);
	  if(FAILED(hr))
	    {
	      hr = pPropBag->Read(L"FriendlyName", &varName, 0);
	    }
	  if(SUCCEEDED(hr))
	    {
          // print_wchar((char*)"使用カメラ", varName.bstrVal);
          print_wchar((char*)"  Device", varName.bstrVal);          // 書き換えた

	      VariantClear(&varName); 
	    }
	  pPropBag->Release();
		} 
#endif
  pMoniker->Release();
  pClassEnum->Release();
  pDevEnum->Release();

  hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, // フィルタグラフの初期化
      IID_IGraphBuilder, (void **)&pGraph);
  if(FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return(NULL);
    }

#ifdef SHOWGRAPH
  hr = AddToRot(pGraph, &dwGraphRegister);
#endif
  // カメラのフィルタを追加する
  if(add_graph_filter(pGraph,pSrc, (WCHAR*)L"Video Capture"))
  {
	  return(NULL);
  }

  IPin *pSrcOutams;
  IAMStreamConfig *pAMStreamConfig;
  AM_MEDIA_TYPE *amt;

  pSrcOutams = GetPin(pSrc, PINDIR_OUTPUT);
  hr = pSrcOutams->QueryInterface(IID_IAMStreamConfig,(void**)&pAMStreamConfig);
  if(hr != S_OK)
  {
    return(NULL);
  }
  // デフォルトのMediaTypeを表示
  hr = pAMStreamConfig->GetFormat(&amt);
  if(hr != S_OK)
  {
    return(NULL);
  }


  // comment out by hashimoto
  // printf("Default : ");
  // disp_mediatype(amt);

  pAMStreamConfig->Release();

  // カメラの初期化完了
  ret_code = xalloc(camera_data,1);
  ret_code->pGraph = pGraph;
  ret_code->pSrc = pSrc;

  return (ret_code);
}

void wait_camera_property(camera_data *cam_data)
{
  DWORD dwParam;

  if(cam_data->hThread != NULL)
    {
      printf("Wait process.\n");
      GetExitCodeThread(cam_data->hThread , &dwParam);
      while(dwParam == STILL_ACTIVE)
	{
	  Sleep(30);
	  GetExitCodeThread(cam_data->hThread , &dwParam);
	}
      cam_data->hThread = NULL;
    }
}

camera_data *free_camera_data(camera_data *cam_data)
{
  HRESULT hr;

  if(cam_data != NULL)
    {

    // comment out by hashimoto
	//wait_camera_property(cam_data);


#ifdef SHOWGRAPH
	  if(cam_data->dwGraphRegister != 0)
	    {
		  RemoveFromRot(cam_data->dwGraphRegister);
	    }
#endif
	  if(cam_data->pPin != NULL)
		{
		  hr = cam_data->pPin->Release();
		}

	  if(cam_data->pVidCtrl != NULL)
	    {
		  hr = cam_data->pVidCtrl->Release();
	    }
	  if(cam_data->pVidProc != NULL)
	    {
		  hr = cam_data->pVidProc->Release();
	    }
	  if(cam_data->pCamCtrl != NULL)
	    {
		  hr = cam_data->pCamCtrl->Release();
	    }
	  if(cam_data->pSrc != NULL)
	    {
		  hr = cam_data->pSrc->Release();
	    }
	  if(cam_data->pGraph != NULL)
	    {
		  hr = cam_data->pGraph->Release();
	    }
      free(cam_data);
    }
  CoUninitialize();
  return(NULL);
}


struct id_list {
	char *id;
	long property;
};

struct id_list ccp_list[] = {
	{(char*)"CameraControl_Pan",CameraControl_Pan},
	{(char*)"CameraControl_Tilt",CameraControl_Tilt},
  	{(char*)"CameraControl_Roll",CameraControl_Roll},
  	{(char*)"CameraControl_Zoom",CameraControl_Zoom},
  	{(char*)"CameraControl_Exposure",CameraControl_Exposure},
  	{(char*)"CameraControl_Iris",CameraControl_Iris},
  	{(char*)"CameraControl_Focus",CameraControl_Focus},
	{NULL,0}};

struct id_list vpa_list[] = {
	{(char*)"VideoProcAmp_Brightness",VideoProcAmp_Brightness},
	{(char*)"VideoProcAmp_Contrast",VideoProcAmp_Contrast},
  	{(char*)"VideoProcAmp_Hue",VideoProcAmp_Hue},
  	{(char*)"VideoProcAmp_Saturation",VideoProcAmp_Saturation},
  	{(char*)"VideoProcAmp_Sharpness",VideoProcAmp_Sharpness},
  	{(char*)"VideoProcAmp_Gamma",VideoProcAmp_Gamma},
  	{(char*)"VideoProcAmp_ColorEnable",VideoProcAmp_ColorEnable},
  	{(char*)"VideoProcAmp_WhiteBalance",VideoProcAmp_WhiteBalance},
  	{(char*)"VideoProcAmp_BacklightCompensation",VideoProcAmp_BacklightCompensation},
	{(char*)"VideoProcAmp_Gain",VideoProcAmp_Gain},
	{NULL,0}};

struct id_list vcp_list[] = {
	{(char*)"VideoControlFlag_FlipHorizontal",VideoControlFlag_FlipHorizontal},
	{(char*)"VideoControlFlag_FlipVertical",VideoControlFlag_FlipVertical},
	{(char*)"VideoControlFlag_ExternalTriggerEnable",VideoControlFlag_ExternalTriggerEnable},
	{(char*)"VideoControlFlag_Trigger",VideoControlFlag_Trigger},
	{NULL,0}};

int search_list(struct id_list list[],char id[])
{
  int i;
  for(i=0;list[i].id!=NULL;i++)
	{
	  if(strstr(list[i].id,id)!= NULL) return(i);
	}
  return(-1);
}

#define m_camera_flag(a)	((a)==CameraControl_Flags_Auto?"Auto":((a)==CameraControl_Flags_Manual?"Manual":"Unknown"))

void set_camera_control(camera_data *cam_data,char head[],int cat,long value,long flag)
{
  long ssvalue,ssflag;
  HRESULT hr;

  if(cam_data->pCamCtrl == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMCameraControl,(void**)&(cam_data->pCamCtrl));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMCameraControl failed.\n");
		  cam_data->pCamCtrl = NULL;
		  return;
		}
	}
  
  printf("Now will try set param %s.\n",head);
  hr = cam_data->pCamCtrl->Get(cat,&ssvalue,&ssflag);
  if(hr != S_OK)
	{
	  printf("%s Get failed. code = %08x\n",head,hr);
	  return;
	}
  printf("Now param : %ld %ld(%s)\n",ssvalue,ssflag,m_camera_flag(ssflag));
  printf("Try to set param : %ld %ld(%s)\n",value,flag,m_camera_flag(flag));
  hr = cam_data->pCamCtrl->Set(cat,value,flag);
  if(hr != S_OK)
	{
	  printf("%s Set failed. code = %08x\n",head,hr);
	}
  hr = cam_data->pCamCtrl->Get(cat,&ssvalue,&ssflag);
  if(hr != S_OK)
	{
	  printf("%s Get failed. code = %08x\n",head,hr);
	}
  printf("Final param : %ld %ld(%s)\n",ssvalue,ssflag,m_camera_flag(ssflag));
  if((ssvalue != value)||(ssflag != flag))
	{
	  printf("Warning : Parameter not set.\n");
	}
}

void list_camera_control(camera_data *cam_data)
{
  HRESULT hr;
  int i;
  long value,now_flag;
  long min,max,delta,def,capflag;
	
  if(cam_data->pCamCtrl == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMCameraControl,(void**)&(cam_data->pCamCtrl));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMCameraControl failed.\n");
		  cam_data->pCamCtrl = NULL;
		  return;
		}
	}
  for(i=0;ccp_list[i].id!=NULL;i++)
	{
	  value = 0;now_flag = 0;
	  hr = cam_data->pCamCtrl->Get(ccp_list[i].property,&value,&now_flag);
	  hr = cam_data->pCamCtrl->GetRange(ccp_list[i].property,&min,&max,&delta,&def,&capflag);
	  if(hr == S_OK)
		{
			printf("%ld\t%ld\t%ld\t%ld\t%ld\t%ld/%ld\t%s\n",min,value,max,delta,def,
														   now_flag,capflag,ccp_list[i].id);
		}
	}
}

#define m_video_flag(a)	((a)==VideoProcAmp_Flags_Auto?"Auto":((a)==VideoProcAmp_Flags_Manual?"Manual":"Unknown"))

void set_video_procamp(camera_data *cam_data,char head[],int cat,long value,long flag)
{
  long ssvalue,ssflag;
  HRESULT hr;

  if(cam_data->pVidProc == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMVideoProcAmp,(void**)&(cam_data->pVidProc));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMVideoProcAmp failed.\n");
		  cam_data->pVidProc = NULL;
		  return;
		}
	}

  printf("Now will try set param %s.\n",head);
  hr = cam_data->pVidProc->Get(cat,&ssvalue,&ssflag);
  if(hr != S_OK)
	{
	  printf("%s Get failed. code = %08x\n",head,hr);
	  return;
	}
  printf("Now param : %ld %ld(%s)\n",ssvalue,ssflag,m_video_flag(ssflag));
  printf("Try to set param : %ld %ld(%s)\n",value,flag,m_video_flag(flag));
  hr = cam_data->pVidProc->Set(cat,value,flag);
  if(hr != S_OK)
	{
	  printf("%s Set failed. code = %08x\n",head,hr);
	}
  hr = cam_data->pVidProc->Get(cat,&ssvalue,&ssflag);
  if(hr != S_OK)
	{
	    printf("%s Get failed. code = %08x\n",head,hr);
	}
  printf("Final param : %ld %ld(%s)\n",ssvalue,ssflag,m_video_flag(ssflag));
  if((ssvalue != value)||(ssflag != flag))
	{
	  printf("Warning : Parameter not set.\n");
	}
}

void list_video_procamp(camera_data *cam_data)
{
  HRESULT hr;
  int i;
  long value,now_flag;
  long min,max,delta,def,capflag;

  if(cam_data->pVidProc == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMVideoProcAmp,(void**)&(cam_data->pVidProc));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMVideoProcAmp failed.\n");
		  cam_data->pVidProc = NULL;
		  return;
		}
	}
  // printf("Listup Video param.\n");
  for(i=0;vpa_list[i].id!=NULL;i++)
	{
	  value = 0;now_flag = 0;
	  hr = cam_data->pVidProc->Get(vpa_list[i].property,&value,&now_flag);
	  hr = cam_data->pVidProc->GetRange(vpa_list[i].property,&min,&max,&delta,&def,&capflag);
	  if(hr == S_OK)
		{
			printf("%ld\t%ld\t%ld\t%ld\t%ld\t%ld/%ld\t%s\n",min,value,max,delta,def,
														   now_flag,capflag,vpa_list[i].id);
		}
	}
}

//------------------------------------------------------------------------------------------------
void set_video_control(camera_data *cam_data,char head[],int cat,long value,long flag)
{
  HRESULT hr;

  if(cam_data->pVidCtrl == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMVideoControl,(void**)&(cam_data->pVidCtrl));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMVideoControl failed.\n");
		  cam_data->pVidCtrl = NULL;
		  return;
		}
	}


}

void list_video_control(camera_data *cam_data)
{
  HRESULT hr;
  long caps,mode;
  int i;

  if(cam_data->pVidCtrl == NULL)
	{
	  hr = cam_data->pSrc->QueryInterface(IID_IAMVideoControl,(void**)&(cam_data->pVidCtrl));
	  if(hr != S_OK)
		{
		  printf("QueryInterface IID_IAMVideoControl failed.\n");
		  cam_data->pVidCtrl = NULL;
		  return;
		}
    }
  if(cam_data->pPin == NULL)
	{
	  cam_data->pPin = GetPin(cam_data->pSrc,PINDIR_OUTPUT);
	}
  hr = cam_data->pVidCtrl->GetCaps(cam_data->pPin,&caps);
  hr = cam_data->pVidCtrl->GetMode(cam_data->pPin,&mode);
  if(hr == S_OK)
	{
	  for(i=0;vcp_list[i].id!=NULL;i++)
		{
		  if(caps & vcp_list[i].property)
			{
			  printf("\t%ld\t\t\t\t\t%s\n",(mode & vcp_list[i].property?1:0),vcp_list[i].id);
			}
		}
	}
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int set_all_param(camera_data *cam_data,char id[],long value,long flag)
{
  int i;
  i = search_list(ccp_list,id);
  if(i >= 0)
	{
	  set_camera_control(cam_data,ccp_list[i].id,ccp_list[i].property,value,flag);
	}
  else
	{
	  i = search_list(vpa_list,id);
	  if(i >= 0)
		{
		  set_video_procamp(cam_data,vpa_list[i].id,vpa_list[i].property,value,flag);
		}
	  else
		{
		  printf("Can't find control id '%s'\n",id);
		}
	}
  return(0);
}

void list_all_param(camera_data *cam_data)
{
  printf("Min\tCurrent\tMax\tStep\tDefault\tFlags\tName\n");
  list_camera_control(cam_data);
  list_video_procamp(cam_data);
  list_video_control(cam_data);
}

/*
  カメラのプロパティを表示する
  参考：http://msdn.microsoft.com/en-us/library/dd375480%28v=vs.85%29.aspx
*/
static DWORD WINAPI disp_camera_property_thread(LPVOID lpParameter)
{
  camera_data *cam_data;
  HRESULT hr;
  ISpecifyPropertyPages *pProp;
  FILTER_INFO FilterInfo;
  IUnknown *pFilterUnk;
  CAUUID caGUID;
  
  cam_data = (camera_data *)lpParameter;
  hr = cam_data->pSrc->QueryInterface(IID_ISpecifyPropertyPages,(void**)&(pProp));
  if(hr == S_OK)
	{
      pProp->GetPages(&caGUID);	// caGUIDの獲得
	  pProp->Release(); // お役御免
	  hr = cam_data->pSrc->QueryFilterInfo(&FilterInfo); // フィルタ名の獲得
	  cam_data->pSrc->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);	// IUnknownポインタの獲得

	  OleCreatePropertyFrame(
          GetDesktopWindow(),		// 親ウィンドウ
          0,0,						// x,y (予約済み)
		  FilterInfo.achName,		// ダイアログ ボックスのキャプション
          1,						// フィルタの数
          &pFilterUnk,				// フィルタへのポインタ
          caGUID.cElems,			// プロパティ ページの数
          caGUID.pElems,			// プロパティ ページ CLSID へのポインタ
          0,						// ロケール識別子
          0,NULL);					// 予約済み
	  pFilterUnk->Release();
	  FilterInfo.pGraph->Release();
	  CoTaskMemFree(caGUID.pElems);	  
	}
  ExitThread(TRUE);
}

void disp_camera_property(camera_data *cam_data)
{
  HANDLE hr;

  hr = CreateThread(NULL , 0 , disp_camera_property_thread ,(void*)cam_data , 0 , NULL);
  if(hr == NULL)
	{
	  printf("Can't creat property thread.\n");
	}
  cam_data->hThread = hr;
}
