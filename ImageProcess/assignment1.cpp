// assignment1.cpp: 实现文件
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "ImageProcessDlg.h"
#include "assignment1.h"
#include "afxdialogex.h"


// assignment1 对话框
IMPLEMENT_DYNAMIC(assignment1, CDialogEx)

assignment1::assignment1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

assignment1::~assignment1()
{
	
}

BOOL assignment1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void assignment1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(assignment1, CDialogEx)
END_MESSAGE_MAP()


