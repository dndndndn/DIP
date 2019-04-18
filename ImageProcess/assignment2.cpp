// assignment2.cpp: 实现文件
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "ImageProcessDlg.h"
#include "assignment2.h"
#include "afxdialogex.h"


// assignment2 对话框

IMPLEMENT_DYNAMIC(assignment2, CDialogEx)

assignment2::assignment2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

assignment2::~assignment2()
{

	m_pFather = (CImageProcessDlg*)GetParent()->GetParent();
	m_Hight = m_pFather->m_Hight;
	m_Width = m_pFather->m_Width;
	SourceImage= m_pFather->SourceImage;
	ProcessImage= m_pFather->ProcessImage;

	CStatic * pCtrl = static_cast<CStatic *>(m_pFather->GetDlgItem(IDC_ProcessImage));
	ID = pCtrl->GetDlgCtrlID();
}

void assignment2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(assignment2, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &assignment2::OnBnClickedButton1)
END_MESSAGE_MAP()


// assignment2 消息处理程序


void assignment2::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pFather->DrawMat(SourceImage, ID);

}
