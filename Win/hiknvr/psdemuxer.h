#ifndef __PS_DEMUXER_H__
#define __PS_DEMUXER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "buff.h"

#ifndef MEDIA_TYPE_VIDEO
#define MEDIA_TYPE_VIDEO		0x00000001
#endif
#ifndef MEDIA_TYPE_AUDIO
#define MEDIA_TYPE_AUDIO		0x00000002
#endif
#ifndef MEDIA_TYPE_EVENT
#define MEDIA_TYPE_EVENT		0x00000004
#endif


typedef struct __PS_FILE_OBJ_T
{
	FILE		*fPs;		//File Handle

	BUFF_T		demuxBuf;
	BUFF_T		frame;
	int			offset;
	int			mediaType;
	unsigned int	pts;

	unsigned int filesize;
	unsigned int filepos;

	int			clear;		//标志位
}PS_FILE_OBJ_T;

typedef void *PSDEMUX_HANDLE;

#if defined (__cplusplus)
extern "C"
{
#endif
	int		PSDemux_Init(PSDEMUX_HANDLE *handle, int bufsize, int framesize);
	int		PSDemux_Deinit(PSDEMUX_HANDLE *handle);

	int		PSDemux_OpenFile(PSDEMUX_HANDLE handle, char *filename);
	int		PSDemux_CloseFile(PSDEMUX_HANDLE handle);
	int		PSDemux_GotoFileHead(PSDEMUX_HANDLE handle);		//跳转至文件头
	int		PSDemux_GotoFileOffset(PSDEMUX_HANDLE handle, unsigned int offset);		//跳转至指定位置
	int		PSDemux_GetMediaInfo(PSDEMUX_HANDLE handle, unsigned char *videoinfo, int *videoinfolength, unsigned char *audioinfo, int *audioinfolength);


	int		PSDemux_demux(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata);
	int		PSDemux_demux2(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata);

	int		PSDemux_GetNextFrame(PSDEMUX_HANDLE handle, int *mediatype, int *framesize, char **framedata, unsigned char *reserveddata, int *reservedsize);

	int		PSDemux_demuxPacket(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata);		//解析海康PS流包

	//for internal use
	int		PSDemux_demuxInternalPacket(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata);		//解析海康PS流包

#if defined (__cplusplus)
}
#endif

/*
	int mediatype = 0;
	char *framedata = NULL;
	int  framesize = 0;
	unsigned int frametimestamp = 0;
	FILE *fES = fopen("output.h264", "wb");

	char *filename = "1.mpg";

	PSDEMUX_HANDLE	psDemuxer;
	PSDemux_Init(&psDemuxer);

	PSDemux_OpenFile(psDemuxer, filename);

#if 1
	//流式输入
	{
		char pbuf[1024+1] = {0,};
		FILE *fPS = fopen(filename, "rb");
		int readBytes = 0;
		int ret = 0;

		while (! feof(fPS) )
		{
			memset(pbuf, 0x00, sizeof(pbuf));
			readBytes = fread(pbuf, 1, 1024, fPS);

			ret = PSDemux_demux2(psDemuxer, pbuf, readBytes, &mediatype, &framesize, &framedata);

			if (ret == 0 && mediatype == MEDIA_TYPE_VIDEO)
			{
				fwrite(framedata, 1, framesize, fES);
				fflush(fES);
			}
		}
		fclose(fPS);
	}
#else
	//直接获取完整的一帧数据
	while (1)
	{
		int ret = PSDemux_GetNextFrame(psDemuxer, &mediatype, &frametimestamp, &framesize, &framedata);
		if (ret < 0)	break;

		_TRACE("MEDIATYPE[%d] timestamp[%u]  framesize[%d] \n", mediatype, frametimestamp, framesize);

		if (mediatype == MEDIA_TYPE_VIDEO)
		{
			fwrite(framedata, 1, framesize, fES);
			//fflush(fES);
		}
	}
#endif
	fclose(fES);

	PSDemux_Deinit(&psDemuxer);
*/



#endif
