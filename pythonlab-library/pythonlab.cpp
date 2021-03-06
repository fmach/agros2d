// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#include "pythonlab.h"

PythonLabApplication::PythonLabApplication(int& argc, char ** argv) : QtSingleApplication(argc, argv)
{
    setWindowIcon(icon("pythonlab"));
    setApplicationVersion(versionString());
    setOrganizationName("hpfem.org");
    setOrganizationDomain("hpfem.org");
    setApplicationName("PythonLab");

#ifdef Q_WS_MAC
    // don't show icons in menu
    setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    // std::string codec
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // force number format
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
}

PythonLab::PythonLab(QWidget *parent)
    : QMainWindow(parent)
{
}

PythonLab::~PythonLab()
{
    
}
