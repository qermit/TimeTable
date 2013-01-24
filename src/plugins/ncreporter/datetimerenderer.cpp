/****************************************************************************
* 
*  Copyright (C) 2002-2008 Helta Kft. / NociSoft Software Solutions
*  All rights reserved.
*  Author: Norbert Szabo
*  E-mail: nszabo@helta.hu, info@nocisoft.com
*  Web: www.nocisoft.com
* 
*  This file is part of the NCReport reporting software
* 
*  Licensees holding a valid NCReport License Agreement may use this
*  file in accordance with the rights, responsibilities, and obligations
*  contained therein. Please consult your licensing agreement or contact
*  nszabo@helta.hu if any conditions of this licensing are not clear
*  to you.
* 
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* 
****************************************************************************/
#include "datetimerenderer.h"
#include "ncreportoutput.h"

#include <QPainter>
#include <QDate>

TimeItemRendering::TimeItemRendering()
{
}

TimeItemRendering::~TimeItemRendering()
{
}

void TimeItemRendering::paintItem(QPainter* painter, NCReportOutput* /*output*/, const QRectF& rect, const QString& itemdata)
{
	painter->setPen( QPen(Qt::black) );
	painter->setBrush( Qt::NoBrush );
	
    painter->setFont( QFont("Arial",10) );

    QDateTime date(QDateTime::fromTime_t(itemdata.toUInt()));
    painter->drawText( rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, date.toString("hh:mm:ss"));
}



DateItemRendering::DateItemRendering()
{
}

DateItemRendering::~DateItemRendering()
{
}

void DateItemRendering::paintItem(QPainter* painter, NCReportOutput* /*output*/, const QRectF& rect, const QString& itemdata)
{
    painter->setPen( QPen(Qt::black) );
    painter->setBrush( Qt::NoBrush );

    painter->setFont( QFont("Arial",10) );

    QDateTime date(QDateTime::fromTime_t(itemdata.toUInt()));
    painter->drawText( rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, date.toString("dd MMM yyyy"));
}



