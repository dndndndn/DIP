#pragma once


// assignment2 对话框

class assignment2 : public CDialogEx
{
	DECLARE_DYNAMIC(assignment2)

public:
	assignment2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~assignment2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_Hight = 0;
	UINT m_Width = 0;
	Mat SourceImage;
	Mat ProcessImage;
	UINT ID;
	class CImageProcessDlg *m_pFather;
	afx_msg void OnBnClickedButton1();
};
