/**
 * DICOM Viewer - Utilitários do Menu
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MENUUTILS_H
#define MENUUTILS_H

#include <QString>
#include <QImage>
#include "../../core/MedicalImage.h"

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
	bool isValidDicomFile(const QString &filePath);

	/**
	 * @brief Converte uma imagem médica DICOM para QImage.
	 *
	 * @param rawImg A imagem DICOM a ser convertida.
	 * @return QImage contendo os dados da imagem convertida.
	 */
	QImage convertMedicalImage(const dicom_viewer_core::MedicalImage& rawImg);
}

#endif // MENUUTILS_H
