#ifndef __DATE_TIME_ITEM_RENDERER_H__
#define __DATE_TIME_ITEM_RENDERER_H__

#include "ncreportabstractitemrendering.h"

class TimeItemRendering : public NCReportAbstractItemRendering
{
public:
    TimeItemRendering();
    ~TimeItemRendering();

	void paintItem( QPainter* painter, NCReportOutput* output, const QRectF& rect, const QString& itemdata );
};


class DateItemRendering : public NCReportAbstractItemRendering
{
public:
    DateItemRendering();
    ~DateItemRendering();

    void paintItem( QPainter* painter, NCReportOutput* output, const QRectF& rect, const QString& itemdata );
};


#endif //__DATE_TIME_ITEM_RENDERER_H__
