
#include "stdafx.h"
#include "DlgVideo.h"
#include "afxdialogex.h"

#include "hiknvr\libHikNvrAPI.h"
//#include "gui_common.h"


// CDlgVideo 对话框
int CALLBACK __LivePlayerCallBack(EASY_CALLBACK_TYPE_ENUM callbackType, int channelId, void *userPtr, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo);
IMPLEMENT_DYNAMIC(CDlgVideo, CDialogEx)

CDlgVideo::CDlgVideo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgVideo::IDD, pParent)
{
	m_WindowId	=	-1;
	m_ChannelId	=	-1;
	sourceMultiplex	=	0;
	onlyDecodeKeyFrame	=	0;
	bDrag		=	false;
#if HIK_NVR_ENABLE == 0x01
	hikNvrHandle	=	NULL;
	memset(&hikNvrChannel, 0x00, sizeof(HIK_NVR_CHANNEL_T));
#endif

	m_BrushBtn = ::CreateSolidBrush(DIALOG_BASE_BACKGROUND_COLOR);
	m_BrushEdt = ::CreateSolidBrush(RGB(0xef,0xef,0xef));
	m_BrushStatic = ::CreateSolidBrush(DIALOG_BASE_BACKGROUND_COLOR);

	InitialComponents();
}

CDlgVideo::~CDlgVideo()
{
	DeleteObject(m_BrushBtn);
	DeleteObject(m_BrushEdt);
	DeleteObject(m_BrushStatic);
}

void CDlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVideo, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CDlgVideo::OnBnClickedButtonPreview)
	ON_BN_CLICKED(IDC_CHECK_OSD, &CDlgVideo::OnBnClickedCheckOsd)
	ON_WM_HSCROLL()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_RECORDING_CMPLETE, OnRecordingComplete)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgVideo 消息处理程序
LRESULT CDlgVideo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE==message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}



BOOL CDlgVideo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CreateComponents();

	SetBackgroundColor(RGB(0x7c,0x7c,0x7c));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgVideo::DestroyWindow()
{
	DeleteComponents();

	return CDialogEx::DestroyWindow();
}



void CDlgVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	HWND hWnd = ::GetParent(GetSafeHwnd());
	if (NULL != hWnd)
	{
		::SendMessageW(hWnd, WM_WINDOW_MAXIMIZED, m_WindowId, 0);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CDlgVideo::OnLButtonDown(UINT nFlags, CPoint point)
{
	bDrag = true;

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDlgVideo::OnLButtonUp(UINT nFlags, CPoint point)
{
	bDrag = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDlgVideo::OnMouseMove(UINT nFlags, CPoint point)
{
	if (bDrag)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		if (! rcClient.IsRectEmpty())
		{
			int nX = (int)(((float)point.x / (float)rcClient.Width() * 100.0f));
			int nY = (int)(((float)point.y / (float)rcClient.Height() * 100.0f));

			TRACE("X: %d\tY: %d\n", nX, nY);
		}
	}


	CDialogEx::OnMouseMove(nFlags, point);
}

void	CDlgVideo::SetWindowId(int _windowId)	
{
	m_WindowId = _windowId;

	//if (NULL != pDlgRender)	pDlgRender->SetChannelId(m_WindowId);

	if (m_WindowId == 0)
	{
		//if (NULL != pEdtURL)		pEdtURL->SetWindowText(TEXT("rtsp://121.15.129.227"));
		//if (NULL != pEdtURL)		pEdtURL->SetWindowText(TEXT("rtsp://192.168.1.100"));
	}
}
void	CDlgVideo::SetURL(char *url, int scale, int osd, int tcp, int multiple, int cache, int showToolbar, int autoplay)
{
	wchar_t wszURL[128] = {0};
	MByteToWChar(url, wszURL, sizeof(wszURL)/sizeof(wszURL[0]));
	if (NULL != pEdtURL)		pEdtURL->SetWindowText(wszURL);

	if (NULL != pChkOSD)		pChkOSD->SetCheck(osd);
	if (NULL != pChkTCP)		pChkTCP->SetCheck(tcp);
	if (NULL != pSliderCache)	pSliderCache->SetPos(cache);

	shownToScale = scale;
	sourceMultiplex = multiple;

	if (showToolbar==0x00)
	{
		pEdtURL->ShowWindow(SW_HIDE);
		pChkOSD->ShowWindow(SW_HIDE);
		pChkTCP->ShowWindow(SW_HIDE);
		pSliderCache->ShowWindow(SW_HIDE);
		pBtnPreview->ShowWindow(SW_HIDE);
	}

	if (autoplay==0x01)
	{
		OnBnClickedButtonPreview();
	}


}

void	CDlgVideo::SetShownToScale(int _shownToScale)
{
	if (m_ChannelId > 0)
	{
		shownToScale = _shownToScale;
		libEasyPlayer_SetScaleDisplay(playerHandle, m_ChannelId, shownToScale, RGB(0x26,0x26,0x26));
	}
}
void	CDlgVideo::SetMultiplex(unsigned char multiplex)
{
	sourceMultiplex = multiplex;
}
void	CDlgVideo::SetDecodeType(unsigned char onlyDecodeKeyframe)	
{
	onlyDecodeKeyFrame = onlyDecodeKeyframe;
}

void	CDlgVideo::InitialComponents()
{
	pDlgRender	=	NULL;
	pEdtURL		=	NULL;
	pEdtUsername=	NULL;
	pEdtPassword=	NULL;
	pChkOSD		=	NULL;
	pChkTCP		=	NULL;
	pSliderCache=	NULL;
	pBtnPreview	=	NULL;
}

void	CDlgVideo::CreateComponents()
{
	if (NULL == pDlgRender)
	{
		pDlgRender = new CDlgRender();
		pDlgRender->Create(IDD_DIALOG_RENDER, this);
		pDlgRender->ShowWindow(SW_SHOW);
	}

	__CREATE_WINDOW(pEdtURL,		CEdit,		IDC_EDIT_RTSP_URL);
	__CREATE_WINDOW(pEdtUsername,	CEdit,		IDC_EDIT_USERNAME);
	__CREATE_WINDOW(pEdtPassword,	CEdit,		IDC_EDIT_PASSWORD);
	__CREATE_WINDOW(pChkOSD,		CButton,	IDC_CHECK_OSD);
	__CREATE_WINDOW(pChkTCP,		CButton,	IDC_CHECK_TCP);
	__CREATE_WINDOW(pSliderCache,	CSliderCtrl,IDC_SLIDER_CACHE);
	__CREATE_WINDOW(pBtnPreview,	CButton,	IDC_BUTTON_PREVIEW);

	//if (NULL != pEdtURL)		pEdtURL->SetWindowText(TEXT("rtsp://"));
	if (NULL != pEdtUsername)	pEdtUsername->SetWindowText(TEXT("admin"));
	if (NULL != pEdtPassword)	pEdtPassword->SetWindowText(TEXT("12345"));
	if (NULL != pSliderCache)	pSliderCache->SetRange(0, 20);
	if (NULL != pSliderCache)	pSliderCache->SetPos(3);
	if (NULL != pChkOSD)		pChkOSD->SetCheck(1);
	if (NULL != pChkTCP)		pChkTCP->SetCheck(1);

	if (NULL != pBtnPreview)		pBtnPreview->SetWindowText(TEXT("Play"));
}
void	CDlgVideo::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	bool bShowToolbar = true;
	if (NULL != pEdtURL && (!pEdtURL->IsWindowVisible()))	bShowToolbar = false;

	CRect	rcRender;
	rcRender.SetRect(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom-(bShowToolbar?20:0));
	__MOVE_WINDOW(pDlgRender, rcRender);
	if (NULL != pDlgRender)		pDlgRender->Invalidate();

	if (! bShowToolbar)	return;

	CRect	rcURL;
	rcURL.SetRect(rcClient.left, rcRender.bottom+2, rcClient.right-300, rcClient.bottom);
	__MOVE_WINDOW(pEdtURL, rcURL);
	if (NULL != pEdtURL)		pEdtURL->Invalidate();

	CRect	rcUsername;
	rcUsername.SetRect(rcURL.right+2, rcURL.top, rcURL.right+2+50, rcURL.bottom);
	__MOVE_WINDOW(pEdtUsername, rcUsername);
	if (NULL != pEdtUsername)		pEdtUsername->Invalidate();

	CRect	rcPassword;
	rcPassword.SetRect(rcUsername.right+2, rcUsername.top, rcUsername.right+2+rcUsername.Width(), rcUsername.bottom);
	__MOVE_WINDOW(pEdtPassword, rcPassword);
	if (NULL != pEdtPassword)		pEdtPassword->Invalidate();

	CRect	rcOSD;
	rcOSD.SetRect(rcPassword.right+2, rcPassword.top, rcPassword.right+2+40, rcPassword.bottom);
	__MOVE_WINDOW(pChkOSD, rcOSD);
	if (NULL != pChkOSD)		pChkOSD->Invalidate();
	CRect	rcTCP;
	rcTCP.SetRect(rcOSD.right+2, rcOSD.top, rcOSD.right+2+40, rcOSD.bottom);
	__MOVE_WINDOW(pChkTCP, rcTCP);
	CRect	rcCache;
	rcCache.SetRect(rcTCP.right+2, rcTCP.top, rcTCP.right+2+60, rcTCP.bottom);
	__MOVE_WINDOW(pSliderCache, rcCache);
	if (NULL != pSliderCache)		pSliderCache->Invalidate();

	CRect	rcPreview;
	rcPreview.SetRect(rcCache.right+2, rcURL.top, rcClient.right-3, rcURL.bottom);
	__MOVE_WINDOW(pBtnPreview, rcPreview);
	if (NULL != pBtnPreview)		pBtnPreview->Invalidate();
}
void	CDlgVideo::DeleteComponents()
{
	if (m_ChannelId > 0)
	{
		libEasyPlayer_CloseStream(playerHandle, m_ChannelId);
		m_ChannelId = -1;
	}
#if HIK_NVR_ENABLE == 0x01
	if (NULL != hikNvrHandle)
	{
		libHIKNVR_Logout(hikNvrHandle);
		libHIKNVR_Deinit(&hikNvrHandle);
	}
#endif
	__DELETE_WINDOW(pDlgRender);
}

void CDlgVideo::OnBnClickedButtonPreview()
{
	if (NULL == playerHandle)
	{
		libEasyPlayer_Create(&playerHandle, 128);
	}
#if HIK_NVR_ENABLE == 0x01
	//PlayStreamFile();
	OpenHikNvrRealStream();
	return;
#endif

	if (m_ChannelId > 0)
	{
		//Player_CloseStream(m_ChannelId);
		libEasyPlayer_StopPlayStream(playerHandle, m_ChannelId);
		libEasyPlayer_CloseStream(playerHandle, m_ChannelId);
		m_ChannelId = -1;

		if (NULL != pDlgRender)
		{
			pDlgRender->SetChannelId(m_ChannelId);
			pDlgRender->ResetChannel();
		}

		if (NULL != pDlgRender)			pDlgRender->Invalidate();
		if (NULL != pBtnPreview)		pBtnPreview->SetWindowText(TEXT("Play"));
	}
	else
	{
		wchar_t wszURL[256] = {0};
		if (NULL != pEdtURL)	pEdtURL->GetWindowTextW(wszURL, sizeof(wszURL));
		if (wcslen(wszURL) < 16)		return;

		wchar_t wszUsername[32] = {0};
		wchar_t wszPassword[32] = {0};
		if (NULL != pEdtUsername)	pEdtUsername->GetWindowText(wszUsername, sizeof(wszUsername));
		if (NULL != pEdtPassword)	pEdtPassword->GetWindowText(wszPassword, sizeof(wszPassword));

		char szURL[256] = {0};
		WCharToMByte(wszURL, szURL, sizeof(szURL)/sizeof(szURL[0]));
		char szUsername[32] = {0};
		char szPassword[32] = {0};
		WCharToMByte(wszUsername, szUsername, sizeof(szUsername)/sizeof(szUsername[0]));
		WCharToMByte(wszPassword, szPassword, sizeof(szPassword)/sizeof(szPassword[0]));

		int	rtpOverTcp = 0x00;
		if (NULL != pChkTCP)		rtpOverTcp = pChkTCP->GetCheck();

		HWND hWnd = NULL;
		if (NULL != pDlgRender)	hWnd = pDlgRender->GetSafeHwnd();
#ifdef _DEBUG1
		m_ChannelId = Player_OpenStream(szURL, hWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, szUsername, szPassword);
#else


		LIVE_CHANNEL_SOURCE_TYPE_ENUM		sourceType = LIVE_CHANNEL_SOURCE_TYPE_RTSP;
		if (0 == strncmp(szURL, "rtsp", 4))	sourceType = LIVE_CHANNEL_SOURCE_TYPE_RTSP;
		else if (0 == strncmp(szURL, "rtmp", 4))	sourceType = LIVE_CHANNEL_SOURCE_TYPE_RTMP;
		else if (0 == strncmp(szURL, "http", 4))	sourceType = LIVE_CHANNEL_SOURCE_TYPE_HLS;
		else if (0 == strncmp(szURL, "file", 4))	sourceType = LIVE_CHANNEL_SOURCE_TYPE_FILE;

		int queueSize = 1024 * 1024 * 2;		//2MB
		if (sourceType == LIVE_CHANNEL_SOURCE_TYPE_HLS)		queueSize = 1024 * 1024 * 5;		//5MB

#ifdef _DEBUG1
		static int iiDebug = 0;
		iiDebug = (iiDebug==0x00?0x01:0x00);
		if (iiDebug == 0x00)
			m_ChannelId = Player_OpenStream("rtsp://192.168.7.108:8557", hWnd, (RENDER_FORMAT)RenderFormat, 0x01, szUsername, szPassword);
		else
			m_ChannelId = Player_OpenStream("rtsp://192.168.7.108:8556", hWnd, (RENDER_FORMAT)RenderFormat, 0x01, szUsername, szPassword);
#else
		//m_ChannelId = Player_OpenStream(szURL, hWnd, (RENDER_FORMAT)RenderFormat, 0x01, szUsername, szPassword);
		m_ChannelId = libEasyPlayer_OpenStream(playerHandle, sourceType, szURL, 
												rtpOverTcp, szUsername, szPassword, 
												MEDIA_TYPE_VIDEO|MEDIA_TYPE_AUDIO|MEDIA_TYPE_EVENT,
											__LivePlayerCallBack, this, 0x01, 0x01, queueSize, sourceMultiplex);
		
#endif
#endif	

		if (m_ChannelId > 0)
		{
			//libEasyPlayer_StartPlayStream(playerHandle, m_ChannelId, hWnd, RenderFormat);
			libEasyPlayer_StartPlayStream(playerHandle, m_ChannelId, hWnd, RenderFormat);

			libEasyPlayer_SetDecodeType(playerHandle, m_ChannelId, onlyDecodeKeyFrame);

			int iPos = pSliderCache->GetPos();
			libEasyPlayer_SetPlayFrameCache(playerHandle, m_ChannelId, iPos);		//设置缓存
			//libEasyPlayer_StartPlaySound(playerHandle, m_ChannelId);				//播放声音
			if (NULL != pDlgRender)	pDlgRender->SetChannelId(m_ChannelId);

			libEasyPlayer_SetScaleDisplay(playerHandle, m_ChannelId, shownToScale, RGB(0x26,0x26,0x26));

			OnBnClickedCheckOsd();

			if (NULL != pBtnPreview)		pBtnPreview->SetWindowText(TEXT("Stop"));
		}
	}
}



void CDlgVideo::OnBnClickedCheckOsd()
{
	int nShow = 0x00;

	if (NULL != pChkOSD)		nShow = pChkOSD->GetCheck();

	if (m_ChannelId > 0)
	{
		libEasyPlayer_ShowStatisticalInfo(playerHandle, m_ChannelId, nShow);
	}
}


void CDlgVideo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( NULL != pScrollBar && NULL != pSliderCache &&
		pSliderCache->GetDlgCtrlID() == pScrollBar->GetDlgCtrlID())
	{
		int iPos = pSliderCache->GetPos();
		
		if (m_ChannelId > 0)
		{
			libEasyPlayer_SetPlayFrameCache(playerHandle, m_ChannelId, iPos);
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CDlgVideo::OnRButtonUp(UINT nFlags, CPoint point)
{


	CDialogEx::OnRButtonUp(nFlags, point);
}

void CDlgVideo::OnMouseWheel(short zDelta, CPoint pt)
{
	if (NULL != pDlgRender)	pDlgRender->OnMouseWheel(zDelta, pt);
}




LRESULT CDlgVideo::OnRecordingComplete(WPARAM wParam, LPARAM lParam)
{
	if (NULL != pDlgRender)		pDlgRender->SetRecordingFlag(0);

	return 0;
}

void	CDlgVideo::onVideoData(int channelId, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo)
{
	if (NULL != pDlgRender)		pDlgRender->onVideoData(channelId, mediaType, pbuf, frameInfo);
}


int CALLBACK __LivePlayerCallBack(EASY_CALLBACK_TYPE_ENUM callbackType, int channelId, void *userPtr, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo)
{
	CDlgVideo	*pLiveVideo = (CDlgVideo *)userPtr;

	if (callbackType == EASY_TYPE_CONNECTING)
	{
		OutputDebugString(TEXT("EASY_TYPE_CONNECTING...\n"));
	}
	else if (callbackType == EASY_TYPE_CONNECTED)
	{
		OutputDebugString(TEXT("EASY_TYPE_CONNECTED.\n"));
	}
	else if (callbackType == EASY_TYPE_DISCONNECT)
	{
		OutputDebugString(TEXT("EASY_TYPE_DISCONNECT.\n"));
	}
	else if (callbackType == EASY_TYPE_RECONNECT)
	{
		OutputDebugString(TEXT("EASY_TYPE_RECONNECT.\n"));
	}
	else if (callbackType == EASY_TYPE_FILE_DURATION)
	{
		wchar_t wszLog[128] = {0};
		wsprintf(wszLog, TEXT("总时长: %u\n"), frameInfo->timestamp_sec);
		OutputDebugString(wszLog);
	}
	else if (callbackType == EASY_TYPE_CODEC_DATA)
	{
		if (mediaType == MEDIA_TYPE_SDP)
		{
		}
		else if (mediaType == MEDIA_TYPE_CODEC_INFO)
		{

		}
		else if (mediaType == MEDIA_TYPE_VIDEO)
		{
			wchar_t wszLog[128] = {0};
			wsprintf(wszLog, TEXT("播放时间: %u\n"), frameInfo->timestamp_sec);
			//OutputDebugString(wszLog);

			pLiveVideo->onVideoData(channelId, mediaType, pbuf, frameInfo);
		}
		//else if (mediaType == 


		//_TRACE(TRACE_LOG_WARNING, "[ch%d] type[%d] channelId[%d] mediaType[%d]\n", pLiveVideo->channelId, callbackType, channelId, mediaType);

		

#ifdef _DEBUG1
		if (mediaType == 0x01)
		{
			static int iH264FrameNo = 0;
			char sztmp[128] = {0};
			sprintf(sztmp, "C:\\test\\h264\\%d.txt", frameInfo->length);//iH264FrameNo++, frameInfo->length);
			FILE *f = fopen(sztmp, "wb");
			if (NULL != f)
			{
				fwrite(pbuf, 1, frameInfo->length, f);
				fclose(f);
			}
		}
#endif
	}
	else if (callbackType == EASY_TYPE_DECODE_DATA)
	{
		//_TRACE(TRACE_LOG_DEBUG, "解码数据[ch%d]type[%d] channelId[%d] mediaType[%d] [%d x %d] framesize[%d]\n",  pLiveVideo->channelId,
			//	callbackType, channelId, mediaType, frameInfo->width, frameInfo->height, frameInfo->length);
	}
	else if (callbackType == EASY_TYPE_SNAPSHOT)
	{
		//_TRACE(TRACE_LOG_DEBUG, "抓拍图片[ch%d] %s. filename:%s\n",  channelId, mediaType==1?"成功":"失败", pbuf);
		if (mediaType == MEDIA_TYPE_VIDEO)		OutputDebugString(TEXT("抓拍图片成功\n"));
		else if (mediaType == MEDIA_TYPE_EVENT)		OutputDebugString(TEXT("抓拍图片失败\n"));
	}
	else if (callbackType == EASY_TYPE_RECORDING)
	{
		if (mediaType == MEDIA_TYPE_VIDEO)		OutputDebugString(TEXT("录像成功\n"));
		else if (mediaType == MEDIA_TYPE_EVENT)		OutputDebugString(TEXT("录像失败\n"));

		pLiveVideo->PostMessageW(WM_RECORDING_CMPLETE, MEDIA_TYPE_VIDEO == mediaType ? 0 : -1, 0);
	}
	else if (callbackType == EASY_TYPE_INSTANT_REPLAY_RECORDING)
	{
		if (mediaType == MEDIA_TYPE_VIDEO)		OutputDebugString(TEXT("即时回放录像成功\n"));
		else if (mediaType == MEDIA_TYPE_EVENT)		OutputDebugString(TEXT("即时回放录像失败\n"));
	}
	else if (callbackType == EASY_TYPE_PLAYBACK_TIME)		//
	{
		char szTime[64] = {0};
		time_t tt = frameInfo->timestamp_sec;
		struct tm *_timetmp = NULL;
		_timetmp = localtime(&tt);
		if (NULL != _timetmp)	strftime(szTime, 32, "%Y-%m-%d %H:%M:%S ", _timetmp);

		wchar_t wszLog[128] = {0};
		char szLog[128] = {0};
		sprintf(szLog, "[ch%d]当前回放时间: %s\n", channelId, szTime);

		MByteToWChar(szLog, wszLog, sizeof(wszLog)/sizeof(wszLog[0]));
		OutputDebugString(wszLog);
	}
	else if (callbackType == EASY_TYPE_METADATA)
	{

	}

	return 0;
}





int	makeH264Header(int *keyframe, int *out_width, int *out_height, unsigned char *buf, int bufsize)
{
	int		width = 0, height = 0, fps = 0;

	// For 264 , we present NAL unit type
	unsigned char naltype = (buf[0] & 0x1F);
	switch (naltype) {
	case 7:	//sps
	case 8:	// pps
	case 6:	//sei
	case 5:	//idr
		if (NULL!=keyframe)	*keyframe = 0x01;
		break;
	case 1: // slice
	case 9:	// unknown ???
		break;
	default:
		break;
	}

	/*
		parse SPS for extract video width and height....
	*/

	if(naltype == 7) 
	{
		h264_sps_t sps_t;
		memset(&sps_t, 0, sizeof(h264_sps_t));
		int ret = h264_sps_read(  buf, bufsize, &sps_t );
		if ( (ret >=0 || ret==-1000) && (sps_t.i_mb_width > 1 && sps_t.i_mb_height > 1) )
		{
			int iWidth = sps_t.i_mb_width*16;
			int iHeight = sps_t.i_mb_height*16;

			width = iWidth;
			height = iHeight;
			if (sps_t.vui.i_time_scale>0 && sps_t.vui.i_num_units_in_tick>0)
			{
				fps =  sps_t.vui.i_time_scale / sps_t.vui.i_num_units_in_tick / 2;
			}
		}
	}

	if (NULL != out_width)		*out_width	= width;
	if (NULL != out_height)		*out_height = height;

	return naltype;
}

void CALLBACK __HIKNVR_CallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void *pUser)
{

	printf("dwDataType: %d   dwBufSize: %d\n", dwDataType, dwBufSize);

	HIK_NVR_CHANNEL_T	*pNvrChannel = (HIK_NVR_CHANNEL_T *)pUser;

	if (NULL == pNvrChannel->psDemuxHandle)
	{
		PSDemux_Init(&pNvrChannel->psDemuxHandle, 1024*1024, 1024*1024);
	}
	if (NULL != pNvrChannel->psDemuxHandle)
	{
		int mediaType = 0;
		int framesize = 0;
		char *framedata = NULL;
		if (0 == PSDemux_demuxPacket(pNvrChannel->psDemuxHandle, (char*)pBuffer, (int)dwBufSize, &mediaType, &framesize, &framedata))
		{
			if (mediaType != 0x01)		return;

			int keyframe=0,width=0, height=0;
			makeH264Header(&keyframe, &width, &height, (unsigned char*)(framedata+4), framesize);

			if (pNvrChannel->width<1 && width>0)
			{
				pNvrChannel->width = width;
				pNvrChannel->height = height;
			}

			if (pNvrChannel->width>0)
			{
#ifdef _DEBUG1
				static int iFrameNo = 0;
				static FILE *fH264 = NULL;
				if (NULL == fH264)		fH264 = fopen("ps.h264", "wb");
				if (NULL != fH264)
				{
					fwrite(framedata, 1, framesize, fH264);
					fflush(fH264);
				}

				wchar_t wszLog[128] = {0};
				wsprintf(wszLog, TEXT("H264 Frame No[%d] size[%d]\n"), ++iFrameNo, framesize);
				TRACE(wszLog);
#endif

				LIVE_FRAME_INFO		frameInfo;
				memset(&frameInfo, 0x00, sizeof(LIVE_FRAME_INFO));
				frameInfo.codec = 0x1C;
				frameInfo.width = pNvrChannel->width;
				frameInfo.height = pNvrChannel->height;
				frameInfo.length = framesize;
				frameInfo.type  = keyframe;

				libEasyPlayer_PutFrameData(playerHandle, pNvrChannel->channelId, MEDIA_TYPE_VIDEO, &frameInfo, (unsigned char *)framedata);
				//Sleep(40);
#if 0
				static int iFileNo = 0;
				char sztmp[128] = {0};
				sprintf(sztmp, "C:\\test\\metadata2\\%d.txt", iFileNo++);
				if (iFileNo>270)	iFileNo = 0;
				FILE *f = fopen(sztmp, "rb");
				if (NULL != f)
				{
					fseek(f, 0, SEEK_END);
					long lSize = ftell(f);
					fseek(f, 0, SEEK_SET);

					char *pbuf = new char[lSize+1];
					fread(pbuf, 1, lSize, f);
					fclose(f);

					LIVE_FRAME_INFO		frameInfo;
					memset(&frameInfo, 0x00, sizeof(LIVE_FRAME_INFO));
					frameInfo.codec = 0x6D766361;
					frameInfo.width = pNvrChannel->width;
					frameInfo.height = pNvrChannel->height;
					frameInfo.length = lSize;
					libEasyPlayer_PutFrameData(playerHandle, pNvrChannel->channelId, MEDIA_TYPE_EVENT, &frameInfo, (unsigned char *)(pbuf));

					delete []pbuf;
				}
#endif
			}
			//Sleep(40);
		}
	}
	return;
}

#if HIK_NVR_ENABLE == 0x01
DWORD WINAPI __ReadStreamFileThread(LPVOID lpParam)
{
	HIK_NVR_CHANNEL_T	*pNvrChannel = (HIK_NVR_CHANNEL_T *)lpParam;

	FILE *fPS = fopen("ps_size.mpg", "rb");
	if (NULL == fPS)		return 0;

	int bufsize = 1024*1024;
	char *pbuf = new char[bufsize];
	while (! feof(fPS))
	{
		int packet_size = 0;
		int readBytes = fread((void *)&packet_size, 1, sizeof(int), fPS);
		if (readBytes < 1)		break;

		readBytes = fread(pbuf, 1, packet_size, fPS);
		if (readBytes != packet_size)		break;

		__HIKNVR_CallBack(0, 0, (BYTE*)pbuf,packet_size,lpParam);
	}
	fclose(fPS);
	return 0;

}
#endif
void	CDlgVideo::PlayStreamFile()
{
#if HIK_NVR_ENABLE == 0x01
	HWND hWnd = NULL;
	if (NULL != pDlgRender)	hWnd = pDlgRender->GetSafeHwnd();

	hikNvrChannel.channelId = libEasyPlayer_OpenStream(playerHandle, LIVE_CHANNEL_TYPE_ENCODE_DATA, "", 0, "", "", NULL, NULL, 0, 0, 1024*1024*2, 0);
	if (hikNvrChannel.channelId > 0)
	{
		libEasyPlayer_StartPlayStream(playerHandle, hikNvrChannel.channelId, hWnd, RenderFormat, 0);
		libEasyPlayer_SetPlayFrameCache(playerHandle, hikNvrChannel.channelId, 3);
		libEasyPlayer_ShowStatisticalInfo(playerHandle, hikNvrChannel.channelId, 1);
	}
	CreateThread(NULL, 0, __ReadStreamFileThread, &hikNvrChannel, 0, NULL);
#endif
}

void	CDlgVideo::OpenHikNvrRealStream()
{
#if HIK_NVR_ENABLE == 0x01
	int iChannelID = 0;
	if (NULL == hikNvrHandle)
	{
		libHIKNVR_Init(&hikNvrHandle);
		libHIKNVR_Login(hikNvrHandle, "192.168.203.64", 8000, "admin", "admin12345");
		
		libHIKNVR_GetChannelIdByCameraIP(hikNvrHandle, "192.168.200.151", &iChannelID);
	}

	if (iChannelID < 1)
	{
		libHIKNVR_Logout(hikNvrHandle);
		libHIKNVR_Deinit(&hikNvrHandle);
		return;
	}

		if (NULL != hikNvrChannel.psDemuxHandle)
		{
			PSDemux_Deinit(&hikNvrChannel.psDemuxHandle);
			hikNvrChannel.psDemuxHandle = NULL;
		}

		HWND hWnd = NULL;
		if (NULL != pDlgRender)	hWnd = pDlgRender->GetSafeHwnd();

		hikNvrChannel.channelId = libEasyPlayer_OpenStream(playerHandle, LIVE_CHANNEL_TYPE_ENCODE_DATA, "", 0, "", "", NULL, NULL, 0, 0, 1024*1024*2, 0);
		if (hikNvrChannel.channelId > 0)
		{
			libEasyPlayer_StartPlayStream(playerHandle, hikNvrChannel.channelId, hWnd, RenderFormat, 0);
			libEasyPlayer_SetPlayFrameCache(playerHandle, hikNvrChannel.channelId, 3);
		}


		hikNvrChannel.playHandle = libHIKNVR_StartRealStream(hikNvrHandle, iChannelID, 0, __HIKNVR_CallBack, &hikNvrChannel);
#endif
}

void	CDlgVideo::OpenHIKNVR_Playback()
{
#if HIK_NVR_ENABLE == 0x01
	int iChannelID = 0;
	if (NULL == hikNvrHandle)
	{
		libHIKNVR_Init(&hikNvrHandle);
		libHIKNVR_Login(hikNvrHandle, "192.168.203.64", 8000, "admin", "admin12345");
		
		libHIKNVR_GetChannelIdByCameraIP(hikNvrHandle, "192.168.200.151", &iChannelID);
	}

	if (iChannelID < 1)
	{
		libHIKNVR_Logout(hikNvrHandle);
		libHIKNVR_Deinit(&hikNvrHandle);
		return;
	}

		if (NULL != hikNvrChannel.psDemuxHandle)
		{
			PSDemux_Deinit(&hikNvrChannel.psDemuxHandle);
			hikNvrChannel.psDemuxHandle = NULL;
		}

		HWND hWnd = NULL;
		if (NULL != pDlgRender)	hWnd = pDlgRender->GetSafeHwnd();

		hikNvrChannel.channelId = libEasyPlayer_OpenStream(playerHandle, LIVE_CHANNEL_TYPE_ENCODE_DATA, "", 0, "", "", NULL, NULL, 0, 0, 1024*1024*2, 0);
		if (hikNvrChannel.channelId > 0)
		{
			libEasyPlayer_StartPlayStream(playerHandle, hikNvrChannel.channelId, hWnd, RenderFormat, 0);
			libEasyPlayer_SetPlayFrameCache(playerHandle, hikNvrChannel.channelId, 3);
		}

		DEVICE_TIME_T	startTime;
		DEVICE_TIME_T	endTime;
		memset(&startTime, 0x00, sizeof(DEVICE_TIME_T));
		memset(&endTime, 0x00, sizeof(DEVICE_TIME_T));
		startTime.ulYear = 2017;
		startTime.ulMonth = 2;
		startTime.ulDay = 22;
		startTime.ulHour = 13;
		startTime.ulMinute = 01;
		startTime.ulSecond = 43;
		memcpy(&endTime, &startTime, sizeof(DEVICE_TIME_T));
		endTime.ulMinute += 5;

		hikNvrChannel.playHandle = libHIKNVR_StartPlaybackStream(hikNvrHandle, iChannelID,   &startTime, &endTime, __HIKNVR_CallBack, &hikNvrChannel);
#endif
}



HBRUSH CDlgVideo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	switch (nCtlColor)
	{
	case CTLCOLOR_BTN:	//按钮
		{
			return m_BrushBtn;
		}
		break;
		case CTLCOLOR_EDIT:	//编辑框
		{
			//pDC->SelectObject(&fontText);
			pDC->SetTextColor(DIALOG_BASE_TEXT_COLOR);
			return m_BrushEdt;
		}
		break;
	case CTLCOLOR_STATIC:	
		{
			pDC->SetBkColor(DIALOG_BASE_BACKGROUND_COLOR);
			pDC->SetTextColor(DIALOG_BASE_TEXT_COLOR);
			return m_BrushStatic;
		}
		break;
	default:
		break;
	}


	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
