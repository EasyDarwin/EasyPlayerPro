#include "psdemuxer.h"
#include <winsock2.h>

int		PSDemux_Init(PSDEMUX_HANDLE *handle, int bufsize, int framesize)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *) malloc(sizeof(PS_FILE_OBJ_T));
	if (NULL == pFileObj)		return -1;

	memset(pFileObj, 0x00, sizeof(PS_FILE_OBJ_T));
	if (bufsize > 0)
	{
		BUFF_MALLOC(&pFileObj->demuxBuf, bufsize);
	}

	if (framesize>0 && bufsize>0)// && bufsize>=framesize)
	{
		memset(&pFileObj->frame, 0x00, sizeof(BUFF_T));
		BUFF_MALLOC(&pFileObj->frame, framesize);
	}

	*handle = pFileObj;

	return 0;
}

int		PSDemux_Deinit(PSDEMUX_HANDLE *handle)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)*handle;
	if (NULL == pFileObj)		return -1;

	PSDemux_CloseFile(*handle);

	free(pFileObj);
	*handle = NULL;

	return 0;
}


int		PSDemux_OpenFile(PSDEMUX_HANDLE handle, char *filename)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;

	if (NULL == pFileObj)		return -1;
	if (NULL == filename || (0==strcmp(filename, "\0")))	return -1;

	if (NULL != pFileObj->fPs)
	{
		fclose(pFileObj->fPs);
		pFileObj->fPs = NULL;
	}

	pFileObj->fPs = fopen(filename, "rb");
	if (NULL == pFileObj->fPs)		return -3;		//open file fail

	fseek(pFileObj->fPs, 0, SEEK_END);
	pFileObj->filesize = ftell(pFileObj->fPs);
	fseek(pFileObj->fPs, 0, SEEK_SET);

	if (pFileObj->filesize < 1)
	{
		fclose(pFileObj->fPs);
		pFileObj->fPs = NULL;
		return -4;		//data is empty
	}

	if (pFileObj->demuxBuf.bufsize < 1)
	{
		BUFF_MALLOC(&pFileObj->demuxBuf, 1024*1024*2);
	}
	BUFF_CLEAR(&pFileObj->demuxBuf);

	if (NULL == pFileObj->frame.pbuf)
	{
		BUFF_MALLOC(&pFileObj->frame, 1024*1024*2);
	}
	BUFF_CLEAR(&pFileObj->frame);

	return 0;
}

int		PSDemux_CloseFile(PSDEMUX_HANDLE handle)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;

	if (NULL == pFileObj)		return -1;
	if (NULL != pFileObj->fPs)
	{
		fclose(pFileObj->fPs);
		pFileObj->fPs = NULL;
	}
	pFileObj->filesize = 0;

	BUFF_FREE(&pFileObj->demuxBuf);
	BUFF_FREE(&pFileObj->frame);

	return 0;
}

int		PSDemux_GotoFileHead(PSDEMUX_HANDLE handle)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;

	if (NULL == pFileObj)		return -1;
	if (NULL == pFileObj->fPs)	return -1;
	
	fseek(pFileObj->fPs, 0x00, SEEK_SET);
	return 0;
}

//跳转至指定位置
int		PSDemux_GotoFileOffset(PSDEMUX_HANDLE handle, unsigned int offset)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;

	if (NULL == pFileObj)		return -1;
	if (NULL == pFileObj->fPs)	return -1;
	
	fseek(pFileObj->fPs, offset, SEEK_SET);
	return 0;
}

int		PSDemux_GetMediaInfo(PSDEMUX_HANDLE handle, unsigned char *videoinfo, int *videoinfolength, unsigned char *audioinfo, int *audioinfolength)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;

	int	ret = -1;
	int mediatype = 0;
	unsigned char reserveddata[256];
	int reservedsize = 0;
	int getMediaType = 0;
	int readFrameNum = 0;

	if (NULL == pFileObj)		return -1;
	if (NULL == pFileObj->fPs)	return -1;
	
	do
	{
		int getRet = PSDemux_GetNextFrame(handle, &mediatype, NULL, NULL, reserveddata, &reservedsize);
		if (getRet < 0)	break;

		readFrameNum ++;

		if (mediatype == MEDIA_TYPE_VIDEO)
		{
			if (NULL != videoinfo)			memcpy(videoinfo, reserveddata, reservedsize);
			if (NULL != videoinfolength)	*videoinfolength = reservedsize;

			memset(reserveddata, 0x00, sizeof(reserveddata));
			reservedsize = 0;

			getMediaType |= 0x00000001;
		}
		else if (mediatype == MEDIA_TYPE_AUDIO)
		{
			if (NULL != audioinfo)			memcpy(audioinfo, reserveddata, reservedsize);
			if (NULL != audioinfolength)	*audioinfolength = reservedsize;

			memset(reserveddata, 0x00, sizeof(reserveddata));
			reservedsize = 0;

			getMediaType |= 0x00000002;
		}

		if ( ( (getMediaType & 0x00000001) && (getMediaType & 0x00000002) ) || (readFrameNum>30) )
		{
			ret = 0;

			PSDemux_GotoFileHead(handle);

			break;
		}
	}while (1);

	return ret;
}

int		PSDemux_GetNextFrame(PSDEMUX_HANDLE handle, int *mediatype, int *framesize, char **framedata, unsigned char *reserveddata, int *reservedsize)
{
	int nReadBytes = 0;
	int PSPackHeaderSize = 14;
	int PS_ReadBytes = 1024;
	int nMediaType = 0;
	int i=0;
	int	getFrameResult=0;

	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;
	if (NULL == pFileObj)		return -1;
	if (NULL == pFileObj->fPs)	return -1;
	if (feof(pFileObj->fPs))			return -1000;			//end


	BUFF_CLEAR(&pFileObj->frame);

	do
	{
		memset(pFileObj->demuxBuf.pbuf, 0x00, pFileObj->demuxBuf.bufsize);
		nReadBytes = fread(pFileObj->demuxBuf.pbuf, 1, PS_ReadBytes, pFileObj->fPs);
		if (nReadBytes != PS_ReadBytes)
		{
			if (nReadBytes < 1)				break;

			//_TRACE(TRACE_LOG_DEBUG, "读取字节: %d / %d\n", nReadBytes, PS_ReadBytes);
			if (feof(pFileObj->fPs))	getFrameResult	=	-1000;
			else						getFrameResult	=	-10;
			//break;
		}

		for (i=0; i<nReadBytes-4; i++)
		{
			if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBA))
			{
				////_TRACE(TRACE_LOG_DEBUG, "-------------------------------------\n");
				////_TRACE(TRACE_LOG_DEBUG, "Pack Header 0x000001BA\n");

				if (pFileObj->frame.bufpos > 0)
				{
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					getFrameResult = 0x01;

					fseek(pFileObj->fPs, -(nReadBytes-i), SEEK_CUR);

					break;
				}
				else
				{
					i += 14;
					i -= 1;
				}

			}
			else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBB))
			{
				//_TRACE(TRACE_LOG_DEBUG, "System Header 0x000001BB\n");
			}
			else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBC))
			{
				//_TRACE(TRACE_LOG_DEBUG, "Map Header 0x000001BC\n");

				i+= 24;
				i-=1;
			}
			else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xE0) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xEF) )		//VIDEO
			{
				int ext_size = 0x00;
				int pes_length = 0;
				int remainReadBytes = 0;
				int ii=0;
				int PES_extension_flag = 0;

				pes_length = (((unsigned char)pFileObj->demuxBuf.pbuf[i+4]) << 8) | ((unsigned char)pFileObj->demuxBuf.pbuf[i+5]);
				PES_extension_flag = (unsigned char)pFileObj->demuxBuf.pbuf[i+7] & 0x01;
				ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

				if ( (pes_length < 10) || (PES_extension_flag == 0x01 && ext_size<=10) )
				{
					break;
					//return -1;
				}

				if (PES_extension_flag == 0x01)
				{
					if (NULL != reserveddata)		memcpy(reserveddata, pFileObj->demuxBuf.pbuf+i+9+10, ext_size-10);
					if (NULL != reservedsize)		*reservedsize = ext_size - 10;
				}
				////_TRACE(TRACE_LOG_DEBUG, "PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3], (unsigned char)pFileObj->demuxBuf.pbuf[i+4], (unsigned char)pFileObj->demuxBuf.pbuf[i+5], pes_length);

#ifdef _DEBUG1
				//_TRACE(TRACE_LOG_DEBUG, "[VIDEO]: ");
				for (ii=0; ii<25; ii++)
				{
					//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
				}
				//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif

				if (pes_length+i < nReadBytes)
				{
					//fwrite(pFileObj->demuxBuf.pbuf+i+8+1+ext_size, 1, pes_length-3-ext_size, fES);
					//fflush(fES);

					if (NULL != pFileObj->frame.pbuf)
					{
						memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+1+ext_size, pes_length-3-ext_size);
						pFileObj->frame.bufpos += (pes_length-3-ext_size);
#if 1
						if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
						if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
						if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;


						if (pFileObj->frame.bufpos > 0)
						{
							if (getFrameResult ==0x00)
							{
								int nn1 = ftell(pFileObj->fPs), nn2=0;
								fseek(pFileObj->fPs, -(nReadBytes-(i+pes_length+4+2)), SEEK_CUR);
								nn2 = ftell(pFileObj->fPs);
								//_TRACE(TRACE_LOG_DEBUG, "nn1: %d\t\tnn2: %d\n", nn1, nn2);
							}
							getFrameResult = 0x01;
							break;
						}
#endif
					}

					i += (pes_length+4+2);	//4字节header, 2字节pes length
					i-= 1;	//此处-1, 因为for循环后面会+1
				}
				else if (pes_length+i > nReadBytes)
				{
					int need_bytes = pes_length - (nReadBytes - i-6);
					int ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

					fread(pFileObj->demuxBuf.pbuf+nReadBytes, 1, need_bytes, pFileObj->fPs);

					if (NULL != pFileObj->frame.pbuf)
					{
						memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+ext_size+1, pes_length-3-ext_size);
						pFileObj->frame.bufpos += (pes_length-3-ext_size);
					}

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
					//if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF)
					if ( pes_length < 1024)
					{
						////_TRACE("一帧结束.\n");
						if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
						if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

						getFrameResult = 0x01;
					}
					break;
				}
			}
			else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xC0) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xDF) )		//AUDIO
			{
				int ext_size = 0x00;
				int pes_length = 0;
				int remainReadBytes = 0;
				int ii=0;
				int PES_extension_flag = 0;

				pes_length = (((unsigned char)pFileObj->demuxBuf.pbuf[i+4]) << 8) | ((unsigned char)pFileObj->demuxBuf.pbuf[i+5]);
				PES_extension_flag = (unsigned char)pFileObj->demuxBuf.pbuf[i+7] & 0x01;
				ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

				if ( (pes_length < 10) || (PES_extension_flag == 0x01 && ext_size<=10) )
				{
					//return -1;
					break;
				}

				if (PES_extension_flag == 0x01)
				{
					if (NULL != reserveddata)		memcpy(reserveddata, pFileObj->demuxBuf.pbuf+i+9+10, ext_size-10);
					if (NULL != reservedsize)		*reservedsize = ext_size - 10;
				}

				////_TRACE(TRACE_LOG_DEBUG, "PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3], (unsigned char)pFileObj->demuxBuf.pbuf[i+4], (unsigned char)pFileObj->demuxBuf.pbuf[i+5], pes_length);
				
#ifdef _DEBUG1
				//_TRACE(TRACE_LOG_DEBUG, "[AUDIO]: ");
				for (ii=0; ii<25; ii++)
				{
					//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
				}
				//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif
				if (pes_length+i < nReadBytes)
				{
					if (NULL != pFileObj->frame.pbuf)
					{
						memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+1+ext_size, pes_length-3-ext_size);
						pFileObj->frame.bufpos += (pes_length-3-ext_size);

#if 1
						if (NULL != mediatype)	*mediatype = MEDIA_TYPE_AUDIO;
						if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
						if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

						if (pFileObj->frame.bufpos > 0)
						{
							if (getFrameResult ==0x00)
							{
								//int nn1 = ftell(pFileObj->fPs), nn2=0;
								//fseek(pFileObj->fPs, -(nReadBytes-(i+8+ext_size+pFileObj->frame.bufpos+1)), SEEK_CUR);
								fseek(pFileObj->fPs, -(nReadBytes-(pes_length+i+4+2)), SEEK_CUR);
								//nn2 = ftell(pFileObj->fPs);
								////_TRACE(TRACE_LOG_DEBUG, "nn1: %d\t\tnn2: %d\n", nn1, nn2);
							}
							getFrameResult = 0x01;
							break;
						}
#endif
					}
					i += (pes_length+4+2);	//4字节header, 2字节pes length
					i-= 1;	//此处-1, 因为for循环后面会+1
				}
				else if (pes_length+i > nReadBytes)
				{
					int need_bytes = pes_length - (nReadBytes - i-6);
					int ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

					fread(pFileObj->demuxBuf.pbuf+nReadBytes, 1, need_bytes, pFileObj->fPs);

					if (NULL != pFileObj->frame.pbuf)
					{
						memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+ext_size+1, pes_length-3-ext_size);
						pFileObj->frame.bufpos += (pes_length-3-ext_size);
					}

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_AUDIO;
					//if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF)
					if ( pes_length < 1024)
					{
						////_TRACE("一帧结束.\n");
						if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
						if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

						getFrameResult = 0x01;
					}
					break;
				}
			}
			else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				  ((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) )		//unknown
			{
#ifdef _DEBUG
				int ii=0;
				//_TRACE(TRACE_LOG_DEBUG, "###############[UNKNOWN]: ");
				for (ii=0; ii<25; ii++)
				{
					//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
				}
				//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif
			}
			else
			{
				int isEmpty = 0x01;
				int	iIdx=0;
				for (iIdx=0; iIdx<10; iIdx++)
				{
					if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+iIdx]  != 0x00)
					{
						isEmpty = 0x00;
						break;
					}
				}

				if (isEmpty == 0x01)
				{
					getFrameResult = -100;
					break;
				}

#ifdef _DEBUG
				{
					int ii=0;

					int nn1 = ftell(pFileObj->fPs), nn2=0;
				
					//nn2 = ftell(pFileObj->fPs);
					////_TRACE(TRACE_LOG_DEBUG, "nn1: %d\t\tnn2: %d\n", nn1, nn2);


					//_TRACE(TRACE_LOG_DEBUG, "#######[OFFSET: %d]########[UNKNOWN]: ", nn1);
					for (ii=0; ii<25; ii++)
					{
						//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
					}
					//_TRACE(TRACE_LOG_DEBUG, "\n");
				}
#endif
			}
		}

		if (getFrameResult == 1)	break;
		else if (getFrameResult == -100)		break;

	}while (1);

	return getFrameResult==1?0:getFrameResult;
}



int		PSDemux_demux(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata)
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;
	int i=0;
	int	getFrameResult=0;
	if (NULL == pFileObj->demuxBuf.pbuf)		return -1;

	memcpy(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, pbuf, bufsize);
	pFileObj->demuxBuf.bufpos += bufsize;

	if (pFileObj->clear == 0x01)
	{
		pFileObj->clear = 0x00;
		BUFF_CLEAR(&pFileObj->frame);
	}

	for (i=0; i<pFileObj->demuxBuf.bufpos-4; i++)
	{
		if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBA))
		{
			//_TRACE(TRACE_LOG_DEBUG, "-------------------------------------\n");
			//_TRACE(TRACE_LOG_DEBUG, "Pack Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBB))
		{
			//_TRACE(TRACE_LOG_DEBUG, "System Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBC))
		{
			//_TRACE(TRACE_LOG_DEBUG, "Map Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xE0) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xEF) )		//VIDEO
		{
			int ext_size = 0x00;
			int pes_length = 0;
			int remainReadBytes = 0;
			int ii=0;

			pes_length = (((unsigned char)pFileObj->demuxBuf.pbuf[i+4]) << 8) | ((unsigned char)pFileObj->demuxBuf.pbuf[i+5]);
			ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

			////_TRACE(TRACE_LOG_DEBUG, "PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3], (unsigned char)pFileObj->demuxBuf.pbuf[i+4], (unsigned char)pFileObj->demuxBuf.pbuf[i+5], pes_length);
#ifdef _DEBUG
			//_TRACE(TRACE_LOG_DEBUG, "[VIDEO]: ");
			for (ii=0; ii<25; ii++)
			{
				//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
			}
			//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif

			if (pes_length+i < (pFileObj->demuxBuf.bufpos-i))
			{
				if (NULL != pFileObj->frame.pbuf)
				{
					memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+1+ext_size, pes_length-3-ext_size);
					pFileObj->frame.bufpos += (pes_length-3-ext_size);
				}

				//if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF && pFileObj->frame.bufpos>128)
				if ( pes_length < 1024  && pFileObj->frame.bufpos>128)
				{
					////_TRACE("一帧结束.\n");
					int remain_bytes = 0;

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					pFileObj->clear = 0x01;
					getFrameResult = 0x01;

					//_TRACE(TRACE_LOG_DEBUG, "[VIDEO] framesize[%d]\n", pFileObj->frame.bufpos);
					for (ii=0; ii<25; ii++)
					{
						//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->frame.pbuf[ii]);
					}
					//_TRACE(TRACE_LOG_DEBUG, "\t\t后25字节: ");
					for (ii=24; ii>=0; ii--)
					{
						//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->frame.pbuf[pFileObj->frame.bufpos-ii]);
					}
					//_TRACE(TRACE_LOG_DEBUG, "\n");


					//remain_bytes = pFileObj->demuxBuf.bufpos-(pes_length-3-ext_size);
					remain_bytes = pFileObj->demuxBuf.bufpos-i-(pes_length-3-ext_size);
					//memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i+8+1+ext_size+(pes_length-3-ext_size), remain_bytes);
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i+8+1+ext_size+(pes_length-3-ext_size), remain_bytes);
					pFileObj->demuxBuf.bufpos = remain_bytes;//(pes_length-3-ext_size);
					memset(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, 0x00, pFileObj->demuxBuf.bufsize-pFileObj->demuxBuf.bufpos);
					

					//_TRACE(TRACE_LOG_DEBUG, "处理后数据[%d]: ", pFileObj->demuxBuf.bufpos);
					for (ii=0; ii<25; ii++)
					{
						//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[ii]);
					}
					//_TRACE(TRACE_LOG_DEBUG, "\t\t后25字节: ");
					for (ii=24; ii>=0; ii--)
					{
						//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[pFileObj->demuxBuf.bufpos-ii]);
					}
					//_TRACE(TRACE_LOG_DEBUG, "\n");
				}

				if (pFileObj->demuxBuf.bufpos < pes_length)	break;

				i += (pes_length+4+2);	//4字节header, 2字节pes length
				i -= 1;	//此处-1, 因为for循环后面会+1
			}
			else if (pes_length+i > (pFileObj->demuxBuf.bufpos-i))
			{
				if (i > 0)
				{
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i, pFileObj->demuxBuf.bufpos-i);
					pFileObj->demuxBuf.bufpos -= i;
				}
				break;

/*
				int need_bytes = pes_length - (nReadBytes - i-6);
				int ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

				fread(pFileObj->demuxBuf.pbuf+nReadBytes, 1, need_bytes, pFileObj->fPs);

				//fwrite(pFileObj->demuxBuf.pbuf+i+8+ext_size+1, 1, pes_length-3-ext_size, fES);
				//fflush(fES);

				if (NULL != pFileObj->frame.pbuf)
				{
					memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+ext_size+1, pes_length-3-ext_size);
					pFileObj->frame.bufpos += (pes_length-3-ext_size);
				}

				if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF)
				{
					////_TRACE("一帧结束.\n");

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					getFrameResult = 0x01;
				}
				break;
*/
			}




		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xC0) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xDF) )		//AUDIO
		{


		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) )		//unknown
		{
#ifdef _DEBUG
			int ii=0;
			//_TRACE(TRACE_LOG_DEBUG, "###############[UNKNOWN][%d]: ", pFileObj->demuxBuf.bufpos);
			for (ii=0; ii<25; ii++)
			{
				//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
			}
			//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif
		}
	}

	return getFrameResult==1?0:-1;
}



int		PSDemux_demux2(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata)
{

	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;
	int i=0;
	int	getFrameResult=0;
	if (NULL == pFileObj->demuxBuf.pbuf)		return -1;

	memcpy(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, pbuf, bufsize);
	pFileObj->demuxBuf.bufpos += bufsize;

	////_TRACE("bufpos: %d\n", pFileObj->demuxBuf.bufpos);

	if (pFileObj->clear == 0x01)
	{
		pFileObj->clear = 0x00;
		BUFF_CLEAR(&pFileObj->frame);
	}

	for (i=0; i<pFileObj->demuxBuf.bufpos-4; i++)
	{
		if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBA))
		{
			//_TRACE(TRACE_LOG_DEBUG, "-------------------------------------\n");
			//_TRACE(TRACE_LOG_DEBUG, "Pack Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBB))
		{
			//_TRACE(TRACE_LOG_DEBUG, "System Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+3]==0xBC))
		{
			//_TRACE(TRACE_LOG_DEBUG, "Map Header 0x000001%02X\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3]);
		}
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xE0) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xEF) )		//VIDEO
		{
			int ext_size = 0x00;
			int pes_length = 0;
			int remainReadBytes = 0;
			//int ii=0;

			pes_length = (((unsigned char)pFileObj->demuxBuf.pbuf[i+4]) << 8) | ((unsigned char)pFileObj->demuxBuf.pbuf[i+5]);
			ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

			////_TRACE(TRACE_LOG_DEBUG, "PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3], (unsigned char)pFileObj->demuxBuf.pbuf[i+4], (unsigned char)pFileObj->demuxBuf.pbuf[i+5], pes_length);
				
			if (pes_length+6+i < (pFileObj->demuxBuf.bufpos-i))
			{
				if (NULL != pFileObj->frame.pbuf)
				{
					memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+1+ext_size, pes_length-3-ext_size);
					pFileObj->frame.bufpos += (pes_length-3-ext_size);
				}

				//if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF && pFileObj->frame.bufpos>128)
				 if (pes_length < 1024 && pFileObj->frame.bufpos>128 )
				{
					////_TRACE("一帧结束.\n");
					int remain_bytes = 0;

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					pFileObj->clear = 0x01;
					getFrameResult = 0x01;

					////_TRACE(TRACE_LOG_DEBUG, "[VIDEO] framesize[%d]\n", pFileObj->frame.bufpos);

					remain_bytes = pFileObj->demuxBuf.bufpos-(i+8+1+ext_size+pes_length-3-ext_size);
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i+8+1+ext_size+(pes_length-3-ext_size), remain_bytes);
					pFileObj->demuxBuf.bufpos = remain_bytes;//(pes_length-3-ext_size);
					memset(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, 0x00, pFileObj->demuxBuf.bufsize-pFileObj->demuxBuf.bufpos);
					
#if 0
					//_TRACE("处理后数据[%d]: ", pFileObj->demuxBuf.bufpos);
					for (ii=0; ii<25; ii++)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[ii]);
					}
					//_TRACE("\t\t后25字节: ");
					for (ii=24; ii>=0; ii--)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[pFileObj->demuxBuf.bufpos-ii]);
					}
					//_TRACE("\n");
#endif
				}

				if (pFileObj->demuxBuf.bufpos < pes_length)	break;

				i += (pes_length+4+2);	//4字节header, 2字节pes length
				i -= 1;	//此处-1, 因为for循环后面会+1
			}
			else// if (pes_length+i > (pFileObj->demuxBuf.bufpos-i))
			{
				if (i > 0)
				{
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i, pFileObj->demuxBuf.bufpos-i);
					pFileObj->demuxBuf.bufpos -= i;

					memset(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, 0x00, pFileObj->demuxBuf.bufsize-pFileObj->demuxBuf.bufpos);
				}
				break;
			}
		}
		//=========================================================================================================================
		//=========================================================================================================================
		//=========================================================================================================================
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) && 
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]>=0xC0) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+3]<=0xDF) )		//AUDIO
		{
			int ext_size = 0x00;
			int pes_length = 0;
			int remainReadBytes = 0;
			int ii=0;

			pes_length = (((unsigned char)pFileObj->demuxBuf.pbuf[i+4]) << 8) | ((unsigned char)pFileObj->demuxBuf.pbuf[i+5]);
			ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

			////_TRACE("PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pFileObj->demuxBuf.pbuf[i+3], (unsigned char)pFileObj->demuxBuf.pbuf[i+4], (unsigned char)pFileObj->demuxBuf.pbuf[i+5], pes_length);
			
			if (pes_length+6+i < (pFileObj->demuxBuf.bufpos-i))
			{
				//fwrite(pFileObj->demuxBuf.pbuf+i+8+1+ext_size, 1, pes_length-3-ext_size, fES);
				//fflush(fES);

				if (NULL != pFileObj->frame.pbuf)
				{
					memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+1+ext_size, pes_length-3-ext_size);
					pFileObj->frame.bufpos += (pes_length-3-ext_size);
				}

				//if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF && pFileObj->frame.bufpos>128)
				if ( pes_length < 1024 && pFileObj->frame.bufpos>128)
				{
					////_TRACE("一帧结束.\n");
					int remain_bytes = 0;

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_AUDIO;
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					pFileObj->clear = 0x01;
					getFrameResult = 0x01;

					//_TRACE(TRACE_LOG_DEBUG, "[AUDIO] framesize[%d]\n", pFileObj->frame.bufpos);
#if 0
					for (ii=0; ii<25; ii++)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->frame.pbuf[ii]);
					}
					//_TRACE("\t\t后25字节: ");
					for (ii=24; ii>=0; ii--)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->frame.pbuf[pFileObj->frame.bufpos-ii]);
					}
					//_TRACE("\n");
#endif
					remain_bytes = pFileObj->demuxBuf.bufpos-(i+8+1+ext_size+pes_length-3-ext_size);
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i+8+1+ext_size+(pes_length-3-ext_size), remain_bytes);
					pFileObj->demuxBuf.bufpos = remain_bytes;//(pes_length-3-ext_size);
					memset(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, 0x00, pFileObj->demuxBuf.bufsize-pFileObj->demuxBuf.bufpos);
#if 0			
					//_TRACE("处理后数据[%d]: ", pFileObj->demuxBuf.bufpos);
					for (ii=0; ii<25; ii++)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[ii]);
					}
					//_TRACE("\t\t后25字节: ");
					for (ii=24; ii>=0; ii--)
					{
						//_TRACE("%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[pFileObj->demuxBuf.bufpos-ii]);
					}
					//_TRACE("\n");
#endif
				}

				if (pFileObj->demuxBuf.bufpos < pes_length)	break;

				i += (pes_length+4+2);	//4字节header, 2字节pes length
				i -= 1;	//此处-1, 因为for循环后面会+1
			}
			else
			{
				if (i > 0)
				{
					memmove(pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.pbuf+i, pFileObj->demuxBuf.bufpos-i);
					pFileObj->demuxBuf.bufpos -= i;

					memset(pFileObj->demuxBuf.pbuf+pFileObj->demuxBuf.bufpos, 0x00, pFileObj->demuxBuf.bufsize-pFileObj->demuxBuf.bufpos);
				}
				break;

/*
				int need_bytes = pes_length - (nReadBytes - i-6);
				int ext_size = (unsigned char)pFileObj->demuxBuf.pbuf[i+8];

				fread(pFileObj->demuxBuf.pbuf+nReadBytes, 1, need_bytes, pFileObj->fPs);

				//fwrite(pFileObj->demuxBuf.pbuf+i+8+ext_size+1, 1, pes_length-3-ext_size, fES);
				//fflush(fES);

				if (NULL != pFileObj->frame.pbuf)
				{
					memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pFileObj->demuxBuf.pbuf+i+8+ext_size+1, pes_length-3-ext_size);
					pFileObj->frame.bufpos += (pes_length-3-ext_size);
				}

				if ( (unsigned char)pFileObj->demuxBuf.pbuf[i+4] < 0xFF)
				{
					////_TRACE("一帧结束.\n");

					if (NULL != mediatype)	*mediatype = MEDIA_TYPE_VIDEO;
					if (NULL != framesize)	*framesize = pFileObj->frame.bufpos;
					if (NULL != framedata)	*framedata = pFileObj->frame.pbuf;

					getFrameResult = 0x01;
				}
				break;
*/
			}
		}
		//=========================================================================================================================
		//=========================================================================================================================
		//=========================================================================================================================
		else if (  ((unsigned char)pFileObj->demuxBuf.pbuf[i]==0x00) && ( (unsigned char)pFileObj->demuxBuf.pbuf[i+1]==0x00) &&
				((unsigned char)pFileObj->demuxBuf.pbuf[i+2]==0x01) )		//unknown
		{
#ifdef _DEBUG
			int ii=0;
			//_TRACE(TRACE_LOG_DEBUG, "###############[UNKNOWN][%d]: ", pFileObj->demuxBuf.bufpos);
			for (ii=0; ii<25; ii++)
			{
				//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pFileObj->demuxBuf.pbuf[i+ii]);
			}
			//_TRACE(TRACE_LOG_DEBUG, "\n");
#endif
		}
	}

	return getFrameResult==1?0:-1;
}




int		PSDemux_demuxPacket(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *mediatype, int *framesize, char **framedata)		//解析海康PS流包
{
	if (bufsize < 4)
	{
		return -1;
	}

	int ret = -1;
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;
	if (pFileObj->demuxBuf.bufpos > 0)
	{
		ret = PSDemux_demuxInternalPacket(handle, pFileObj->demuxBuf.pbuf, pFileObj->demuxBuf.bufpos, mediatype, framesize, framedata);
		BUFF_CLEAR(&pFileObj->demuxBuf);

		if (ret == 0x00)
		{
			BUFF_ENQUEUE(&pFileObj->demuxBuf, pbuf, bufsize);
			return ret;
		}
	}
	ret = PSDemux_demuxInternalPacket(handle, pbuf, bufsize, mediatype, framesize, framedata);

	return ret;

}


int		PSDemux_demuxInternalPacket(PSDEMUX_HANDLE handle, char *pbuf, int bufsize, int *out_mediatype, int *out_framesize, char **out_framedata)		//解析海康PS流包
{
	PS_FILE_OBJ_T *pFileObj = (PS_FILE_OBJ_T *)handle;
	int i=0;
	int	getFrameResult=0;

	if (pFileObj->clear == 0x01)
	{
		pFileObj->clear = 0x00;
		BUFF_CLEAR(&pFileObj->frame);
	}

	for (i=0; i<bufsize-4; i++)
	{
		if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) &&
				((unsigned char)pbuf[i+2]==0x01) && ( (unsigned char)pbuf[i+3]==0xBA))
		{
			////_TRACE(TRACE_LOG_DEBUG, "-------------------------------------\n");
			////_TRACE(TRACE_LOG_DEBUG, "Pack Header 0x000001%02X\n", (unsigned char)pbuf[i+3]);
		}
		else if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) &&
				((unsigned char)pbuf[i+2]==0x01) && ( (unsigned char)pbuf[i+3]==0xBB))
		{
			////_TRACE(TRACE_LOG_DEBUG, "System Header 0x000001%02X\n", (unsigned char)pbuf[i+3]);
		}
		else if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) &&
				((unsigned char)pbuf[i+2]==0x01) && ( (unsigned char)pbuf[i+3]==0xBC))
		{
			////_TRACE(TRACE_LOG_DEBUG, "Map Header 0x000001%02X\n", (unsigned char)pbuf[i+3]);

			int pes_length = (((unsigned char)pbuf[i+4]) << 8) | ((unsigned char)pbuf[i+5]);
			i += (pes_length+4+2);	//4字节header, 2字节pes length

			if (bufsize - i < 0)
			{
				bufsize += 1;
				bufsize -= 1;
			}
			bufsize -= i;

			i -= 1;	//此处-1, 因为for循环后面会+1
		}
		else if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) &&
				((unsigned char)pbuf[i+2]==0x01) && ( (unsigned char)pbuf[i+3]==0xBD))
		{
			////_TRACE(TRACE_LOG_DEBUG, "Header 0x000001%02X\n", (unsigned char)pbuf[i+3]);

			int pes_length = (((unsigned char)pbuf[i+4]) << 8) | ((unsigned char)pbuf[i+5]);
			i += (pes_length+4+2);	//4字节header, 2字节pes length

			if (bufsize - i < 0)
			{
				bufsize += 1;
				bufsize -= 1;
			}
			bufsize -= i;
			
			i -= 1;	//此处-1, 因为for循环后面会+1
		}
		else if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) && ((unsigned char)pbuf[i+2]==0x01) && 
				(( ((unsigned char)pbuf[i+3]>=0xE0) && ((unsigned char)pbuf[i+3]<=0xEF) ) ||		//VIDEO
				 (((unsigned char)pbuf[i+3]>=0xC0) &&  ((unsigned char)pbuf[i+3]<=0xDF) ) ) )		//AUDIO
		{
			int ext_size = 0x00;
			int pes_length = 0;
			int remainReadBytes = 0;
			unsigned char pts_dts_flag = 0;
			unsigned int pts = pFileObj->pts;
			int hour=0, minute=0, second=0, msec=0;
			int	mediaType = MEDIA_TYPE_EVENT;

			if (((unsigned char)pbuf[i+3]>=0xE0) &&	((unsigned char)pbuf[i+3]<=0xEF) )
			{
				mediaType = MEDIA_TYPE_VIDEO;
			}
			else if(((unsigned char)pbuf[i+3]>=0xC0) && ((unsigned char)pbuf[i+3]<=0xDF) )
			{
				mediaType = MEDIA_TYPE_AUDIO;
			}

			pes_length = (((unsigned char)pbuf[i+4]) << 8) | ((unsigned char)pbuf[i+5]);
			pts_dts_flag = (unsigned char)pbuf[i+7];
			ext_size = (unsigned char)pbuf[i+8];

			if (pes_length < 1)
			{
				//_TRACE(TRACE_LOG_DEBUG, "数据长度小于1: %d\n", pes_length);
/*
				FILE *ff = fopen("pes_err.txt", "wb");
				if (NULL != ff)
				{
					fwrite(pbuf, 1, bufsize, ff);
					fclose(ff);
				}
*/
				bufsize = 0;
				break;
			}

			pts_dts_flag = (pts_dts_flag>>6) & 0x03;

			if (pts_dts_flag == 0x02)		//has PTS
			{
				unsigned short pts29_15 = 0, pts14_0=0;
				unsigned int pts32_30 = 0;

				unsigned char pts1 = (unsigned char)pbuf[i+9];
				unsigned char pts2 = (unsigned char)pbuf[i+10];
				unsigned char pts3 = (unsigned char)pbuf[i+11];
				unsigned char pts4 = (unsigned char)pbuf[i+12];
				unsigned char pts5 = (unsigned char)pbuf[i+13];

				pts32_30 = ( pts1 & 0x0E) >> 1;
				pts29_15 = ((pts2<<8) | (pts3 & 0xFE)) >> 1;
				pts14_0 = ((pts4<<8) | (pts5 & 0xFE)) >> 1;
				pts = (pts32_30 << 30) | (pts29_15 << 15) | pts14_0;
				//pts = pts / 90000;

				hour = pts /90000 / 60 / 60;
				minute = pts /90000 / 60 % 60;
				second = (pts /90000) % 60;

#ifdef _DEBUG
				wchar_t wszLog[128] = {0};
				wsprintf(wszLog, TEXT("PS Time: %02d:%02d:%02d\n"), hour, minute, second);
				OutputDebugString(wszLog);
#endif

/*
    第0~3位 ： 0010  默认规定占用四位
    第4~6位： 000     占用3位  PTS[32~30]
    第7     位： 1          占用1位  marker_bit
    第8~22位：0000 0000 0000 000 占用15位 PTS[29..15]
    第23位    ：1         占用1位marker_bit
    第24~38位：1001 0110 0000 011 占用15位 PTS[14..0]
    第39位    ：1         占用1位marker_bit
*/
			}
			else if (pts_dts_flag == 0x03)		//has PTS and DTS
			{
				
			}

			////_TRACE(TRACE_LOG_DEBUG, "pts_dts_flag: %d\text_size: %d   pts: 0x%08X [%u]%u\n", pts_dts_flag, ext_size, pts, pts, pts/90000);
			////_TRACE(TRACE_LOG_DEBUG, "PES Header 0x000001%X  length:  0x%02X%02X\t\t%d\n", (unsigned char)pbuf[i+3], (unsigned char)pbuf[i+4], (unsigned char)pbuf[i+5], pes_length);

			if (pes_length+6+i <= (bufsize-i))
			//if (pes_length+6 <= (bufsize-i))
			{
				if (pFileObj->pts != pts)// && pts>0U)	//下一帧
				{
					if (pFileObj->frame.bufpos > 0)
					{
						//返回上一帧数据

						if (NULL != out_mediatype)	*out_mediatype = pFileObj->mediaType;
						if (NULL != out_framesize)	*out_framesize = pFileObj->frame.bufpos;
						if (NULL != out_framedata)	*out_framedata = pFileObj->frame.pbuf;
						getFrameResult = 0x01;
						pFileObj->clear = 0x01;
					}

					BUFF_CLEAR(&pFileObj->demuxBuf);
					BUFF_ENQUEUE(&pFileObj->demuxBuf, pbuf, bufsize);
					pFileObj->mediaType = mediaType;	//更新当前媒体类型
					pFileObj->pts = pts;
					break;
				}
				else
				{
					if (NULL != pFileObj->frame.pbuf)
					{
						int bufpos = pFileObj->frame.bufpos;
						memcpy(pFileObj->frame.pbuf+pFileObj->frame.bufpos, pbuf+i+8+1+ext_size, pes_length-3-ext_size);
						pFileObj->frame.bufpos += (pes_length-3-ext_size);

						////_TRACE(TRACE_LOG_DEBUG, "pFileObj->frame.bufpos:   %d + %d = %d   drop bytes:%d\n",		\
							bufpos, (pes_length-3-ext_size), pFileObj->frame.bufpos,	\
							bufsize-((pes_length-3-ext_size)));


						if (pFileObj->mediaType==MEDIA_TYPE_AUDIO)
						{
							if (NULL != out_mediatype)	*out_mediatype = pFileObj->mediaType;
							if (NULL != out_framesize)	*out_framesize = pFileObj->frame.bufpos;
							if (NULL != out_framedata)	*out_framedata = pFileObj->frame.pbuf;
							getFrameResult = 0x01;
							pFileObj->clear = 0x01;
							pFileObj->mediaType = mediaType;	//更新当前媒体类型
							pFileObj->pts = pts;
							BUFF_CLEAR(&pFileObj->demuxBuf);
							break;
						}

					}
				}
				if (bufsize < pes_length && getFrameResult==0x00)	break;

				i += (pes_length+4+2);	//4字节header, 2字节pes length
				i -= 1;	//此处-1, 因为for循环后面会+1
			}
			else// if (pes_length+i > (bufsize-i))
			{
				break;
			}
		}
		//=========================================================================================================================
		//=========================================================================================================================
		//=========================================================================================================================
		else if (  ((unsigned char)pbuf[i]==0x00) && ( (unsigned char)pbuf[i+1]==0x00) &&
				((unsigned char)pbuf[i+2]==0x01) )		//unknown
		{
#ifdef _DEBUG
			int ii=0;
			//_TRACE(TRACE_LOG_DEBUG, "###############[UNKNOWN][%d]: ", bufsize);
			for (ii=0; ii<25; ii++)
			{
				//_TRACE(TRACE_LOG_DEBUG, "%02X ", (unsigned char)pbuf[i+ii]);
			}
			//_TRACE(TRACE_LOG_DEBUG, "\n");

			static int iPacketNo = 0;
			char sztmp[128] = {0};
			sprintf(sztmp, "packet\\%d.txt", iPacketNo++);
			FILE *fp = fopen(sztmp, "wb");
			if (NULL != fp)
			{
				fwrite(pbuf, 1, bufsize, fp);
				fclose(fp);
			}

#endif
		}
	}

	return getFrameResult==1?0:-1;
}
