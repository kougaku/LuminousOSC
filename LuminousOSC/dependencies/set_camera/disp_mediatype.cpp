/*
  disp_mediatype.cpp　Ver.1.02
  MediaTypeの詳細表示サブルーチン

  Ver.1.00 2011/03/01 disp_mediatype.fから独立ルーチン化、整理。  
  Ver.1.01 2011/06/29 GUIDが構造体なので、ポインタ化してみる。
  Ver.1.02 2011/07/22 FOURCCの表示を整理する
*/
#include <stdio.h>
#include <stdlib.h>
#include <dshow.h>
#include <Dvdmedia.h>
#include <Mmreg.h>

typedef struct{
	const GUID *guid;
	char *list;
} guid_list;

void disp_guid(const GUID *guid)
{
  printf("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
	  guid->Data1,guid->Data2,guid->Data3,guid->Data4[0],guid->Data4[1],
	  guid->Data4[2],guid->Data4[3],guid->Data4[4],guid->Data4[5],guid->Data4[6],guid->Data4[7]);
}

// C++だと意味ないかもしれない
int is_same_guid(const GUID *id1,const GUID *id2)
{
	return((id1->Data1==id2->Data1)&&(id1->Data2==id2->Data2)&&(id1->Data3==id2->Data3)
		&&(id1->Data4[0]==id2->Data4[0])&&(id1->Data4[1]==id2->Data4[1])&&(id1->Data4[2]==id2->Data4[2])&&(id1->Data4[3]==id2->Data4[3])
		&&(id1->Data4[4]==id2->Data4[4])&&(id1->Data4[5]==id2->Data4[5])&&(id1->Data4[6]==id2->Data4[6])&&(id1->Data4[7]==id2->Data4[7]));
}

// GUIDの表示: -1 : GUID_NULL -2: 完全に不明
int disp_guid_list(const GUID *guid,guid_list *list)
{
  int i;
  if(list == NULL) return(-100);
  i = 0;
  while(list[i].list != NULL)
  {
    if(is_same_guid(guid,list[i].guid))
	  {
	    printf(list[i].list);
		return(i);
	  }
	i++;
  }
  if(is_same_guid(guid,list[i].guid)) // 最後のものに一致すると
	{
      printf("GUID_NULL");
	  return(-1);
	}
  // 未知のもの
#if 0
  printf("Unknown.");
#endif
  return(-2);
}

guid_list formattype[] ={
	{&FORMAT_None,			(char*)"None"},
	{&FORMAT_DvInfo,		(char*)"DvInfo"},
	{&FORMAT_MPEGVideo,		(char*)"MPEGVideo"},
	{&FORMAT_MPEG2Video,	(char*)"MPEG2Video"},
	{&FORMAT_VideoInfo,		(char*)"VideoInfo"},
	{&FORMAT_VideoInfo2,	(char*)"VideoInfo2"},
	{&FORMAT_WaveFormatEx,	(char*)"WaveFormatEx"},
	{&GUID_NULL,NULL}};

void disp_formattype(const GUID *guid)
{
  if(disp_guid_list(guid,formattype) == -2)
  {
    disp_guid(guid);
  }
}

int is_forcc_guid(const GUID *guid)
{
  return((guid->Data2 == 0x0000)
	  && (guid->Data3 == 0x0010)
	  && (guid->Data4[0] == 0x80)
	  && (guid->Data4[1] == 0x00)
	  && (guid->Data4[2] == 0x00)
	  && (guid->Data4[3] == 0xaa)
	  && (guid->Data4[4] == 0x00)
	  && (guid->Data4[5] == 0x38)
	  && (guid->Data4[6] == 0x9b)
	  && (guid->Data4[7] == 0x71));
}

static void print_char(const unsigned char c)
{
  if(isgraph(c))
    {
      putchar(c);
      return;
    }
  printf("[%02x]",c);
  return;
}

void print_forcc(const GUID *guid)
{

  printf("(FOURCC'");
  print_char((guid->Data1 >>  0) & 0xff);
  print_char((guid->Data1 >>  8) & 0xff);
  print_char((guid->Data1 >> 16) & 0xff);
  print_char((guid->Data1 >> 24) & 0xff);
  printf("')");
}

guid_list mediasubtype[] = {
	{&MEDIASUBTYPE_PCM,(char*)"PCMオーディオ"},
	{&MEDIASUBTYPE_MPEG1Packet,(char*)"MPEG1オーディオPacket"},
	{&MEDIASUBTYPE_MPEG1Payload,(char*)"MPEG1オーディオPayload"},
	{&MEDIASUBTYPE_MPEG2_AUDIO,(char*)"MPEG-2オーディオ"},
	{&MEDIASUBTYPE_DVD_LPCM_AUDIO,(char*)"DVDオーディオ"},
	{&MEDIASUBTYPE_MPEG2_AUDIO,(char*)"MPEG-2オーディオ"},
	{&MEDIASUBTYPE_DRM_Audio,(char*)"WAVE_FORMAT_DRMに対応"},
	{&MEDIASUBTYPE_IEEE_FLOAT,(char*)"WAVE_FORMAT_IEEE_FLOATに対応"},
	{&MEDIASUBTYPE_DOLBY_AC3,(char*)"Dolby データ。"},
	{&MEDIASUBTYPE_DOLBY_AC3_SPDIF,(char*)"SPDIFのDolbyAC3"},
	{&MEDIASUBTYPE_RAW_SPORT,(char*)"MEDIASUBTYPE_DOLBY_AC3_SPDIFと同等"},
	{&MEDIASUBTYPE_SPDIF_TAG_241h,(char*)"MEDIASUBTYPE_DOLBY_AC3_SPDIFと同等"},
	{&MEDIASUBTYPE_RGB1,(char*)"RGB1(1bpp)"},
	{&MEDIASUBTYPE_RGB4,(char*)"RGB4(4bpp)"},
	{&MEDIASUBTYPE_RGB8,(char*)"RGB8(8bpp)"},
	{&MEDIASUBTYPE_RGB555,(char*)"RGB555(16bpp)"},
	{&MEDIASUBTYPE_RGB565,(char*)"RGB565(16bpp)"},
	{&MEDIASUBTYPE_RGB24,(char*)"RGB24(24bpp)"},
	{&MEDIASUBTYPE_RGB32,(char*)"RGB32(32bpp)"},
	{&MEDIASUBTYPE_ARGB32,(char*)"ARGB(32 bpp)"},
	{&MEDIASUBTYPE_AYUV,(char*)"AYUV(4:4:4)"},
	{&MEDIASUBTYPE_UYVY,(char*)"UYVY(4:2:2)"},
	{&MEDIASUBTYPE_Y411,(char*)"Y411(4:1:1)"},
	{&MEDIASUBTYPE_Y41P,(char*)"Y41P(4:1:1)"},
	{&MEDIASUBTYPE_Y211,(char*)"Y211"},
	{&MEDIASUBTYPE_YUY2,(char*)"YUY2(4:2:2)"},
	{&MEDIASUBTYPE_YVYU,(char*)"YVYU(4:2:2)"},
	{&MEDIASUBTYPE_YUYV,(char*)"YUYV"},
	{&MEDIASUBTYPE_CFCC,(char*)"MJPG('CFCC')"},
	{&MEDIASUBTYPE_CLJR,(char*)"CLJR('CLJR')"},
	{&MEDIASUBTYPE_CPLA,(char*)"CinepakUYVY('CPLA')"},
	{&MEDIASUBTYPE_CLPL,(char*)"CirrusLogicYUV('CLPL')"},
	{&MEDIASUBTYPE_IJPG,(char*)"IntergraphJPEG('IJPG')"},
	{&MEDIASUBTYPE_MDVF,(char*)"DVencoded('MDVF')"},
	{&MEDIASUBTYPE_MJPG,(char*)"MotionJPEG('MJPG')"},
	{&MEDIASUBTYPE_MPEG1Packet,(char*)"MPEG1ビデオ パケット"},
	{&MEDIASUBTYPE_MPEG1Payload,(char*)"MPEG1ビデオ ペイロード"},
	{&MEDIASUBTYPE_Overlay,(char*)"ハードウェアオーバーレイを使って配信されるビデオ"},
	{&MEDIASUBTYPE_Plum,(char*)"PlumMJPG('Plum')"},
	{&MEDIASUBTYPE_QTJpeg,(char*)"Quicktime JPEG 圧縮"},
	{&MEDIASUBTYPE_QTMovie,(char*)"Apple(c)QuickTime(c)圧縮"},
	{&MEDIASUBTYPE_QTRle,(char*)"QuicktimeRLE圧縮"},
	{&MEDIASUBTYPE_QTRpza,(char*)"QuicktimeRPZA 圧縮"},
	{&MEDIASUBTYPE_QTSmc,(char*)"QuicktimeSMC 圧縮"},
	{&MEDIASUBTYPE_TVMJ,(char*)"TrueVisionMJPG('TVMJ')"},
	{&MEDIASUBTYPE_VPVBI,(char*)"ビデオポート垂直同期間隔(VBI)"},
	{&MEDIASUBTYPE_VPVideo,(char*)"ビデオポートビデオ"},
	{&MEDIASUBTYPE_WAKE,(char*)"一部のカードによって生成されるMJPG('WAKE')"},
	// -----------------------------以下、不明
	{&MEDIASUBTYPE_Avi,(char*)"MEDIASUBTYPE_Avi"},
	{&GUID_NULL,NULL}};

void disp_subtype(const GUID *guid)
{
  if(disp_guid_list(guid,mediasubtype) == -2)
  {
	if(is_forcc_guid(guid)) print_forcc(guid);
	else { printf("Unknown."); disp_guid(guid); }
  }
}

guid_list mediamajortype[] = {
	{&MEDIATYPE_Stream,(char*)"MEDIATYPE_Stream"},
	{&GUID_NULL,NULL}};

void disp_majortype(const GUID *guid)
{
  if(disp_guid_list(guid,mediamajortype) == -2)
  {
	if(is_forcc_guid(guid)) print_forcc(guid);
	else { printf("Unknown."); disp_guid(guid); }
  }
}

void disp_mediatype(const AM_MEDIA_TYPE *amt)
{
  VIDEOINFOHEADER *vih;
  VIDEOINFOHEADER2 *vih2;
  BITMAPINFOHEADER *bih;
  WAVEFORMATEX *wf;

  disp_majortype(&(amt->majortype));
  printf(" ");
  disp_subtype(&(amt->subtype));
  printf(" ");
  disp_formattype(&(amt->formattype));
  vih = NULL;
  vih2 = NULL;
  bih = NULL;
  wf = NULL;
  if((amt->formattype == FORMAT_VideoInfo) || (amt->formattype == FORMAT_MPEGVideo))
	  {
		 vih = (VIDEOINFOHEADER *)amt->pbFormat; // MPEG1VIDEOINFO構造体の先頭がVIDEOINFOHEADER
		 bih = &(vih->bmiHeader);
      }
  else if((amt->formattype == FORMAT_VideoInfo2) || (amt->formattype == FORMAT_MPEG2Video))
     {
  		 vih2 = (VIDEOINFOHEADER2 *)amt->pbFormat; // MPEG2VIDEOINFO構造体の先頭がVIDEOINFOHEADER2
		 bih = &(vih2->bmiHeader);
     }
  else if(amt->formattype == FORMAT_WaveFormatEx)
	{
	  wf = (WAVEFORMATEX *)amt->pbFormat;
    }

  if(bih != NULL)
	{
	  printf(" (%d,%d)",bih->biWidth,bih->biHeight);
	}
  if(vih2 != NULL) vih = (VIDEOINFOHEADER *)vih2;
  if(vih != NULL)
    {
	  printf("[%.1ffps]",10000000.0/(vih->AvgTimePerFrame));
	  if(vih->dwBitRate != 0.0)
		{
		  printf("(%.2fMbps)",(double)(vih->dwBitRate)/1000000.0);
		}
	  //ほとんどの場合ErrorRateがゼロなので非表示にする
	  //printf("(Err%dbps)",vih->dwBitErrorRate);
    }
  if(vih2 != NULL)
    {
	  if(vih2->dwInterlaceFlags & AMINTERLACE_IsInterlaced) printf("(Interlaced)");
	  if(vih2->dwInterlaceFlags & AMINTERLACE_1FieldPerSample) printf("(1Field)");
	  if(vih2->dwInterlaceFlags & AMINTERLACE_Field1First) printf("(Field1First)");
	  switch (vih2->dwInterlaceFlags & AMINTERLACE_FieldPatternMask)
		{
		case AMINTERLACE_FieldPatField1Only: printf("(Field1Only)"); break;
		// ストリームにフィールド 2 はない。
		case AMINTERLACE_FieldPatField2Only: printf("(Field2Only)"); break;
		// ストリームにフィールド 1 はない。
		case AMINTERLACE_FieldPatBothRegular: printf("(BothRegular)"); break;
		// フィールド 1 ごとにフィールド 2 が必ず 1 つある。
		case AMINTERLACE_FieldPatBothIrregular: printf("(BothIrregular)"); break;
		// フィールド 1 とフィールド 2 が不規則に配置されている。
		}
	  switch (vih2->dwInterlaceFlags & AMINTERLACE_DisplayModeMask)
		{
		case AMINTERLACE_DisplayModeBobOnly: printf("(BobOnly)"); break;
		// ディスプレイ モードはボブのみ。
		case AMINTERLACE_DisplayModeWeaveOnly: printf("(WeaveOnly)"); break;
		// ディスプレイ モードはウィーブのみ。
		case AMINTERLACE_DisplayModeBobOrWeave: printf("(BobOrWeave)"); break;
		// ボブとウィーブの両方のモードがある。
		}
    }
  if(wf != NULL)
    {
		if(wf->wFormatTag == WAVE_FORMAT_PCM) printf(" WAVE_FORMAT_PCM");
		printf("(%dch)",wf->nChannels);
		printf("(%dSamplesPerSec)",wf->nSamplesPerSec);
		printf("(%dbits)",wf->wBitsPerSample);
	    printf("(%d)",wf->nBlockAlign); //ブロックサイズ
		printf("(%d)",wf->cbSize); //追加ヘッダサイズ
    }
  printf("\n");
}
