/********************************************************************************
** Form generated from reading UI file 'qaxiswidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QAXISWIDGET_H
#define UI_QAXISWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *checkBoxAxis;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_10;
    QComboBox *comboBoxAxisPosition;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_4;
    QComboBox *comboBoxX;
    QComboBox *comboBoxY;
    QComboBox *comboBoxZ;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName("Form");
        Form->resize(400, 300);
        gridLayout = new QGridLayout(Form);
        gridLayout->setObjectName("gridLayout");
        groupBox_2 = new QGroupBox(Form);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName("gridLayout_2");
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        checkBoxAxis = new QCheckBox(groupBox_2);
        checkBoxAxis->setObjectName("checkBoxAxis");

        horizontalLayout_8->addWidget(checkBoxAxis);


        gridLayout_2->addLayout(horizontalLayout_8, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName("label_10");

        horizontalLayout_4->addWidget(label_10);

        comboBoxAxisPosition = new QComboBox(groupBox_2);
        comboBoxAxisPosition->addItem(QString());
        comboBoxAxisPosition->addItem(QString());
        comboBoxAxisPosition->setObjectName("comboBoxAxisPosition");

        horizontalLayout_4->addWidget(comboBoxAxisPosition);


        gridLayout_2->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");

        verticalLayout_3->addWidget(label_6);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName("label_7");

        verticalLayout_3->addWidget(label_7);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName("label_8");

        verticalLayout_3->addWidget(label_8);


        horizontalLayout_7->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        comboBoxX = new QComboBox(groupBox_2);
        comboBoxX->addItem(QString());
        comboBoxX->addItem(QString());
        comboBoxX->addItem(QString());
        comboBoxX->setObjectName("comboBoxX");
        comboBoxX->setMinimumSize(QSize(80, 0));
        comboBoxX->setMaximumSize(QSize(80, 16777215));

        verticalLayout_4->addWidget(comboBoxX);

        comboBoxY = new QComboBox(groupBox_2);
        comboBoxY->addItem(QString());
        comboBoxY->addItem(QString());
        comboBoxY->addItem(QString());
        comboBoxY->setObjectName("comboBoxY");
        comboBoxY->setMinimumSize(QSize(80, 0));
        comboBoxY->setMaximumSize(QSize(80, 16777215));

        verticalLayout_4->addWidget(comboBoxY);

        comboBoxZ = new QComboBox(groupBox_2);
        comboBoxZ->addItem(QString());
        comboBoxZ->addItem(QString());
        comboBoxZ->addItem(QString());
        comboBoxZ->setObjectName("comboBoxZ");
        comboBoxZ->setMinimumSize(QSize(80, 0));
        comboBoxZ->setMaximumSize(QSize(80, 16777215));

        verticalLayout_4->addWidget(comboBoxZ);


        horizontalLayout_7->addLayout(verticalLayout_4);


        gridLayout_2->addLayout(horizontalLayout_7, 2, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "Form", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("Form", "Axies", nullptr));
        checkBoxAxis->setText(QCoreApplication::translate("Form", "Show Axies", nullptr));
        label_10->setText(QCoreApplication::translate("Form", "Axis Position", nullptr));
        comboBoxAxisPosition->setItemText(0, QCoreApplication::translate("Form", "Left", nullptr));
        comboBoxAxisPosition->setItemText(1, QCoreApplication::translate("Form", "Right", nullptr));

        label_6->setText(QCoreApplication::translate("Form", "X", nullptr));
        label_7->setText(QCoreApplication::translate("Form", "Y", nullptr));
        label_8->setText(QCoreApplication::translate("Form", "Z", nullptr));
        comboBoxX->setItemText(0, QCoreApplication::translate("Form", "Up", nullptr));
        comboBoxX->setItemText(1, QCoreApplication::translate("Form", "Aside", nullptr));
        comboBoxX->setItemText(2, QCoreApplication::translate("Form", "Inside", nullptr));

        comboBoxY->setItemText(0, QCoreApplication::translate("Form", "Aside", nullptr));
        comboBoxY->setItemText(1, QCoreApplication::translate("Form", "Inside", nullptr));
        comboBoxY->setItemText(2, QCoreApplication::translate("Form", "Up", nullptr));

        comboBoxZ->setItemText(0, QCoreApplication::translate("Form", "Inside", nullptr));
        comboBoxZ->setItemText(1, QCoreApplication::translate("Form", "Up", nullptr));
        comboBoxZ->setItemText(2, QCoreApplication::translate("Form", "Aside", nullptr));

    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QAXISWIDGET_H
