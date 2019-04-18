
// ImageProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "ImageProcessDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include<vector>
#include<algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//------------------------自适应中值滤波中的宏定义-------------------------
#define CV_ROI_ELEM(src,vector,m,n,ks)  \
{                                      \
    uchar* kn;                         \
    int st0=src.step[0];\
    int st1=src.step[1];\
    for(int k=0;k<(ks);k++)            \
	    {                                  \
        for(int s=0;s<(ks);s++)        \
		        {                              \
            kn =src.data+(k+m)*st0+(s+n)*st1;   \
            vector.push_back(*kn);              \
		        }                                       \
	    }                                           \
}

#define CV_MAT_ELEM2(src,dtype,y,x) \
    (dtype*)(src.data+src.step[0]*(y)+src.step[1]*(x))
//--------------------------------------------------------------------------

// CImageProcessDlg dialog

CImageProcessDlg::CImageProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageProcessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CImageProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageProcessDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OpenImage, &CImageProcessDlg::OnBnClickedOpenimage)
	ON_BN_CLICKED(IDC_GlobalEqualizeHist, &CImageProcessDlg::OnBnClickedEqualizehist)
	ON_BN_CLICKED(IDC_MedeanFilter, &CImageProcessDlg::OnBnClickedMedeanfilter)
	ON_BN_CLICKED(IDC_LocalEqualizeHist, &CImageProcessDlg::OnBnClickedLocalEqualizehist)
	ON_BN_CLICKED(IDC_HistStatEnhance, &CImageProcessDlg::OnBnClickedHistStatEnhance)
	ON_BN_CLICKED(IDC_NorchFilter, &CImageProcessDlg::OnBnClickedNorchfilter)
END_MESSAGE_MAP()


// CImageProcessDlg message handlers

BOOL CImageProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString str;
	str.Format(_T("%f"), 2.0);
	SetDlgItemText(IDC_EDITE0, str);
	str.Format(_T("%f"), 0.6);
	SetDlgItemText(IDC_EDITk0, str);
	str.Format(_T("%f"), 0.4);
	SetDlgItemText(IDC_EDITk1, str);
	str.Format(_T("%f"), 0.9);
	SetDlgItemText(IDC_EDITk2, str);
	str.Format(_T("%d"), 3);
	SetDlgItemText(IDC_EDITx, str);
	str.Format(_T("%d"), 3);
	SetDlgItemText(IDC_EDITy, str);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();
		CDialog::UpdateWindow();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImageProcessDlg::OnBnClickedOpenimage() //打开图片
{
	SetDlgItemText(IDC_ImagePath, _T(" "));
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("ALL Files (*.*) |*.*||"), NULL);

	//打开文件对话框的标题名
	dlg.m_ofn.lpstrTitle = _T("Open Image ");
	if (dlg.DoModal() != IDOK)  return;
	CString  mPath = dlg.GetPathName();

	USES_CONVERSION;
	LPCSTR lpstr = (LPCSTR)T2A(mPath);

	//	SourceImage = imread(mPath, CV_LOAD_IMAGE_UNCHANGED);
	SourceImage = imread(lpstr, CV_LOAD_IMAGE_UNCHANGED);
	if (SourceImage.empty())
	{
		MessageBox(_T("读取文件为空"));
		return;
	}
	SetDlgItemText(IDC_ImagePath, mPath);
	m_Hight = SourceImage.rows;
	m_Width = SourceImage.cols;

	DrawMat(SourceImage, IDC_Image);

}

void CImageProcessDlg::DrawMat(cv::Mat & img, UINT nID)
{
	cv::Mat imgTmp;
	CRect rect;
	GetDlgItem(nID)->GetClientRect(&rect);  // 获取控件大小
	cv::resize(img, imgTmp, cv::Size(rect.Width(), rect.Height()));// 缩小或放大Mat并备份

	// 转一下格式 ,这段可以放外面,
	switch (imgTmp.channels())
	{
	case 1:
		cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY单通道
		break;
	case 3:
		cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);  // BGR三通道
		break;
	default:
		break;
	}

	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1); // 计算一个像素多少个字节
	// 制作bitmapinfo(数据头)
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = imgTmp.cols;
	bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	// Mat.data + bitmap数据头 -> MFC
	CDC *pDC = GetDlgItem(nID)->GetDC();
	::StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, rect.Width(), rect.Height(),
		0, 0, rect.Width(), rect.Height(),
		imgTmp.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	ReleaseDC(pDC);
}


void CImageProcessDlg::OnBnClickedEqualizehist()
{
	/*直方图均衡化主要有四步：
	第一步是统计每个灰度值出现的次数。
	第二步是计算累积函数。
	第三步是根据映射函数，建立查找表。
	第四步是根据查找表计算新的像素值。*/
	//------------------第一种直接调用OPENCV库函数------------------------------------//
		//if (SourceImage.empty())
		//{
		//MessageBox(_T("请先打开一副图片"));
		//return ;
		//}
		//Mat imageRGB[3];

		//switch (SourceImage.channels())
		//{
		//case 1:
		//equalizeHist(SourceImage, ProcessImage);

		//break;
		//case 3:

		//split(SourceImage, imageRGB);
		//for (int i = 0; i < 3; i++)
		//{
		//equalizeHist(imageRGB[i], imageRGB[i]);
		//}
		//merge(imageRGB, 3, ProcessImage);
		//break;
		//default:
		//break;
		//}
		//DrawMat(ProcessImage, IDC_ProcessImage);
		//
	//--------------------------------------------------------------------------------//

	if (SourceImage.empty())
	{
		MessageBox(_T("请先打开一副图片"));
		return;
	}
	if (!ProcessImage.empty())
		ProcessImage.release();
	Mat imageRGB[3];

	switch (SourceImage.channels())
	{
	case 1://灰度直方图
		SourceImage.copyTo(ProcessImage);
		MyGlobalEqualizeHist(ProcessImage);
		break;

	case 3://彩色直方图
		split(SourceImage, imageRGB);
		for (int i = 0; i < 3; i++)
		{
			MyGlobalEqualizeHist(imageRGB[i]);
		}
		merge(imageRGB, 3, ProcessImage);
		break;
	}
	DrawMat(ProcessImage, IDC_ProcessImage);
}

void  CImageProcessDlg::MyGlobalEqualizeHist(Mat& Image)  //全局直方图均衡化
{
	int p[256];
	const int hist_size = 256;   // p数组长度为图像灰度级
	memset(p, 0, sizeof(p));

	for (int i = 0; i < (int)m_Hight; i++)//m_Hight rows
	{
		uchar * data = Image.ptr<uchar>(i);
		for (int j = 0; j < (int)m_Width; j++)//m_Width cols
		{
			p[data[j]] ++;   //相当于[i][j]这个点对应的像素值 ++ 
		}
	}

	int c[hist_size]; //计算累计分布函数 
	c[0] = p[0];
	for (int i = 1; i < hist_size; i++)
	{
		c[i] = c[i - 1] + p[i];
	}

	uchar lut[hist_size];   //根据映射函数，建立 look up table 
	for (int i = 0; i < hist_size; i++)
	{
		int val = (int)round(c[i] * (255.f / (SourceImage.rows * SourceImage.cols)));
		lut[i] = saturate_cast<uchar>(val);//像素值i映射之后值为lut[i]  saturate的意思就是说，当输入值v超高了目标类型的范围，该值会被限幅
	}

	for (int i = 0; i < m_Hight; i++)   // //根据look up table，改变图像像素值  
	{
		uchar * data = Image.ptr<uchar>(i);
		for (int j = 0; j < m_Width; j++)
		{
			data[j] = lut[data[j]];
		}
	}
}

void CImageProcessDlg::OnBnClickedMedeanfilter() //中值滤波
{
	if (SourceImage.empty())
	{
		MessageBox(_T("请先打开一副图片"));
		return;
	}
	if (!ProcessImage.empty())
		ProcessImage.release();

	Mat imageRGB[3];

	switch (SourceImage.channels())
	{
	case 1:
		SourceImage.copyTo(ProcessImage);
		selfAdaptiveMedianBlur(SourceImage, ProcessImage);
		//	MyMedeanfilter(ProcessImage);
		break;

	case 3:
		/*split(SourceImage, imageRGB);
		for (int i = 0; i < 3; i++)
		{
			MyMedeanfilter(imageRGB[i]);
		}
		merge(imageRGB, 3, ProcessImage);*/
		medianBlur(SourceImage, ProcessImage, 3);
		break;
	}
	//	medianBlur(SourceImage, ProcessImage, 3 );

	DrawMat(ProcessImage, IDC_ProcessImage);
}

void  CImageProcessDlg::MyMedeanfilter(Mat& Image)
{
	int win_size = 3;
	int start = win_size / 2;

	for (int m = start; m < m_Hight - start; m++) {
		for (int n = start; n < m_Width - start; n++) {
			vector<uchar> model;
			for (int i = -start + m; i <= start + m; i++) {
				for (int j = -start + n; j <= start + n; j++) {

					model.push_back(Image.at<uchar>(i, j));
				}
			}
			sort(model.begin(), model.end());     //采用快速排序进行
			Image.at<uchar>(m, n) = model[win_size*win_size / 2];
		}
	}
}


void CImageProcessDlg::selfAdaptiveMedianBlur(Mat&src, Mat&dst)
{
	int kernal_size = 7;
	uchar* pdst = dst.data;
	uchar Zmin, Zmax, Zmed, Zxy;
	int step0 = src.step[0];
	int step1 = src.step[1];
	for (int i = kernal_size / 2; i < src.rows - kernal_size / 2; i++)
	{
		for (int j = kernal_size / 2; j < src.cols - kernal_size / 2; j++)
		{
			int ks = 3;//kernal_size;
			int count = 0;
			Zxy = *CV_MAT_ELEM2(src, uchar, i, j);//Sxy覆盖区域的中心点像素值，即锚点像素值
			vector<uchar> v;//将模板覆盖区域的像素，压入矢量v中
			do {
				if (count == 0)
				{//获取模板ks*ks覆盖区域的像素，压入矢量v中
					CV_ROI_ELEM(src, v, i - ks / 2, j - ks / 2, ks);
				}
				else
				{
					/****************下面的for循环，将外扩的四个边的像素添加到v中**************/
					uchar* p = src.data + (i - ks / 2)*step0 + (j - ks / 2)*step1;
					for (int u = 0; u < ks; u++)
					{
						v.push_back(*(p + u * step1));//向外扩展的四个边的上边
						v.push_back(*(p + (ks - 1)*step0 + u * step1));//向外扩展的四个边的下边
						if (u != 0 && u != ks - 1)
						{
							v.push_back(*(p + u * step0));//向外扩展的四个边的左边
							v.push_back(*(p + u * step0 + (ks - 1)*step1));//向外扩展的四个边的右边
						}
					}
				}

				//对v的元素排序
				//排序后，Sxy覆盖区域内，最大值为Zmax=v[v.size-1],最小值为Zmin=v[0]
				std::sort(v.begin(), v.end());
				Zmin = v[0], Zmax = v[v.size() - 1], Zmed = v[ks*ks / 2];
				pdst = CV_MAT_ELEM2(dst, uchar, i, j);
				if (Zmin < Zmed&&Zmed < Zmax)
				{
					if (Zmin < Zxy&&Zxy < Zmax)
					{
						*pdst = Zxy; break;
					}
					else
					{
						*pdst = Zmed; break;
					}
				}
				else
				{
					ks += 2;
				}
				count++;
			} while (ks <= kernal_size);

			*pdst = Zmed;
		}
	}
}

void CImageProcessDlg::OnBnClickedLocalEqualizehist()
{
	// TODO: 在此添加控件通知处理程序代码
	int padding = 3;
	if (SourceImage.empty())
	{
		MessageBox(_T("请先打开一副图片"));
		return;
	}
	if (!ProcessImage.empty())
		ProcessImage.release();
	Mat imageRGB[3];

	switch (SourceImage.channels())
	{
	case 1://灰度直方图
		copyMakeBorder(SourceImage, ProcessImage, padding, padding, padding, padding, BORDER_REPLICATE);//边界填充，复制邻近像素点
		MyLocalEqualizeHist(ProcessImage, padding);
		break;

	case 3://彩色直方图
		copyMakeBorder(SourceImage, ProcessImage, padding, padding, padding, padding, BORDER_REPLICATE);//边界填充，复制邻近像素点
		split(ProcessImage, imageRGB);
		for (int i = 0; i < 3; i++)
		{
			MyLocalEqualizeHist(imageRGB[i], padding);
		}
		merge(imageRGB, 3, ProcessImage);
		break;
	}
	Rect rect(padding, padding, m_Width , m_Hight );
	Mat pass;
	ProcessImage(rect).copyTo(pass);
	pass.copyTo(ProcessImage);
	DrawMat(ProcessImage, IDC_ProcessImage);
}

void  CImageProcessDlg::MyLocalEqualizeHist(Mat& Image, int padding)  //局部直方图均衡化
{
	int p[256];
	const int hist_size = 256;   // p数组长度为图像灰度级
	memset(p, 0, sizeof(p));
	int sort = padding / 2;
	Mat Image2;
	Image.copyTo(Image2);
	for (int i = 0 + padding; i < (int)m_Hight + padding; i++)//m_Hight rows
	{

		for (int j = 0 + padding; j < (int)m_Width + padding; j++)//m_Width cols
		{

			if (j == 0 + padding)
			{
				memset(p, 0, sizeof(p));
				for (int k = -sort; k <= sort; k++)
				{
					uchar * data = Image.ptr<uchar>(i + k);
					for (int m = -sort; m <= sort; m++)
						p[data[j + m]] ++;//相当于[i][j]这个点对应的像素值 ++ 
				}
			}
			else
			{
				for (int m = -sort; m <= sort; m++)
				{
					p[Image.ptr<uchar>(i + m)[j - sort]] --;//相当于[i][j]这个点对应的离开列像素值 -- 
					p[Image.ptr<uchar>(i + m)[j + sort]] ++;//相当于[i][j]这个点对应的像素值 ++ 
				}
			}
			int c[hist_size]; //计算累计分布函数 
			c[0] = p[0];
			for (int i = 1; i < hist_size; i++)
			{
				c[i] = c[i - 1] + p[i];
			}

			uchar lut[hist_size];   //根据映射函数，建立 look up table 
			for (int i = 0; i < hist_size; i++)
			{
				int val = (int)round(c[i] * (255.f / (padding * padding)));
				lut[i] = saturate_cast<uchar>(val);//像素值i映射之后值为lut[i]  saturate的意思就是说，当输入值v超高了目标类型的范围，该值会被限幅
			}

			Image2.ptr<uchar>(i)[j] = lut[Image.ptr<uchar>(i)[j]];  //根据look up table，改变图像像素值  


		}
	}
	Image2.copyTo(Image);

}

void CImageProcessDlg::OnBnClickedHistStatEnhance()//直方图统计增强
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码

	if (SourceImage.empty())
	{
		MessageBox(_T("请先打开一副图片"));
		return;
	}
	if (!ProcessImage.empty())
		ProcessImage.release();

	switch (SourceImage.channels())
	{
	case 1://灰度直方图
		HistStatEnhance(SourceImage, ProcessImage);
		break;

	case 3://彩色直方图
		/*Mat imageRGB[3];
		split(SourceImage, imageRGB);
		for (int i = 0; i < 3; i++)
		{
		}
		merge(imageRGB, 3, ProcessImage);*/
		MessageBox(_T("todo：彩色"));
		break;
	}
	DrawMat(ProcessImage, IDC_ProcessImage);

}

void  CImageProcessDlg::HistStatEnhance(Mat& src, Mat& dst)
{
	double m_g = 0;//全局均值
	double sigma_g = 0;//全局方差
	//更新参数E0，k0,k1,k2，Sxy大小x，y
	float E0;
	float k0;
	float k1;
	float k2;
	int x;
	int y;
	CString str;
	GetDlgItemText(IDC_EDITE0, str);
	::_stscanf_s(str, _T("%f"), &E0);
	GetDlgItemText(IDC_EDITk0, str);
	::_stscanf_s(str, _T("%f"), &k0);
	GetDlgItemText(IDC_EDITk1, str);
	::_stscanf_s(str, _T("%f"), &k1);
	GetDlgItemText(IDC_EDITk2, str);
	::_stscanf_s(str, _T("%f"), &k2);
	GetDlgItemText(IDC_EDITx, str);
	::_stscanf_s(str, _T("%d"), &x);
	GetDlgItemText(IDC_EDITy, str);
	::_stscanf_s(str, _T("%d"), &y);
	int p[256];
	memset(p, 0, sizeof(p));
	//计算全局均值
	for (int i = 0; i < m_Hight; i++)
	{
		uchar *data = src.ptr<uchar>(i);
		for (int j = 0; j < m_Width; j++)
		{
			m_g += data[j];
		}
	}
	m_g = m_g / (double)(m_Hight*m_Width);
	//计算全局方差
	for (int i = 0; i < m_Hight; i++)
	{
		uchar *data = src.ptr<uchar>(i);
		for (int j = 0; j < m_Width; j++)
		{
			sigma_g += ((double)data[j] - m_g)*((double)data[j] - m_g);
		}
	}
	sigma_g = sigma_g / (double)(m_Hight*m_Width);
	Mat pass;//中间矩阵
	copyMakeBorder(src, pass, x, x, y, y, BORDER_REPLICATE);//边界填充，复制邻近像素点
	src.copyTo(dst);
	//领域增强
	double m_L;//局部均值
	double sigma_L;//均部方差
	for (int i = 0 + x; i < m_Hight + x; i++)
	{

		for (int j = 0 + y; j < m_Width + y; j++)
		{

			m_L = 0; sigma_L = 0;
			//计算局部均值
			for (int k = -x / 2; k <= x / 2; k++)
			{
				uchar *data = pass.ptr<uchar>(i + k);
				for (int m = -y / 2; m <= y / 2; m++)
				{
					m_L=m_L +(double)data[j + m];
				}
			}
			m_L = m_L / (double)(x*y);
			//计算局部方差
			for (int k = -x / 2; k <= x / 2; k++)
			{
				uchar *data = pass.ptr<uchar>(i + k);
				for (int m = -y / 2; m <= y / 2; m++)
				{
					sigma_L += ((double)data[j + m] - m_L)*((double)data[j + m] - m_L);
				}
			}
			sigma_L = sigma_L / (double)(x*y);
			if (m_L<k0*m_g&&(k1*sigma_g<sigma_L &&sigma_L < k2*sigma_g))
			{
				dst.ptr<uchar>(i-x)[j-y] = saturate_cast<uchar>((int)E0*src.ptr<uchar>(i-x)[j-y]);
			}
			else
			{
				dst.ptr<uchar>(i-x)[j-y] = src.ptr<uchar>(i-x)[j-y];
			}
		}
	}

}



void CImageProcessDlg::OnBnClickedNorchfilter()
{
	if (SourceImage.empty())
	{
		MessageBox(_T("请先打开一副图片"));
		return;
	}
	if (!ProcessImage.empty())
		ProcessImage.release();

	switch (SourceImage.channels())
	{
	case 1://灰度直方图
		NorchFiltering(SourceImage, ProcessImage);
		break;

	case 3://彩色直方图
		/*Mat imageRGB[3];
		split(SourceImage, imageRGB);
		for (int i = 0; i < 3; i++)
		{
		}
		merge(imageRGB, 3, ProcessImage);*/
		MessageBox(_T("todo：彩色"));
		break;
	}
	DrawMat(ProcessImage, IDC_ProcessImage);
}

void CImageProcessDlg::NorchFiltering(Mat& src, Mat& dst)
{
	Mat padded;                 //以0填充输入图像矩阵
	int m = src.rows;
	int n = src.cols;

	//0填充输入图像src，输入矩阵为padded，上方和左方不做填充处理
	copyMakeBorder(src, padded, 0,2* m - src.rows, 0, 2*n - src.cols, BORDER_CONSTANT, Scalar::all(0));

	//进行傅里叶变换
	Mat relocated= Mat_<float>(padded);
	for(int i=0;i< relocated.rows;i++)
		for (int j = 0; j < relocated.cols; j++)
		{
			relocated.ptr<float>(i)[j] = relocated.ptr<float>(i)[j] * pow(-1.0, i+j);
		}
			
	Mat planes[] = { relocated , Mat::zeros(relocated.size(),CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);     //将planes融合合并成一个多通道数组complexI

	dft(complexI, complexI);        

	//可视化,计算幅值，转换到对数尺度(logarithmic scale)
	//=> log(1 + sqrt(Re(DFT(src))^2 + Im(DFT(src))^2))
	split(complexI, planes);        //planes[0] = Re(DFT(src),planes[1] = Im(DFT(src))
									//即planes[0]为实部,planes[1]为虚部
	magnitude(planes[0], planes[1], planes[0]);     //planes[0] = magnitude
	Mat magI = planes[0];
	
	Mat I = magI;
	I += Scalar::all(1);
	log(I, I);                //转换到对数尺度(logarithmic scale)
	//归一化处理，用0-1之间的浮点数将矩阵变换为可视的图像格式
	normalize(I, I, 0, 1, CV_MINMAX);
	imshow("DFT", I);

	//构造滤波模板
	Mat H = Mat::ones(magI.size(), CV_32F);
	for (int i = magI.cols / 2-4; i < magI.cols /2+4; i++)
		for (int j = 0; j < magI.rows; j++)
		{
			if(abs(j- magI.rows/2)>=25)
				H.ptr<float>(j)[i] = 0;
		}
	//滤波
	for (int i = 0; i < magI.rows; i++)
		for (int j = 0; j < magI.cols; j++)
		{
			magI.ptr<float>(i)[j] = H.ptr<float>(i)[j]* magI.ptr<float>(i)[j];
			planes[0].ptr<float>(i)[j] = H.ptr<float>(i)[j] * planes[0].ptr<float>(i)[j];
			planes[1].ptr<float>(i)[j] = H.ptr<float>(i)[j] * planes[1].ptr<float>(i)[j];
		}

	//显示滤波模板
	magI.copyTo(I);
	I += Scalar::all(1);
	log(I, I);                //转换到对数尺度(logarithmic scale)
	//归一化处理，用0-1之间的浮点数将矩阵变换为可视的图像格式
	normalize(I, I, 0, 1, CV_MINMAX);
	imshow("H(u，v)*G(u,v)", I);
		
	//IDFT
	merge(planes, 2, complexI);
	idft(complexI, complexI);
	split(complexI, planes);        //planes[0] = Re(DFT(src),planes[1] = Im(DFT(src))
									//即planes[0]为实部,planes[1]为虚部
	planes[0].copyTo(magI);
	for (int i = 0; i < magI.rows; i++)
		for (int j = 0; j < magI.cols; j++)
		{
			magI.ptr<float>(i)[j] = magI.ptr<float>(i)[j] * pow(-1.0, i+j);
		}
	//截取
	Rect rect(0, 0, src.cols, src.rows);
	Mat pass;
	magI(rect).copyTo(pass);
	pass.copyTo(magI);

	normalize(magI, magI, 0, 255, CV_MINMAX);
	magI.convertTo(magI,CV_8U);
	waitKey(1);
	dst = magI;
}