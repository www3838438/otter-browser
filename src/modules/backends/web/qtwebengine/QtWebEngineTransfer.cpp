/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2016 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "QtWebEngineTransfer.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

namespace Otter
{

QtWebEngineTransfer::QtWebEngineTransfer(QWebEngineDownloadItem *item, TransferOptions options, QObject *parent) : Transfer(options, parent),
	m_item(item),
	m_suggestedFileName(QFileInfo(item->path()).fileName())
{
	m_item->accept();
	m_item->setParent(this);

	markAsStarted();

	connect(m_item, &QWebEngineDownloadItem::finished, this, &QtWebEngineTransfer::markAsFinished);
	connect(m_item, &QWebEngineDownloadItem::downloadProgress, this, &QtWebEngineTransfer::handleDownloadProgress);
}

void QtWebEngineTransfer::cancel()
{
	if (m_item)
	{
		m_item->cancel();
		m_item->deleteLater();
	}
	else
	{
		Transfer::cancel();
	}
}

QUrl QtWebEngineTransfer::getSource() const
{
	if (!m_item)
	{
		return Transfer::getSource();
	}

	return m_item->url();
}

QString QtWebEngineTransfer::getSuggestedFileName()
{
	return m_suggestedFileName;
}

QString QtWebEngineTransfer::getTarget() const
{
	if (!m_item)
	{
		return Transfer::getTarget();
	}

	return m_item->path();
}

QMimeType QtWebEngineTransfer::getMimeType() const
{
	if (!m_item)
	{
		return Transfer::getMimeType();
	}

	return QMimeDatabase().mimeTypeForName(m_item->mimeType());
}

qint64 QtWebEngineTransfer::getBytesReceived() const
{
	if (!m_item)
	{
		return Transfer::getBytesReceived();
	}

	return m_item->receivedBytes();
}

qint64 QtWebEngineTransfer::getBytesTotal() const
{
	if (!m_item)
	{
		return Transfer::getBytesTotal();
	}

	return m_item->totalBytes();
}

Transfer::TransferState QtWebEngineTransfer::getState() const
{
	if (!m_item)
	{
		return Transfer::getState();
	}

	switch (m_item->state())
	{
		case QWebEngineDownloadItem::DownloadRequested:
		case QWebEngineDownloadItem::DownloadInProgress:
			return RunningState;
		case QWebEngineDownloadItem::DownloadCompleted:
			return FinishedState;
		case QWebEngineDownloadItem::DownloadCancelled:
			return CancelledState;
		case QWebEngineDownloadItem::DownloadInterrupted:
			return ErrorState;
		default:
			return UnknownState;
	}

	return UnknownState;
}

bool QtWebEngineTransfer::setTarget(const QString &target, bool canOverwriteExisting)
{
	if (!m_item)
	{
		return Transfer::setTarget(target, canOverwriteExisting);
	}

	m_item->setPath(target);

	return true;
}

}
