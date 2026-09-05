
// Final ProjectDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Final Project.h"
#include "Final ProjectDlg.h"
#include "afxdialogex.h"

#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/videoio.hpp>

#include <iostream> 
#include <string>

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Mat mat, frame1, previousFrame, areaMask, doorsMask, doubleMask;
VideoCapture cap;
VideoWriter videoOut;
bool m_bCaptured;

Ptr<BackgroundSubtractor> bgSubtractor;
int people_enter = 0;
int people_exit = 0;

// Structure to hold tracked objects
struct Track {
	Rect bbox; Point centroid, prevCentroid, middleDown, prevMiddleDown; bool counted;
	int id; int missingFrames = 0; Scalar color; int estimatedPeople;
};
static std::vector<Track> tracks;
static int nextTrackID = 1;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFinalProjectDlg dialog



CFinalProjectDlg::CFinalProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FINAL_PROJECT_DIALOG, pParent)
	, file_link(_T(""))
	, area_link(_T("C:/Users/gogsa/Desktop/CVTests/Videos Plaça Ferrandiz avi/1.plazaMask.jpg"))
	, doors_link(_T("C:/Users/gogsa/Desktop/CVTests/Videos Plaça Ferrandiz avi/3.doorsMask.png"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFinalProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, file_link);
	DDX_Text(pDX, IDC_EDIT2, area_link);
	DDX_Text(pDX, IDC_EDIT3, doors_link);
}

BEGIN_MESSAGE_MAP(CFinalProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SelectVideo, &CFinalProjectDlg::OnBnClickedSelectvideo)
	ON_BN_CLICKED(IDOK, &CFinalProjectDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT1, &CFinalProjectDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_Skip, &CFinalProjectDlg::OnBnClickedSkip)
	ON_EN_CHANGE(IDC_EDIT2, &CFinalProjectDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CFinalProjectDlg::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_SelectAreaImage, &CFinalProjectDlg::OnBnClickedSelectareaimage)
	ON_BN_CLICKED(IDC_SelectDoorImage, &CFinalProjectDlg::OnBnClickedSelectdoorimage)
	ON_BN_CLICKED(IDC_AreaSelector, &CFinalProjectDlg::OnBnClickedAreaselector)
	ON_BN_CLICKED(IDC_DoorsSelector, &CFinalProjectDlg::OnBnClickedDoorsselector)
END_MESSAGE_MAP()


// CFinalProjectDlg message handlers

BOOL CFinalProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFinalProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFinalProjectDlg::OnPaint()
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
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFinalProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFinalProjectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (!m_bCaptured) {
		string ss;
		int fourcc;
		fourcc = VideoWriter::fourcc('X', '2', '6', '4');

		if (file_link.IsEmpty())
		{
			AfxMessageBox(_T("Failed to load video!"));
			return;

		}

		if (areaMask.empty() && !area_link.IsEmpty())
		{
			ss = CW2A((LPCTSTR)area_link);
			areaMask = imread(ss, IMREAD_COLOR);

			if (areaMask.empty()) {
				AfxMessageBox(_T("Failed to load Area Mask!"));
				return;
			}

			cvtColor(areaMask, areaMask, COLOR_BGR2GRAY);
			threshold(areaMask, areaMask, 1, 255, THRESH_BINARY);
			Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
			cv::morphologyEx(areaMask, areaMask, cv::MORPH_OPEN, kernel);
			cv::morphologyEx(areaMask, areaMask, cv::MORPH_CLOSE, kernel);
		}

		if (doorsMask.empty() && !doors_link.IsEmpty()) {
			ss = CW2A((LPCTSTR)doors_link);
			doorsMask = imread(ss, IMREAD_COLOR);
			if (doorsMask.empty()) {
				AfxMessageBox(_T("Failed to load image!"));
				return;
			}
			cvtColor(doorsMask, doorsMask, COLOR_BGR2GRAY);
			threshold(doorsMask, doorsMask, 1, 255, THRESH_BINARY);
			Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
			cv::morphologyEx(areaMask, areaMask, cv::MORPH_OPEN, kernel);
			cv::morphologyEx(areaMask, areaMask, cv::MORPH_CLOSE, kernel);
		}

		if (areaMask.empty()) {
			OnBnClickedAreaselector();
		}
		if (doorsMask.empty()) {
			OnBnClickedDoorsselector();
		}


		ss = CW2A((LPCTSTR)file_link);
		cap.open(ss);
		ss = CW2A((LPCTSTR)_T("result_out.mp4"));

		if (!cap.isOpened())
		{
			AfxMessageBox(_T("Unable to open video"), MB_OK);
			return;
		}

		cap.read(mat);

		GetDlgItem(IDC_Skip)->EnableWindow(FALSE);

		// -------------------------- initial work ------------------------------------
		bgSubtractor = createBackgroundSubtractorMOG2(500, 16, true);
		bitwise_or(areaMask, doorsMask, doubleMask);
		// ----------------------------------------------------------------------------


		namedWindow("Live", WINDOW_NORMAL);
		imshow("Live", mat);
		videoOut.open(ss, fourcc, (int)cap.get(CAP_PROP_FPS), mat.size(), TRUE);


		if (!videoOut.isOpened())
		{
			AfxMessageBox(_T("Unable to record camera or video"), MB_OK);
			return;
		}

		SetTimer(1, (uint)(10), NULL);
		GetDlgItem(IDOK)->SetWindowTextW(_T("Stop"));
		m_bCaptured = true;
	}
	else {
		KillTimer(1);
		GetDlgItem(IDOK)->SetWindowTextW(_T("Show"));
		videoOut.release();
		cap.release();
		m_bCaptured = false;
		destroyWindow("Live");
	}

	waitKey(0);
}

int sizemin = 120;
int sizemax = 9000;
int i = 0;
int estimateN(double area, Point center) {
	int frameHeight = mat.rows;
	int estimateAreaOfPerson = 0;
	if (center.y < frameHeight / 2) {
		estimateAreaOfPerson = 320.0f;
	}
	else {
		estimateAreaOfPerson = 520.0f;
	}
	int estimatedCount = std::round(area / estimateAreaOfPerson); // estimate people in track
	return std::max(1, estimatedCount);
}
void TrackPeople() {
	// 1. Background subtraction to get foreground mask
	Mat fgMask;
	bgSubtractor->apply(mat, fgMask);
	fgMask.setTo(Scalar(255), fgMask == 127);
	// Wait for background to stabilize
	if (i < 10) {
		i++;
		return;
	}

	// 2. Threshold and blur to reduce noise
	GaussianBlur(fgMask, fgMask, Size(5, 5), 0);
	threshold(fgMask, fgMask, 25, 255, THRESH_BINARY);

	// 3. Morphological operations to clean up the mask
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(fgMask, fgMask, MORPH_OPEN, kernel);
	morphologyEx(fgMask, fgMask, MORPH_CLOSE, kernel);

	// 4. Find contours of moving objects
	vector<vector<Point>> contours;
	findContours(fgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//imshow("test", fgMask);
	
	// Mark all existing tracks as not updated this frame
	vector<bool> used(tracks.size(), false);

	// 5. Process each contour
	for (const auto& contour : contours) {
		if (contourArea(contour) < sizemin || contourArea(contour) > sizemax) continue; // size thresholds
		Rect bbox = boundingRect(contour);
		if (contourArea(contour) / (bbox.width * bbox.height) > 0.7) continue; // probably not human
		Point centroid(bbox.x + bbox.width / 2, bbox.y + bbox.height / 2);

		// Try to match this centroid to existing tracks
		double minDist = 1e6;
		int matchedIdx = -1;
		for (size_t i = 0; i < tracks.size(); i++) {
			if (used[i]) continue; // already matched
			double dist = norm(tracks[i].centroid - centroid);
			if (dist < minDist && dist < 50) { // threshold distance
				minDist = dist;
				matchedIdx = (int)i;
			}
		}

		if (matchedIdx != -1) {
			// Update existing track
			Track& t = tracks[matchedIdx];
			t.bbox = bbox;
			t.prevCentroid = t.centroid;
			t.prevMiddleDown = t.middleDown;
			t.centroid = centroid;
			t.middleDown = Point(bbox.x + bbox.width / 2, bbox.y + bbox.height);
			t.estimatedPeople = estimateN(contourArea(contour), centroid);
			used[matchedIdx] = true;
		}
		else {
			// Create new track
			Track newTrack;
			newTrack.bbox = bbox;
			newTrack.centroid = centroid;
			newTrack.prevCentroid = centroid;
			newTrack.middleDown = Point(bbox.x + bbox.width / 2, bbox.y + bbox.height);
			newTrack.prevMiddleDown = newTrack.middleDown;
			newTrack.counted = false;
			newTrack.id = nextTrackID++;
			newTrack.estimatedPeople = estimateN(contourArea(contour), centroid);
			tracks.push_back(newTrack);
			used.push_back(true);
			
		}
	}

	// Visualization
	for (const auto& t : tracks) {
		if (t.bbox.area() == 0) continue;
		if (t.color == Scalar(0, 0, 0)) continue;
		rectangle(mat, t.bbox, t.color, 2);
		circle(mat, t.centroid, 4, Scalar(0, 0, 255), -1);
		Point middleUp(t.bbox.x + t.bbox.width / 2, t.bbox.y - 10);
		putText(mat, to_string(t.estimatedPeople), middleUp,
			FONT_HERSHEY_SIMPLEX, 0.7, t.color, 2);
	}

	// Handle missing tracks
	for (size_t i = 0; i < tracks.size(); ++i) {
		if (!used[i])
			tracks[i].missingFrames++;
		else
			tracks[i].missingFrames = 0;
	}

	tracks.erase(remove_if(tracks.begin(), tracks.end(),
		[](const Track& t) {
			return t.missingFrames > 3;
		}), tracks.end());
}


void countEnterExit() {
	for (auto& t : tracks) {
		uchar pixel = doorsMask.at<uchar>(t.middleDown.y, t.middleDown.x);
		uchar pixelPrev = doorsMask.at<uchar>(t.prevMiddleDown.y, t.prevMiddleDown.x);

		//Only count if not already counted
		if (!t.counted) {
			if (pixel < 128 && pixelPrev > 128) {
				people_exit += t.estimatedPeople; // (exited)
				t.color = Scalar(0, 0, 255);
				t.counted = true;
			}else if (pixelPrev < 128 && pixel > 128) {
				people_enter += t.estimatedPeople; // (entered)
				t.color = Scalar(0, 255, 0);
				t.counted = true;
			}
		}
	}
}

int countAndColorPeople() {
	int n = 0;
	for (auto& t : tracks) {
		bool change = true;
		uchar pixel = doorsMask.at<uchar>(t.centroid.y, t.centroid.x);
		if (pixel > 128) {
			change = false;
		}
		pixel = areaMask.at<uchar>(t.centroid.y, t.centroid.x);
		if (pixel > 128) {
			if(change) t.color = Scalar(255, 0, 0);
			n += t.estimatedPeople;
		}
		pixel = doubleMask.at<uchar>(t.centroid.y, t.centroid.x);
		if (pixel < 128) {
			if(change) t.color = Scalar(0, 0, 0);
		}
	}
	return n;
}


void CFinalProjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (cap.isOpened()) {
		previousFrame = mat.clone();
		cap.read(mat);

		if (mat.empty()) {
			GetDlgItem(IDOK)->SetWindowTextW(_T("Show"));
			videoOut.release();
			cap.release();
			m_bCaptured = false;
			destroyWindow("Live");
			KillTimer(1);
			return;
		}

		GetDlgItem(IDC_Skip)->EnableWindow(TRUE);

		
		TrackPeople();
		int people_area = countAndColorPeople();
		countEnterExit();

		

		Mat res = mat;
		//Mat res;
		//bitwise_and(mat, mat, res, doorsMask);
		// Draw text on image
		putText(res, "people on Area:" + to_string(people_area), Point(20, 30),
			FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
		putText(res, "Enter:" + to_string(people_enter), Point(320, 30),
			FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
		putText(res, "Exit:" + to_string(people_exit), Point(470, 30),
			FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);
		putText(res, "people in Building:" + to_string(people_enter - people_exit), Point(20, 60),
			FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 2);

		imshow("Live", res);
		if (videoOut.isOpened()) {
			videoOut.write(res);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CFinalProjectDlg::OnBnClickedSkip()
{
	int skipFrames = 500;

	int currentFrame = (int)cap.get(CAP_PROP_POS_FRAMES);
	int totalFrames = (int)cap.get(CAP_PROP_FRAME_COUNT);

	int targetFrame = currentFrame + skipFrames;

	if (targetFrame >= totalFrames) {
		cap.set(CAP_PROP_POS_FRAMES, totalFrames - 1);
	}
	else {
		cap.set(CAP_PROP_POS_FRAMES, targetFrame);
	}
}

void CFinalProjectDlg::OnBnClickedSelectvideo()
{
	UpdateData(TRUE);
	CFileDialog seleccionaarchivo(TRUE, NULL, file_link);
	if (seleccionaarchivo.DoModal() == IDOK)
	{
		file_link = seleccionaarchivo.GetPathName();
		UpdateData(FALSE);
	}
}

void CFinalProjectDlg::OnEnChangeEdit1()
{
	UpdateData(true);
}

void CFinalProjectDlg::OnBnClickedSelectareaimage()
{
	UpdateData(TRUE);
	CFileDialog seleccionaarchivo(TRUE, NULL, area_link);
	if (seleccionaarchivo.DoModal() == IDOK)
	{
		area_link = seleccionaarchivo.GetPathName();
		UpdateData(FALSE);
	}
}

void CFinalProjectDlg::OnEnChangeEdit2()
{
	UpdateData(true);
}

void CFinalProjectDlg::OnBnClickedSelectdoorimage()
{
	UpdateData(TRUE);
	CFileDialog seleccionaarchivo(TRUE, NULL, doors_link);
	if (seleccionaarchivo.DoModal() == IDOK)
	{
		doors_link = seleccionaarchivo.GetPathName();
		UpdateData(FALSE);
	}
}
void CFinalProjectDlg::OnEnChangeEdit3()
{
	UpdateData(true);
}


bool drawing = false;
vector<Point> polygonPoints;

void selectArea(int event, int x, int y, int, void*)
{
	if (event == EVENT_LBUTTONDOWN) {
		polygonPoints.push_back(Point(x, y));
		drawing = true;
	}
	else if (event == EVENT_MOUSEMOVE && drawing && !polygonPoints.empty()) {
		// Optional: show dynamic line from last point to current mouse pos
		Mat tempImg = frame1.clone();
		for (size_t i = 0; i < polygonPoints.size() - 1; i++)
			line(tempImg, polygonPoints[i], polygonPoints[i + 1], Scalar(0, 255, 0), 2);
		line(tempImg, polygonPoints.back(), Point(x, y), Scalar(0, 255, 0), 2);
		imshow("Select Area", tempImg);
	}
	else if (event == EVENT_RBUTTONDOWN) {
		// Right click to finish polygon
		if (polygonPoints.size() > 2) {
			// Close polygon by connecting last and first points
			polygonPoints.push_back(polygonPoints.front());

			Mat tempImg = frame1.clone();
			for (size_t i = 0; i < polygonPoints.size() - 1; i++)
				line(tempImg, polygonPoints[i], polygonPoints[i + 1], Scalar(255, 0, 0), 2);

			imshow("Select Area", tempImg);

			drawing = false;
			// Now you have polygonPoints as ROI polygon
		}
	}
}

vector<vector<Point>> allPolygons;

void selectDoors(int event, int x, int y, int, void*)
{
	if (event == EVENT_LBUTTONDOWN) {
		if (!drawing) {
			polygonPoints.clear();
			drawing = true;
		}
		polygonPoints.push_back(Point(x, y));
	}
	else if (event == EVENT_MOUSEMOVE && drawing && !polygonPoints.empty()) {
		Mat tempImg = frame1.clone();

		// Draw all previous polygons
		for (const auto& poly : allPolygons) {
			polylines(tempImg, poly, true, Scalar(255, 0, 0), 2);
		}

		// Draw current polygon
		for (size_t i = 0; i < polygonPoints.size() - 1; i++)
			line(tempImg, polygonPoints[i], polygonPoints[i + 1], Scalar(0, 255, 0), 2);

		line(tempImg, polygonPoints.back(), Point(x, y), Scalar(0, 255, 0), 2);
		imshow("Select Doors", tempImg);
	}
	else if (event == EVENT_RBUTTONDOWN) {
		if (drawing && polygonPoints.size() > 2) {
			polygonPoints.push_back(polygonPoints.front()); // close polygon
			allPolygons.push_back(polygonPoints);

			drawing = false;

			Mat tempImg = frame1.clone();

			// Draw all polygons including the new one
			for (const auto& poly : allPolygons) {
				polylines(tempImg, poly, true, Scalar(255, 0, 0), 2);
			}

			imshow("Select Doors", tempImg);
		}
	}
}


void CFinalProjectDlg::OnBnClickedAreaselector()
{
	string ss;
	ss = CW2A((LPCTSTR)file_link);
	cap.open(ss);
	cap.grab();
	cap.read(frame1);

	namedWindow("Select Area", WINDOW_NORMAL);
	setMouseCallback("Select Area", selectArea, 0);
	imshow("Select Area", frame1);

	waitKey(0);

	Mat mask = Mat::zeros(frame1.size(), CV_8UC1);
	vector<vector<Point>> pts{ polygonPoints };
	fillPoly(mask, pts, Scalar(255));

	imshow("Select Area", mask);
	waitKey(0);
	if (getWindowProperty("Select Area", WND_PROP_VISIBLE) >= 1) {
		destroyWindow("Select Area");
	}
	areaMask = mask;
}

void CFinalProjectDlg::OnBnClickedDoorsselector()
{
	string ss;
	ss = CW2A((LPCTSTR)file_link);
	cap.open(ss);
	cap.grab();
	cap.read(frame1);

	namedWindow("Select Doors", WINDOW_NORMAL);
	setMouseCallback("Select Doors", selectDoors, 0);
	imshow("Select Doors", frame1);

	waitKey(0);

	Mat mask = Mat::zeros(frame1.size(), CV_8UC1);

	// Fill each polygon with white (value 255)
	for (const auto& poly : allPolygons) {
		const Point* pts = poly.data();
		int npts = static_cast<int>(poly.size());
		fillPoly(mask, &pts, &npts, 1, Scalar(255));
	}
	imshow("Select Doors", mask);

	waitKey(0);

	if (getWindowProperty("Select Doors", WND_PROP_VISIBLE) >= 1) {
		destroyWindow("Select Doors");
	}
	doorsMask = mask;
}
