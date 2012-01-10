/////////////////////////////////////////////////////////////////////////////
// Name:        wx/test.h
// Purpose:     A testing system for wxWidgets
// Author:      Joe Yates
// Created:     2005-11-06
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_test_h__
#define __wx_test_h__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <vector>
#include <wx/exception.h>

namespace wx
{

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test framework

class FailureException : public Exception
	{
	public:
		FailureException(const wxString& sError, ...)
			{
      va_list argptr;
#ifdef _MSC_VER
      WorkaroundVa_start(argptr, sError);
#else
      va_start(argptr, sError);
#endif // def _MSC_VER
			PrintfV(sError, argptr);
      va_end(argptr);
			}
	};

class TestHarness;

class Test
  {
  friend class TestHarness;

  public:
    Test(wxString const& sNewTestName, int nNewTestCount) :
      m_sTestName(sNewTestName), m_nTestCount(nNewTestCount),
      m_nCurrentIndex(0)
      {
      }
    virtual ~Test()
      {}
  
  protected:
    virtual bool Run() = 0;

    wxString GetName()
      {
      return m_sTestName;
      }
    
    // Did exactly the right number of tests get passed, and none fail?
    bool Ok()
      {
      return ((m_nCurrentIndex == m_nTestCount))? true : false;
      }

    int GetTestCount()
      {
      return m_nTestCount;
      }

    int GetCurrentIndex()
      {
      return m_nCurrentIndex;
      }

    bool Pass(int nSubTestIndex)
      {
      m_nCurrentIndex++;
      if(m_nCurrentIndex != nSubTestIndex)
        {
        wxString sError;
				sError.Printf(_T("Test error, wrong sub-test number - %s.Pass(%u), expecting %u"), m_sTestName.c_str(), nSubTestIndex, m_nCurrentIndex);
        throw FailureException(sError);
        }
      if(m_nCurrentIndex > m_nTestCount)
        {
        wxString sError;
				sError.Printf(_T("Test error: too many tests run - expecting max %u, got %u"), m_sTestName.c_str(), m_nTestCount, nSubTestIndex);
        throw FailureException(sError);
        }

			wxLogDebug(wxT("Passed Test %u"), nSubTestIndex);

      return true;
      }

  private:
    wxString  m_sTestName;
    int       m_nTestCount;
    int       m_nCurrentIndex;
  };

class TestHarness : private std::vector<Test *>
  {
  public:
    ~TestHarness()
      {
      for(int i = 0; i < size(); i++)
        {
        delete at(i);
        }
      clear();
      }

  public:
    void Add(Test * ptst)
      {
      push_back(ptst);
      }
    void Run()
      {
      for(int i = 0; i < size(); i++)
        {
        bool bTestedOk = RunTest(at(i));
        }
      }
  
  private:
    bool RunTest(Test * ptst)
      {
			wxString sName = ptst->GetName();
      try
        {
        ptst->Run();
        }
      catch(const FailureException& fle)
				{
				int nFailIndex = ptst->GetCurrentIndex() + 1;
        wxLogDebug(_T("'%s' Failed in test %u: %s"), sName.c_str(), nFailIndex, fle.GetString().c_str());
	
				return false;
				}
      catch(...)
        {
				int nFailIndex = ptst->GetCurrentIndex() + 1;
        wxLogDebug(_T("'%s' Unhandled error thrown in sub-test %u"), sName.c_str(), nFailIndex);

        return false;
        }

			int nTestCount = ptst->GetTestCount();
			int nPassed = ptst->GetCurrentIndex();
			if(ptst->Ok())
				{
				wxLogDebug(_T("'%s' %u out of %u tests passed successfully."), sName.c_str(), nPassed, nTestCount);
				return true;
				}
			else
				{
				wxLogDebug(_T("'%s' Not all the expected tests were run. Expected %u, got %u"), sName.c_str(), nTestCount, nPassed);
				return false;
				}

      return true;
      }
  };

} // namespace wx

#endif // ndef __wx_test_h__
