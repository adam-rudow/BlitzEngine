// Implements custom assert handler
#include <cstdlib>
#include <wchar.h>
#include <windows.h>
//#include <dxerr.h>


#if _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

bool DbgAssertFunction(bool expr, wchar_t* expr_string, wchar_t* desc, int line_num, wchar_t* file_name)
{
	bool bShouldHalt = !expr;
	if (bShouldHalt)
	{
		//static wchar_t szBuffer[1024];
		//_snwprintf_s(szBuffer, 1024, _TRUNCATE,
		//	L"Assertion Failed!\n\nDescription: %s\nExpression: %s\nFile: %s\nLine: %d\n\nPress Retry to debug.",
		//	desc, expr_string, file_name, line_num);
		//int msgbox = MessageBox(NULL, (LPCSTR)szBuffer, "Assert", MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2);
		//switch (msgbox)
		//{
		//case IDABORT:
		//	std::exit(1);
		//	break;
		//case IDRETRY:
		//	// Do nothing which will cause it to break into the debugger
		//	break;
		//case IDIGNORE:
		//	bShouldHalt = false; // If we're ignoring the assert, the show must go on!
		//	break;
		//}
		//DXTrace(__FILE__, (DWORD)__LINE__, S_OK, "fail", true);
	}

	return bShouldHalt;
}