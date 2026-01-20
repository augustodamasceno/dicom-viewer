/**
 * DICOM Viewer - Utilitários do Menu
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <QFile>
#include <QByteArray>

#include "utils.h"


namespace dicom_viewer_windows {
    /**
     * @brief Verifica se um arquivo possui a assinatura padrão de um arquivo DICOM (Parte 10).
     *
     * A função verifica se o arquivo existe, se tem o tamanho mínimo necessário,
     * pula o preâmbulo de 128 bytes e verifica se os 4 bytes seguintes são "DICM".
     *
     * @param filePath O caminho completo para o arquivo a ser verificado.
     * @return true Se o arquivo for um DICOM válido, false caso contrário.
     */
    bool isValidDicomFile(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
        if (file.size() < 132) {
            return false;
        }
        if (!file.seek(128)) {
            return false;
        }
        if (file.read(4) != "DICM") {
            return false;
        }
        return true;
    }

    /**
     * @brief Converte uma imagem médica DICOM para QImage.
     *
     * Transforma os dados brutos de pixel da estrutura MedicalImage em um QImage
     * compatível com Qt para exibição em widgets gráficos. Suporta imagens em escala
     * de cinza (1 amostra por pixel) e RGB (3 amostras por pixel).
     *
     * @param rawImg A imagem DICOM a ser convertida.
     * @return QImage contendo os dados da imagem convertida, ou QImage nula se inválida.
     */
    QImage convertMedicalImage(const dicom_viewer_core::MedicalImage& rawImg) {
        if (!rawImg.isValid()) return QImage();
        
        long long width = rawImg.width;
        long long height = rawImg.height;
        long long requiredBytes = 0;
        QImage::Format format;
        int bytesPerLine = 0;
        
        if (rawImg.samplesPerPixel == 3) {
            requiredBytes = width * height * 3;
            format = QImage::Format_RGB888;
            bytesPerLine = width * 3;
        } else if (rawImg.samplesPerPixel == 1) {
            requiredBytes = width * height;
            format = QImage::Format_Grayscale8;
            bytesPerLine = width;
        } else {
            return QImage();
        }
        
        if ((long long)rawImg.buffer.size() < requiredBytes) {
            return QImage();
        }
        
        QImage img(rawImg.buffer.data(), width, height, bytesPerLine, format);
        return img.copy();
    }
}
