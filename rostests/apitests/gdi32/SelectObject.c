/*
 * PROJECT:         ReactOS api tests
 * LICENSE:         GPL - See COPYING in the top level directory
 * PURPOSE:         Test for SelectObject
 * PROGRAMMERS:     Timo Kreuzer
 */

#include <stdio.h>
#include <wine/test.h>
#include <windows.h>
#include <winddi.h>
#include <reactos/win32k/ntgdityp.h>
#include <reactos/win32k/ntgdihdl.h>

#define TEST(x) ok(x, #x"\n")
#define RTEST(x) ok(x, #x"\n")

void Test_SelectObject()
{
	HGDIOBJ hOldObj, hNewObj;
	HDC hScreenDC, hDC, hDC2;
//	PGDI_TABLE_ENTRY pEntry;
//	PDC_ATTR pDc_Attr;
//	HANDLE hcmXform;
	BYTE bmBits[4] = {0};

	hScreenDC = GetDC(NULL);
	ok(hScreenDC != NULL, "GetDC failed. Skipping tests.\n");
	if (hScreenDC == NULL) return;
	hDC = CreateCompatibleDC(hScreenDC);
	ok(hDC != NULL, "CreateCompatibleDC failed. Skipping tests.\n");
	if (hDC == NULL) return;

	/* Get the Dc_Attr for later testing */
//	pEntry = &GdiHandleTable[GDI_HANDLE_GET_INDEX(hDC)];
//	pDc_Attr = pEntry->UserData;
//	ok(pDc_Attr != NULL, "Skipping tests.\n");
//	if (pDc_Attr == NULL) return;

	/* Test incomplete dc handle doesn't work */
	SetLastError(ERROR_SUCCESS);
	hNewObj = GetStockObject(GRAY_BRUSH);
	hOldObj = SelectObject((HDC)GDI_HANDLE_GET_INDEX(hDC), hNewObj);
	RTEST(GetLastError() == ERROR_INVALID_HANDLE);
	RTEST(hOldObj == NULL);
//	RTEST(pDc_Attr->hbrush == GetStockObject(WHITE_BRUSH));
	SelectObject(hDC, hOldObj);

	/* Test incomplete hobj handle works */
	hNewObj = GetStockObject(GRAY_BRUSH);
	hOldObj = SelectObject(hDC, (HGDIOBJ)GDI_HANDLE_GET_INDEX(hNewObj));
	RTEST(hOldObj == GetStockObject(WHITE_BRUSH));
//	RTEST(pDc_Attr->hbrush == hNewObj);
	SelectObject(hDC, hOldObj);

	/* Test wrong hDC handle type */
	SetLastError(ERROR_SUCCESS);
	hNewObj = GetStockObject(GRAY_BRUSH);
	hDC2 = (HDC)((UINT_PTR)hDC & ~GDI_HANDLE_TYPE_MASK);
	hDC2 = (HDC)((UINT_PTR)hDC2 | GDI_OBJECT_TYPE_PEN);
	hOldObj = SelectObject(hDC2, hNewObj);
	RTEST(GetLastError() == ERROR_INVALID_HANDLE);
	RTEST(hOldObj == NULL);
//	RTEST(pDc_Attr->hbrush == GetStockObject(WHITE_BRUSH));

	/* Test wrong hobj handle type */
	SetLastError(ERROR_SUCCESS);
	hNewObj = GetStockObject(GRAY_BRUSH);
	hNewObj = (HGDIOBJ)((UINT_PTR)hNewObj & ~GDI_HANDLE_TYPE_MASK);
	hNewObj = (HGDIOBJ)((UINT_PTR)hNewObj | GDI_OBJECT_TYPE_PEN);
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(GetLastError() == ERROR_SUCCESS);
	RTEST(hOldObj == NULL);
//	RTEST(pDc_Attr->hbrush == GetStockObject(WHITE_BRUSH));

	SetLastError(ERROR_SUCCESS);
	hNewObj = (HGDIOBJ)0x00761234;
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(hOldObj == NULL);
	RTEST(GetLastError() == ERROR_SUCCESS);
	SelectObject(hDC, hOldObj);

	/* Test DC */
	SetLastError(ERROR_SUCCESS);
	hOldObj = SelectObject(hDC, hScreenDC);
	RTEST(hOldObj == NULL);
	TEST(GetLastError() == ERROR_SUCCESS);

	/* Test REGION */
	SetLastError(ERROR_SUCCESS);
	hNewObj = CreateRectRgn(0,0,0,0);
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST((UINT_PTR)hOldObj == NULLREGION);
	DeleteObject(hNewObj);

	hNewObj = CreateRectRgn(0,0,10,10);
	RTEST((UINT_PTR)SelectObject(hDC, hNewObj) == SIMPLEREGION);
	hOldObj = CreateRectRgn(5,5,20,20);
	RTEST(CombineRgn(hNewObj, hNewObj, hOldObj, RGN_OR) == COMPLEXREGION);
	DeleteObject(hOldObj);
	RTEST((UINT_PTR)SelectObject(hDC, hNewObj) == SIMPLEREGION); // ??? Why this?
	DeleteObject(hNewObj);
//	TEST(IsHandleValid(hNewObj) == TRUE);

	RTEST(GetLastError() == ERROR_SUCCESS);

	/* Test BITMAP */
	hNewObj = CreateBitmap(2, 2, 1, 1, &bmBits);
	ok(hNewObj != NULL, "CreateBitmap failed. Skipping tests.\n");
	if (hNewObj == NULL) return;
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(GDI_HANDLE_GET_TYPE(hOldObj) == GDI_OBJECT_TYPE_BITMAP);
	hOldObj = SelectObject(hDC, hOldObj);
	RTEST(hOldObj == hNewObj);

	/* Test CLIOBJ */

	/* Test PATH */

	/* Test PALETTE */
	SetLastError(ERROR_SUCCESS);
	hNewObj = GetStockObject(DEFAULT_PALETTE);
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(hOldObj == NULL);
	RTEST(GetLastError() == ERROR_INVALID_FUNCTION);

	/* Test COLORSPACE */

	/* Test FONT */

	/* Test PFE */

	/* Test BRUSH */
	hNewObj = GetStockObject(GRAY_BRUSH);
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(hOldObj == GetStockObject(WHITE_BRUSH));
//	RTEST(pDc_Attr->hbrush == hNewObj);
	RTEST(GDI_HANDLE_GET_TYPE(hOldObj) == GDI_OBJECT_TYPE_BRUSH);
	SelectObject(hDC, hOldObj);

	/* Test DC_BRUSH */
	hNewObj = GetStockObject(DC_BRUSH);
	hOldObj = SelectObject(hDC, hNewObj);
//	RTEST(pDc_Attr->hbrush == hNewObj);
	SelectObject(hDC, hOldObj);

	/* Test BRUSH color xform */
//	hcmXform = (HANDLE)pDc_Attr->hcmXform;


	/* Test EMF */

	/* test METAFILE */

	/* Test ENHMETAFILE */

	/* Test PEN */
	hNewObj = GetStockObject(GRAY_BRUSH);
	hOldObj = SelectObject(hDC, hNewObj);
	RTEST(hOldObj == GetStockObject(WHITE_BRUSH));
//	RTEST(pDc_Attr->hbrush == hNewObj);
	RTEST(GDI_HANDLE_GET_TYPE(hOldObj) == GDI_OBJECT_TYPE_BRUSH);
	SelectObject(hDC, hOldObj);


	/* Test EXTPEN */

	/* Test METADC */
}

START_TEST(SelectObject)
{
    Test_SelectObject();
}

