/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_renderer.h"
#include "qwt_plot.h"
#include "qwt_painter.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_layout.h"
#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_dyngrid_layout.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_engine.h"
#include "qwt_text.h"
#include "qwt_text_label.h"
#include "qwt_math.h"
#include <qpainter.h>
#include <qpaintengine.h>
#include <qtransform.h>
#include <qprinter.h>
#include <qimagewriter.h>
#include <qfileinfo.h>
#ifndef QWT_NO_SVG
#ifdef QT_SVG_LIB
#include <qsvggenerator.h>
#endif
#endif

class QwtPlotRenderer::PrivateData
{
public:
    PrivateData():
        discardFlags( QwtPlotRenderer::DiscardBackground ),
        layoutFlags( QwtPlotRenderer::DefaultLayout )
    {
    }

    QwtPlotRenderer::DiscardFlags discardFlags;
    QwtPlotRenderer::LayoutFlags layoutFlags;
};

/*! 
   Constructor
   \param parent Parent object
*/
QwtPlotRenderer::QwtPlotRenderer( QObject *parent ):
    QObject( parent )
{
    d_data = new PrivateData;
}

//! Destructor
QwtPlotRenderer::~QwtPlotRenderer()
{
    delete d_data;
}

/*!
  Change a flag, indicating what to discard from rendering

  \param flag Flag to change
  \param on On/Off

  \sa DiscardFlag, testDiscardFlag(), setDiscardFlags(), discardFlags()
*/
void QwtPlotRenderer::setDiscardFlag( DiscardFlag flag, bool on )
{
    if ( on )
        d_data->discardFlags |= flag;
    else
        d_data->discardFlags &= ~flag;
}

/*!
  Check if a flag is set.

  \param flag Flag to be tested
  \sa DiscardFlag, setDiscardFlag(), setDiscardFlags(), discardFlags()
*/
bool QwtPlotRenderer::testDiscardFlag( DiscardFlag flag ) const
{
    return d_data->discardFlags & flag;
}

/*!
  Set the flags, indicating what to discard from rendering

  \param flags Flags
  \sa DiscardFlag, setDiscardFlag(), testDiscardFlag(), discardFlags()
*/
void QwtPlotRenderer::setDiscardFlags( DiscardFlags flags )
{
    d_data->discardFlags = flags;
}

/*!
  \return Flags, indicating what to discard from rendering
  \sa DiscardFlag, setDiscardFlags(), setDiscardFlag(), testDiscardFlag()
*/
QwtPlotRenderer::DiscardFlags QwtPlotRenderer::discardFlags() const
{
    return d_data->discardFlags;
}

/*!
  Change a layout flag

  \param flag Flag to change
  \param on On/Off

  \sa LayoutFlag, testLayoutFlag(), setLayoutFlags(), layoutFlags()
*/
void QwtPlotRenderer::setLayoutFlag( LayoutFlag flag, bool on )
{
    if ( on )
        d_data->layoutFlags |= flag;
    else
        d_data->layoutFlags &= ~flag;
}

/*!
  Check if a flag is set.

  \param flag Flag to be tested
  \sa LayoutFlag, setLayoutFlag(), setLayoutFlags(), layoutFlags()
*/
bool QwtPlotRenderer::testLayoutFlag( LayoutFlag flag ) const
{
    return d_data->layoutFlags & flag;
}

/*!
  Set the layout flags

  \param flags Flags
  \sa LayoutFlag, setLayoutFlag(), testLayoutFlag(), layoutFlags()
*/
void QwtPlotRenderer::setLayoutFlags( LayoutFlags flags )
{
    d_data->layoutFlags = flags;
}

/*!
  \return Layout flags
  \sa LayoutFlag, setLayoutFlags(), setLayoutFlag(), testLayoutFlag()
*/
QwtPlotRenderer::LayoutFlags QwtPlotRenderer::layoutFlags() const
{
    return d_data->layoutFlags;
}

/*!
  Render a plot to a file

  The format of the document will be autodetected from the
  suffix of the filename.

  \param plot Plot widget
  \param fileName Path of the file, where the document will be stored
  \param sizeMM Size for the document in millimeters.
  \param resolution Resolution in dots per Inch (dpi)
*/
void QwtPlotRenderer::renderDocument( QwtPlot *plot,
    const QString &fileName, const QSizeF &sizeMM, int resolution )
{
    renderDocument( plot, fileName,
        QFileInfo( fileName ).suffix(), sizeMM, resolution );
}

/*!
  Render a plot to a file

  Supported formats are:

  - pdf\n
  - ps\n
  - svg\n
  - all image formats supported by Qt, see QImageWriter::supportedImageFormats()

  \param plot Plot widget
  \param fileName Path of the file, where the document will be stored
  \param format Format for the document
  \param sizeMM Size for the document in millimeters.
  \param resolution Resolution in dots per Inch (dpi)

  \sa renderTo(), render(), QwtPainter::setRoundingAlignment()
*/
void QwtPlotRenderer::renderDocument( QwtPlot *plot,
    const QString &fileName, const QString &format,
    const QSizeF &sizeMM, int resolution )
{
    if ( plot == NULL || sizeMM.isEmpty() || resolution <= 0 )
        return;

    QString title = plot->title().text();
    if ( title.isEmpty() )
        title = "Plot Document";

    const double mmToInch = 1.0 / 25.4;
    const QSizeF size = sizeMM * mmToInch * resolution;

    const QRectF documentRect( 0.0, 0.0, size.width(), size.height() );

    const QString fmt = format.toLower();
    if ( format == "pdf" || format == "ps" )
    {
        QPrinter printer;
        printer.setFullPage( true );
        printer.setPaperSize( sizeMM, QPrinter::Millimeter );
        printer.setDocName( title );
        printer.setOutputFileName( fileName );
        printer.setOutputFormat( ( format == "pdf" )
            ? QPrinter::PdfFormat : QPrinter::PostScriptFormat );
        printer.setResolution( resolution );

        QPainter painter( &printer );
        render( plot, &painter, documentRect );
    }
#ifndef QWT_NO_SVG
#ifdef QT_SVG_LIB
#if QT_VERSION >= 0x040500
    else if ( format == "svg" )
    {
        QSvgGenerator generator;
        generator.setTitle( title );
        generator.setFileName( fileName );
        generator.setResolution( resolution );
        generator.setViewBox( documentRect );

        QPainter painter( &generator );
        render( plot, &painter, documentRect );
    }
#endif
#endif
#endif
    else
    {
        if ( QImageWriter::supportedImageFormats().indexOf(
            format.toLatin1() ) >= 0 )
        {
            const QRect imageRect = documentRect.toRect();
            const int dotsPerMeter = qRound( resolution * mmToInch * 1000.0 );

            QImage image( imageRect.size(), QImage::Format_ARGB32 );
            image.setDotsPerMeterX( dotsPerMeter );
            image.setDotsPerMeterY( dotsPerMeter );
            image.fill( QColor( Qt::white ).rgb() );

            QPainter painter( &image );
            render( plot, &painter, imageRect );
            painter.end();

            image.save( fileName, format.toLatin1() );
        }
    }
}

/*!
  \brief Render the plot to a \c QPaintDevice

  This function renders the contents of a QwtPlot instance to
  \c QPaintDevice object. The target rectangle is derived from
  its device metrics.

  \param plot Plot to be rendered
  \param paintDevice device to paint on, f.e a QImage

  \sa renderDocument(), render(), QwtPainter::setRoundingAlignment()
*/

void QwtPlotRenderer::renderTo(
    QwtPlot *plot, QPaintDevice &paintDevice ) const
{
    int w = paintDevice.width();
    int h = paintDevice.height();

    QPainter p( &paintDevice );
    render( plot, &p, QRectF( 0, 0, w, h ) );
}

/*!
  \brief Render the plot to a QPrinter

  This function renders the contents of a QwtPlot instance to
  \c QPaintDevice object. The size is derived from the printer
  metrics.

  \param plot Plot to be rendered
  \param printer Printer to paint on

  \sa renderDocument(), render(), QwtPainter::setRoundingAlignment()
*/

void QwtPlotRenderer::renderTo(
    QwtPlot *plot, QPrinter &printer ) const
{
    int w = printer.width();
    int h = printer.height();

    QRectF rect( 0, 0, w, h );
    double aspect = rect.width() / rect.height();
    if ( ( aspect < 1.0 ) )
        rect.setHeight( aspect * rect.width() );

    QPainter p( &printer );
    render( plot, &p, rect );
}

#ifndef QWT_NO_SVG
#ifdef QT_SVG_LIB
#if QT_VERSION >= 0x040500

/*!
  \brief Render the plot to a QSvgGenerator

  If the generator has a view box, the plot will be rendered into it.
  If it has no viewBox but a valid size the target coordinates
  will be (0, 0, generator.width(), generator.height()). Otherwise
  the target rectangle will be QRectF(0, 0, 800, 600);

  \param plot Plot to be rendered
  \param generator SVG generator
*/
void QwtPlotRenderer::renderTo(
    QwtPlot *plot, QSvgGenerator &generator ) const
{
    QRectF rect = generator.viewBoxF();
    if ( rect.isEmpty() )
        rect.setRect( 0, 0, generator.width(), generator.height() );

    if ( rect.isEmpty() )
        rect.setRect( 0, 0, 800, 600 ); // something

    QPainter p( &generator );
    render( plot, &p, rect );
}
#endif
#endif
#endif

/*!
  Paint the contents of a QwtPlot instance into a given rectangle.

  \param plot Plot to be rendered
  \param painter Painter
  \param plotRect Bounding rectangle

  \sa renderDocument(), renderTo(), QwtPainter::setRoundingAlignment()
*/
void QwtPlotRenderer::render( QwtPlot *plot,
    QPainter *painter, const QRectF &plotRect ) const
{
    int axisId;

    if ( painter == 0 || !painter->isActive() ||
            !plotRect.isValid() || plot->size().isNull() )
        return;

    if ( !( d_data->discardFlags & DiscardBackground ) )
    {
        const QBrush brush = plot->palette().brush( plot->backgroundRole() );
        painter->fillRect( plotRect, brush );
    }

    /*
      The layout engine uses the same methods as they are used
      by the Qt layout system. Therefore we need to calculate the
      layout in screen coordinates and paint with a scaled painter.
     */
    QTransform transform;
    transform.scale(
        double( painter->device()->logicalDpiX() ) / plot->logicalDpiX(),
        double( painter->device()->logicalDpiY() ) / plot->logicalDpiY() );

    painter->save();

    int baseLineDists[QwtPlot::axisCnt];
    if ( d_data->layoutFlags & FrameWithScales )
    {
        for ( axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
        {
            QwtScaleWidget *scaleWidget =
                ( QwtScaleWidget * )plot->axisWidget( axisId );
            if ( scaleWidget )
            {
                baseLineDists[axisId] = scaleWidget->margin();
                scaleWidget->setMargin( 0 );
            }
        }
    }
    // Calculate the layout for the print.

    int layoutOptions = QwtPlotLayout::IgnoreScrollbars
        | QwtPlotLayout::IgnoreFrames;
    if ( !d_data->layoutFlags & KeepMargins )
        layoutOptions |= QwtPlotLayout::IgnoreMargin;
    if ( d_data->discardFlags & DiscardLegend )
        layoutOptions |= QwtPlotLayout::IgnoreLegend;

    const QRectF layoutRect = transform.inverted().mapRect( plotRect );
    plot->plotLayout()->activate( plot, layoutRect, layoutOptions );

    painter->setWorldTransform( transform, true );

    // canvas

    QwtScaleMap maps[QwtPlot::axisCnt];
    buildCanvasMaps( plot, plot->plotLayout()->canvasRect(), maps );
    renderCanvas( plot, painter, plot->plotLayout()->canvasRect(), maps );

    if ( !( d_data->discardFlags & DiscardTitle )
        && ( !plot->titleLabel()->text().isEmpty() ) )
    {
        renderTitle( plot, painter, plot->plotLayout()->titleRect() );
    }

    if ( !( d_data->discardFlags & DiscardLegend )
        && plot->legend() && !plot->legend()->isEmpty() )
    {
        renderLegend( plot, painter, plot->plotLayout()->legendRect() );
    }

    for ( axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
    {
        QwtScaleWidget *scaleWidget = plot->axisWidget( axisId );
        if ( scaleWidget )
        {
            int baseDist = scaleWidget->margin();

            int startDist, endDist;
            scaleWidget->getBorderDistHint( startDist, endDist );

            renderScale( plot, painter, axisId, startDist, endDist,
                baseDist, plot->plotLayout()->scaleRect( axisId ) );
        }
    }


    plot->plotLayout()->invalidate();

    // reset all widgets with their original attributes.
    if ( d_data->layoutFlags & FrameWithScales )
    {
        // restore the previous base line dists

        for ( axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
        {
            QwtScaleWidget *scaleWidget = plot->axisWidget( axisId );
            if ( scaleWidget  )
                scaleWidget->setMargin( baseLineDists[axisId] );
        }
    }

    painter->restore();
}

/*!
  Print the title into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderTitle( const QwtPlot *plot,
    QPainter *painter, const QRectF &rect ) const
{
    painter->setFont( plot->titleLabel()->font() );

    const QColor color = plot->titleLabel()->palette().color(
            QPalette::Active, QPalette::Text );

    painter->setPen( color );
    plot->titleLabel()->text().draw( painter, rect );
}

/*!
  Print the legend into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderLegend( const QwtPlot *plot,
    QPainter *painter, const QRectF &rect ) const
{
    if ( !plot->legend() || plot->legend()->isEmpty() )
        return;

    QLayout *l = plot->legend()->contentsWidget()->layout();
    if ( l == 0 || !l->inherits( "QwtDynGridLayout" ) )
        return;

    QwtDynGridLayout *legendLayout = ( QwtDynGridLayout * )l;

    uint numCols = legendLayout->columnsForWidth( rect.width() );
    QList<QRect> itemRects =
        legendLayout->layoutItems( rect.toRect(), numCols );

    int index = 0;

    for ( int i = 0; i < legendLayout->count(); i++ )
    {
        QLayoutItem *item = legendLayout->itemAt( i );
        QWidget *w = item->widget();
        if ( w )
        {
            painter->save();

            painter->setClipRect( itemRects[index] );
            renderLegendItem( plot, painter, w, itemRects[index] );

            index++;
            painter->restore();
        }
    }
}

/*!
  Print the legend item into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param widget Widget representing a legend item
  \param rect Bounding rectangle

  \note When widget is not derived from QwtLegendItem renderLegendItem
        does nothing and needs to be overloaded
*/
void QwtPlotRenderer::renderLegendItem( const QwtPlot *plot,
    QPainter *painter, const QWidget *widget, const QRectF &rect ) const
{
    if ( widget->inherits( "QwtLegendItem" ) )
    {
        QwtLegendItem *item = ( QwtLegendItem * )widget;

        const QRect identifierRect(
            rect.x() + item->margin(), rect.y(),
            item->identifierSize().width(), rect.height() );

        QwtLegendItemManager *itemManger = plot->legend()->find( item );
        if ( itemManger )
        {
            painter->save();
            itemManger->drawLegendIdentifier( painter, identifierRect );
            painter->restore();
        }

        // Label

        QRectF titleRect = rect;
        titleRect.setX( identifierRect.right() + 2 * item->spacing() );

        painter->setFont( item->font() );
        item->text().draw( painter, titleRect );
    }
}

/*!
  \brief Paint a scale into a given rectangle.
  Paint the scale into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param axisId Axis
  \param startDist Start border distance
  \param endDist End border distance
  \param baseDist Base distance
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderScale( const QwtPlot *plot,
    QPainter *painter,
    int axisId, int startDist, int endDist, int baseDist,
    const QRectF &rect ) const
{
    if ( !plot->axisEnabled( axisId ) )
        return;

    const QwtScaleWidget *scaleWidget = plot->axisWidget( axisId );
    if ( scaleWidget->isColorBarEnabled()
        && scaleWidget->colorBarWidth() > 0 )
    {
        scaleWidget->drawColorBar( painter, scaleWidget->colorBarRect( rect ) );

        const int off = scaleWidget->colorBarWidth() + scaleWidget->spacing();
        if ( scaleWidget->scaleDraw()->orientation() == Qt::Horizontal )
            baseDist += off;
        else
            baseDist += off;
    }

    painter->save();

    QwtScaleDraw::Alignment align;
    double x, y, w;

    switch ( axisId )
    {
        case QwtPlot::yLeft:
        {
            x = rect.right() - 1.0 - baseDist;
            y = rect.y() + startDist;
            w = rect.height() - startDist - endDist;
            align = QwtScaleDraw::LeftScale;
            break;
        }
        case QwtPlot::yRight:
        {
            x = rect.left() + baseDist;
            y = rect.y() + startDist;
            w = rect.height() - startDist - endDist;
            align = QwtScaleDraw::RightScale;
            break;
        }
        case QwtPlot::xTop:
        {
            x = rect.left() + startDist;
            y = rect.bottom() - 1.0 - baseDist;
            w = rect.width() - startDist - endDist;
            align = QwtScaleDraw::TopScale;
            break;
        }
        case QwtPlot::xBottom:
        {
            x = rect.left() + startDist;
            y = rect.top() + baseDist;
            w = rect.width() - startDist - endDist;
            align = QwtScaleDraw::BottomScale;
            break;
        }
        default:
            return;
    }

    scaleWidget->drawTitle( painter, align, rect );

    painter->setFont( scaleWidget->font() );

    QwtScaleDraw *sd = ( QwtScaleDraw * )scaleWidget->scaleDraw();
    const QPointF sdPos = sd->pos();
    const double sdLength = sd->length();

    sd->move( x, y );
    sd->setLength( w );

    QPalette palette = scaleWidget->palette();
    palette.setCurrentColorGroup( QPalette::Active );
    sd->draw( painter, palette );

    // reset previous values
    sd->move( sdPos );
    sd->setLength( sdLength );

    painter->restore();
}

/*!
  Print the canvas into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param map Maps mapping between plot and paint device coordinates
  \param canvasRect Canvas rectangle
*/
void QwtPlotRenderer::renderCanvas( const QwtPlot *plot,
    QPainter *painter, const QRectF &canvasRect, 
    const QwtScaleMap *map ) const
{
    painter->save();

    QRectF r = canvasRect.adjusted( 0.0, 0.0, -1.0, -1.0 );

    if ( d_data->layoutFlags & FrameWithScales )
    {
        r.adjust( -1.0, -1.0, 1.0, 1.0 );
        painter->setPen( QPen( Qt::black ) );
    }
    else
        painter->setPen( Qt::NoPen );

    if ( !( d_data->discardFlags & DiscardCanvasBackground ) )
    {
        const QBrush bgBrush =
            plot->canvas()->palette().brush( plot->backgroundRole() );
        painter->setBrush( bgBrush );
    }

    QwtPainter::drawRect( painter, r );

    painter->restore();

    painter->save();
    painter->setClipRect( canvasRect );

    plot->drawItems( painter, canvasRect, map );

    painter->restore();
}

/*!
   Calculated the scale maps for rendering the canvas

   \param plot Plot widget
   \param canvasRect Target rectangle
   \param maps Scale maps to be calculated
*/
void QwtPlotRenderer::buildCanvasMaps( const QwtPlot *plot,
    const QRectF &canvasRect, QwtScaleMap maps[] ) const
{
    for ( int axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
    {
        maps[axisId].setTransformation(
            plot->axisScaleEngine( axisId )->transformation() );

        const QwtScaleDiv &scaleDiv = *plot->axisScaleDiv( axisId );
        maps[axisId].setScaleInterval(
            scaleDiv.lowerBound(), scaleDiv.upperBound() );

        double from, to;
        if ( plot->axisEnabled( axisId ) )
        {
            const int sDist = plot->axisWidget( axisId )->startBorderDist();
            const int eDist = plot->axisWidget( axisId )->endBorderDist();
            const QRectF &scaleRect = plot->plotLayout()->scaleRect( axisId );

            if ( axisId == QwtPlot::xTop || axisId == QwtPlot::xBottom )
            {
                from = scaleRect.left() + sDist;
                to = scaleRect.right() - eDist;
            }
            else
            {
                from = scaleRect.bottom() - eDist;
                to = scaleRect.top() + sDist;
            }
        }
        else
        {
            int margin = plot->plotLayout()->canvasMargin( axisId );
            if ( axisId == QwtPlot::yLeft || axisId == QwtPlot::yRight )
            {
                from = canvasRect.bottom() - margin;
                to = canvasRect.top() + margin;
            }
            else
            {
                from = canvasRect.left() + margin;
                to = canvasRect.right() - margin;
            }
        }
        maps[axisId].setPaintInterval( from, to );
    }
}