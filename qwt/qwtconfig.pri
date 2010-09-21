################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

VER_MAJ      = 6
VER_MIN      = 0
VER_PAT      = 0
VERSION      = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

######################################################################
# Install paths
######################################################################

QWT_INSTALL_PREFIX = $$[QT_INSTALL_PREFIX]

unix {
    QWT_INSTALL_PREFIX    = /usr/local/qwt-$$VERSION-svn
}

win32 {
    QWT_INSTALL_PREFIX    = C:/Qwt-$$VERSION-svn
}

QWT_INSTALL_DOCS      = $${QWT_INSTALL_PREFIX}/doc
QWT_INSTALL_HEADERS   = $${QWT_INSTALL_PREFIX}/include
QWT_INSTALL_LIBS      = $${QWT_INSTALL_PREFIX}/lib

######################################################################
# Designer plugin
######################################################################

QWT_INSTALL_PLUGINS   = $${QWT_INSTALL_PREFIX}/plugins/designer
# QWT_INSTALL_PLUGINS   = $${QT_INSTALL_PREFIX}/plugins/designer

######################################################################
# Features
# When building a Qwt application with qmake you might want to load
# the compiler/linker flags, that are required to build a Qwt application
# from qwt.prf. Therefore all you need to do is to add "CONFIG += qwt" 
# to your project file and take care, that qwt.prf can be found by qmake.
# ( see http://doc.trolltech.com/4.6/qmake-advanced-usage.html#adding-new-configuration-features )
# I recommend not to install the Qwt features together with the
# Qt features, because you will have to reinstall the Qwt features,
# with every Qt upgrade. Better use a different directory and make it
# known to Qt: "qmake -set QMAKEFEATURES ...".
######################################################################

QWT_INSTALL_FEATURES  = $${QWT_INSTALL_PREFIX}/features
# QWT_INSTALL_FEATURES  = $${QT_INSTALL_PREFIX}/features

######################################################################
# Build the static/shared libraries.
# If QwtDll is enabled, a shared library is built, otherwise
# it will be a static library.
######################################################################

QWT_CONFIG           += QwtDll

######################################################################
# QwtPlot enables all classes, that are needed to use the QwtPlot 
# widget. 
######################################################################

QWT_CONFIG       += QwtPlot

######################################################################
# QwtWidgets enables all classes, that are needed to use the all other
# widgets (sliders, dials, ...), beside QwtPlot. 
######################################################################

QWT_CONFIG     += QwtWidgets

######################################################################
# If you want to display svg images on the plot canvas, enable the 
# line below. Note that Qwt needs the svg+xml, when enabling 
# QwtSVGItem.
######################################################################

QWT_CONFIG     += QwtSVGItem

######################################################################
# You can use the MathML renderer of the Qt solutions package to 
# enable MathML support in Qwt.  # If you want this, copy 
# qtmmlwidget.h + qtmmlwidget.cpp to # textengines/mathml and enable 
# the line below.
######################################################################

#QWT_CONFIG     += QwtMathML

######################################################################
# If you want to build the Qwt designer plugin, 
# enable the line below.
# Otherwise you have to build it from the designer directory.
######################################################################

QWT_CONFIG     += QwtDesigner

######################################################################
# If you want to auto build the examples, enable the line below
# Otherwise you have to build them from the examples directory.
######################################################################

QWT_CONFIG     += QwtExamples