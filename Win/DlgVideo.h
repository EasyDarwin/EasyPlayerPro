#pragma once

#include "DlgRender.h"
// CDlgVideo 对话框
#define		HIK_NVR_ENABLE		0x00
#if HIK_NVR_ENABLE == 0x01
#include "hiknvr\libHikNvrAPI.h"
#pragma comment(lib, "hiknvr/libHikNVR.lib")
#endif
#include "hiknvr\psdemuxer.h"
#include "hiknvr\sps_pps.h"

#define		DIALOG_BASE_BACKGROUND_COLOR		RGB(0x75,0x75,0x75)
#define		DIALOG_BASE_TEXT_COLOR				RGB(0x20,0x20,0x20)

#define		WM_RECORDING_CMPLETE		(WM_USER+10001)

typedef struct _HIK_NVR_CHANNEL_T
{
	PSDEMUX_HANDLE			psDemuxHandle;
	long					playHandle;
	int						channelId;

	int			width;
	int			height;
}HIK_NVR_CHANNEL_T;

class CDlgVideo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVideo)

public:
	CDlgVideo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVideo();


	void	OpenHikNvrRealStream();
	void	OpenHIKNVR_Playback();
	void	PlayStreamFile();


	void	SetWindowId(int _windowId);
	void	SetShownToScale(int shownToScale);
	void	SetMultiplex(unsigned char multiplex);
	void	SetDecodeType(unsigned char onlyDecodeKeyframe);
	void	SetURL(char *url, int scale, int osd, int tcp, int multiple, int cache, int showToolbar, int autoplay);
	void	OnMouseWheel(short zDelta, CPoint pt);

	void	onVideoData(int channelId, int mediaType, char *pbuf, LIVE_FRAME_INFO *frameInfo);

	bool	bDrag;
	int		shownToScale;

	int		m_WindowId;
	int		m_ChannelId;
	unsigned char	sourceMultiplex;	//源复用
	unsigned char	onlyDecodeKeyFrame;	//仅解码关键帧
	CDlgRender	*pDlgRender;
	CEdit	*pEdtURL;		//IDC_EDIT_RTSP_URL
	CEdit	*pEdtUsername;	//IDC_EDIT_USERNAME
	CEdit	*pEdtPassword;	//IDC_EDIT_PASSWORD
	CButton	*pChkOSD;		//IDC_CHECK_OSD
	CButton *pChkTCP;		//IDC_CHECK_TCP
	CSliderCtrl	*pSliderCache;	//IDC_SLIDER_CACHE
	CButton	*pBtnPreview;	//IDC_BUTTON_PREVIEW
	void	InitialComponents();
	void	CreateComponents();
	void	UpdateComponents();
	void	DeleteComponents();

	HBRUSH	m_BrushBtn;
	HBRUSH	m_BrushEdt;
	HBRUSH	m_BrushStatic;

#if HIK_NVR_ENABLE == 0x01
	HIKNVR_HANDLE			hikNvrHandle;
	HIK_NVR_CHANNEL_T		hikNvrChannel;
#endif

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnRecordingComplete(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonPreview();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedCheckOsd();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
