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
 * FILE DESCR	:  sample application.
 * AUTHOR		:  Balaji MNA
 * Version		:  1.0
 * Date			:  29-Nov-2008
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/
#pragma once
#include "afxwin.h"
#include "ShapeRecognizer.h"
#include "LTKScreenContext.h"

typedef struct sre_id{
	int sreid;
	vector<CString> value;
}StructSRE;
class CIMEAppDlg : public CDialog
{

public:
	CIMEAppDlg(CWnd* pParent = NULL);	
	enum { IDD = IDD_IMEAPP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
	virtual BOOL OnInitDialog();

	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLoadPreBuildRecognizer();
	afx_msg void OnClear();

private:
	CStatic			m_DisplayChoice[4];
	CStatic			m_TopChoice;
	CCommandBar		m_dlgCommandBar;

	vector<vector<CPoint>> m_penstrokes;  
	vector<CPoint>  m_PenPointsVec;
	CPoint          m_PrevPenPoint;
	int             m_NumOfStrokes;
	RECT            m_DrawAreaRect;
	bool            m_IsDrawing;
	bool			m_IsProjectLoaded;
	bool            m_IsMappingFileFound;
	
	CString         m_FontName;
	string			m_lipiRoot;
	CFont			m_pFont;
	CShapeRecognizer	m_ShapeRecognizer;
	vector<StructSRE>	m_SREID_Value;


	void recognizeStrokes(void);
	int loadProject(string& project);
	int getChoices(vector<LTKShapeRecoResult>& resultset, 
			   vector<CString>& topChoices);
	int loadMapping(string& mapFile);
	void displayChoice(HWND hWnd,TCHAR lpDisplayString);
	int checkProjectDirectory(LPTSTR path,CString& projectName);
	void displayMessage(const CString& message);
	CString m_DisplayMsg;
};
