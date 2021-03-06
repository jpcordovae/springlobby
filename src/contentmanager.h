/*
 * contentmanager.h
 *
 *  Created on: 11 ���� 2015 �.
 *      Author: ������
 */

#ifndef SRC_CONTENTMANAGER_H_
#define SRC_CONTENTMANAGER_H_

#include <vector>

#include <wx/thread.h>
#include <wx/string.h>

class ContentDownloadRequest;
class IBattle;
class Exception;
class IDownload;
class DownloadInfo;

class ContentManager {

public:
	static ContentManager* Instance();
	static void Release();

	bool IsNewApplicationVersionAvailable();
	bool UpdateApplication();
	wxString GetLatestApplicationVersionAvailable();

	bool IsHavingSpringVersion(const wxString& engineString, const wxString& versionString);
	ContentDownloadRequest WhatContentForBattleIsRequired(const IBattle& battle);
	bool DownloadContent(const ContentDownloadRequest& request);

	void OnDownloadStarted(IDownload* download);
	void OnDownloadFinished(IDownload* download);

	bool IsContentAlreadyBeingDownloaded(const wxString& name);

private:
	ContentManager();
	virtual ~ContentManager();

private:
	wxString latestApplicationVersionAvailable;
	std::vector<DownloadInfo*> downloadsList;
	wxMutex mutex;

private:
	static ContentManager* m_Instance;
};

#endif /* SRC_CONTENTMANAGER_H_ */
