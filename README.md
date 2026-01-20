# dicom-viewer  
DICOM (Digital Imaging and Communications in Medicine) Viewer  

> Copyright (c) 2018-2026, Augusto Damasceno.  
> All rights reserved.  
> SPDX-License-Identifier: BSD-2-Clause  
> Contact: augustodamasceno@protonmail.com  

## Overview

DICOM Viewer is a professional cross-platform medical image viewer built with modern C++ technologies. The application demonstrates a clean architecture separating frontend UI from backend image processing logic.

**Current Build**: Windows with MinGW toolchain

**Cross-Platform Ready**: While currently configured for Windows, Qt's exceptional portability allows this codebase to compile natively on Linux and macOS with minimal configuration changes - Qt abstracts all platform-specific details, enabling true write-once-compile-anywhere development.

### Key Features
- **Multi-platform Ready**: Built on Qt 6.5+ for seamless compilation on Windows, Linux, and macOS
- **DICOM File Support**: Loads and displays DICOM medical images with comprehensive metadata extraction
- **Grayscale & RGB Images**: Supports both monochrome and color medical images
- **Metadata Display**: Shows complete DICOM metadata including patient info, study date, modality
- **Dark Theme UI**: Professional dark gray interface with blue accents for extended viewing sessions

## Screenshots

![DICOM Viewer Main Interface](docs/screenshot_gray.jpg)

*Main application window displaying DICOM image with metadata panel*

## Architecture

### Frontend (UI Layer)
- **Framework**: Qt 6.5+
- **Portability**: Qt provides true cross-platform abstraction - this Windows application requires only CMakeLists.txt adjustments to compile identically on Linux and macOS, with zero code changes
- **Current Toolchain**: MinGW 64-bit on Windows
- **Alternative Toolchains**: MSVC on Windows, GCC on Linux, Clang on macOS - all fully supported by Qt
- **Components**: 
  - Main window with menu system
  - QGraphicsView for image display with fit-to-view scaling
  - QPlainTextEdit for metadata display

### Backend (Core Processing)
- **Purpose**: Medical image processing and DICOM file handling
- **Responsibilities**:
  - DICOM file validation and parsing
  - Image pixel data extraction and format conversion
  - Metadata extraction from DICOM files
  - Support for multiple pixel formats (Grayscale8, RGB888)
- **Technology**: Direct DCMTK integration for robust DICOM support

### Technical Stack
- **Language**: C++ 17
- **Build System**: CMake 3.19+
- **Dependency Management**: VCPKG for Windows/MinGW with automated package resolution
- **DICOM Library**: DCMTK

## Libraries and Compilation

### Required Libraries

#### 1. [Qt Framework](https://www.qt.io/development/download-open-source)
- **Version**: 6.5 or later
- **Components**: Core, Widgets, LinguistTools
- **Platform Support**: Windows (MSVC, MinGW), Linux (GCC, Clang), macOS (Clang)
- **License**: LGPL/Commercial

#### 2. [DCMTK - DICOM Toolkit](https://dicom.offis.de/en/dcmtk/dcmtk-tools/)
- **Version**: Latest stable
- **Features**: DICOM file I/O, image processing, JPEG codec support
- **License**: BSD license
- **Integration**: Via VCPKG package manager

#### 3. [VCPKG - C++ Package Manager](https://github.com/Microsoft/vcpkg)
- **Purpose**: Automatic dependency management on Windows/MinGW
- **Benefits**: One-line installation of DCMTK with all codecs

### Windows/MinGW Compilation Guide

#### Setup Prerequisites
```powershell
# 1. Add MinGW to PATH
$env:Path = "C:\Qt\Tools\mingw1310_64\bin;" + $env:Path
```

#### Verify GCC
```powershell
gcc --version
```

#### Initialize VCPKG
```powershell
cd $env:USERPROFILE\gitwin\vcpkg

# Bootstrap vcpkg if needed
if (-not (Test-Path vcpkg.exe)) { .\bootstrap-vcpkg.bat }
```

#### Install DCMTK with JPEG Support
```powershell
.\vcpkg install dcmtk:x64-mingw-dynamic
```

#### Build the Project
```powershell
cd C:\Users\cafec\gitwin\dicom-viewer

# Clean previous build
rm build -r -Force -ErrorAction SilentlyContinue

# Configure with CMake preset
cmake --preset default

# Build in Debug mode
cmake --build build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug
```

### Project Structure
```
dicom-viewer/
├── src/
│   ├── core/                    # Backend image processing
│   │   ├── MedicalImage.h       # Core data structure for DICOM images
│   │   ├── MedicalImage.cpp     # DICOM loading and metadata extraction
│   ├── ui/
│   │   ├── windows/             # Main application window
│   │   │   ├── mainwindow.h/cpp
│   │   │   ├── utils.h/cpp      # Image conversion utilities
│   │   ├── forms/               # Qt Designer UI files
│   │   ├── resources/           # Icons and assets
│   ├── translations/            # i18n files
│   ├── CMakeLists.txt           # Build configuration
│   ├── main.cpp                 # Entry point
├── README.md                    # This file
```

### Supported Formats
- **DICOM Files**: .dcm extension
- **Pixel Encodings**: 
  - Grayscale 8-bit (MONOCHROME2)
  - RGB 24-bit (RGB color images)
- **Compression**: JPEG (via DCMTK codec)

### Porting to Linux/macOS

To compile this application on Linux or macOS:

1. **Install Qt 6.5+** using the official installer or package manager
2. **Install DCMTK** via system package manager or build from source
3. **Update CMakeLists.txt**:
   - Adjust MinGW paths to your system's compiler paths
   - Remove Windows-specific `WIN32` flag from `qt_add_executable`
4. **Build**:
   ```bash
   cmake --preset default
   cmake --build build/Release
   ```
