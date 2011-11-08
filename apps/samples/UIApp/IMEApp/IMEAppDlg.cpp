/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*****************************************************************************************/
/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate:  $
 * $Revision:  $
 * $Author:  $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR	:  sample application to get strokes and display's the 
 *                 recognized values.
 * AUTHOR		:  Balaji MNA
 * Version		:  1.0
 * Date			:  29-Nov-2008
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/

#include "stdafx.h"
#include "IMEApp.h"
#include "IMEAppDlg.h"
#include "LTKShapeRecoResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAPPING_PATH_STRING		SEPARATOR + "mapping.txt"
#define IDT_RECOGNIZE_TIMER 1000
#define IDT_MENU_ID			2000
#define DELAY_TIME          650
#define MAX_CHOICE          4

CIMEAppDlg::CIMEAppDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIMEAppDlg::IDD, pParent),m_PrevPenPoint(0,0),m_NumOfStrokes(-1),
  m_IsDrawing(false),
  m_IsProjectLoaded(false),
  m_IsMappingFileFound(false)
  , m_DisplayMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_DrawAreaRect.left=10;
	m_DrawAreaRect.top=40;
	m_DrawAreaRect.right=165;
	m_DrawAreaRect.bottom=182;

}

void CIMEAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHOICE1, m_DisplayChoice[0]);
	DDX_Control(pDX, IDC_STATIC_CHOICE2, m_DisplayChoice[1]);
	DDX_Control(pDX, IDC_STATIC_CHOICE3, m_DisplayChoice[2]);
	DDX_Control(pDX, IDC_STATIC_CHOICE4, m_DisplayChoice[3]);
	DDX_Control(pDX, IDC_STATIC_WRITINGAREA_DISP, m_TopChoice);
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_DisplayMsg);
}

BEGIN_MESSAGE_MAP(CIMEAppDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_COMMAND(ID_LOAD_HINDI, &CIMEAppDlg::OnLoadPreBuildRecognizer)
	ON_COMMAND(ID_CLEAR_CLEAR, &CIMEAppDlg::OnClear)
END_MESSAGE_MAP()

BOOL CIMEAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

#ifdef _WIN32_WCE
	TCHAR envstring[MAX_PATH]={0};
	Common::GetAppPath(NULL,envstring);
	char environmentVal[MAX_PATH]={0};
	wcstombs(environmentVal,envstring,MAX_PATH);
	m_lipiRoot = environmentVal;
#else
	char *environmentVal = getenv("LIPI_ROOT");
	m_lipiRoot = environmentVal;
#endif

	m_ShapeRecognizer.loadLipiInterface();

	m_dlgCommandBar.Create(this);
	m_dlgCommandBar.InsertMenuBar(IDR_MENU_LOAD);

	m_pFont.CreateFont(35,0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		  DEFAULT_PITCH | FF_DONTCARE, L"english");

	m_TopChoice.ShowWindow(false);
	return TRUE; 
}

void CIMEAppDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	KillTimer(IDT_RECOGNIZE_TIMER);
	m_IsDrawing = true;

	displayMessage(L"");

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
		(point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);
		m_PrevPenPoint = penpoint;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CIMEAppDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(false == m_IsDrawing)
		return;

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
	   (point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CDC* pDC = GetDC();

		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);

		HPEN hPen,hOld;
		hPen = ::CreatePen (PS_SOLID, 2 ,RGB (0.5, 0.5, 0)); 
		hOld = (HPEN)pDC->SelectObject( hPen);

		if(m_PrevPenPoint.x != 0 && m_PrevPenPoint.y != 0)
		{
			pDC->MoveTo(m_PrevPenPoint.x,m_PrevPenPoint.y);
			pDC->LineTo(point.x,point.y);
		}
			
		m_PrevPenPoint = penpoint;

		pDC->SelectObject (hOld);
		DeleteObject (hPen);	
	}	
	CDialog::OnMouseMove(nFlags, point);
}

void CIMEAppDlg::OnLButtonUp(UINT nFlags, CPoint point)
{

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
		(point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);

		m_penstrokes.push_back(m_PenPointsVec);

		m_PrevPenPoint.x = 0;
		m_PrevPenPoint.y = 0;
	}
	
	m_IsDrawing = false;

	m_DisplayChoice[0].SetWindowTextW(L"");
	m_DisplayChoice[1].SetWindowTextW(L"");
	m_DisplayChoice[2].SetWindowTextW(L"");
	m_DisplayChoice[3].SetWindowTextW(L"");

	SetTimer(IDT_RECOGNIZE_TIMER, DELAY_TIME, NULL);
}

void CIMEAppDlg::OnPaint()
{
	CPaintDC dc(this); 
	
	HPEN hPen,hOld;
	hPen = ::CreatePen (PS_SOLID, 2 ,RGB (0.5, 0.5, 0)); 
	hOld = (HPEN)SelectObject (dc, hPen);

	Rectangle (dc,m_DrawAreaRect.left,m_DrawAreaRect.top,
		m_DrawAreaRect.right,m_DrawAreaRect.bottom);

    HRGN hrgn = CreateRectRgn(m_DrawAreaRect.left,m_DrawAreaRect.top,
		m_DrawAreaRect.right,m_DrawAreaRect.bottom);

	SelectObject (dc,hOld);
	DeleteObject (hPen);	

}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 10-Dec-2008
* NAME			: recognizeStrokes
* DESCRIPTION	: Recognize the strokes and displays the top most choices
* ARGUMENTS		: none
* RETURNS		: none 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

void CIMEAppDlg::recognizeStrokes(void)
{
	displayMessage(L"Recognizing...");

	if(0 != m_PenPointsVec.size())
	{
		vector<CString> topchoices;
		vector<LTKShapeRecoResult> resultSet;
		if( FAILURE == m_ShapeRecognizer.recognize(m_penstrokes,MAX_CHOICE,resultSet))
		{
			displayMessage(L"Wrong gesture!");
			return;
		}
		
		vector<CString> topChoices;
		if(m_IsMappingFileFound)
			getChoices(resultSet,topChoices);
		else
		{
			int iIndex = 0;
			for(vector<LTKShapeRecoResult>::iterator t = resultSet.begin(); 
						t < resultSet.end() && iIndex < MAX_CHOICE; t++,iIndex++)
			{
				CString displayValue;
				displayValue.Format(L"%d",(*t).getShapeId());
				m_DisplayChoice[iIndex].SetWindowTextW(displayValue);
				UpdateData(false);
			}
			topChoices.clear();
		}

		if(topChoices.size() != 0)
		{
			for(int i=0;i<MAX_CHOICE && i< (int)topChoices.size();i++)
			{
				long choice=0;
				if(topChoices[i].Find(L"0X") != -1 || topChoices[i].Find(L"0x") != -1)
				{
					choice = (long)_tcstol(topChoices[i].GetBuffer(topChoices[i].GetLength()),NULL,16);
				}
				else
				{
					topChoices[i].Trim(L" ");
					choice = (long)topChoices[i].GetAt(0);
				}
				displayChoice(m_DisplayChoice[i].m_hWnd,(TCHAR)choice);
			}
		}
		displayMessage(L" ");

	}
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 10-Dec-2008
* NAME			: OnClear
* DESCRIPTION	: Event handler to clear the writing area and choice 
* ARGUMENTS		: none
* RETURNS		: none 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

void CIMEAppDlg::OnClear()
{
	InvalidateRect(LPCRECT(&m_DrawAreaRect),TRUE);

	m_PrevPenPoint.x = 0;
	m_PrevPenPoint.y = 0;

	m_PenPointsVec.clear();
	m_penstrokes.clear();

	m_DisplayChoice[0].SetWindowTextW(L"");
	m_DisplayChoice[1].SetWindowTextW(L"");
	m_DisplayChoice[2].SetWindowTextW(L"");
	m_DisplayChoice[3].SetWindowTextW(L"");

	UpdateData(false);
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: loadProject
* DESCRIPTION	: Method to load pre-build recognizer 
* ARGUMENTS		: string - project type
* RETURNS		: return 0 - if project successfully loads 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

int CIMEAppDlg::loadProject(string& project)
{
	m_ShapeRecognizer.unloadModelData();

	if(FAILURE == m_ShapeRecognizer.loadModelData(project))
	{
		displayMessage(L"Fail to load project!");
		return FAILURE;
	}

	string mapFile;
	m_IsMappingFileFound = false;

	mapFile = m_lipiRoot + PROJECTS_PATH_STRING +
	        project + PROFILE_PATH_STRING + MAPPING_PATH_STRING;
	if(FAILURE == loadMapping(mapFile))
	{
		displayMessage(L"Mapping file not found!");
	}
	
	string strTitle("Lipi Demo : ");
	strTitle += project.c_str();

	TCHAR title[MAX_PATH]={0};
	mbstowcs(title,strTitle.c_str(),MAX_PATH);

	this->SetWindowTextW(title);
	displayMessage(L" ");

	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: getChoices
* DESCRIPTION	: gets UNICODE/values for respective SRE-ID in resultset 
* ARGUMENTS		: vector<LTKShapeRecoResult> - result set of SRE-ID after recognize
*                 vector<CString> - result set of UNICODE/values for SRE-ID
* RETURNS		: return 0 - after successfull result 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CIMEAppDlg::getChoices(vector<LTKShapeRecoResult>& resultset, 
			   vector<CString>& topChoices)
{
	if(m_SREID_Value.size() == 0 && resultset.size() == 0)
		return FAILURE;

	for(vector<LTKShapeRecoResult>::iterator t = resultset.begin(); 
		t < resultset.end(); t++)
	{
		LTKShapeRecoResult shaperecoresult = (*t);
		for(vector<StructSRE>::iterator sre = m_SREID_Value.begin(); 
			sre < m_SREID_Value.end(); sre++)
		{
			if((*sre).sreid == shaperecoresult.getShapeId() &&
				(*sre).value.size() != 0)
				topChoices.push_back((*sre).value[0]);
		}
	}
	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: loadMapping
* DESCRIPTION	: load mapping files
* ARGUMENTS		: string - map path
* RETURNS		: return 0 - after successfull retrieves values for SRE-ID 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CIMEAppDlg::loadMapping(string& mapFile)
{
	FILE *fp = fopen(mapFile.c_str(),"r");;

	if(NULL == fp)
		return FAILURE;

	m_IsMappingFileFound =true;

	m_SREID_Value.clear();

	TCHAR line[MAX_PATH] = {0};
	while(!feof(fp))
	{
		_fgetts(line,MAX_PATH,fp);
		if(_tcsstr(line,L"[") != 0)
			continue;

		TCHAR *token = _tcstok(line,L"=,");
		
		if(NULL == token)
			continue;

		if(_tcscmp(token,L"font") == 0)
		{
			token = _tcstok(NULL,L"=,");
			m_FontName =  token;
			continue;
		}

		StructSRE sretemp;
		sretemp.sreid = _ttoi(token);
		token = _tcstok(NULL,L"=,");
		while(NULL != token)
		{
			sretemp.value.push_back(token);
			token = _tcstok(NULL,L"=,");
		}
		m_SREID_Value.push_back(sretemp);
	}
	fclose(fp);
	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: OnTimer
* DESCRIPTION	: recognize the strokes after delay time
* ARGUMENTS		: UINT_PTR - Timer event ID
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

void CIMEAppDlg::OnTimer(UINT_PTR nIDEvent)
{

	if(nIDEvent == IDT_RECOGNIZE_TIMER)
	{
		KillTimer(IDT_RECOGNIZE_TIMER);

		if(m_IsProjectLoaded)
		{
			displayMessage(L"Recognizing");
			recognizeStrokes();
		}
		else
		{
			displayMessage(L"Please load project...");
		}

		InvalidateRect(LPCRECT(&m_DrawAreaRect),TRUE);

		m_PrevPenPoint.x = 0;
		m_PrevPenPoint.y = 0;

		m_PenPointsVec.clear();
		m_penstrokes.clear();
			
		displayMessage(L"");
	}

	CDialog::OnTimer(nIDEvent);
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: OnLoadHindi
* DESCRIPTION	: Loads hindi project
* ARGUMENTS		: none
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
void CIMEAppDlg::OnLoadPreBuildRecognizer()
{
	
	displayMessage(L"Loading project");

	OnClear();
	
	m_IsProjectLoaded = false;

	CString projectName;
	string	path = m_lipiRoot + PROJECTS_PATH_STRING;
	TCHAR pathFile[MAX_PATH] = {0};
	mbstowcs(pathFile, path.c_str(), MAX_PATH);

	checkProjectDirectory(pathFile,projectName);

	char projectnm[MAX_PATH] = {0};
	wcstombs(projectnm, projectName.GetBuffer(projectName.GetLength()),MAX_PATH);
	if(loadProject(string(projectnm)) == FAILURE)
		return ;

	if(!m_FontName.IsEmpty())
	{
		m_pFont.DeleteObject();
		m_FontName.Trim();
		TCHAR szFontNm[MAX_PATH] = {0};
		_tcscpy(szFontNm,m_FontName.GetBuffer(m_FontName.GetLength()));
		m_pFont.CreateFont(35,0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		  DEFAULT_PITCH | FF_DONTCARE, szFontNm);
	}
	for(int i = 0; i< MAX_CHOICE;i++)
		m_DisplayChoice[i].SetFont(&m_pFont,TRUE);
	
	m_IsProjectLoaded = true;

	displayMessage(L"");

}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 17-Dec-2008
* NAME			: displayChoice
* DESCRIPTION	: display choice in list
* ARGUMENTS		: none
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
void CIMEAppDlg::displayChoice(HWND hWnd,TCHAR lpDisplayString)
{
	HDC	hdc = ::GetDC(hWnd);
	HFONT font = (HFONT)m_pFont.GetSafeHandle();
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));

	RECT rect;
	::GetClientRect(hWnd, &rect);

	rect.left +=4;
	DrawText(hdc, &lpDisplayString, 1, &rect, DT_TOP); 
	SelectObject(hdc, oldFont);
	::ReleaseDC(hWnd, hdc);
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 19-Dec-2008
* NAME			: checkProjectDirectory
* DESCRIPTION	: check project in <LIPI_ROOT>\projects directory
* ARGUMENTS		: LPTSTR - <LIPI_ROOT>\projects directory
*                 CString - output project name
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CIMEAppDlg::checkProjectDirectory(LPTSTR path,CString& projectName)
{
	CString projectpath;
	projectpath = path + CString("*.*");

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(projectpath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		displayMessage(L"Project not found");
		return FAILURE;
	} 

	if(FILE_ATTRIBUTE_DIRECTORY == FindFileData.dwFileAttributes )
		projectName = FindFileData.cFileName;
	else
	{
		bool bFound = false;
		while(FindNextFile(hFind, &FindFileData))
		{
			if(FILE_ATTRIBUTE_DIRECTORY == FindFileData.dwFileAttributes )
			{
				projectName = FindFileData.cFileName;	
				break;
			}
		}

		displayMessage(L"Project not found");

		return FAILURE;
	} 

	FindClose(hFind);
	return SUCCESS;

}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 19-Dec-2008
* NAME			: displayMessage
* DESCRIPTION	: display message on dialog box [Currently messages are not getting displayed in 
*                                                user interface]
* ARGUMENTS		: CString - message
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
void CIMEAppDlg::displayMessage(const CString& message)
{
	m_DisplayMsg = message;
	UpdateData(false);	
}