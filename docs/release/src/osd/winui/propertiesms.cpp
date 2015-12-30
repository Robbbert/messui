// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************
#define WIN32_LEAN_AND_MEAN

#ifndef _MSC_VER
#define NONAMELESSUNION 1
#endif

#include <windows.h>
#include <string.h>
#include <commctrl.h>
#include <commdlg.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tchar.h>

#include "emu.h"
#include "emuopts.h"
#include "mui_opts.h"
#include "image.h"
#include "screenshot.h"
#include "datamap.h"
#include "bitmask.h"
#include "winui.h"
#include "directories.h"
#include "mui_util.h"
#include "resourcems.h"
#include "propertiesms.h"
#include "optionsms.h"
#include "msuiutil.h"
#include "strconv.h"
#include "winutf8.h"
#include "machine/ram.h"
#include "drivenum.h"

static BOOL SoftwareDirectories_OnInsertBrowse(HWND hDlg, BOOL bBrowse, LPCTSTR lpItem);
static BOOL SoftwareDirectories_OnDelete(HWND hDlg);
static BOOL SoftwareDirectories_OnBeginLabelEdit(HWND hDlg, NMHDR* pNMHDR);
static BOOL SoftwareDirectories_OnEndLabelEdit(HWND hDlg, NMHDR* pNMHDR);

extern BOOL BrowseForDirectory(HWND hwnd, LPCTSTR pStartDir, TCHAR* pResult);
BOOL g_bModifiedSoftwarePaths = FALSE;



static void MarkChanged(HWND hDlg)
{
	HWND hCtrl;

	/* fake a CBN_SELCHANGE event from IDC_SIZES to force it to be changed */
	hCtrl = GetDlgItem(hDlg, IDC_SIZES);
	PostMessage(hDlg, WM_COMMAND, (CBN_SELCHANGE << 16) | IDC_SIZES, (LPARAM) hCtrl);
}


static void AppendList(HWND hList, LPCTSTR lpItem, int nItem)
{
	LV_ITEM Item;
	HRESULT res;
	memset(&Item, 0, sizeof(LV_ITEM));
	Item.mask = LVIF_TEXT;
	Item.pszText = (LPTSTR) lpItem;
	Item.iItem = nItem;
	res = ListView_InsertItem(hList, &Item);
	res++;
}


static BOOL SoftwareDirectories_OnInsertBrowse(HWND hDlg, BOOL bBrowse, LPCTSTR lpItem)
{
	int nItem;
	TCHAR inbuf[MAX_PATH];
	TCHAR outbuf[MAX_PATH];
	HWND hList;
	LPTSTR lpIn;
	BOOL res;

	g_bModifiedSoftwarePaths = TRUE;

	hList = GetDlgItem(hDlg, IDC_DIR_LIST);
	nItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED);

	if (nItem == -1)
		return FALSE;

	/* Last item is placeholder for append */
	if (nItem == ListView_GetItemCount(hList) - 1)
		bBrowse = FALSE;

	if (!lpItem)
	{
		if (bBrowse)
		{
			ListView_GetItemText(hList, nItem, 0, inbuf, ARRAY_LENGTH(inbuf));
			lpIn = inbuf;
		}
		else
			lpIn = NULL;

		if (!BrowseForDirectory(hDlg, lpIn, outbuf))
			return FALSE;

		lpItem = outbuf;
	}

	AppendList(hList, lpItem, nItem);
	if (bBrowse)
		res = ListView_DeleteItem(hList, nItem+1);
	MarkChanged(hDlg);
	res++;
	return TRUE;
}



static BOOL SoftwareDirectories_OnDelete(HWND hDlg)
{
	int	nCount;
	int	nSelect;
	int	nItem;
	HWND	hList = GetDlgItem(hDlg, IDC_DIR_LIST);
	BOOL res;

	g_bModifiedSoftwarePaths = TRUE;

	nItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED | LVNI_ALL);

	if (nItem == -1)
		return FALSE;

	/* Don't delete "Append" placeholder. */
	if (nItem == ListView_GetItemCount(hList) - 1)
		return FALSE;

	res = ListView_DeleteItem(hList, nItem);

	nCount = ListView_GetItemCount(hList);
	if (nCount <= 1)
		return FALSE;

	/* If the last item was removed, select the item above. */
	if (nItem == nCount - 1)
		nSelect = nCount - 2;
	else
		nSelect = nItem;

	ListView_SetItemState(hList, nSelect, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	MarkChanged(hDlg);
	res++;
	return TRUE;
}



static BOOL SoftwareDirectories_OnBeginLabelEdit(HWND hDlg, NMHDR* pNMHDR)
{
	BOOL          bResult = FALSE;
	NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;
	LVITEM*       pItem = &pDispInfo->item;
	HWND          hList = GetDlgItem(hDlg, IDC_DIR_LIST);

	/* Last item is placeholder for append */
	if (pItem->iItem == ListView_GetItemCount(hList) - 1)
	{
		HWND hEdit = (HWND) (FPTR) SendMessage(hList, LVM_GETEDITCONTROL, 0, 0);
		win_set_window_text_utf8(hEdit, "");
	}

	return bResult;
}



static BOOL SoftwareDirectories_OnEndLabelEdit(HWND hDlg, NMHDR* pNMHDR)
{
	BOOL		  bResult = FALSE;
	NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;
	LVITEM*	   pItem = &pDispInfo->item;

	if (pItem->pszText != NULL)
	{
		struct _stat file_stat;

		/* Don't allow empty entries. */
		if (!_tcscmp(pItem->pszText, TEXT("")))
			return FALSE;

		/* Check validity of edited directory. */
		if ((_tstat(pItem->pszText, &file_stat) == 0) && (file_stat.st_mode & S_IFDIR))
			bResult = TRUE;
		else
		if (win_message_box_utf8(NULL, "Folder does not exist, continue anyway?", MAMEUINAME, MB_OKCANCEL) == IDOK)
			bResult = TRUE;
	}

	if (bResult == TRUE)
		SoftwareDirectories_OnInsertBrowse(hDlg, TRUE, pItem->pszText);

	return bResult;
}



BOOL PropSheetFilter_Config(const machine_config *drv, const game_driver *gamedrv)
{
	ram_device_iterator iter(drv->root_device());
	return (iter.first()!=NULL) || DriverHasDevice(gamedrv, IO_PRINTER);
}



INT_PTR CALLBACK GameMessOptionsProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR rc = 0;
	BOOL bHandled = FALSE;

	switch (Msg)
	{
	case WM_NOTIFY:
		switch (((NMHDR *) lParam)->code)
		{
		case LVN_ENDLABELEDIT:
			rc = SoftwareDirectories_OnEndLabelEdit(hDlg, (NMHDR *) lParam);
			bHandled = TRUE;
			break;

		case LVN_BEGINLABELEDIT:
			rc = SoftwareDirectories_OnBeginLabelEdit(hDlg, (NMHDR *) lParam);
			bHandled = TRUE;
			break;
		}
	}

	if (!bHandled)
		rc = GameOptionsProc(hDlg, Msg, wParam, lParam);

	return rc;
}



BOOL MessPropertiesCommand(HWND hWnd, WORD wNotifyCode, WORD wID, BOOL *changed)
{
	BOOL handled = TRUE;

	switch(wID)
	{
		case IDC_DIR_BROWSE:
			if (wNotifyCode == BN_CLICKED)
				*changed = SoftwareDirectories_OnInsertBrowse(hWnd, TRUE, NULL);
			break;

		case IDC_DIR_INSERT:
			if (wNotifyCode == BN_CLICKED)
				*changed = SoftwareDirectories_OnInsertBrowse(hWnd, FALSE, NULL);
			break;

		case IDC_DIR_DELETE:
			if (wNotifyCode == BN_CLICKED)
				*changed = SoftwareDirectories_OnDelete(hWnd);
			break;

		default:
			handled = FALSE;
			break;
	}
	return handled;
}



