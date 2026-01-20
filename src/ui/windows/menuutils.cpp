/**
 * DICOM VIewer - Utilitários do Menu
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <QFile>
#include <QByteArray>
#include "menuutils.h"

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
}