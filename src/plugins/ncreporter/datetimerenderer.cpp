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

    if(itemdata.toUInt() != 0)
    {
        QDateTime date(QDateTime::fromTime_t(itemdata.toUInt()));
        painter->drawText( rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, date.toString("hh:mm:ss"));
    }
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
