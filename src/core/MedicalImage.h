/**
 * DICOM Viewer - Memória para Imagem Médica
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vector>
#include <cstdint>
#include <iostream>

#include <dcmtk/dcmimgle/dcmimage.h>

#ifndef MEDICALIMAGE_H
#define MEDICALIMAGE_H

namespace dicom_viewer_core {

/**
 * @struct MedicalImage
 * @brief Estrutura para armazenar dados de imagem médica DICOM em memória.
 * 
 * Contém dimensões, profundidade de bits, espaçamento de pixels,
 * metadados DICOM e dados de pixel brutos.
 */
struct MedicalImage {
    int width = 0;                                    ///< Largura da imagem em pixels
    int height = 0;                                   ///< Altura da imagem em pixels
    int bitDepth = 0;                                 ///< Profundidade de bits (8 ou 16)
    double spacingX = 1.0;                            ///< Espaçamento entre pixels no eixo X
    double spacingY = 1.0;                            ///< Espaçamento entre pixels no eixo Y

    // Metadados DICOM
    int samplesPerPixel = 1;                          ///< Número de componentes de cor por pixel
    int bitsAllocated = 8;                            ///< Bits alocados por amostra
    int bitsStored = 8;                               ///< Bits realmente armazenados
    int highBit = 7;                                  ///< Posição do bit mais significativo
    int pixelRepresentation = 0;                      ///< 0 = unsigned, 1 = signed
    double windowCenter = 0.0;                        ///< Centro do janelamento (Window/Level)
    double windowWidth = 0.0;                         ///< Largura do janelamento (Window/Level)
    std::string patientName;                          ///< Nome do paciente
    std::string studyDate;                            ///< Data do estudo
    std::string modality;                             ///< Modalidade DICOM (ex: CT, MR, XA)
    std::string photometricInterpretation = "MONOCHROME2";  ///< Interpretação fotométrica

    std::vector<uint8_t> buffer;                      ///< Buffer com dados de pixel brutos

    /**
     * @brief Verifica se a imagem contém dados válidos.
     * @return true se a imagem é válida, false caso contrário.
     */
    bool isValid() const { return !buffer.empty() && width > 0 && height > 0; }
    
    /**
     * @brief Retorna ponteiro para os dados de pixel brutos.
     * @return Ponteiro para o início do buffer de dados.
     */
    uint8_t* rawPtr() { return buffer.data(); }
};

MedicalImage loadDicomRaw(const std::string& path, bool want16Bit);

/**
 * @brief Formata os metadados DICOM de uma imagem médica em uma string legível.
 *
 * @param medicalImage A imagem DICOM com os metadados.
 * @return std::string contendo os metadados formatados para exibição.
 */
std::string getDicomMetadata(const MedicalImage& medicalImage);

}

#endif // MEDICALIMAGE_H