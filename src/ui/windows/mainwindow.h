/**
 * DICOM Viewer - Janela principal
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construtor da janela principal.
     * @param parent Widget pai (opcional).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destrutor da janela principal.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot chamado ao acionar "Abrir" no menu.
     */
    void on_actionAbrir_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* sceneMedicalImage;
};
#endif // MAINWINDOW_H
