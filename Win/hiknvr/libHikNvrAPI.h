#ifndef __LIB_HIK_NVR_API_H__
#define __LIB_HIK_NVR_API_H__



#define WIN32_LEAN_AND_MEAN
#define LIBHIKNVR_API __declspec(dllexport)
#include <windows.h>


typedef struct __DEVICE_TIME_T
{
    unsigned long ulYear;		//年
    unsigned long ulMonth;		//月
    unsigned long ulDay;		//日
    unsigned long ulHour;		//时
    unsigned long ulMinute;		//分
    unsigned long ulSecond;		//秒
}DEVICE_TIME_T;


typedef struct __NVR_CHANNEL_INFO_T
{
	//int		enable;
	char	name[36];
	char	ip[16];
	int		port;
}NVR_CHANNEL_INFO_T;

typedef void *HIKNVR_HANDLE;			//海康NVR句柄


typedef void(CALLBACK *HIKNVR_CallBack) (LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void *pUser);

#ifdef __cplusplus
extern "C"
{
#endif

	//初始化
	int LIBHIKNVR_API	libHIKNVR_Init(HIKNVR_HANDLE *handle);
	//反初始化
	int LIBHIKNVR_API	libHIKNVR_Deinit(HIKNVR_HANDLE *handle);

	//登录
	int LIBHIKNVR_API	libHIKNVR_Login(HIKNVR_HANDLE handle, char *ipaddr, int port, char *username, char *password);
	//登出
	int LIBHIKNVR_API	libHIKNVR_Logout(HIKNVR_HANDLE handle);

	//获取所有通道信息
	int LIBHIKNVR_API	libHIKNVR_GetChannelList(HIKNVR_HANDLE handle, int *channelNum, NVR_CHANNEL_INFO_T **pChannelInfo);
	//根据通道ID获取相应通道信息
	int LIBHIKNVR_API	libHIKNVR_GetChannelInfoById(HIKNVR_HANDLE handle, const int channelid, NVR_CHANNEL_INFO_T *pChannelInfo);
	//根据相机IP获取对应的通道ID
	int LIBHIKNVR_API	libHIKNVR_GetChannelIdByCameraIP(HIKNVR_HANDLE handle, const char *cameraIP, int *channelId);

	//根据年月获取指定通道的录像日期
	int LIBHIKNVR_API	libHIKNVR_GetRecordingDateByYearMonth(HIKNVR_HANDLE handle, const int channelid, const int year, const int month, unsigned char pDate[]);
	//根据日期和通道ID, 获取相应的录像时间列表
	int LIBHIKNVR_API	libHIKNVR_GetRecordingTimeByChannelId(HIKNVR_HANDLE handle, const int channelid, const int iYear, const int iMonth, const int iDay, unsigned char *_timelist);


	//开始实时流
	long LIBHIKNVR_API	libHIKNVR_StartRealStream(HIKNVR_HANDLE handle, const int channelId, int streamType, HIKNVR_CallBack callback, void *userptr);
	//停止实时流
	int	LIBHIKNVR_API	libHIKNVR_StopRealStream(HIKNVR_HANDLE handle, long realStreamHandle);



	//----------------------------------------------------------------
	//开始回放, 返回回放句柄
	long LIBHIKNVR_API	libHIKNVR_StartPlayback(HIKNVR_HANDLE handle, const int channelid, DEVICE_TIME_T *startTime, DEVICE_TIME_T *endTime, HWND hWnd);
	//暂停回放
	int LIBHIKNVR_API	libHIKNVR_PausePlayback(HIKNVR_HANDLE handle, long playbackHandle);
	//继续回放
	int LIBHIKNVR_API	libHIKNVR_ResumePlayback(HIKNVR_HANDLE handle, long playbackHandle);
	//停止回放
	int LIBHIKNVR_API	libHIKNVR_StopPlayback(HIKNVR_HANDLE handle, long playbackHandle);


	//开始回放流, 返回回放句柄
	long LIBHIKNVR_API	libHIKNVR_StartPlaybackStream(HIKNVR_HANDLE handle, const int channelid, DEVICE_TIME_T *startTime, DEVICE_TIME_T *endTime, 
																			HIKNVR_CallBack callback, void *userptr);
	//关闭回放流
	int LIBHIKNVR_API	libHIKNVR_StopPlaybackStream(HIKNVR_HANDLE handle, long playbackHandle);



#ifdef __cplusplus
}
#endif


#endif
