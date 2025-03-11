/*  Button Diagram
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#include <QCoreApplication>
//#include <QFile>
#include <QMessageBox>
//#include <QGraphicsScene>
//#include <QGraphicsView>
//#include <QGraphicsPixmapItem>
#include "CommonFramework/Globals.h"
#include "WindowTracker.h"
#include "ButtonDiagram.h"

//#include <iostream>
//using std::cout;
//using std::endl;


namespace PokemonAutomation{


ButtonDiagram::ButtonDiagram(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Controller Keyboard Mapping");

    m_image = QPixmap(QString::fromStdString(RESOURCE_PATH() + "/NintendoSwitch/ButtonLayout.jpg"));

    m_image_label = new QLabel(this);
    setCentralWidget(m_image_label);
    m_image_label->setAlignment(Qt::AlignCenter);
//    m_image_label->setPixmap(m_image.scaled(800, 600, Qt::KeepAspectRatio));
//    m_image_label->setPixmap(m_image);
//    m_image_label->setScaledContents(true);
    m_image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

//    image_label->resize(800, 600);
//    image_label->setFixedSize(800, 600);

    resize(800, 600);
    add_window(*this);
}
ButtonDiagram::~ButtonDiagram(){
    remove_window(*this);
}

void ButtonDiagram::resizeEvent(QResizeEvent*){
    int iw = m_image.width();
    int ih = m_image.height();
    int ww = m_image_label->width();
    int wh = m_image_label->height();
//    cout << "ww = " << ww << ", wh = " << wh << endl;

    double scale_w = (double)ww / iw;
    double scale_h = (double)wh / ih;
    double scale = std::min(scale_w, scale_h);

    iw = (int)(iw * scale);
    ih = (int)(ih * scale);

    m_image_label->setPixmap(m_image.scaled(iw, ih, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}



}

