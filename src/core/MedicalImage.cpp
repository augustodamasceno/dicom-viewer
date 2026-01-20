/**
 * DICOM Viewer - Memória para Imagem Médica
 * Copyright (c) 2026, Augusto Damasceno.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <vector>
#include <iostream>
#include <cstring>

#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmjpeg/djdecode.h>

#include "MedicalImage.h"

namespace dicom_viewer_core {

/**
 * @brief Carrega um arquivo DICOM e retorna os dados de imagem e metadados.
 * 
 * Abre um arquivo DICOM, extrai os dados de pixel, metadados DICOM e
 * converte para um formato em memória facilmente acessível.
 *
 * @param path Caminho completo do arquivo DICOM a ser carregado.
 * @param want16Bit Se true, retorna imagem em 16 bits; se false, em 8 bits.
 * @return MedicalImage contendo os dados e metadados da imagem DICOM.
 * 
 * @note Se o arquivo não puder ser carregado, retorna uma MedicalImage inválida (isValid() = false).
 */
MedicalImage loadDicomRaw(const std::string& path, bool want16Bit) {
    MedicalImage output;
    DJDecoderRegistration::registerCodecs();

    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(path.c_str());
    
    if (status.bad()) {
        std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
        DJDecoderRegistration::cleanup();
        return output;
    }
    DcmDataset *dataset = fileformat.getDataset();
    
    // Primeiro, tenta descomprimir para Little Endian Explicit VR
    OFCondition decompResult = dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);
    if (decompResult.bad()) {
        std::cerr << "Warning: chooseRepresentation(EXS_LittleEndianExplicit) failed (" << decompResult.text() << ")" << std::endl;
        
        // Tenta com implicit
        decompResult = dataset->chooseRepresentation(EXS_LittleEndianImplicit, nullptr);
        if (decompResult.bad()) {
            std::cerr << "Warning: chooseRepresentation(EXS_LittleEndianImplicit) also failed (" << decompResult.text() << ")" << std::endl;
        }
    }
    
    // Força a decomposição para forma unencapsulated
    Uint16 planarConfig = 0;
    
    if (dataset->findAndGetUint16(DCM_PlanarConfiguration, planarConfig).good()) {
    }

    OFString photometricInterpretation;
    if (dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation).bad()) {
        photometricInterpretation = "RGB";
        std::cerr << "Warning: PhotometricInterpretation missing, assuming RGB" << std::endl;
    }
    
    Uint16 rows = 0, cols = 0;
    dataset->findAndGetUint16(DCM_Rows, rows);
    dataset->findAndGetUint16(DCM_Columns, cols);
    DicomImage dcmImage(path.c_str());

    if (dcmImage.getStatus() != EIS_Normal) {
        std::cerr << "Error: cannot load DICOM image (status: " << (int)dcmImage.getStatus() << ")" << std::endl;
        
        // Se DicomImage falhar, tenta usar dimensões do dataset
        if (rows > 0 && cols > 0) {
            std::cerr << "  Using dataset dimensions instead..." << std::endl;
            output.width = cols;
            output.height = rows;
        } else {
            std::cerr << "  Dataset dimensions also invalid" << std::endl;
            DJDecoderRegistration::cleanup();
            return output;
        }
    } else {
        output.width = dcmImage.getWidth();
        output.height = dcmImage.getHeight();
    }
    
    output.bitDepth = want16Bit ? 16 : 8;
    
    // Se ainda assim width ou height forem 0, falha
    if (output.width == 0 || output.height == 0) {
        std::cerr << "Error: Image dimensions are still invalid (0x0)" << std::endl;
        DJDecoderRegistration::cleanup();
        return output;
    }

    // Extrair metadados
    Uint16 samplesPerPixel = 0;
    Uint16 bitsAllocated = 0;
    Uint16 bitsStored = 0;
    Uint16 highBit = 0;
    Uint16 pixelRepresentation = 0;
    if (dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel).good()) {
        output.samplesPerPixel = samplesPerPixel;
    }
    if (dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated).good()) {
        output.bitsAllocated = bitsAllocated;
    }
    if (dataset->findAndGetUint16(DCM_BitsStored, bitsStored).good()) {
        output.bitsStored = bitsStored;
    }
    if (dataset->findAndGetUint16(DCM_HighBit, highBit).good()) {
        output.highBit = highBit;
    }
    if (dataset->findAndGetUint16(DCM_PixelRepresentation, pixelRepresentation).good()) {
        output.pixelRepresentation = pixelRepresentation;
    }
    Float64 windowCenter = 0.0;
    Float64 windowWidth = 0.0;
    if (dataset->findAndGetFloat64(DCM_WindowCenter, windowCenter).good()) {
        output.windowCenter = windowCenter;
    }
    if (dataset->findAndGetFloat64(DCM_WindowWidth, windowWidth).good()) {
        output.windowWidth = windowWidth;
    }
    OFString patientName;
    OFString studyDate;
    OFString modality;
    if (dataset->findAndGetOFString(DCM_PatientName, patientName).good()) {
        output.patientName = patientName.c_str();
    }
    if (dataset->findAndGetOFString(DCM_StudyDate, studyDate).good()) {
        output.studyDate = studyDate.c_str();
    }
    if (dataset->findAndGetOFString(DCM_Modality, modality).good()) {
        output.modality = modality.c_str();
    }
    output.photometricInterpretation = photometricInterpretation.c_str();
    
    // Se DicomImage falhou, tentar extrair pixel data do dataset
    bool useDirectDataset = (dcmImage.getStatus() != EIS_Normal);
    
    int bits = want16Bit ? 16 : 8;
    unsigned long size = 0;
    
    if (!useDirectDataset) {
        // Configura Janelamento
        if (!want16Bit && dcmImage.isMonochrome()) {
            dcmImage.setMinMaxWindow();
        }
        size = dcmImage.getOutputDataSize(bits);
    } else {
        size = (unsigned long)output.width * output.height * (bitsAllocated / 8) * samplesPerPixel;
    }
    
    if (samplesPerPixel == 3) {
        bits = 8;
        size = (unsigned long)output.width * output.height * 3;
    }
    
    if (size == 0) {
        std::cerr << "Error: calculated output data size is 0" << std::endl;
        std::cerr << "  width=" << output.width << ", height=" << output.height 
                  << ", samplesPerPixel=" << samplesPerPixel 
                  << ", bitsAllocated=" << bitsAllocated << std::endl;
        DJDecoderRegistration::cleanup();
        return output;
    }
    
    output.buffer.resize(size);
    unsigned long bytesWritten = 0;
    
    if (!useDirectDataset) {
        bytesWritten = dcmImage.getOutputData((void*)output.buffer.data(), size, bits);
    } else {
        // Primeiro tenta como OB (Other Byte)
        const Uint8 *pixelBytes = nullptr;
        if (dataset->findAndGetUint8Array(DCM_PixelData, pixelBytes).good() && pixelBytes != nullptr) {
            DcmElement *element = nullptr;
            if (dataset->findAndGetElement(DCM_PixelData, element).good() && element != nullptr) {
                bytesWritten = element->getLength();
                
                // Verificar se está comprimido
                if (bytesWritten > 0 && bytesWritten < (unsigned long)(output.width * output.height * 10)) {                    
                    if (bytesWritten <= size) {
                        memcpy(output.buffer.data(), pixelBytes, bytesWritten);
                    } else {
                        bytesWritten = size;
                        memcpy(output.buffer.data(), pixelBytes, size);
                    }
                }
            }
        } else {
            std::cerr << "Warning: Could not extract as Uint8Array" << std::endl;
            
            // Tenta como OW (Other Word)
            const Uint16 *pixelWords = nullptr;
            if (dataset->findAndGetUint16Array(DCM_PixelData, pixelWords).good() && pixelWords != nullptr) {
                DcmElement *element = nullptr;
                if (dataset->findAndGetElement(DCM_PixelData, element).good() && element != nullptr) {
                    bytesWritten = element->getLength();
                    if (bytesWritten <= size) {
                        memcpy(output.buffer.data(), pixelWords, bytesWritten);
                    } else {
                        std::cerr << "Warning: PixelData (" << bytesWritten << ") larger than allocated buffer (" << size << ")" << std::endl;
                        bytesWritten = size;
                        memcpy(output.buffer.data(), pixelWords, size);
                    }
                }
            } else {
                std::cerr << "ERROR: Could not extract as Uint16Array either" << std::endl;
            }
        }
    }
    
    if (bytesWritten == 0) {
        std::cerr << "ERROR: Failed to extract any pixel data" << std::endl;
        output.buffer.clear();
        output.width = 0;
        output.height = 0;
        DJDecoderRegistration::cleanup();
        return output;
    }
    
    DJDecoderRegistration::cleanup();

    return output;
}

/**
 * @brief Formata os metadados DICOM de uma imagem médica em uma string legível.
 *
 * @param medicalImage A imagem DICOM com os metadados.
 * @return std::string contendo os metadados formatados para exibição.
 */
std::string getDicomMetadata(const MedicalImage& medicalImage) {
    std::string metadataText;

    metadataText += "Paciente\n";
    metadataText += "  Nome: " + (medicalImage.patientName.empty() ? std::string("N/A") : medicalImage.patientName) + "\n";
    metadataText += "  Data do Estudo: " + (medicalImage.studyDate.empty() ? std::string("N/A") : medicalImage.studyDate) + "\n\n";

    metadataText += "Imagem\n";
    metadataText += "  Dimensões: " + std::to_string(medicalImage.width) + " x " + std::to_string(medicalImage.height) + " pixels\n";
    metadataText += "  Profundidade de Bits: " + std::to_string(medicalImage.bitDepth) + " bits\n";
    metadataText += "  Espaçamento X: " + std::to_string(medicalImage.spacingX) + "\n";
    metadataText += "  Espaçamento Y: " + std::to_string(medicalImage.spacingY) + "\n";
    metadataText += "  Photometric Interpretation: " + medicalImage.photometricInterpretation + "\n\n";

    metadataText += "Informações de Pixel\n";
    metadataText += "  Samples per Pixel: " + std::to_string(medicalImage.samplesPerPixel) + "\n";
    metadataText += "  Bits Allocated: " + std::to_string(medicalImage.bitsAllocated) + "\n";
    metadataText += "  Bits Stored: " + std::to_string(medicalImage.bitsStored) + "\n";
    metadataText += "  High Bit: " + std::to_string(medicalImage.highBit) + "\n";
    metadataText += "  Pixel Representation: " + std::to_string(medicalImage.pixelRepresentation) + " (" + (medicalImage.pixelRepresentation == 0 ? std::string("Unsigned") : std::string("Signed")) + ")\n\n";
    
    metadataText += "Janelamento\n";
    metadataText += "  Window Center: " + std::to_string(medicalImage.windowCenter) + "\n";
    metadataText += "  Window Width: " + std::to_string(medicalImage.windowWidth) + "\n\n";
    
    metadataText += "Modalidade: " + (medicalImage.modality.empty() ? std::string("N/A") : medicalImage.modality) + "\n";
    
    return metadataText;
}

}