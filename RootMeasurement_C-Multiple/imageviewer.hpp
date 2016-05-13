/****************************************************************************
**
** Copyright (C) 2010-2012 Fabien Bessy.
** Contact: fabien.bessy@gmail.com
**
** This file is part of project Ofeli.
**
** http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
** You may use this file under the terms of the CeCILL license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Fabien Bessy and its Subsidiary(-ies) nor the
**     names of its contributors may be used to endorse or promote products
**     derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QMainWindow>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

#include <QtWidgets>
#include "scrollarea.hpp"

//! [0]
namespace ofeli
{

// forward declarations instead of includes
class ActiveContour;
template <typename T> class list;
class Filters;
class PixmapWidget;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public :

    ImageViewer();
    void adjustScroll(float delta_wheel);

    friend class ScrollArea;

private slots :

    // Slots that receive signals in the actions of the menu bar
    void openFileName();
    void openRecentFile();
    void deleteList();
    void saveImage();
    void print();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void start();
    void stop();
    void settings();
    void evaluation();
    void language();
    void about();
    void doc();

    void openImagesInDirection();
    void saveImageWithDefaultValues();
    void executeSegmentationForMultipleFiles();
    void executeSegmentationForOneFile();

    void do_scale0(int value);
    void default_settings();

    // Slot called every change tab
    void tab_visu(int value);

    // Slots called from the initialization tab
    void openFilenamePhi();
    void save_phi();
    void clean_phi_visu();
    void shape_visu(int value);
    void phi_visu(int value);
    void add_visu();
    void subtract_visu();
    void shape_visu();

    // Slots called from the preprocessing tab
    // Slot also called once in the algorithm tab to see the image gradient for geodesic outline
    void filtering_visu();

    // Slots called from the Display tab
    void do_scale(int value);
    void change_display_size();
    void color_out();
    void color_in();

    // Called slots since the evaluation window
    void openFileNameA();
    void openFileNameB();
    void colorA();
    void colorB();
    void compute(int index);
    void scaleA(int value);
    void scaleB(int value);
    void create_listA(int index);
    void create_listB(int index);

    void openWebpage();
    void openLicense();

    void adjustVerticalScroll(int min, int max);
    void adjustHorizontalScroll(int min, int max);
    void adjustVerticalScroll_settings(int min, int max);
    void adjustHorizontalScroll_settings(int min, int max);
    void adjustVerticalScrollA(int min, int max);
    void adjustHorizontalScrollA(int min, int max);
    void adjustVerticalScrollB(int min, int max);
    void adjustHorizontalScrollB(int min, int max);

    void wheel_zoom(int,ScrollArea*);

private :

    // To automatically detect the system
    #ifdef Q_OS_WIN
    static const int operating_system = 1;
    #endif
    #ifdef Q_OS_MAC
    static const int operating_system = 2;
    #endif
    #ifdef Q_OS_LINUX
    static const int operating_system = 3;
    #endif

    static const int list_end = -9999999;

   // Basic functions of the example of the doc IMAGEVIEWER Qt
    void open();
    void createActions();
    void createMenus();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar* scrollBar, double factor);
    double scaleFactor;

    //////////////////////////////////////////
    //     for the main window      //
    /////////////////////////////////////////

    // left of the main window image in a ScrollArea
    ofeli::PixmapWidget* imageLabel;
    ofeli::ScrollArea* scrollArea;

    // Right, 4 groupbox with their respective widgets
    // For groupbox speed model information
    QLabel* Cin_text;
    QLabel* Cout_text;
    QLabel* threshold_text;
    QStackedWidget* stackedWidget;

    // For stopping groupbox Conditions
    QLabel* text;
    QLabel* changes_text;
    QLabel* oscillation_text;

   // groupbox elapsed time
    QLabel* time_text;

    // groupbox Data and level set function
    QLabel* pixel_text;
    QLabel* phi_text;
    QLabel* lists_text;


    QSpinBox* scale_spin0;
    QSlider* scale_slider0;

    /////////////////////////////////////////////////
    //        variables of slot open()       //
    /////////////////////////////////////////////////

    // string path + name of the image obtained from a box of Qt opening dialogue
    QString fileName;
    QString fileName_phi;
    // Buffer and image information
         // Buffer 8 bits
    unsigned char* img1;
    bool isRgb1;
    int image_format;
    // buffer 24 bits si possible
    //unsigned char* img1_rgb;
    // line
    int img_height;
    // colonne
    int img_width;
    // image size
    int img_size;

    bool isMultipleImagesSelected = false;
    QStringList selectedImageFiles;

    ///////////////////////////////////////////////////
    //    related objects and variable slot start()    //
    ///////////////////////////////////////////////////

    // filter object
    ofeli::Filters* filters1;
    ofeli::ActiveContour* ac;

    QImage image_result;
    unsigned char* image_result_uchar;
    QImage image_save_preprocess;
    void wait (double seconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //   variables and functions related to the interactive display of an image in the configuration window  //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ofeli::Filters* filters2;
    // phi before cleaning border
    char* phi_init1;
    char* phi_init2;
    // Initial phi used during the execution of start () slot to perform segmentation
    char* phi_init1_clean;
    // Initial phi determined by the configuration window
    char* phi_init2_clean;
    // form serving to be added or removed a phi
    char* shape;
    int* shape_points;
    int* Lout_shape1;
    int* Lin_shape1;
    int* Lout_2;
    int* Lin_2;
    QImage img;
    QImage image;
    unsigned char* image_uchar;
    QImage image_filter;
    unsigned char* image_filter_uchar;
    QImage image_phi;
    unsigned char* image_phi_uchar;
    QImage image_shape;
    unsigned char* image_shape_uchar;
    // Output parameters of the active contour and results
    const ofeli::list<int>* Lout1;
    const ofeli::list<int>* Lin1;

    QPixmap pixmap_settings;

    void phi_add_shape();
    void phi_subtract_shape();
    void phi_visu(bool dark_color);
    bool isRedundantPointOfLout(char* phi, int x, int y);
    bool isRedundantPointOfLin(char* phi, int x, int y);
    void clean_boundaries(char* phi, char* phi_clean);

    //used to save the final phi
    void saveFinalPhi(QString path = "");

    //////////////////////////////////////////
    //   to the configuration window   //
    /////////////////////////////////////////

   // The configuration window
    QDialog* settings_window;

   // Image in a ScrollArea right in the window
    ofeli::PixmapWidget* imageLabel_settings;
    ScrollArea* scrollArea_settings;

   // Tabs left
    QTabWidget* onglets;

    // Ok Cancel button
    QDialogButtonBox* dial_buttons;

    // These functions are not slots because they do not receive signals widgets (not connected)
         // They are called after closing the modal window modal window that appears thanks to the exec () method
         // The exec () method returns a boolean according to Ok or Cancel
         // Click Ok
    void apply_settings();
    // clic Cancel
    void cancel_settings();
    void closeEvent(QCloseEvent* event);

    /////////////////////////////////////////
    //             onglets                 //
    /////////////////////////////////////////

    //widgets and variables related to the algorithm tab :

    QVBoxLayout* algorithm();

    QSpinBox* Na_spin;
    int Na1;
    int Ns1;
    QSpinBox* Ns_spin;
    QRadioButton* chanvese_radio;
    QSpinBox* lambda_out_spin;
    QSpinBox* lambda_in_spin;
    QRadioButton* geodesic_radio;
    QSpinBox* klength_gradient_spin;
    QGroupBox* yuv_groupbox;
    QSpinBox* Yspin;
    QSpinBox* Uspin;
    QSpinBox* Vspin;
    int model;
    int model2;
    int kernel_gradient_length1;
    int kernel_gradient_length2;
    int lambda_out1;
    int lambda_in1;
    int alpha1;
    int beta1;
    int gamma1;
    int alpha2;
    int beta2;
    int gamma2;

    QGroupBox* internalspeed_groupbox;
    bool hasSmoothingCycle1;
    QSpinBox* klength_spin;
    int kernel_curve1;
    QDoubleSpinBox* std_spin;
    double std_curve1;

    //for threshold in Geodesic model
    double magitiduteEdge;
    QDoubleSpinBox* magnitidute_spin;
    /////////////////////////////////////////

    // widgets and variables related to the initialization tab :

    QPushButton* open_phi_button;
    QPushButton* save_phi_button;

    QVBoxLayout* initialization();

    QImage img_phi;
    void open_phi();
    void phiInit2imgPhi();
    void imgPhi2phiInit();

    QRadioButton* rectangle_radio;
    QRadioButton* ellipse_radio;
    bool hasEllipse1;
    bool hasEllipse2;

    QSpinBox* width_shape_spin;
    QSlider* width_slider;
    double init_width1;
    double init_width2;
    QSpinBox* height_shape_spin;
    QSlider* height_slider;
    double init_height1;
    double init_height2;

    QSpinBox* abscissa_spin;
    QSlider* abscissa_slider;
    double center_x1;
    double center_x2;
    QSpinBox* ordinate_spin;
    QSlider* ordinate_slider;
    double center_y1;
    double center_y2;

    QPushButton* add_button;
    QPushButton* subtract_button;
    QPushButton* clean_button;

    static unsigned char otsu_method(const int histogram[],int img_size);

    /////////////////////////////////////////

    // widgets and variables associated with the preprocessing tab:

    QGroupBox* preprocessing();

    QVBoxLayout* noise_layout();

    QGroupBox* gaussian_noise_groupbox;
    bool hasGaussianNoise;
    bool hasGaussianNoise2;
    QDoubleSpinBox* std_noise_spin;
    double std_noise;
    double std_noise2;

    QGroupBox* salt_noise_groupbox;
    bool hasSaltNoise;
    bool hasSaltNoise2;
    QDoubleSpinBox* proba_noise_spin;
    double proba_noise;
    double proba_noise2;

    QGroupBox* speckle_noise_groupbox;
    bool hasSpeckleNoise;
    bool hasSpeckleNoise2;
    QDoubleSpinBox* std_speckle_noise_spin;
    double std_speckle_noise;
    double std_speckle_noise2;

    QVBoxLayout* filter_layout();

    QGroupBox* median_groupbox;
    QSpinBox* klength_median_spin;
    QRadioButton* complex_radio1;
    QRadioButton* complex_radio2;
    bool hasMedianFilt;
    bool hasMedianFilt2;
    bool hasO1algo1;
    bool hasO1algo2;
    int kernel_median_length1;
    int kernel_median_length2;

    QGroupBox* mean_groupbox;
    QSpinBox* klength_mean_spin;
    bool hasMeanFilt;
    bool hasMeanFilt2;
    int kernel_mean_length1;
    int kernel_mean_length2;

    QGroupBox* gaussian_groupbox;
    QSpinBox* klength_gaussian_spin;
    QDoubleSpinBox* std_filter_spin;
    bool hasGaussianFilt;
    bool hasGaussianFilt2;
    int kernel_gaussian_length1;
    int kernel_gaussian_length2;
    double sigma;
    double sigma2;

    QGroupBox* aniso_groupbox;
    QRadioButton* aniso1_radio;
    QRadioButton* aniso2_radio;
    QSpinBox* iteration_filter_spin;
    QDoubleSpinBox* lambda_spin;
    QDoubleSpinBox* kappa_spin;
    bool hasAnisoDiff;
    bool hasAnisoDiff2;
    int max_itera1;
    int max_itera2;
    double lambda1;
    double lambda2;
    double kappa1;
    double kappa2;
    int aniso_option1;
    int aniso_option2;

    QGroupBox* open_groupbox;
    QSpinBox* klength_open_spin;
    bool hasOpenFilt;
    bool hasOpenFilt2;
    int kernel_open_length1;
    int kernel_open_length2;

    QGroupBox* close_groupbox;
    QSpinBox* klength_close_spin;
    bool hasCloseFilt;
    bool hasCloseFilt2;
    int kernel_close_length1;
    int kernel_close_length2;

    QGroupBox* tophat_groupbox;
    QRadioButton* whitetophat_radio;
    QRadioButton* blacktophat_radio;
    QSpinBox* klength_tophat_spin;
    bool hasTophatFilt;
    bool hasTophatFilt2;
    bool isWhiteTophat;
    bool isWhiteTophat2;
    int kernel_tophat_length1;
    int kernel_tophat_length2;

    QGroupBox* algo_groupbox;
    QRadioButton* complex1_morpho_radio;
    QRadioButton* complex2_morpho_radio;
    bool hasO1morpho1;
    bool hasO1morpho2;

    QTabWidget* page3;
    QGroupBox* preprocessing_groupbox;
    bool hasPreprocess;
    bool hasPreprocess2;
    QLabel* time_filt;


    /////////////////////////////////////////

    // widgets et variables liés à l'onglet display :

    QVBoxLayout* display();

    QSpinBox* scale_spin;
    QSlider* scale_slider;
    QCheckBox* histo_checkbox;
    bool hasHistoNormaliz;
    bool hasHistoNormaliz2;


    QCheckBox* step_checkbox;
    bool hasDisplayEach;
    QComboBox* outsidecolor_combobox;
    unsigned char Rout1;
    unsigned char Gout1;
    unsigned char Bout1;
    unsigned char Rout2;
    unsigned char Gout2;
    unsigned char Bout2;
    unsigned char Rout_selected1;
    unsigned char Gout_selected1;
    unsigned char Bout_selected1;
    unsigned char Rout_selected2;
    unsigned char Gout_selected2;
    unsigned char Bout_selected2;
    QComboBox* insidecolor_combobox;
    unsigned char Rin1;
    unsigned char Gin1;
    unsigned char Bin1;
    unsigned char Rin2;
    unsigned char Gin2;
    unsigned char Bin2;
    unsigned char Rin_selected1;
    unsigned char Gin_selected1;
    unsigned char Bin_selected1;
    unsigned char Rin_selected2;
    unsigned char Gin_selected2;
    unsigned char Bin_selected2;

    int inside_combo;
    int outside_combo;
    void get_color(int index, unsigned char& R, unsigned char& G, unsigned char& B);


    /////////////////////////////////////////
    //   for the assessment window     //
    /////////////////////////////////////////

    QDialog* evaluation_window;

    void open_imageA();
    void open_imageB();

    QTabWidget* onglets_eval;

    // widgets from the left side to the image A
    QString fileNameA;
    ofeli::PixmapWidget* imageLabelA;
    ScrollArea* scrollAreaA;
    QSpinBox* scale_spinA;
    QLabel* label_name1;
    QComboBox* Acolor_combobox;
    unsigned char redAselected;
    unsigned char greenAselected;
    unsigned char blueAselected;

    QPushButton* open1_button;
    QImage image1;
    int imgA_width;
    int imgA_height;
    int imgA_size;
    int positionXa;
    int positionYa;
    int* listA;
    int listA_length;
    void create_listA();
    QLabel* text_listA_length;



    // widgets on the right side for the image B
    QString fileNameB;
    ofeli::PixmapWidget* imageLabelB;
    ScrollArea* scrollAreaB;
    QSpinBox* scale_spinB;
    QLabel* label_name2;
    QComboBox* Bcolor_combobox;
    unsigned char redBselected;
    unsigned char greenBselected;
    unsigned char blueBselected;

    QPushButton* open2_button;
    QImage image2;
    int imgB_width;
    int imgB_height;
    int imgB_size;
    int positionXb;
    int positionYb;
    int* listB;
    int listB_length;
    void create_listB();
    QLabel* text_listB_length;

    // Central widgets for distances and relationships
    QLabel* label_hausdorff;
    QLabel* label_modified_hausdorff;
    QLabel* label_hausdorff_ratio;
    QLabel* label_modified_hausdorff_ratio;
    QLabel* label_centroids_dist;
    QLabel* label_centroids_ratio;

    ///////////////////////////////////
    //    for the window about      //
    ///////////////////////////////////

    QDialog* about_window;
    QTabWidget* onglets1;
    QDialog* license_window;

    QDialog* language_window;
    QListWidget* listWidget;
    QDialogButtonBox* language_button;

    /////////////////////////////////////////////////////
    //   variables for the mouse and keyboard events  //
    /////////////////////////////////////////////////////

    // for the main window
    bool hasClickStopping;
    bool hasStepByStep;
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void infinite_loop();
    bool hasAlgoBreaking;
    int image_disp;
    bool hasResultDisp;

    // For the three windows
         // Filter the events for the position on the image and not the window
    bool eventFilter(QObject* object, QEvent* event);
   // Position of the mouse cursor
    int positionX;
    int positionY;
    void show_phi_list_value();

    // For the parameters configuration window
    void mouseMoveEvent2(QMouseEvent* event2);
    void mousePressEvent2(QMouseEvent* event2);

   // Add to the color of the lists by clicking on the images in the evaluation window
    void mouseMoveEventA(QMouseEvent* event);
    void mousePressEventA(QMouseEvent* event);
    void mouseMoveEventB(QMouseEvent* event);
    void mousePressEventB(QMouseEvent* event);

    bool hasContoursHidden;
    void img1_visu();
    bool hasShowImg1;

    unsigned int RGB;
    int iteration1;

    QStringList nameFilters;
    QString last_directory_used;
    void setCurrentFile(const QString &fileName1);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    QString curFile;


protected :

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);

    void dragEnterEventPhi(QDragEnterEvent* event);
    void dragMoveEventPhi(QDragMoveEvent* event);
    void dropEventPhi(QDropEvent* event);
    void dragLeaveEventPhi(QDragLeaveEvent* event);

    void dragEnterEventA(QDragEnterEvent* event);
    void dragMoveEventA(QDragMoveEvent* event);
    void dropEventA(QDropEvent* event);
    void dragLeaveEventA(QDragLeaveEvent* event);

    void dragEnterEventB(QDragEnterEvent* event);
    void dragMoveEventB(QDragMoveEvent* event);
    void dropEventB(QDropEvent* event);
    void dragLeaveEventB(QDragLeaveEvent* event);

#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    QAction* openAct;
    QAction* separatorAct;
    enum { MaxRecentFiles = 5 };
    QAction* recentFileActs[MaxRecentFiles];
    QAction* deleteAct;
    QAction* saveAct;
    QAction* printAct;
    QAction* exitAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* normalSizeAct;
    QAction* startAct;
    QAction* stopAct;
    QAction* evaluateAct;
    QAction* settingsAct;
    QAction* aboutAct;
    QAction* languageAct;
    QAction* docAct;
    QAction* aboutQtAct;

    QAction* openImagesFromDirectionAct;

    QMenu* fileMenu;
    QMenu* viewMenu;
    QMenu* segmentationMenu;
    QMenu* helpMenu;


signals :

    void changed(const QMimeData* mimeData = 0);

};

}

#endif

//! \class ofeli::ImageViewer
//! The class ImageViewer is a graphical user interface using the Qt4 framework (QtCore and QtGui) to show an image with the active contour, to configure the parameters of the active contour and optionally the preprocessing step and to evaluate two segmentation results thanks to the evaluation window.
//! It based on the Qt example of the documentation http://doc.qt.nokia.com/4.7/widgets-imageviewer.html.
