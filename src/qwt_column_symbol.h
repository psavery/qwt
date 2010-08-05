/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_COLUMN_SYMBOL_H
#define QWT_COLUMN_SYMBOL_H

#include "qwt_global.h"
#include "qwt_double_interval.h"
#include <qpen.h>
#include <qsize.h>
#include <qrect.h>

class QPainter;
class QPalette;
class QRect;
class QwtText;

/*!
    \brief Directed rectangle representing bounding rectangle und orientation
    of a column.
*/
class QWT_EXPORT QwtColumnRect
{
public:
    //! Direction of the column
    enum Direction
    {
        LeftToRight,
        RightToLeft,
        BottomToTop,
        TopToBottom
    };

    //! Build an rectangle with invalid intervals directed BottomToTop.
    QwtColumnRect():
        direction(BottomToTop)
    {
    }

    //! \return A normalized QRect built from the intervals
    QRectF toRect() const
    {
        return QRectF(hInterval.minValue(), vInterval.minValue(),
            hInterval.maxValue() - hInterval.minValue(),
            vInterval.maxValue() - vInterval.minValue() ).normalized();
    }

    //! \return Orientation 
    Qt::Orientation orientation() const
    {
        if ( direction == LeftToRight || direction == RightToLeft )
            return Qt::Horizontal;

        return Qt::Vertical;
    }

    //! Interval for the horizontal coordinates
    QwtDoubleInterval hInterval;

    //! Interval for the vertical coordinates
    QwtDoubleInterval vInterval;

    //! Direction
    Direction direction;
};

//! A drawing primitive for columns
class QWT_EXPORT QwtColumnSymbol
{
public:
    /*!
      Style

      - NoSymbol\n
        No Style. The symbol cannot be drawn.

      - Box\n
        The column is painted with a frame depending on the frameStyle()
        and lineWidth() using the palette().

      - UserSymbol\n
        Styles >= UserSymbol are reserved for derived
        classes of QwtColumnSymbol that overload draw() with
        additional application specific symbol types.

      \sa setStyle(), style()
    */
    enum Style 
    { 
        NoSymbol = -1, 

        Box, 

        UserSymbol = 1000 
    };
   
    /*!
      Frame Style used in Box style().

      - NoFrame
      - Plain
      - Raised

      \sa Style, setFrameStyle(), frameStyle(), setStyle(), setPalette()
     */
    enum FrameStyle
    {
        NoFrame,

        Plain,
        Raised
    };

public:
    QwtColumnSymbol(Style = NoSymbol);
    virtual ~QwtColumnSymbol();
    
    void setFrameStyle(FrameStyle style);
    FrameStyle frameStyle() const;

    void setLineWidth(int width);
    int lineWidth() const;
    
    void setPalette(const QPalette &);
    const QPalette &palette() const;

    void setStyle(Style);
    Style style() const;
    
    virtual void draw(QPainter *, const QwtColumnRect &) const;

protected:
    void drawBox(QPainter *, const QwtColumnRect &) const;

private:
    class PrivateData;
    PrivateData* d_data;
};

#endif