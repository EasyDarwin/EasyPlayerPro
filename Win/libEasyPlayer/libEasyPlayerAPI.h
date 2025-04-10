#ifndef __LIB_EASY_PLAYER_API_H__
#define __LIB_EASY_PLAYER_API_H__

#ifdef _DEBUG
//#include <vld.h>
#endif

#include <winsock2.h>
#include "EasyTypes.h"

typedef enum __RENDER_FORMAT
{
	RENDER_FORMAT_YV12		=		842094169,
	RENDER_FORMAT_YUY2		=		844715353,
	RENDER_FORMAT_UYVY		=		1498831189,
	RENDER_FORMAT_A8R8G8B8	=		21,
	RENDER_FORMAT_X8R8G8B8	=		22,
	RENDER_FORMAT_RGB565	=		23,
	RENDER_FORMAT_RGB555	=		25,

	RENDER_FORMAT_RGB24_GDI=		26,
	RENDER_FORMAT_RGB32_GDI=		27
}RENDER_FORMAT;

//通道源类型 (指定源类型)
typedef enum __LIVE_CHANNEL_TYPE_ENUM
{
	LIVE_CHANNEL_SOURCE_TYPE_RTSP	=	0,		//源为RTSP(内置)
	LIVE_CHANNEL_SOURCE_TYPE_RTMP,				//源为RTMP(内置)
	LIVE_CHANNEL_SOURCE_TYPE_HLS,				//源为HLS(内置)
	LIVE_CHANNEL_SOURCE_TYPE_FILE,				//源为本地文件(内置)
	LIVE_CHANNEL_SOURCE_TYPE_ENCODE_DATA,		//源为外部的编码数据
	LIVE_CHANNEL_SOURCE_TYPE_DECODE_DATA,		//源为外部的解码数据
}LIVE_CHANNEL_SOURCE_TYPE_ENUM;

// 播放速度
typedef enum __PLAY_SPEED_ENUM
{	
	PLAY_SPEED_UNKNOWN	=	-1,
	PLAY_SPEED_NORMAL	=	0x00,		// 正常播放
	PLAY_SPEED_PAUSED,					// 暂停
	PLAY_SPEED_SLOW_X2,					// 1/2
	PLAY_SPEED_SLOW_X4,					// 1/4
	PLAY_SPEED_SLOW_X8,					// 1/8
	PLAY_SPEED_SLOW_X16,				// 1/16
	PLAY_SPEED_FAST_X2,					// x2
	PLAY_SPEED_FAST_X4,					// x4
	PLAY_SPEED_FAST_X8,					// x8
	PLAY_SPEED_FAST_X16,				// x16
	PLAY_SPEED_REWIND_X2,				// -2x
	PLAY_SPEED_REWIND_X4,				// -4x
	PLAY_SPEED_REWIND_X8,				// -8x	
	PLAY_SPEED_REWIND_X16,				// -16x
	PLAY_SPEED_SINGLE_FRAME,			//单帧播放,手动单击
}PLAY_SPEED_ENUM;

//声音设备信息
#define		MAX_MIXER_DEVICE_NUM		16
typedef struct __MIXER_DEVICE_INFO_T
{
	int		id;
	char	name[128];
	char	version[16];
}MIXER_DEVICE_INFO_T;

//声音报警类型
typedef enum __AUDIO_ALARM_TYPE_ENUM
{
	AUDIO_ALARM_TYPE_BEEP		=	0x00000000,		//蜂鸣器
	AUDIO_ALARM_TYPE_WAV		=	0x00000001,		//wav文件
	AUDIO_ALARM_TYPE_VOICE							//语音
}AUDIO_ALARM_TYPE_ENUM;
typedef struct __AUDIO_TTS_VOICE_PACKET
{
	int			id;
	char		name[128];
}AUDIO_TTS_VOICE_PACKET;

//帧信息
typedef struct 
{
	unsigned int	codec;			//编码格式
	unsigned char	type;			//帧类型
	unsigned char	fps;			//帧率
	unsigned char	reserved1;
	unsigned char	reserved2;

	unsigned short	width;			//宽
	unsigned short  height;			//高
	unsigned int	sample_rate;	//采样率
	unsigned int	channels;		//声道
	unsigned int	bitsPerSample;	//采样精度
	unsigned int	length;			//帧大小
	unsigned int    rtptimestamp_sec;	//rtp timestamp	sec
	unsigned int    rtptimestamp_usec;	//rtp timestamp	usec
	unsigned int	timestamp_sec;	//秒
	
	float			bitrate;		//Kbps
	float			losspacket;
	//float			currentPlayTime;//当前播放时间
}LIVE_FRAME_INFO;


//音频采集设备信息
typedef struct __LIVE_AUDIO_CAPTURE_DEVICE_INFO
{
	int			id;
	//LPGUID		lpGuid;
	//LPTSTR		lpstrDescription;
	//LPTSTR		lpstrModule;

	char		description[128];
	char		module[128];
}LIVE_AUDIO_CAPTURE_DEVICE_INFO;
//音频采集格式
typedef struct __LIVE_AUDIO_WAVE_FORMAT_INFO
{
    WORD        wFormatTag;         /* format type */
    WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD       nSamplesPerSec;     /* sample rate */
    DWORD       nAvgBytesPerSec;    /* for buffer estimation */
    WORD        nBlockAlign;        /* block size of data */
    WORD        wBitsPerSample;     /* number of bits per sample of mono data */
    WORD        cbSize;             /* the count in bytes of the size of */
                                    /* extra information (after cbSize) */
}LIVE_AUDIO_WAVE_FORMAT_INFO;


typedef enum __LIVE_CALLBACK_TYPE_ENUM
{
	EASY_TYPE_CONNECTING		=	100,			//当前通道连接中
	EASY_TYPE_CONNECTED,							//当前通道已连接
	EASY_TYPE_RECONNECT,							//当前通道连接已断开,正在重连
	EASY_TYPE_DISCONNECT,							//当前通道连接已中止(内部连接线程已退出),指定了连接次数的情况下会回调该值

	EASY_TYPE_CODEC_DATA,							//编码数据
	EASY_TYPE_DECODE_DATA,							//解码数据
	EASY_TYPE_SNAPSHOT,								//抓拍
	EASY_TYPE_RECORDING,							//录像
	EASY_TYPE_INSTANT_REPLAY_RECORDING,				//即时回放录像完成
	EASY_TYPE_PLAYBACK_TIME,						//当前回放时间
	EASY_TYPE_METADATA,

	EASY_TYPE_START_PLAY_AUDIO,						//开始播放声音
	EASY_TYPE_STOP_PLAY_AUDIO,						//停止播放声音
	EASY_TYPE_CAPTURE_AUDIO_DATA,					//本地采集的音频数据

	EASY_TYPE_FILE_DURATION							//文件时长

}EASY_CALLBACK_TYPE_ENUM;
typedef int (CALLBACK *EasyPlayerCallBack)(EASY_CALLBACK_TYPE_ENUM callbackType, int channelId, void *userPtr, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo);


//播放器句柄
typedef void *PLAYER_HANDLE;

#ifdef __cplusplus
extern "C"
{
#endif

	/*
	libEasyPlayer 分为两种调用方式:
								1. 使用libEasyPlayer_Initialize初始化,  在后面的所有调用中, PLAYER_HANDLE为NULL, 结束调用libEasyPlayer_Deinitialize
								2. 使用libEasyPlayer_Create 创建一个PLAYER_HANDLE句柄, 在后面的所有调用中, 填上创建的句柄, 结束调用libEasyPlayer_Release
	*/

	//=====================================================================================
	//=====================================================================================
	//初始化
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_Initialize(int supportMaxChannel/*按需指定最大通道数, 最大不能超过宏定义MAX_CHANNEL_NUM*/);
	//反初始化
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_Deinitialize();

	//=====================================================================================
	//=====================================================================================
	//创建
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_Create(PLAYER_HANDLE *handle, int supportMaxChannel/*按需指定最大通道数, 最大不能超过宏定义MAX_CHANNEL_NUM*/);
	//释放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_Release(PLAYER_HANDLE *handle);

	//=====================================================================================
	//=====================================================================================

	//打开流, 返回一个channelId, 后续所有操作，都基于该channelId
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_OpenStream(PLAYER_HANDLE handle, 
											LIVE_CHANNEL_SOURCE_TYPE_ENUM channelType/*通道源类型*/, 
											const char *url, unsigned char rtpOverTcp/*1为tcp, 0为udp*/, 
											const char *username, const char *password, 
											unsigned int mediaType, //媒体类型 MEDIA_TYPE_VIDEO | MEDIA_TYPE_AUDIO | MEDIA_TYPE_EVENT
											EasyPlayerCallBack callback, void *userPtr, //回调函数及自定义指针
											unsigned int reconnection/*1表示无限次重连,0表示不重连,大于1表示指定次数重连(需小于1000)*/, 
											unsigned int heartbeatType/*0*/, 
											unsigned int queueSize/*缓冲队列大小,需大于1024*512 */, 
											unsigned char multiplex/*0x01:复用源,即打开同一个url时，到前端的连接只有一个  0x00:打开多少个url,就有多少个连接*/);
	//关闭流
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_CloseStream(PLAYER_HANDLE handle, int channelId);

	//开始播放     对同一个流,最大播放个数不能大于5个, 如果只调用 libEasyPlayer_OpenStream 而不调用 libEasyPlayer_StartPlayStream 则无此限制
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StartPlayStream(PLAYER_HANDLE handle, int channelId, HWND hWnd, 
													RENDER_FORMAT renderFormat, unsigned char decodeType=0/*0:软解 1:硬解*/);
	//送编码流到指定通道, 需配合libEasyPlayer_OpenStream中的channelType==LIVE_CHANNEL_SOURCE_TYPE_ENCODE_DATA
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_PutFrameData(PLAYER_HANDLE handle, int channelId, int mediaType, LIVE_FRAME_INFO *frameInfo, unsigned char *pbuf);
	//清空帧队列, 将从下一个收到的关键帧开始播放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_ResetFrameQueue(PLAYER_HANDLE handle, int channelId);
	//停止播放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StopPlayStream(PLAYER_HANDLE handle, int channelId);

	//获取指定通道的媒体信息
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_GetStreamInfo(PLAYER_HANDLE handle, int channelId, LIVE_MEDIA_INFO_T *pMediaInfo);

	//设置播放帧缓存, 1 - 10   帧数越小表示延时越小
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetPlayFrameCache(PLAYER_HANDLE handle, int channelId, int cache/*缓存大小  1 - 10*/);
	//获取播放帧缓存
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_GetPlayFrameCache(PLAYER_HANDLE handle, int channelId);



	//显示统计信息
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_ShowStatisticalInfo(PLAYER_HANDLE handle, int channelId, unsigned char show);
	//设置是否仅解码显示关键帧
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetDecodeType(PLAYER_HANDLE handle, int channelId, unsigned char decodeKeyframeOnly);
	//设置视频翻转
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetVideoFlip(PLAYER_HANDLE handle, int channelId, unsigned char flip);
	//设置显示区域
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetRenderRect(PLAYER_HANDLE handle, int channelId, LPRECT lpRect);	//RECT为分辨率坐标点
	//按比例显示
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetScaleDisplay(PLAYER_HANDLE handle, int channelId, 
														unsigned char scaleDisplay/*0x00:铺满窗口 0x01:按比例显示*/,
														COLORREF bkColor/*背景色*/);
	//设置叠加文字
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetOverlayText(PLAYER_HANDLE handle, int channelId, const char *text);
	//清空叠加文字
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_ClearOverlayText(PLAYER_HANDLE handle, int channelId);



	//============================================================
	//开始手动录像  如果指定了录像文件大小,则录像时长无效，如filesize为0, 则duration有效
	//如果preRecording设置为1, 则相应的文件大小或文件时长均会增加预录的大小或时长, 例: 指定filesize为100MB,同时preRecording设置为1, 则实际录像文件大小为100MB+预录大小
	//预录大小和时长，库内部指定为10秒,但相应内存均由libEasyPlayer_OpenStream中的queueSize指定, 即:内存给的足够大，最大预录时长为10秒, 内存给的小,预录时长则不足10秒
	//   注:   当开启即时回放时，不允许手动录像, 即会返回-3
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StartRecording(PLAYER_HANDLE handle, int channelId, const char *foldername, const char *filename, 
														unsigned int filesize/*录像文件大小 MB*/, int duration/*录像时长(second)*/,  
														unsigned char preRecording/*0x01:预录  0x00:不预录*/);
	//停止手动录像
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StopRecording(PLAYER_HANDLE handle, int channelId);

	//抓图到文件, 只有在异步抓图模式下，才能使用队列,队列内存为31104000bytes(大约为32MB)
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SnapshotToFile(PLAYER_HANDLE handle, int channelId, unsigned char imageFormat/*0:bmp 1:jpg*/, 
														char *filename, unsigned char sync=0/*0:异步: 1:同步*/, 
														unsigned char useQueue=0/*1:使用队列 0:不使用队列*/);

	//抓图到内存  现仅支持RGB, 内存空间需调用者分配
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SnapshotToMemory(PLAYER_HANDLE handle, int channelId, unsigned char imageFormat/*0:rgb 1:jpg*/, 
														unsigned char *out_imageData, int *out_imageSize, 
														int *out_width, int *out_height);


	//==============================================================================
	//==============================================================================
	//================电子放大控制==================================================
	//==============================================================================
	//==============================================================================
	//设置放大起始点   fXPercent 和 fYPercent 为鼠标单击的点相对于窗口的百分比   showBox为是否显示所画的框
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetElectronicZoomStartPoint(PLAYER_HANDLE handle, int channelId, float fXPercent, float fYPercent, unsigned char showBox);
	//设置放大结束点
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetElectronicZoomEndPoint(PLAYER_HANDLE handle, int channelId, float fXPercent, float fYPercent);
	//设置是否放大
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetElectronicZoom(PLAYER_HANDLE handle, int channelId, int zoomIn/*1:放大  0:不放大*/);
	//清除放大参数
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_ResetElectronicZoom(PLAYER_HANDLE handle, int channelId);

	
	//==============================================================================
	//==============================================================================
	//================即时回放及控制================================================
	//==============================================================================
	//==============================================================================
	//开始即时回放    如果当前处于手动录像中, 则不能开启即时回放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_Start(PLAYER_HANDLE handle, int channelId);
	//暂停即时回放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_Pause(PLAYER_HANDLE handle, int channelId);
	//恢复即时回放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_Resume(PLAYER_HANDLE handle, int channelId);
	//停止即时回放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_Stop(PLAYER_HANDLE handle, int channelId);
	//保存即时回放
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_Save(PLAYER_HANDLE handle, int channelId, char *filename/*完整文件名，含括路径*/);
	//上一帧
	//LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_PreviousFrame(PLAYER_HANDLE handle, int channelId);
	//下一帧
	//LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_NextFrame(PLAYER_HANDLE handle, int channelId);
	//获取即时回放中的帧数
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_InstantReplay_GetFrameNum(PLAYER_HANDLE handle, int channelId, int *currentFrameNo, int *totalFrameNum);



	//==============================================================================
	//==============================================================================
	//==========以下为声音播放及控制================================================
	//==============================================================================
	//==============================================================================
	//播放声音
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StartPlaySound(PLAYER_HANDLE handle, int channelId);
	//停止播放声音
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StopPlaySound(PLAYER_HANDLE handle);
	//获取声音播放状态: 是否在播放中		0:播放中, <0:非播放中
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SoundPlaying(PLAYER_HANDLE handle, int channelId);


	//获取音频设备列表
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_GetAudioOutputDeviceList(PLAYER_HANDLE handle, MIXER_DEVICE_INFO_T	**devicelist, int *deviceNum);
	//设置当前音频设备ID
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetAudioOutputDeviceId(PLAYER_HANDLE handle, int deviceId);
	//设置当前音频设备名称
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetAudioOutputDeviceName(PLAYER_HANDLE handle, char *deviceName);

	//设置音量 ( 0 ~ 100 )
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_SetAudioVolume(PLAYER_HANDLE handle, int volume);
	//获取音量
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_GetAudioVolume(PLAYER_HANDLE handle);





	//==============================================================================
	//==============================================================================
	//==========以下为声音采集控制==================================================
	//==============================================================================
	//==============================================================================
	//获取当前音频采集设备
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_GetAudioCaptureDeviceList(PLAYER_HANDLE handle, int *deviceNum, LIVE_AUDIO_CAPTURE_DEVICE_INFO **pDeviceInfo);
	//打开音频采集设备 && 获取支持的格式列表
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_OpenAudioCaptureDevice(PLAYER_HANDLE handle, int captureDeviceIndex, 
																			int *waveFormatNum, LIVE_AUDIO_WAVE_FORMAT_INFO **ppWaveFormatEx);
	//开始音频采集
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StartAudioCaptureById(PLAYER_HANDLE handle, int waveFormatExIndex, unsigned int codec, int frameSize, 
																			EasyPlayerCallBack callback, void *userptr);
	//开始音频采集  自定义采集格式
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StartAudioCaptureByParam(PLAYER_HANDLE handle, 
																			unsigned int codec/*编码类型: RTSP_AUDIO_CODEC_G711A  RTSP_AUDIO_CODEC_G711U RTSP_AUDIO_CODEC_AAC*/, 
																			int frameSize/*帧大小, g711时为160,320, aac时为1024或2048*/, 
																			int samplerate/*采样率*/, int bitsPerSample/*采样精度*/, int channels/*通道*/, 
																			EasyPlayerCallBack callback, void *userptr);
	//停止音频采集
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_StopAudioCapture(PLAYER_HANDLE handle);
	//关闭音频采集设备
	LIB_LIVEPLAYER_API int LIB_APICALL	libEasyPlayer_CloseAudioCaptureDevice(PLAYER_HANDLE handle);




#ifdef __cplusplus
}
#endif


#if 0

//回调函数示例如下:

int CALLBACK __LivePlayerCallBack(EASY_CALLBACK_TYPE_ENUM callbackType, int channelId, void *userPtr, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo)
{
	LIVE_VIDEO_T	*pLiveVideo = (LIVE_VIDEO_T *)userPtr;

	if (callbackType == EASY_TYPE_CODEC_DATA)	//编码数据
	{
		_TRACE(TRACE_LOG_DEBUG, "编码数据[ch%d]type[%d] channelId[%d] mediaType[%d] [%d x %d] framesize[%d]\n",  pLiveVideo->channelId,
			callbackType, channelId, mediaType, frameInfo->width, frameInfo->height, frameInfo->length);
	}
	else if (callbackType == EASY_TYPE_DECODE_DATA)
	{
		_TRACE(TRACE_LOG_DEBUG, "解码数据[ch%d]type[%d] channelId[%d] mediaType[%d] [%d x %d] framesize[%d]\n",  pLiveVideo->channelId,
			callbackType, channelId, mediaType, frameInfo->width, frameInfo->height, frameInfo->length);
	}
	else if (callbackType == EASY_TYPE_SNAPSHOT)
	{
		//抓拍时, 回调的frameInfo为NULL
		//mediaType为MEDIA_TYPE_VIDEO时表示成功，为MEDIA_TYPE_EVENT时表示失败
		//pbuf为抓拍的文件名

		if (mediaType == MEDIA_TYPE_VIDEO)		//抓图成功
			_TRACE(TRACE_LOG_DEBUG, "抓拍图片成功[ch%d] %s. filename:%s\n",  channelId, mediaType==MEDIA_TYPE_VIDEO?"成功":"失败", pbuf);
		else  if (mediaType == MEDIA_TYPE_EVENT)	//抓图失败
			_TRACE(TRACE_LOG_DEBUG, "抓拍图片失败[ch%d] %s. filename:%s\n",  channelId, mediaType==MEDIA_TYPE_VIDEO?"成功":"失败", pbuf);
	}
	else if (callbackType == EASY_TYPE_RECORDING)
	{
		if (mediaType == MEDIA_TYPE_VIDEO)		//录像成功
			_TRACE(TRACE_LOG_DEBUG, "手动录像成功[ch%d] %s. filename:%s\n",  channelId, mediaType==MEDIA_TYPE_VIDEO?"成功":"失败", pbuf);
		else  if (mediaType == MEDIA_TYPE_EVENT)	//录像失败
			_TRACE(TRACE_LOG_DEBUG, "手动录像失败[ch%d] %s. filename:%s\n",  channelId, mediaType==MEDIA_TYPE_VIDEO?"成功":"失败", pbuf);
	}
	else if (callbackType == EASY_TYPE_PLAYBACK_TIME)		//回放时间
	{
		char szTime[64] = {0};
		time_t tt = frameInfo->timestamp_sec;
		struct tm *_timetmp = NULL;
		_timetmp = localtime(&tt);
		if (NULL != _timetmp)	strftime(szTime, 32, "%Y-%m-%d %H:%M:%S ", _timetmp);

		_TRACE(TRACE_LOG_DEBUG, "[ch%d]当前回放时间: %s\n", channelId, szTime);
	}


	return 0;
}





#endif



#endif
