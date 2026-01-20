/**
 * DICOM VIewer - Janela principal
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @file mainwindow.cpp
 * @brief Implementação da classe MainWindow.
 */

#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "../forms/ui_mainwindow.h"
#include "menuutils.h"


/**
 * @brief Construtor da janela principal.
 * @param parent Widget pai (opcional).
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resources/dicom-viewer.ico"));
}

/**
 * @brief Destrutor da janela principal.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Slot chamado ao acionar "Abrir" no menu.
 */
void MainWindow::on_actionAbrir_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir arquivo DICOM"), "", tr("Arquivos DICOM (*.dcm);;Todos os arquivos (*.*)"));
    if (fileName.isEmpty()) {
        QMessageBox::information(this, tr("Abertura cancelada"), tr("Nenhum arquivo foi selecionado."));
        return;
    }

    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::critical(this, tr("Erro ao abrir arquivo"), tr("O arquivo selecionado não foi encontrado."));
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Erro ao abrir arquivo"), tr("Permissão negada ou arquivo em uso."));
        return;
    }

    if (!dicom_viewer_windows::isValidDicomFile(fileName)) {
        QMessageBox::critical(this, tr("Arquivo inválido"), tr("O arquivo selecionado não é um DICOM válido."));
        file.close();
        return;
    }
    file.close();
}

