/*! \mainpage Developer's documentation
 *
 * \section intro_sec Introduction
 *
 * <p>Ofeli, as an acronym for <b>O</b>pen, <b>F</b>ast and <b>E</b>fficient <b>L</b>evel set
   <b>I</b>mplementation, demonstrates how to operate an image
   segmentation algorithm of Y. Shi and W. C. Karl <b>[1],</b> using a
   discrete approach for the approximation of level-set-based
   curve evolution (implicit active contours).</p>
   <p>This is a novel (2005) and fast algorithm without the need
   of solving partial differential equations (PDE) while preserving the
   advantages of level set methods, such as the automatic handling of
   topological changes. Considerable speedups (×100) have been
   demonstrated as compared to PDE-based narrow band level-set implementations.</p>

 * \section structure_sec Structure
 *
 * In this project, the <b>G</b>raphical <b>U</b>ser <b>I</b>nterface (GUI) is clearly separated of the image processing part.
 *
 * GUI part :
 * - file imageviewer.cpp file with the class ofeli::ImageViewer (dependant of framework Qt 5, QtCore and QtWidgets modules)
 * - file pixmapwidget.cpp with the class ofeli::PixmapWidget (dependant of framework Qt 5, QtCore and QtWidgets modules)
 *
 * Image processing part :
 * - file linked_list.tpp with the class ofeli::list
 * - file activecontour.cpp with the class ofeli::ActiveContour
 *      - file ac_withoutedges.cpp with the class ofeli::ACwithoutEdges
 *      - file ac_withoutedges_yuv.cpp with the class ofeli::ACwithoutEdgesYUV
 *      - file geodesic_ac.cpp with the class ofeli::GeodesicAC
 * - file hausdorff_distance.cpp with the class ofeli::HausdorffDistance
 * - file filters.cpp with the class ofeli::Filters
 *
 *
 * \section reusability_sec Reusability
 *
 * <p>This Qt project is built as an application and not as a static or shared library. So if you are interested to use this C++ code, especially for the image processing part of the project,
 * you must just include this file(s) thanks to the preprocessor directive in your file(s). After you must just pass to each constructor an input argument pointer on a row-wise image data buffer and for the class ACwithoutEdgesYUV, a pointer on a RGB interleaved data buffer (R1 G1 B1 R2 G2 B2 ...).</p>
 * <p>If you prefer a command-line interface or if you are interested in a tracking example of this algorithm, you can find a fork of this project interfaced with Matlab (MEX-file). Each constructor takes an input pointer on a column-wise image data buffer and for the class ACwithoutEdgesYUV, a pointer on a RGB planar data buffer (R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3 ...).</p>
 *
 */

#include <QApplication>
#include <QCoreApplication>

#include "imageviewer.hpp"

int main(int argc, char *argv[])
{
    // to have the windows theme
    //QApplication::setStyle(new QWindowsStyle);

    QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
    //QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );
    //QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );

    QApplication app(argc, argv);

    QSettings settings("CVL", "RootMeasurement");

    int language = settings.value("language",0).toInt();

    QTranslator translator_qt_widget;
    QTranslator translator_ofeli;
    QString locale;

    switch( language )
    {
    case 0 :
    {
        locale = QLocale::system().name().section('_', 0, 0);
        break;
    }
    case 1 :
    {
        locale = QString("en");
        break;
    }
    case 2 :
    {
        locale = QString("fr");
        break;
    }
    default :
    {
        locale = QString("en");
    }
    }

    translator_qt_widget.load( QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    app.installTranslator(&translator_qt_widget);

    translator_ofeli.load( QString(":Ofeli_") + locale );
    app.installTranslator(&translator_ofeli);

    ofeli::ImageViewer imageViewer;
    imageViewer.showMaximized();//show();

    return app.exec();
}
