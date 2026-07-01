/********************************************************************************
** Form generated from reading UI file 'qscadaobjectinfodialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QSCADAOBJECTINFODIALOG_H
#define UI_QSCADAOBJECTINFODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QScadaObjectInfoDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_5;
    QWidget *widget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_9;
    QSpinBox *spinBoxId;
    QCheckBox *checkBoxShowBackground;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QVBoxLayout *verticalLayout_2;
    QSpinBox *spinBoxX;
    QSpinBox *spinBoxY;
    QSpinBox *spinBoxWidth;
    QSpinBox *spinBoxHeight;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QScrollArea *properties;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_2;
    QPushButton *pushButton;

    void setupUi(QWidget *QScadaObjectInfoDialog)
    {
        if (QScadaObjectInfoDialog->objectName().isEmpty())
            QScadaObjectInfoDialog->setObjectName("QScadaObjectInfoDialog");
        QScadaObjectInfoDialog->resize(394, 684);
        QScadaObjectInfoDialog->setMinimumSize(QSize(300, 0));
        verticalLayout_3 = new QVBoxLayout(QScadaObjectInfoDialog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(5, 5, 5, 5);
        scrollArea = new QScrollArea(QScadaObjectInfoDialog);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName("scrollAreaWidgetContents_2");
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 382, 672));
        verticalLayout_5 = new QVBoxLayout(scrollAreaWidgetContents_2);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(5, 5, 5, 5);
        widget = new QWidget(scrollAreaWidgetContents_2);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(0, 240));
        widget->setMaximumSize(QSize(16777215, 240));
        verticalLayout_4 = new QVBoxLayout(widget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 10, 0, 10);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_9 = new QLabel(widget);
        label_9->setObjectName("label_9");

        horizontalLayout_3->addWidget(label_9);

        spinBoxId = new QSpinBox(widget);
        spinBoxId->setObjectName("spinBoxId");
        spinBoxId->setMaximum(255);

        horizontalLayout_3->addWidget(spinBoxId);


        verticalLayout_4->addLayout(horizontalLayout_3);

        checkBoxShowBackground = new QCheckBox(widget);
        checkBoxShowBackground->setObjectName("checkBoxShowBackground");

        verticalLayout_4->addWidget(checkBoxShowBackground);

        groupBox = new QGroupBox(widget);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        verticalLayout->addWidget(label_4);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        verticalLayout->addWidget(label_5);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        spinBoxX = new QSpinBox(groupBox);
        spinBoxX->setObjectName("spinBoxX");
        spinBoxX->setMaximum(9999999);

        verticalLayout_2->addWidget(spinBoxX);

        spinBoxY = new QSpinBox(groupBox);
        spinBoxY->setObjectName("spinBoxY");
        spinBoxY->setMaximum(9999999);

        verticalLayout_2->addWidget(spinBoxY);

        spinBoxWidth = new QSpinBox(groupBox);
        spinBoxWidth->setObjectName("spinBoxWidth");
        spinBoxWidth->setMaximum(999999999);

        verticalLayout_2->addWidget(spinBoxWidth);

        spinBoxHeight = new QSpinBox(groupBox);
        spinBoxHeight->setObjectName("spinBoxHeight");
        spinBoxHeight->setMaximum(99999999);

        verticalLayout_2->addWidget(spinBoxHeight);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout_4->addWidget(groupBox);


        verticalLayout_5->addWidget(widget);

        groupBox_2 = new QGroupBox(scrollAreaWidgetContents_2);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName("gridLayout_2");
        properties = new QScrollArea(groupBox_2);
        properties->setObjectName("properties");
        properties->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 340, 323));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setObjectName("gridLayout");
        tableWidget = new QTableWidget(scrollAreaWidgetContents);
        tableWidget->setObjectName("tableWidget");

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);

        properties->setWidget(scrollAreaWidgetContents);

        gridLayout_2->addWidget(properties, 0, 0, 1, 1);


        verticalLayout_5->addWidget(groupBox_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(-1, 0, -1, -1);
        pushButton_2 = new QPushButton(scrollAreaWidgetContents_2);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_4->addWidget(pushButton_2);

        pushButton = new QPushButton(scrollAreaWidgetContents_2);
        pushButton->setObjectName("pushButton");

        horizontalLayout_4->addWidget(pushButton);


        verticalLayout_5->addLayout(horizontalLayout_4);

        scrollArea->setWidget(scrollAreaWidgetContents_2);

        verticalLayout_3->addWidget(scrollArea);


        retranslateUi(QScadaObjectInfoDialog);

        QMetaObject::connectSlotsByName(QScadaObjectInfoDialog);
    } // setupUi

    void retranslateUi(QWidget *QScadaObjectInfoDialog)
    {
        QScadaObjectInfoDialog->setWindowTitle(QCoreApplication::translate("QScadaObjectInfoDialog", "Form", nullptr));
        label_9->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Object id", nullptr));
        checkBoxShowBackground->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Show Backgound", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QScadaObjectInfoDialog", "Geometry", nullptr));
        label_2->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "X", nullptr));
        label_3->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Y", nullptr));
        label_4->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Width", nullptr));
        label_5->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Height", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QScadaObjectInfoDialog", "QML Properties", nullptr));
        pushButton_2->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Save Changes", nullptr));
        pushButton->setText(QCoreApplication::translate("QScadaObjectInfoDialog", "Delete Object", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QScadaObjectInfoDialog: public Ui_QScadaObjectInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QSCADAOBJECTINFODIALOG_H
