
// ImageProcessDlg.h : header file
#pragma once


// CImageProcessDlg dialog
class CImageProcessDlg : public CDialogEx
{
// Construction
public:
	CImageProcessDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMAGEPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	UINT m_Hight = 0;
	UINT m_Width = 0;
	Mat SourceImage;
	Mat ProcessImage;
	afx_msg void selfAdaptiveMedianBlur(Mat&src, Mat&dst );
	afx_msg void OnBnClickedOpenimage();
	afx_msg void MyGlobalEqualizeHist(Mat & Image );
	afx_msg void MyLocalEqualizeHist(Mat& Image, int width = 3);
	afx_msg void MyMedeanfilter(Mat & Image);
	afx_msg void DrawMat(cv::Mat & img, UINT nID);
	afx_msg void OnBnClickedEqualizehist();
	afx_msg void OnBnClickedMedeanfilter();
	afx_msg void OnBnClickedLocalEqualizehist();
	afx_msg void OnBnClickedHistStatEnhance();
	afx_msg void HistStatEnhance(Mat& src, Mat& dst);
	afx_msg void OnBnClickedNorchfilter();
	afx_msg void DFT(Mat & src, Mat & dst);
	afx_msg void NorchFiltering(Mat& src, Mat& dst);
};
