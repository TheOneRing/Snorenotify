/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2015  Hannah von Reth <vonreth@kde.org>

    SnoreNotify is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SnoreNotify is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with SnoreNotify.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "utils.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <winuser.h>
#endif

#include <QRegExp>
#include <QMutex>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QBuffer>

using namespace Snore;

Utils::Utils(QObject *parent):
    QObject(parent)
{

}

Utils::~Utils()
{

}

void Utils::bringWindowToFront(const QWindow *window, bool focus)
{
#ifdef Q_OS_WIN
    bringWindowToFront((HWND)window->winId(), focus);
#else
    // TODO: implement
    Q_UNUSED(window);
    Q_UNUSED(focus);
#endif
}

#ifdef Q_OS_WIN
void Utils::bringWindowToFront(HWND wid, bool focus)
{
    int active = attatchToActiveProcess();
    SetForegroundWindow(wid);
    if (focus) {
        SetFocus(wid);
    }
    detatchActiveProcess(active);
}
#endif

void Utils::raiseWindowToFront(const QWindow *window)
{
    // Looks like qt is handling it on linux.
#ifdef Q_OS_WIN
    int active = attatchToActiveProcess();
    SetWindowPos((HWND)window->winId(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    detatchActiveProcess(active);
#else
    Q_UNUSED(window);
#endif
}

#define HTML_REPLACE(STRING, PATTERN){\
        static QRegExp regexp(QLatin1String(PATTERN));\
        STRING = STRING.replace(regexp, QStringLiteral("\\1"));\
    }\
     
QString Utils::normalizeMarkup(QString string, MarkupFlags tags)
{
    static QMutex mutex;
    if (tags == AllMarkup) {
        return string;
    } else if (tags == NoMarkup) {
        return QTextDocumentFragment::fromHtml(string).toPlainText();
    }

    QMutexLocker lock(&mutex);
    if (~tags & Utils::Break) {
        static QRegExp br(QLatin1String("<br>"));
        string = string.replace(br, QStringLiteral("\n"));
    }
    if (~tags & Utils::Href) {
        HTML_REPLACE(string, "<a href=.*>([^<]*)</a>");
    }
    if (~tags & Utils::Italic) {
        HTML_REPLACE(string, "<i>([^<]*)</i>");
    }
    if (~tags & Utils::Bold) {
        HTML_REPLACE(string, "<b>([^<]*)</b>");
    }
    if (~tags & Utils::Underline) {
        HTML_REPLACE(string, "<u>([^<]*)</u>");
    }
    if (~tags & Utils::Font) {
        HTML_REPLACE(string, "<font.*>([^<]*)</font>");
    }
    return string;
}

QByteArray Utils::dataFromImage(const QImage &image)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QBuffer::WriteOnly);
    image.save(&buffer, "PNG");
    return data;
}

#ifdef Q_OS_WIN
int Utils::attatchToActiveProcess()
{
    int idActive = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    return AttachThreadInput(GetCurrentThreadId(), idActive, TRUE) ? idActive : -1;
}

void Utils::detatchActiveProcess(int idActive)
{
    if (idActive != -1) {
        AttachThreadInput(GetCurrentThreadId(), idActive, FALSE);
    }
}

#endif
