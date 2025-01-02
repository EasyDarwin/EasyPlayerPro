
#ifndef __RTSP_TYPES_H__
#define __RTSP_TYPES_H__

#ifdef _WIN32
#define LIB_LIVEPLAYER_API  __declspec(dllexport)
#ifndef LIB_APICALL
#define LIB_APICALL  __stdcall
#endif
#define WIN32_LEAN_AND_MEAN
#else
#define LIB_LIVEPLAYER_API
#define LIB_APICALL 
#endif



//媒体类型
#ifndef MEDIA_TYPE_VIDEO
#define MEDIA_TYPE_VIDEO		0x00000001
#endif
#ifndef MEDIA_TYPE_AUDIO
#define MEDIA_TYPE_AUDIO		0x00000002
#endif
#ifndef MEDIA_TYPE_EVENT
#define MEDIA_TYPE_EVENT		0x00000004
#endif
#ifndef MEDIA_TYPE_RTP
#define MEDIA_TYPE_RTP			0x00000008
#endif
#ifndef MEDIA_TYPE_SDP
#define MEDIA_TYPE_SDP			0x00000010
#endif
#ifndef MEDIA_TYPE_CODEC_INFO
#define	MEDIA_TYPE_CODEC_INFO	0x00000020
#endif


typedef enum __RTSP_SERVER_ERROR_CODE_ENUM
{
	RTSP_SERVER_ERR_NoErr			=	0,
	RTSP_SERVER_ERR_NotInitialized	=	-1,			//没有初始化
	RTSP_SERVER_ERR_BadArgument		=	-2,			//参数错误
	RTSP_SERVER_ERR_ALLOC_MEMORY	=	-3,			//内存分配失败
	RTSP_SERVER_ERR_OPERATE			=	-4,			//操作失败
}RTSP_SERVER_ERROR_CODE_ENUM;


//视频编码
#define RTSP_VIDEO_CODEC_H264	0x1C				//H264
#define RTSP_VIDEO_CODEC_H265	0xAE				//H265		174
#define	RTSP_VIDEO_CODEC_MJPEG	0x08				//MJPEG
#define	RTSP_VIDEO_CODEC_MPEG4	0x0D				//MPEG4

//音频编码
#define RTSP_AUDIO_CODEC_AAC	0x15002		//AAC
#define RTSP_AUDIO_CODEC_G711U	0x10006		//G711 ulaw
#define RTSP_AUDIO_CODEC_G711A	0x10007		// G711 alaw
#define RTSP_AUDIO_CODEC_G726	0x1100B		//G726


//视频关键字标识
//#define RTSP_VIDEO_FRAME_I		0x01		//I帧
#define LIVE_FRAME_TYPE_I		0x01		//I帧
#define LIVE_FRAME_TYPE_P		0x02		//P帧
#define LIVE_FRAME_TYPE_B		0x03		//B帧
#define LIVE_FRAME_TYPE_J		0x04		//JPEG

//媒体信息
typedef struct __LIVE_MEDIA_INFO_T
{
	unsigned int videoCodec;			//视频编码类型
	unsigned int videoFps;				//视频帧率
	int			 videoWidth;
	int			 videoHeight;
	float		 videoBitrate;

	unsigned int audioCodec;			//音频编码类型
	unsigned int audioSampleRate;		//音频采样率
	unsigned int audioChannel;			//音频通道数
	unsigned int audioBitsPerSample;	//音频采样精度

	unsigned int metadataCodec;			//Metadata类型

	unsigned int vpsLength;				//视频vps帧长度
	unsigned int spsLength;				//视频sps帧长度
	unsigned int ppsLength;				//视频pps帧长度
	unsigned int seiLength;				//视频sei帧长度
	unsigned char	 vps[255];			//视频vps帧内容
	unsigned char	 sps[255];			//视频sps帧内容
	unsigned char	 pps[128];			//视频sps帧内容
	unsigned char	 sei[128];			//视频sei帧内容
}LIVE_MEDIA_INFO_T;




#endif
