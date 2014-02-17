/****************************************************************************
** Copyright (c) 2000-2003 Wayne Roth
** Copyright (c) 2004-2007 Stefan Sander
** Copyright (c) 2007 Michal Policht
** Copyright (c) 2008 Brandon Fosdick
** Copyright (c) 2009-2010 Liam Staskawicz
** Copyright (c) 2011 Debao Zhang
** Copyright (c) 2012 Doug Brown
** All right reserved.
** Web: http://code.google.com/p/qextserialport/
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef _QEXTSERIALENUMERATOR_P_H_
#define _QEXTSERIALENUMERATOR_P_H_

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QESP API.  It exists for the convenience
// of other QESP classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qextserialenumerator.h"

#include <QtCore/qt_windows.h>

class QextSerialRegistrationWidget;
class QextSerialEnumeratorPrivate
{
    Q_DECLARE_PUBLIC(QextSerialEnumerator)
public:
    QextSerialEnumeratorPrivate(QextSerialEnumerator *enumrator);
    ~QextSerialEnumeratorPrivate();
    void platformSpecificInit();
    void platformSpecificDestruct();

    static QStringList getPorts_sys();
    bool setUpNotifications_sys(bool setup);

    LRESULT onDeviceChanged(WPARAM wParam, LPARAM lParam);
    bool matchAndDispatchChangedDevice(const QString &deviceID, const GUID &guid, WPARAM wParam);
    QextSerialRegistrationWidget *notificationWidget;

private:
    QextSerialEnumerator *q_ptr;
};

#endif //_QEXTSERIALENUMERATOR_P_H_
