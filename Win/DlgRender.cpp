#include "stdafx.h"
#include "EasyPlayer.h"
#include "DlgRender.h"
#include "afxdialogex.h"

#include "libEasyPlayer/libEasyPlayerAPI.h"


// CDlgRender 对话框



IMPLEMENT_DYNAMIC(CDlgRender, CDialogEx)

CDlgRender::CDlgRender(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRender::IDD, pParent)
{
	memset(&channelStatus, 0x00, sizeof(CHANNELSTATUS));
	hMenu		=	NULL;
	mDrag		=	false;
	scaleMultiple=	1;
	memset(&pt_start, 0x00, sizeof(POINT));
	memset(&pt_start_org, 0x00, sizeof(POINT));

	InitZoom();

	channelStatus.faceMatchThreshold = 70;

	mChannelId	=	0;
}

CDlgRender::~CDlgRender()
{
	ClosePopupMenu();
	//libAudioTalk_CloseAudioCaptureDevice();
}

void CDlgRender::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRender, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgRender 消息处理程序

BOOL CDlgRender::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(RGB(0x92,0x92,0x92));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgRender::ClosePopupMenu()
{
	if (NULL != hMenu)
	{
		DestroyMenu(hMenu);
		hMenu = NULL;
	}
}


void	CDlgRender::ResetChannel()
{
	memset(&channelStatus, 0x00, sizeof(CHANNELSTATUS));
}

void CDlgRender::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, 0, 0);

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

#define POP_MENU_SEPARATOR	10000

#define	POP_MENU_AUDIO	10009


#define	POP_MENU_RECORDING_MPG	10010
#define POP_MENU_DECODE_KEYFRAME_ONLY		10011
#define POP_MENU_SNAPSHOT_BMP	10012
#define POP_MENU_SNAPSHOT_JPG	10013
#define POP_MENU_STREAM_PAUSE	10014
#define POP_MENU_STREAM_RESUME	10015

#define POP_MENU_STREAM_INSTANT_REPLAY	10016		//即时回放
#define POP_MENU_STREAM_PREVIOUS_FRAME	10017
#define POP_MENU_STREAM_NEXT_FRAME	10018
#define POP_MENU_STREAM_INSTANT_REPLAY_RECORDING		10019		//即时回放录像

#define POP_MENU_STREAM_FAST_X2			10020		//2倍速播放
#define POP_MENU_STREAM_FAST_X4			10021		//4倍速播放
#define POP_MENU_STREAM_FAST_X8			10022		//8倍速播放
#define POP_MENU_STREAM_NORMAL_X1		10023		//1倍速播放
#define POP_MENU_STREAM_SLOW_X2			10024		// 2/1倍速播放
#define POP_MENU_STREAM_SLOW_X4			10025		// 4/1倍速播放
#define POP_MENU_STREAM_SLOW_X8			10026		// 8/1倍速播放
#define	POP_MENU_STREAM_REWIND_X2		10027		//2倍速倒放
#define	POP_MENU_STREAM_REWIND_X4		10028		//4倍速倒放
#define	POP_MENU_STREAM_REWIND_X8		10029		//8倍速倒放


//#define POP_MENU_VA_ENABLE				10030		//启用视频分析设置
#define POP_MENU_VA_WARNING_AREA		10031		//警戒区
#define POP_MENU_VA_WARNING_LINE		10032		//警戒线
#define POP_MENU_VA_WARNING_AREA_DIRECTION	10033	//带方向警戒区
#define POP_MENU_VA_LANE				10034		//车道
#define POP_MENU_VA_FACE_MATCH			10035		// 人脸对比


#define POP_MENU_AUDIO_ALARM_BEEP		10050		//声音报警 Beep
#define POP_MENU_AUDIO_ALARM_WAV		10051		//声音报警 wav文件
#define POP_MENU_AUDIO_ALARM_TTS		10052		//声音报警 tts
#define	POP_MENU_AUDIO_ALARM_CLEAR		10053		//清空队列

#define POP_MENU_ELECTORIC_ZOOM			10060
#define POP_MENU_TALK					10061		//对讲
#define POP_MENU_VIDEO_FLIP				10062		//视频翻转
#define	POP_MENU_SET_RENDER_RECT		10063
#define POP_MENU_SET_OVERLAY_TEXT		10100		//设置叠加文字
#define POP_MENU_CLEAR_OVERLAY_TEXT		10101		//清除叠加文字

#define	POP_MENU_RECORDING_ES	10110			//ES流
#define	POP_MENU_RECORDING_DEBUG	10111	//DEBUG文件, 用于rtspserver转发测试

#define POP_MENU_U10_ZOOM				10200		//U10缩放显示

#define POP_MENU_FACE_MATCH_THRESHOLD	10300		//人脸比对阀值

void CDlgRender::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClosePopupMenu();

	InitZoom();

	//Player_SetRenderRect(mChannelId, NULL);
	scaleMultiple = 1;

	{
#ifndef _DEBUG
		if (mChannelId > 0)
#endif
		{
			hMenu = CreatePopupMenu();
			if (NULL != hMenu)
			{
				//播放声音
				channelStatus.audio = (libEasyPlayer_SoundPlaying(playerHandle, mChannelId) == 0x00?0x01:0x00);
				AppendMenu(hMenu, MF_STRING|(channelStatus.audio==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_AUDIO, TEXT("播放声音"));
#if ENABLE_IVA_MODULE == 0x00
				//AppendMenu(hMenu, MF_STRING|(gTalkChannelId==mChannelId?MF_CHECKED:MF_UNCHECKED), POP_MENU_TALK, TEXT("Talk"));
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));

				//录像
				//AppendMenu(hMenu, MF_STRING|(channelStatus.recordingMpg==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_RECORDING_MPG, TEXT("录象(mpg)"));
				AppendMenu(hMenu, MF_STRING|(channelStatus.recordingES==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_RECORDING_ES, TEXT("录像(ES)"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.recordingDebug==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_RECORDING_DEBUG, TEXT("Recording(debug)"));
#endif
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
#if ENABLE_IVA_MODULE == 0x00
				//视频翻转
				AppendMenu(hMenu, MF_STRING|(channelStatus.flip==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_VIDEO_FLIP, TEXT("视频翻转"));


				//AppendMenu(hMenu, MF_STRING|(channelStatus.setRenderRect==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_SET_RENDER_RECT, TEXT("Set Render Rect"));
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
#endif

				//抓图
				AppendMenu(hMenu, MF_STRING, POP_MENU_SNAPSHOT_BMP, TEXT("抓图(BMP)"));
#if ENABLE_IVA_MODULE == 0x00
				//AppendMenu(hMenu, MF_STRING, POP_MENU_SNAPSHOT_JPG, TEXT("Snapshot(JPG)"));

				//电子放大
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_STRING|(channelStatus.digitalZoom==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_ELECTORIC_ZOOM, TEXT("电子放大"));

				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_STRING|(channelStatus.decodeKeyframeOnly==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_DECODE_KEYFRAME_ONLY, TEXT("仅解码关键帧"));
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));

				//AppendMenu(hMenu, MF_STRING, POP_MENU_STREAM_PAUSE, TEXT("Pause"));
				//AppendMenu(hMenu, MF_STRING, POP_MENU_STREAM_RESUME, TEXT("Resume"));

				AppendMenu(hMenu, MF_STRING|(channelStatus.instantReplay==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_INSTANT_REPLAY, TEXT("即时回放"));	//即时回放
		

				//AppendMenu(hMenu, MF_STRING, POP_MENU_STREAM_PREVIOUS_FRAME, TEXT("Previous Frame"));
				//AppendMenu(hMenu, MF_STRING, POP_MENU_STREAM_NEXT_FRAME, TEXT("Next Frame"));

				if (channelStatus.instantReplay == 0x01)
				{
					//AppendMenu(hMenu, MF_STRING|(channelStatus.instantReplaySave==0x01?MF_CHECKED:MF_UNCHECKED), 
					//	POP_MENU_STREAM_INSTANT_REPLAY_RECORDING, TEXT("即时回放保存"));	//即时回放保存
				}

				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));

				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_FAST_X2?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_FAST_X2, TEXT("x2"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_FAST_X4?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_FAST_X4, TEXT("x4"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_FAST_X8?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_FAST_X8, TEXT("x8"));

				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_NORMAL?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_NORMAL_X1, TEXT("x1"));
		
				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));

				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_SLOW_X2?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_SLOW_X2, TEXT("1/2"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_SLOW_X4?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_SLOW_X4, TEXT("1/4"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_SLOW_X8?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_SLOW_X8, TEXT("1/8"));

				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_REWIND_X2?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_REWIND_X2, TEXT("-x2"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_REWIND_X4?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_REWIND_X4, TEXT("-x4"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.playSpeed==PLAY_SPEED_REWIND_X8?MF_CHECKED:MF_UNCHECKED), POP_MENU_STREAM_REWIND_X8, TEXT("-x8"));

				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				//AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				//AppendMenu(hMenu, MF_STRING|(channelStatus.videoAnalysis==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_VA_ENABLE, TEXT("Video Analysis Settings"));
				//if (channelStatus.videoAnalysis == 0x01)
				//{
					//RENDER_MODE_ENUM	renderMode = libVA_GetRenderMode(playerHandle,mChannelId);
					//AppendMenu(hMenu, MF_STRING|(renderMode==RENDER_MODE_ZONE?MF_CHECKED:MF_UNCHECKED), POP_MENU_VA_WARNING_AREA, TEXT("Warning Area"));
					//AppendMenu(hMenu, MF_STRING|(renderMode==RENDER_MODE_LINE?MF_CHECKED:MF_UNCHECKED), 	POP_MENU_VA_WARNING_LINE, TEXT("Warning Line"));
					//AppendMenu(hMenu, MF_STRING|(renderMode==RENDER_MODE_DIRECT_ZONE?MF_CHECKED:MF_UNCHECKED), POP_MENU_VA_WARNING_AREA_DIRECTION, TEXT("Direction Area"));
					//AppendMenu(hMenu, MF_STRING|(renderMode==RENDER_MODE_LANE?MF_CHECKED:MF_UNCHECKED), 	POP_MENU_VA_LANE, TEXT("Lane"));
#endif

#if ENABLE_IVA_MODULE == 0x01
					AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
					AppendMenu(hMenu, MF_STRING, POP_MENU_VA_FACE_MATCH, TEXT("Face Match"));
					AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
					int idx = 0;
					for (int i = 10; i <= 100; i += 10)
					{
						wchar_t wszName[32] = { 0 };
						wsprintf(wszName, TEXT("%d%%"), i);

						AppendMenu(hMenu, MF_STRING | (channelStatus.faceMatchThreshold == i ? MF_CHECKED : MF_UNCHECKED), POP_MENU_FACE_MATCH_THRESHOLD + (idx++), wszName);
					}
#endif

				//}
//#if ENABLE_IVA_MODULE == 0x00
#if 0
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));

				AppendMenu(hMenu, MF_STRING, POP_MENU_AUDIO_ALARM_BEEP, TEXT("Alarm(Beep)"));
				AppendMenu(hMenu, MF_STRING, POP_MENU_AUDIO_ALARM_WAV, TEXT("Alarm(wav)"));
				AppendMenu(hMenu, MF_STRING, POP_MENU_AUDIO_ALARM_TTS, TEXT("Alarm(TTS)"));
				AppendMenu(hMenu, MF_STRING, POP_MENU_AUDIO_ALARM_CLEAR, TEXT("Clear"));
				

				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_STRING, POP_MENU_SET_OVERLAY_TEXT, TEXT("SetOverlayText"));
				AppendMenu(hMenu, MF_STRING, POP_MENU_CLEAR_OVERLAY_TEXT, TEXT("Clear Overlay Text"));


				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_SEPARATOR, POP_MENU_SEPARATOR, TEXT("-"));
				AppendMenu(hMenu, MF_STRING|(channelStatus.U10_Zoom==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_U10_ZOOM, TEXT("U10 Zoom"));
#endif	

				CPoint	pMousePosition;
				GetCursorPos(&pMousePosition);
				SetForegroundWindow();
				TrackPopupMenu(hMenu, TPM_LEFTALIGN, pMousePosition.x, pMousePosition.y, 0, GetSafeHwnd(), NULL);
			}
		}
	}

	CDialogEx::OnRButtonUp(nFlags, point);
}


BOOL CDlgRender::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD	wID = (WORD)wParam;
	switch (wID)
	{
	case POP_MENU_AUDIO:
		{
			if (mChannelId > 0)
			{
				channelStatus.audio = (channelStatus.audio==0x00?0x01:0x00);
				
				if (channelStatus.audio == 0x01)
				{
					int ret = libEasyPlayer_StartPlaySound(playerHandle, mChannelId);
					if (ret < 0)	channelStatus.audio = 0x00;
				}
				else
				{
					libEasyPlayer_StopPlaySound(playerHandle);
				}
			}
		}
		break;
	case POP_MENU_RECORDING_MPG:
		{
			//channelStatus.recording = (channelStatus.recording==0x00?0x01:0x00);
			if (mChannelId > 0)
			{
				channelStatus.recordingMpg = (channelStatus.recordingMpg==0x00?0x01:0x00);

				char sztmp[36] = {0};
				time_t tt = time(NULL);
				struct tm *_time = localtime(&tt);
				memset(sztmp, 0x00, sizeof(sztmp));
				strftime(sztmp, 32, "%Y%m%d_%H%M%S", _time);
				
				char szFilename[MAX_PATH] = {0};
				sprintf(szFilename, "ch%d_%s.mpg", mChannelId, sztmp);

				if (channelStatus.recordingMpg == 0x01)
				{
					int ret = libEasyPlayer_StartRecording(playerHandle, mChannelId, "", szFilename, 512, 300, 0x01);
					if (ret < 0)	channelStatus.recordingMpg = 0x00;
				}
				else											libEasyPlayer_StopRecording(playerHandle, mChannelId);
			}
		}
		break;
	case POP_MENU_RECORDING_ES:
		{
			if (mChannelId > 0)
			{
				char sztmp[36] = {0};
				time_t tt = time(NULL);
				struct tm *_time = localtime(&tt);
				memset(sztmp, 0x00, sizeof(sztmp));
				strftime(sztmp, 32, "%Y%m%d_%H%M%S", _time);
				
				sprintf(channelStatus.filenameES, "ch%d_%s", mChannelId, sztmp);

				channelStatus.recordingES = (channelStatus.recordingES == 0x00 ? 0x01 : 0x00);
			}
		}
		break;
	case POP_MENU_RECORDING_DEBUG:
		{
			if (mChannelId > 0)
			{
				channelStatus.recordingDebug = (channelStatus.recordingDebug==0x00?0x01:0x00);

				if (channelStatus.recordingDebug == 1)
				{
					char sztmp[36] = {0};
					time_t tt = time(NULL);
					struct tm *_time = localtime(&tt);
					memset(sztmp, 0x00, sizeof(sztmp));
					strftime(sztmp, 32, "%Y%m%d_%H%M%S", _time);

					sprintf(channelStatus.filenameDebug, "ch%d_%s.raw", mChannelId, sztmp);
				}
			}
		}
		break;
	case POP_MENU_SET_RENDER_RECT:
		{
			channelStatus.setRenderRect = (channelStatus.setRenderRect==0x00?0x01:0x00);

			libEasyPlayer_ResetFrameQueue(playerHandle, mChannelId);

			/*
			RECT	rcSrc;
			SetRect(&rcSrc, 50, 50, 900, 600);
			if (channelStatus.setRenderRect == 0x01)
			{
				libEasyPlayer_SetRenderRect(playerHandle, mChannelId, &rcSrc);
			}
			else
			{
				libEasyPlayer_SetRenderRect(playerHandle, mChannelId, NULL);
			}
			*/



		}
		break;
	case POP_MENU_VIDEO_FLIP:
		{
			if (mChannelId > 0)
			{
				channelStatus.flip = (channelStatus.flip==0x00?0x01:0x00);

				libEasyPlayer_SetVideoFlip(playerHandle, mChannelId, channelStatus.flip);
			}
		}
		break;
	case POP_MENU_DECODE_KEYFRAME_ONLY:
		{
			if (mChannelId > 0)
			{
				channelStatus.decodeKeyframeOnly = (channelStatus.decodeKeyframeOnly==0x00?0x01:0x00);

				libEasyPlayer_SetDecodeType(playerHandle, mChannelId, channelStatus.decodeKeyframeOnly);
			}
		}
		break;
	case POP_MENU_SNAPSHOT_BMP:
	case POP_MENU_SNAPSHOT_JPG:
		{
			char sztmp[128] ={0};
			static int iSnapshotTally = 0;
			for (int i=0; i<1; i++)
			{
				memset(sztmp, 0x00, sizeof(sztmp));
				sprintf(sztmp, "Snapshot/%d.%s", ++iSnapshotTally, wID==POP_MENU_SNAPSHOT_BMP?"bmp":"jpg");
				while (0 != libEasyPlayer_SnapshotToFile(playerHandle, mChannelId, wID==POP_MENU_SNAPSHOT_BMP?0:1, sztmp, 0,1))
				{
					Sleep(1);
				}
			}
		}
		break;
	case POP_MENU_STREAM_PAUSE:
		{
			if (channelStatus.instantReplay == 0x00)
			{
			}
			else
			{
				libEasyPlayer_InstantReplay_Pause(playerHandle, mChannelId);			//暂停即时回放
			}
		}
		break;
	case POP_MENU_STREAM_RESUME:
		{
			if (channelStatus.instantReplay == 0x00)
			{
			}
			else
			{
				libEasyPlayer_InstantReplay_Resume(playerHandle, mChannelId);			//恢复即时回放
			}
		}
		break;
	case POP_MENU_STREAM_INSTANT_REPLAY:
		{
			channelStatus.instantReplay = (channelStatus.instantReplay==0x00?0x01:0x00);

			if (channelStatus.instantReplay == 0x01)
			{
				libEasyPlayer_InstantReplay_Start(playerHandle, mChannelId);		//开始即时回放

				int frameno=0, framenum=0;
				libEasyPlayer_InstantReplay_GetFrameNum(playerHandle, mChannelId, &frameno, &framenum);
				TRACE("video frame num: %d  / %d\n", frameno, framenum);
			}
			else
			{
				libEasyPlayer_InstantReplay_Stop(playerHandle, mChannelId);			//停止即时回放
			}
		}
		break;
	case POP_MENU_STREAM_INSTANT_REPLAY_RECORDING:
		{
		char filename[128] = { 0 };
		sprintf(filename, "ch%04d_InstantReplay.mpg", mChannelId);
			libEasyPlayer_InstantReplay_Save(playerHandle, mChannelId, filename);
		}
		break;
	case POP_MENU_STREAM_PREVIOUS_FRAME:
		{
		}
		break;
	case POP_MENU_STREAM_NEXT_FRAME:
		{
		}
		break;

	case POP_MENU_STREAM_FAST_X2:
		{
			channelStatus.playSpeed = PLAY_SPEED_FAST_X2;
		}
		break;
	case POP_MENU_STREAM_FAST_X4:
		{
			channelStatus.playSpeed = PLAY_SPEED_FAST_X4;
		}
		break;
	case POP_MENU_STREAM_FAST_X8:
		{
			channelStatus.playSpeed = PLAY_SPEED_FAST_X8;
		}
		break;
	case POP_MENU_STREAM_NORMAL_X1:
		{
			channelStatus.playSpeed = PLAY_SPEED_NORMAL;
		}
		break;
	case POP_MENU_STREAM_SLOW_X2:
		{
			channelStatus.playSpeed = PLAY_SPEED_SLOW_X2;
		}
		break;
	case POP_MENU_STREAM_SLOW_X4:
		{
			channelStatus.playSpeed = PLAY_SPEED_SLOW_X4;
		}
		break;
	case POP_MENU_STREAM_SLOW_X8:
		{
			channelStatus.playSpeed = PLAY_SPEED_SLOW_X8;

		}
		break;
	case POP_MENU_STREAM_REWIND_X2:
		{
			channelStatus.playSpeed = PLAY_SPEED_REWIND_X2;

		}
		break;
	case POP_MENU_STREAM_REWIND_X4:
		{
			channelStatus.playSpeed = PLAY_SPEED_REWIND_X4;

		}
		break;
	case POP_MENU_STREAM_REWIND_X8:
		{
			channelStatus.playSpeed = PLAY_SPEED_REWIND_X8;

		}
		break;

		//视频分析设置
/*
	case POP_MENU_VA_ENABLE:
		{
			channelStatus.videoAnalysis = (channelStatus.videoAnalysis==0x01?0x00:0x01);
			libVA_SetRenderMode(playerHandle, mChannelId, channelStatus.videoAnalysis==0x01?RENDER_MODE_ZONEANDRULE:RENDER_MODE_VIDEO);
		}
		break;
*/


	case POP_MENU_FACE_MATCH_THRESHOLD:
	case POP_MENU_FACE_MATCH_THRESHOLD + 1:
	case POP_MENU_FACE_MATCH_THRESHOLD + 2:
	case POP_MENU_FACE_MATCH_THRESHOLD + 3:
	case POP_MENU_FACE_MATCH_THRESHOLD + 4:
	case POP_MENU_FACE_MATCH_THRESHOLD + 5:
	case POP_MENU_FACE_MATCH_THRESHOLD + 6:
	case POP_MENU_FACE_MATCH_THRESHOLD + 7:
	case POP_MENU_FACE_MATCH_THRESHOLD + 8:
	case POP_MENU_FACE_MATCH_THRESHOLD + 9:
	case POP_MENU_FACE_MATCH_THRESHOLD + 10:
	{
		channelStatus.faceMatchThreshold = (wID - POP_MENU_FACE_MATCH_THRESHOLD+1)*10;
	}
		break;
	case POP_MENU_AUDIO_ALARM_BEEP:
		{
			
		}
		break;
	case POP_MENU_AUDIO_ALARM_WAV:
		{
			char *filename = "C:\\test\\1.wav";

		}
		break;
	case POP_MENU_AUDIO_ALARM_TTS:
		{
			char *soundTxt = "语音报警测试";

		}
		break;
	case POP_MENU_AUDIO_ALARM_CLEAR:
		{

		}
		break;
	case POP_MENU_ELECTORIC_ZOOM:
		{
			channelStatus.digitalZoom = (channelStatus.digitalZoom==0x00?0x01:0x00);

			if (channelStatus.digitalZoom == 0x01)
			{
				//开始电子放大
			}
			else
			{
				libEasyPlayer_SetElectronicZoom(playerHandle, mChannelId, 0);			//恢复电子放大
				libEasyPlayer_ResetElectronicZoom(playerHandle, mChannelId);
			}
		}
		break;
	case POP_MENU_U10_ZOOM:
		{
			channelStatus.U10_Zoom = (channelStatus.U10_Zoom==0x00?0x01:0x00);

			if (channelStatus.U10_Zoom == 0x01)
			{
				//开启放大模式

				channelStatus.u10ZoomTally = 2;
			}
			else
			{
				libEasyPlayer_SetElectronicZoom(playerHandle, mChannelId, 0);			//恢复电子放大
				libEasyPlayer_ResetElectronicZoom(playerHandle, mChannelId);
			}
		}
		break;
	case POP_MENU_SET_OVERLAY_TEXT:
		{
			libEasyPlayer_SetOverlayText(playerHandle, mChannelId, "测试叠加文字");
		}
		break;
	case POP_MENU_CLEAR_OVERLAY_TEXT:
		{
			libEasyPlayer_ClearOverlayText(playerHandle, mChannelId);
		}
		break;
	default:
		break;
	}


	return CDialogEx::OnCommand(wParam, lParam);
}


void CDlgRender::OnLButtonDown(UINT nFlags, CPoint point)
{
	mDrag = true;

	startPoint.x = point.x;
	startPoint.y = point.y;

	CRect rcClient;
	GetClientRect(&rcClient);
	int nLeftPercent = (int)((float)startPoint.x / (float)rcClient.Width()*100.0f);
	int nTopPercent  = (int)((float)startPoint.y / (float)rcClient.Height()*100.0f);

	POINT pt;
	pt.x = nLeftPercent;
	pt.y = nTopPercent;

	//Player_SetDragStartPoint(mChannelId, pt);

	if (channelStatus.digitalZoom == 0x01)		//电子放大
	{
		channelStatus.drag = 0x01;

		float fXPercent = 0.0f, fYPercent=0.0f;

		CRect rcClient;
		GetClientRect(&rcClient);
		fXPercent = ((float)point.x / (float)rcClient.Width()*100.0f);
		fYPercent  = ((float)point.y / (float)rcClient.Height()*100.0f);
		libEasyPlayer_SetElectronicZoomStartPoint(playerHandle, mChannelId, fXPercent, fYPercent, 0x01);
	}
	else if (channelStatus.U10_Zoom == 0x01)		//U10缩放显示
	{
#if 0
		channelStatus.drag = 0x01;

		float fXPercent = 0.0f, fYPercent=0.0f;

		CRect rcClient;
		GetClientRect(&rcClient);
		fXPercent = ((float)point.x / (float)rcClient.Width()*100.0f);
		fYPercent  = ((float)point.y / (float)rcClient.Height()*100.0f);
		libEasyPlayer_SetElectronicZoomStartPoint(playerHandle, mChannelId, fXPercent, fYPercent, 0x01);

		channelStatus.fXPercentStart = fXPercent;
		channelStatus.fYPercentStart = fYPercent;

#else
		LIVE_MEDIA_INFO_T	mediaInfo;
		memset(&mediaInfo, 0x00, sizeof(LIVE_MEDIA_INFO_T));
		libEasyPlayer_GetStreamInfo(playerHandle, mChannelId, &mediaInfo);
		
		int resolutionWidth = mediaInfo.videoWidth;
		int resolutionHeight = mediaInfo.videoHeight;

		if (resolutionWidth > 0 && resolutionHeight > 0)
		{
			float fW = 48.0f;
			float fH = 36.0f;
			float fXInterval = fW / 2.0f;
			float fYInterval = fH / 2.0f;

			float fXPercent = 0.0f, fYPercent = 0.0f;
			float fXPercentStart = 0.0f, fYPercentStart=0.0f;
			float fXPercentEnd = 0.0f, fYPercentEnd=0.0f;

			CRect rcClient;
			GetClientRect(&rcClient);
			fXPercent = ((float)point.x / (float)rcClient.Width()*100.0f);
			fYPercent  = ((float)point.y / (float)rcClient.Height()*100.0f);

			fXPercentStart = fXPercent - fXInterval;
			if (fXPercentStart < 0.0000001f)	fXPercentStart = 0.0f;
			fYPercentStart = fYPercent - fYInterval;
			if (fYPercentStart < 0.9999999f)	fYPercentStart = 1.0f;

			fXPercentEnd = fXPercentStart + fW;
			if (fXPercentEnd >= 99.9999999f)
			{
				fXPercentEnd = 100.0f;
				fXPercentStart = fXPercentEnd - fW;
			}

			fYPercentEnd = fYPercentStart + fH;
			if (fYPercentEnd >= 99.9999999f)
			{
				fYPercentEnd = 100.0f;
				fYPercentStart = fYPercentEnd - fH;
			}
			libEasyPlayer_SetElectronicZoomStartPoint(playerHandle, mChannelId, fXPercentStart, fYPercentStart, 0x01);
			libEasyPlayer_SetElectronicZoomEndPoint(  playerHandle, mChannelId, fXPercentEnd,   fYPercentEnd);
			Sleep(300);
			libEasyPlayer_SetElectronicZoom(playerHandle, mChannelId, 1);

			channelStatus.u10ZoomTally ++;

		}
#endif
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void	CDlgRender::InitZoom()
{
	base_left_percent = 0.0f;
	base_right_percent = 0.0f;
	fWidthPercent = 0.0f;

	base_top_percent = 0;
	base_bottom_percent = 0;
	fHeightPercent = 0;
}

void CDlgRender::OnLButtonUp(UINT nFlags, CPoint point)
{

	if (mDrag)
	{
		if (channelStatus.digitalZoom == 0x01)
		{
			libEasyPlayer_SetElectronicZoom(playerHandle, mChannelId, 1);
			//libEasyPlayer_ResetDragPoint(playerHandle, mChannelId);
		}
		else
		{

		}
	}




	mDrag = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDlgRender::OnMouseMove(UINT nFlags, CPoint point)
{
	if (mDrag)
	{
		CRect rcClient;
		GetClientRect(&rcClient);



		if (channelStatus.digitalZoom == 0x01)		//电子放大
		{
			float fXPercent = 0.0f, fYPercent=0.0f;

			CRect rcClient;
			GetClientRect(&rcClient);
			fXPercent = ((float)point.x / (float)rcClient.Width()*100.0f);
			fYPercent  = ((float)point.y / (float)rcClient.Height()*100.0f);
			libEasyPlayer_SetElectronicZoomEndPoint(playerHandle, mChannelId, fXPercent, fYPercent);
		}
		else if (channelStatus.U10_Zoom == 0x01)		//U10缩放显示
		{
#if 0
			LIVE_MEDIA_INFO_T	mediaInfo;
			memset(&mediaInfo, 0x00, sizeof(LIVE_MEDIA_INFO_T));
			libEasyPlayer_GetStreamInfo(playerHandle, mChannelId, &mediaInfo);
		
			int resolutionWidth = mediaInfo.videoWidth;
			int resolutionHeight = mediaInfo.videoHeight;

			float fW = 0.48f;
			float fH = 0.64f;

			if (resolutionWidth > 0 && resolutionHeight > 0)
			{
				//设置起始位置
				libEasyPlayer_SetElectronicZoomStartPoint(playerHandle, mChannelId, channelStatus.fXPercentStart, channelStatus.fYPercentStart, 0x01);


				float fXPercentEnd = 0.0f, fYPercentEnd=0.0f;
				fXPercentEnd = ((float)point.x / (float)rcClient.Width()*100.0f);
				fYPercentEnd  = ((float)point.y / (float)rcClient.Height()*100.0f);

				float fXPercent = 0.0f, fYPercent = 0.0f;

				if (fXPercentEnd - channelStatus.fXPercentStart >= fW)		fXPercent = 

				//设置终点位置
				libEasyPlayer_SetElectronicZoomEndPoint(playerHandle, mChannelId, fXPercentEnd, fYPercentEnd);

			}



#endif

		}

		else
		{

		}
	}


	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CDlgRender::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	
	TRACE("zDelta: %d\n", zDelta);


	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
void CDlgRender::OnMouseWheel(short zDelta, CPoint pt)
{
	//TRACE("Window[%d]  MouseWheel zDelta:%d   pt.x:%d\tpt.y:%d\n", mChannelId, zDelta, pt.x, pt.y);


	TRACE("======================================================\n");

	ScreenToClient(&pt);

	CRect rcClient;
	GetClientRect(&rcClient);
	int nLeftPercent = (int)((float)pt.x / (float)rcClient.Width()*100.0f);
	int nTopPercent  = (int)((float)pt.y / (float)rcClient.Height()*100.0f);

	//TRACE("leftPercent: %d\t\ttopPercent:%d\n", nLeftPercent, nTopPercent);

	int video_width = 1920;
	int video_height = 1080;

	int nLeftPos = (int)((float)video_width / 100.0f * nLeftPercent);
	int nTopPos  = (int)((float)video_height/ 100.0f * nTopPercent);

	int max_scale_multiple = 20;	//缩放倍数

	if (zDelta > 0)		//放大
	{
		if (scaleMultiple < max_scale_multiple)	scaleMultiple ++;
		
	}
	else		//缩小
	{
		if (scaleMultiple > 1)	scaleMultiple --;
	}

	static int fix_width = video_width;
	static int fix_height  = video_height;

	int nLeft = (int)((float)video_width * (float)(scaleMultiple*2) / 100.0f);
	int nTop  = (int)((float)video_height * (float)(scaleMultiple*2) / 100.0f);

	int nWidth  = (int)((float)video_width - (nLeft * 2) );
	int nHeight = (int)((float)video_height - (nTop * 2) );

	if (scaleMultiple > 1)
	{
		nLeft = (int)((float)fix_width * (float)(scaleMultiple*2) / 100.0f);
		nTop  = (int)((float)fix_height * (float)(scaleMultiple*2) / 100.0f);

		nWidth  = (int)((float)fix_width - (nLeft * 2) );
		nHeight = (int)((float)fix_height - (nTop * 2) );

		fix_width = nWidth;
		fix_height= nHeight;
	}
	else
	{
		fix_width = video_height;
		fix_height  = video_height;
	}

	//nWidth = (int)((float)scale_width / 100.0f * (100-nLeftPercent));
	//nHeight= (int)((float)scale_height / 100.0f * nTopPercent);

	int nW_Percent = 10;
	int nH_Percent = 10;

	if (nLeftPercent < 50 - nW_Percent)
	{
		if (nLeftPos < nLeft)
		{
			int nn = nLeft - nLeftPos;

			nLeft -= nn;
		}
		else
		{
			//int nn = (nLeftPos-nLeft)/2;
			int nn = (nLeftPos-nLeft);
			nLeft += nn;
		}
	}
	else if (nLeftPercent > 50 + nW_Percent)
	{
		nLeft = (video_width - nWidth);
	}
	else
	{
		//int nn = (nLeftPos - nLeft)/2;
		//nLeft += (nn/2);
	}

	if (nTopPercent < 50 - nH_Percent)
	{
		if (nTopPos < nTop)
		{
			nTop -= (nTop - nTopPos);
		}
		else
		{
			nTop += (nTopPos - nTop);
		}
	}
	else if (nTopPercent > 50 + nH_Percent)
	{
		nTop = (video_height - nHeight);
	}
	else
	{
		//int nn = (nTopPos - nTop)/2;
		//nTop += (nn/2);
	}


#if 0
	nLeft = (int)((float)nWidth * (float)(scaleMultiple*2) / 100.0f);
	nTop  = (int)((float)nHeight * (float)(scaleMultiple*2) / 100.0f);

	nWidth  = (int)((float)nWidth - (nLeft * 2) );
	nHeight = (int)((float)nHeight - (nTop * 2) );

	nW_Percent = 3;
	nH_Percent = 3;
	if (nLeftPercent < 50 - nW_Percent)
	{
		if (nLeftPos < nLeft)
		{
			int nn = nLeft - nLeftPos;

			nLeft -= nn;
		}
		else
		{
			//int nn = (nLeftPos-nLeft)/2;
			int nn = (nLeftPos-nLeft);
			nLeft += nn;
		}
	}
	else if (nLeftPercent > 50 + nW_Percent)
	{
		nLeft = (video_width - nWidth);
	}
	else
	{
		//int nn = (nLeftPos - nLeft)/2;
		//nLeft += (nn/2);
	}

	if (nTopPercent < 50 - nH_Percent)
	{
		if (nTopPos < nTop)
		{
			nTop -= (nTop - nTopPos);
		}
		else
		{
			nTop += (nTopPos - nTop);
		}
	}
	else if (nTopPercent > 50 + nH_Percent)
	{
		nTop = (video_height - nHeight);
	}
	else
	{
		//int nn = (nTopPos - nTop)/2;
		//nTop += (nn/2);
	}

#endif
	
	if (nWidth > video_width)	nWidth = video_width;
	if (nHeight > video_height)	nHeight = video_height;

	if (scaleMultiple == 1)
	{
		nLeft = 0;
		nTop  = 0;
		nWidth = video_width;
		nHeight= video_height;
	}

	/*
	if (nTopPos < nTop)
	{
		int nn = nTop - nTopPos;

		nTop -= nn;
		nHeight -= nn;
	}
	else
	{
		int nn = nTopPos - nTop;
		nTop += nn;
		nHeight += nn;
	}
	*/

	//if (nLeft > x_startpixel)
		//nWidth = scale_width - (nLeft-x_startpixel);
	//else
		//nWidth = scale_width - (x_startpixel - nLeft);

	//nWidth = scale_width 

#if 0
	if (scale_width < video_width)
	{
		//TRACE("nLeft: %d\tnLeft2: %d\n", nLeft, nLeft2);

		if (nLeft < x_startpixel)
		{
			nWidth = scale_width - (x_startpixel-nLeft);
			TRACE("Scale_X:%d Scale_Width:%d\n", nLeft, nWidth);
		}
		else if (nLeft > x_startpixel)
		{
			nWidth = scale_width + (nLeft - x_startpixel);
		}

		
	}

	if (scale_height < video_height)
	{
		if (nTop < y_startpixel)
		{
			nHeight = scale_height - (y_startpixel-nTop);
			TRACE("Scale_Y:%d Scale_Height:%d\n", nTop, nHeight);
		}
		else if (nTop > y_startpixel)
		{
			nHeight = scale_height + (nTop - y_startpixel);
		}

	}
#endif
	

	RECT rcSrc;
	SetRect(&rcSrc, nLeft, nTop, nWidth+nLeft, nHeight+nTop);

	RECT rcTmp;
	CopyRect(&rcTmp, &rcSrc);

	while (rcSrc.left % 2 != 0x00)	{rcSrc.left++;}
	while (rcSrc.top  % 2 != 0x00)	{rcSrc.top ++;}
	while (rcSrc.right % 2 != 0x00)	{rcSrc.right --;}
	while (rcSrc.bottom % 2 != 0x00)	{rcSrc.bottom --;}



	//Player_SetRenderRect(mChannelId, &rcSrc);


	TRACE("[%d]缩放位置: left[%d]-->[%d] top[%d]-->[%d] right[%d]-->[%d] bottom[%d]-->[%d]\n",  scaleMultiple,
		rcTmp.left, rcSrc.left, 
		rcTmp.top,  rcSrc.top, 
		rcTmp.right, rcSrc.right,
		rcTmp.bottom, rcSrc.bottom);

	//TRACE("缩放位置: left[%d] top[%d]  right[%d] bottom[%d]\n", rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom);
}



void	CDlgRender::SetRecordingFlag(int flag)
{
	channelStatus.recordingMpg = flag;
	channelStatus.recordingES = flag;
	channelStatus.recordingDebug = flag;
}


void CDlgRender::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);


}

#if 0
#include "VCASourceAPI.h"
#pragma comment(lib, "VCASource.lib")
#endif
unsigned short percenttopixel_gdi(unsigned short percent, unsigned short maxvalue)
{
	return (unsigned short)((float)percent/65535.0f* (float)(maxvalue-1));		//maxvalue: 当前分辨率的宽度或高度

	//return ((unsigned short)(float)(((float)(maxvalue-1) * (float)percent) / 65535.0f));
}
//将实际像素转换至对应百分比
unsigned short pixeltopercent_gdi(unsigned short pixel, unsigned short maxvalue)
{
	return (unsigned short)(float)((float)pixel / (float)(maxvalue-1) * 65535.0f);
}
void	CDlgRender::LoadZones()
{
#if 0
	char *filename = "VCAConf_01.xml";
	FILE *f = fopen(filename, "rb");
	if (NULL == f)		return;

	fseek(f, 0, SEEK_END);
	int bufsize = ftell(f)+1;
	fseek(f, 0, SEEK_SET);
	char *pbuf = new char[bufsize];
	fread(pbuf, 1, bufsize, f);
	fclose(f);

	libVCASource_Init(pbuf, bufsize-1);


	CRect	rcClient;
	GetClientRect(&rcClient);

	VCA_ZONE_T		*pZones = NULL;
	int		zoneNum	=	0;
	libVCASource_GetZone(&pZones, &zoneNum);

	if (zoneNum > 0)
	{
		int width = 1920;
		int height = 1080;

#if 0
		VA_DETECT_ZONE_LIST_T *pZoneList = new VA_DETECT_ZONE_LIST_T[zoneNum];
		memset(&pZoneList[0], 0x00, sizeof(VA_DETECT_ZONE_LIST_T)*zoneNum);
		for (int i=0; i<zoneNum; i++)
		{
			pZoneList[i].zone.id = pZones[i].id;
			pZoneList[i].zone.color = pZones[i].color;
			pZoneList[i].zone.borderColor = pZones[i].color;
			pZoneList[i].zone.textColor = pZones[i].color;
			pZoneList[i].zone.complete = 1;
			pZoneList[i].zone.show = 1;
			pZoneList[i].zone.alpha_normal = 50;
			pZoneList[i].zone.alpha_selected = 130;

			strcpy(pZoneList[i].zone.name, pZones[i].name);
			pZoneList[i].zone.min_point_num = 3;
			pZoneList[i].zone.max_point_num = 40;
			pZoneList[i].zone.point_num = pZones[i].totalPoints;
			for (int j=0; j<pZones[i].totalPoints; j++)
			{
				VA_DETECT_POINT_T	p;
				memset(&p, 0x00, sizeof(VA_DETECT_POINT_T));
				p.x = percenttopixel_gdi(pZones[i].points[j].x, width);
				p.xPercent = (int)(int)((float)p.x / (float)width * 100.0f);
				p.y = percenttopixel_gdi(pZones[i].points[j].y, height);
				p.yPercent = (int)(int)((float)p.y / (float)height * 100.0f);

				pZoneList[i].zone.point[j].xPercent = p.xPercent;
				pZoneList[i].zone.point[j].yPercent = p.yPercent;
			}
		}

		libVA_SetCustomZone(playerHandle, mChannelId, pZoneList, zoneNum);


		delete []pZoneList;
		pZoneList = NULL;
#else

		libVA_SetCustomZone(playerHandle, mChannelId, NULL, 0);

		for (int i=0; i<zoneNum; i++)
		{
			for (int j=0; j<pZones[i].totalPoints; j++)
			{
				VA_DETECT_POINT_T	p;
				memset(&p, 0x00, sizeof(VA_DETECT_POINT_T));
				p.x = percenttopixel_gdi(pZones[i].points[j].x, width);
				p.xPercent = (int)(int)((float)p.x / (float)width * 100.0f);
				p.y = percenttopixel_gdi(pZones[i].points[j].y, height);
				p.yPercent = (int)(int)((float)p.y / (float)height * 100.0f);

				libVA_AddCustomZoneNode(playerHandle, mChannelId, i, pZones[i].name, &p, 0x00, 1, 3, 
					pZones[i].totalPoints, 0x00, pZones[i].color, pZones[i].color, RGB(0x00,0xFF,0x00), 20, 60, 180);
			}
			libVA_EndCustomZoneNode(playerHandle, mChannelId, 0x00);
		}

		libVA_AddCustomZoneNode(playerHandle, mChannelId, 0, "", NULL, 0x00, 1, 3, 
			1, 0x00, 0x00, 0x00, 0x00, 20, 0, 0);
#endif

		libVA_UpdateCustomZonePosition(playerHandle, mChannelId, &rcClient);
	}

	libVCASource_Deinit();


	delete []pbuf;
#endif
}


typedef struct __DEBUG_FILE_STRUCT_T
{
	unsigned int	mediaType;
	unsigned int	codec;
	unsigned char	type;
	unsigned short	width;
	unsigned short	height;
	unsigned int	framesize;
	unsigned int	timestamp_sec;
	unsigned int	timestamp_usec;
}DEBUG_FILE_STRUCT_T;

void	CDlgRender::onVideoData(int channelId, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo)
{
	//DEBUG File
	if (channelStatus.recordingDebug == 1)
	{
		DEBUG_FILE_STRUCT_T	header;
		memset(&header, 0x00, sizeof(DEBUG_FILE_STRUCT_T));
		header.type = frameInfo->type;
		header.codec = frameInfo->codec;
		header.framesize = frameInfo->length;
		header.width = frameInfo->width;
		header.height = frameInfo->height;
		header.mediaType = mediaType;
		header.timestamp_sec = frameInfo->rtptimestamp_sec;
		header.timestamp_usec = frameInfo->rtptimestamp_usec;

		if (channelStatus.fDebug)
		{
			fwrite((void*)&header, 1, sizeof(DEBUG_FILE_STRUCT_T), channelStatus.fDebug);
			fwrite(pbuf, 1, frameInfo->length, channelStatus.fDebug);
		}
		else if (frameInfo->type == 1)
		{
			channelStatus.fDebug = fopen(channelStatus.filenameDebug, "wb");
			if (channelStatus.fDebug)
			{
				fwrite((void*)&header, 1, sizeof(DEBUG_FILE_STRUCT_T), channelStatus.fDebug);
				fwrite(pbuf, 1, frameInfo->length, channelStatus.fDebug);
			}
		}
	}
	else if (channelStatus.fDebug)
	{
		fclose(channelStatus.fDebug);
		channelStatus.fDebug = NULL;
	}


	//ES
	if (channelStatus.recordingES == 1)
	{
		if (channelStatus.fES)
		{
			fwrite(pbuf, 1, frameInfo->length, channelStatus.fES);
		}
		else if (frameInfo->type == 1)
		{
			char filename[260] = {0};
			strcpy(filename, channelStatus.filenameES);
			if (frameInfo->codec == RTSP_VIDEO_CODEC_H264)	strcat(filename, ".h264");
			else if (frameInfo->codec == RTSP_VIDEO_CODEC_H265)	strcat(filename, ".h265");
			else		strcpy(filename, "es");

			channelStatus.fES = fopen(filename, "wb");
			if (channelStatus.fES)
			{
				fwrite(pbuf, 1, frameInfo->length, channelStatus.fES);
			}
		}
	}
	else if (channelStatus.fES)
	{
		fclose(channelStatus.fES);
		channelStatus.fES = NULL;
	}



}
