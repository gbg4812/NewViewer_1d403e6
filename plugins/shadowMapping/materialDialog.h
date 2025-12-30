#pragma once

#include "qboxlayout.h"
#include "qcolordialog.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qnamespace.h"
#include "qpushbutton.h"
#include "qslider.h"
#include "qwidget.h"

struct gbgMaterial {
    gbgMaterial() {
        matDiffuse = QColor(200, 200, 200, 255);
        matSpecular = QColor(255, 255, 255, 255);
        matRoughness = 0.5;
        matShininess = 64;
    }
    QColor matDiffuse;
    QColor matSpecular;
    float matRoughness;
    float matShininess;
};

class MaterialDialog : public QDialog {

  public:
    MaterialDialog(QWidget *parent, gbgMaterial old) : QDialog(parent) {
        initUI(old);
    }

    gbgMaterial getMaterial() {
        this->exec();

        gbgMaterial res;
        res.matDiffuse = colorBttn.palette().color(QPalette::Button);
        res.matSpecular = specBttn.palette().color(QPalette::Button);
        res.matRoughness =
            (float)roughSldr.value() / (float)roughSldr.maximum();
        res.matShininess = shinSldr.value();
        return res;
    }

  private:
    void showColorDialog() {
        QPalette palette = colorBttn.palette();
        QColor current = palette.color(QPalette::Button);
        QColor color = QColorDialog(current, this).getColor();
        palette.setColor(QPalette::Button, color);
        colorBttn.setPalette(palette);
        colorBttn.update();
    }

    void showSpecDialog() {
        QPalette palette = specBttn.palette();
        QColor current = palette.color(QPalette::Button);
        QColor color = QColorDialog(current, this).getColor();
        palette.setColor(QPalette::Button, color);
        specBttn.setPalette(palette);
        specBttn.update();
    }

    void initUI(gbgMaterial old) {

        // color button
        colorBttn.setText("Base Color");
        colorBttn.setAutoFillBackground(true);
        QPalette cpalette = colorBttn.palette();
        cpalette.setColor(QPalette::Button, old.matDiffuse);
        colorBttn.setPalette(cpalette);
        connect(&colorBttn, &QPushButton::clicked, this,
                &MaterialDialog::showColorDialog);

        layout.addWidget(&colorBttn);

        // spec bttn
        specBttn.setText("Specular Color");
        specBttn.setAutoFillBackground(true);
        QPalette spalette = specBttn.palette();
        spalette.setColor(QPalette::Button, old.matSpecular);
        specBttn.setPalette(spalette);
        connect(&specBttn, &QPushButton::clicked, this,
                &MaterialDialog::showSpecDialog);

        layout.addWidget(&specBttn);
        roughSldrLab.setText("Roughness");
        layout.addWidget(&roughSldrLab);

        roughSldr.setOrientation(Qt::Horizontal);
        roughSldr.setRange(0, 100);
        roughSldr.setValue(old.matRoughness * roughSldr.maximum());
        layout.addWidget(&roughSldr);

        shinSldrLab.setText("Shininess");
        layout.addWidget(&shinSldrLab);

        shinSldr.setOrientation(Qt::Horizontal);
        shinSldr.setRange(0, 100);
        shinSldr.setValue(old.matShininess);
        layout.addWidget(&shinSldr);

        this->setLayout(&layout);
    }

    QVBoxLayout layout;
    QPushButton colorBttn;
    QPushButton specBttn;
    QLabel roughSldrLab;
    QSlider roughSldr;
    QLabel shinSldrLab;
    QSlider shinSldr;
};
