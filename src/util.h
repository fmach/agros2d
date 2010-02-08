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

#ifndef UTIL_H
#define UTIL_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include <QtHelp/QHelpEngine>

#include <typeinfo>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

const double EPS_ZERO = 1e-10;
const double EPS0 = 8.854e-12;
const double MU0 = 4*M_PI*1e-7;
const int NDOF_STOP = 40000;

using namespace std;

// enable log file
void enableLogFile(bool enable);

// set gui style
void setGUIStyle(const QString &styleName);

// set language
void setLanguage(const QString &locale);

// get available languages
QStringList availableLanguages();

// get icon with respect to actual theme
QIcon icon(const QString &name);

// get datadir
QString datadir();

// get external js functions
QString externalFunctions();

// get temp dir
QString tempProblemDir();

// get temp filename
QString tempProblemFileName();

// convert time in ms to QTime
QTime milliSecondsToTime(int ms);

// remove directory content
bool removeDirectory(const QDir &dir);

// sleep function
void msleep(unsigned long msecs);

// log to file
void log(const QString &message);

// read file content
QByteArray readFileContentByteArray(const QString &fileName);
QString readFileContent(const QString &fileName);

// write content into the file
void writeStringContent(const QString &fileName, QString *content);
void writeStringContentByteArray(const QString &fileName, QByteArray content);

// global exception handler
void exception_global();

struct Value
{
    QString text;
    double number;

    Value() { text = "0"; number = 0;}
    inline Value(const QString &value) { text = value; evaluate(true); }

    bool evaluate(bool quiet = false);
};

struct Point
{
    double x, y;

    Point() { this->x = 0; this->y = 0; }
    Point(double x, double y) { this->x = x; this->y = y; }

    inline Point operator+(const Point &vec) const { return Point(x + vec.x, y + vec.y); }
    inline Point operator-(const Point &vec) const { return Point(x - vec.x, y - vec.y); }
    inline Point operator*(double num) const { return Point(x * num, y * num); }
    inline Point operator/(double num) const { return Point(x / num, y / num); }
    inline double operator&(const Point &vec) const { return x*vec.x + y*vec.y; } // dot product
    inline double operator%(const Point &vec) const { return x*vec.y - y*vec.x; } // cross product
    inline bool operator!=(const Point &vec) const { return ((fabs(vec.x-x) > EPS_ZERO) || (fabs(vec.y-y) > EPS_ZERO)); }
    inline bool operator==(const Point &vec) const { return ((fabs(vec.x-x) < EPS_ZERO) && (fabs(vec.y-y) < EPS_ZERO)); }

    inline double magnitude() { return sqrt(x * x + y * y); }
    inline double angle() { return atan2(y, x); }

    Point normalizePoint()
    {
        double m = magnitude();

        double mx = x/m;
        double my = y/m;

        return Point(mx, my);
    }
};

struct Point3
{
    double x, y, z;

    Point3() { this->x = 0; this->y = 0; this->z = 0; }
    Point3(double x, double y, double z) { this->x = x; this->y = y; this->z = z; }

    inline Point3 operator+(const Point3 &vec) const { return Point3(x + vec.x, y + vec.y, z + vec.z); }
    inline Point3 operator-(const Point3 &vec) const { return Point3(x - vec.x, y - vec.y, z - vec.z); }
    inline Point3 operator*(double num) const { return Point3(x * num, y * num, z * num); }
    inline Point3 operator/(double num) const { return Point3(x / num, y / num, z / num); }

    inline double magnitude() { return sqrt(x * x + y * y); }
    inline double anglexy() { return atan2(y, x); }
    inline double angleyz() { return atan2(z, y); }
    inline double anglezx() { return atan2(x, z); }

    Point3 normalizePoint()
    {
        double m = magnitude();

        double mx = x/m;
        double my = y/m;
        double mz = z/m;

        return Point3(mx, my, mz);
    }
};

struct RectPoint
{
    Point start;
    Point end;

    inline RectPoint(const Point &start, const Point &end) { this->start = start; this->end = end; }
    inline RectPoint() { this->start = Point(); this->end = Point(); }

    inline void set(const Point &start, const Point &end) { this->start = start; this->end = end; }
    inline double width() { return fabs(end.x - start.x); }
    inline double height() { return fabs(end.y - start.y); }
};

struct ScriptResult
{
    ScriptResult()
    {
        text = "";
        isError = false;
    }

    ScriptResult(const QString &text, bool isError = false)
    {
        this->text = text;
        this->isError = isError;
    }

    QString text;
    bool isError;
};

struct ExpressionResult
{
    ExpressionResult()
    {
        this->error = "";
        this->value = 0.0;
    }

    ExpressionResult(double value, const QString &error)
    {
        this->error = error;
        this->value = value;
    }

    QString error;
    double value;
};

enum ErrorResultType
{
    ERRORRESULT_NONE,
    ERRORRESULT_INFORMATION,
    ERRORRESULT_WARNING,
    ERRORRESULT_CRITICAL
};

class ErrorResult
{
public:
    inline ErrorResultType type() { return m_type; }
    inline QString message() { return m_message; }

    inline ErrorResult()
    {
        m_type = ERRORRESULT_NONE;
        m_message = "";
    }

    inline ErrorResult(ErrorResultType type, QString message)
    {
        m_type = type;
        m_message = message;
    }

    inline bool isError() { return (m_type != ERRORRESULT_NONE); }

    void showDialog()
    {
        switch (m_type)
        {
        case ERRORRESULT_NONE:
            return;
        case ERRORRESULT_INFORMATION:
            QMessageBox::information(QApplication::activeWindow(), QObject::tr("Information"), m_message);
            break;
        case ERRORRESULT_WARNING:
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"), m_message);
            break;
        case ERRORRESULT_CRITICAL:
            QMessageBox::critical(QApplication::activeWindow(), QObject::tr("Critical"), m_message);
            break;
        }
    }

private:
     ErrorResultType m_type;
     QString m_message;
};

enum SolverMode
{
    SOLVER_MESH,
    SOLVER_MESH_AND_SOLVE
};

enum ProblemType
{
    PROBLEMTYPE_UNDEFINED,
    PROBLEMTYPE_PLANAR,
    PROBLEMTYPE_AXISYMMETRIC
};

enum AnalysisType
{
    ANALYSISTYPE_UNDEFINED,
    ANALYSISTYPE_STEADYSTATE,
    ANALYSISTYPE_TRANSIENT,
    ANALYSISTYPE_HARMONIC
};

enum AdaptivityType
{
    ADAPTIVITYTYPE_UNDEFINED = 1000,
    ADAPTIVITYTYPE_NONE = 3,
    ADAPTIVITYTYPE_H = 1,
    ADAPTIVITYTYPE_P = 2,
    ADAPTIVITYTYPE_HP = 0
                    };

enum PhysicFieldVariableComp
{
    PHYSICFIELDVARIABLECOMP_UNDEFINED,
    PHYSICFIELDVARIABLECOMP_SCALAR,
    PHYSICFIELDVARIABLECOMP_MAGNITUDE,
    PHYSICFIELDVARIABLECOMP_X,
    PHYSICFIELDVARIABLECOMP_Y
};

enum PhysicField
{
    PHYSICFIELD_UNDEFINED,
    PHYSICFIELD_GENERAL,
    PHYSICFIELD_ELECTROSTATIC,
    PHYSICFIELD_CURRENT,
    PHYSICFIELD_HEAT,
    PHYSICFIELD_ELASTICITY,
    PHYSICFIELD_MAGNETIC
};

enum PhysicFieldVariable
{
    PHYSICFIELDVARIABLE_UNDEFINED,
    PHYSICFIELDVARIABLE_NONE,
    PHYSICFIELDVARIABLE_ORDER,
    PHYSICFIELDVARIABLE_GENERAL_VARIABLE,
    PHYSICFIELDVARIABLE_GENERAL_GRADIENT,
    PHYSICFIELDVARIABLE_GENERAL_CONSTANT,
    PHYSICFIELDVARIABLE_ELECTROSTATIC_POTENTIAL,
    PHYSICFIELDVARIABLE_ELECTROSTATIC_ELECTRICFIELD,
    PHYSICFIELDVARIABLE_ELECTROSTATIC_DISPLACEMENT,
    PHYSICFIELDVARIABLE_ELECTROSTATIC_ENERGY_DENSITY,
    PHYSICFIELDVARIABLE_ELECTROSTATIC_PERMITTIVITY,
    PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL,
    PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_FLUX_DENSITY,
    PHYSICFIELDVARIABLE_MAGNETIC_FLUX_DENSITY_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_FLUX_DENSITY_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_MAGNETICFIELD,
    PHYSICFIELDVARIABLE_MAGNETIC_MAGNETICFIELD_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_MAGNETICFIELD_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY_REAL,
    PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY_IMAG,
    PHYSICFIELDVARIABLE_MAGNETIC_POWER_LOSSES,    
    PHYSICFIELDVARIABLE_MAGNETIC_LORENTZ_FORCE,
    PHYSICFIELDVARIABLE_MAGNETIC_REMANENCE,
    PHYSICFIELDVARIABLE_MAGNETIC_ENERGY_DENSITY,
    PHYSICFIELDVARIABLE_MAGNETIC_PERMEABILITY,
    PHYSICFIELDVARIABLE_MAGNETIC_CONDUCTIVITY,
    PHYSICFIELDVARIABLE_MAGNETIC_VELOCITY,
    PHYSICFIELDVARIABLE_CURRENT_POTENTIAL,
    PHYSICFIELDVARIABLE_CURRENT_ELECTRICFIELD,
    PHYSICFIELDVARIABLE_CURRENT_CURRENT_DENSITY,
    PHYSICFIELDVARIABLE_CURRENT_LOSSES,
    PHYSICFIELDVARIABLE_CURRENT_CONDUCTIVITY,
    PHYSICFIELDVARIABLE_HEAT_TEMPERATURE,
    PHYSICFIELDVARIABLE_HEAT_TEMPERATURE_GRADIENT,
    PHYSICFIELDVARIABLE_HEAT_FLUX,
    PHYSICFIELDVARIABLE_HEAT_CONDUCTIVITY,
    PHYSICFIELDVARIABLE_ELASTICITY_VON_MISES_STRESS
};


enum PhysicFieldBC
{
    PHYSICFIELDBC_UNDEFINED,
    PHYSICFIELDBC_NONE,
    PHYSICFIELDBC_GENERAL_VALUE,
    PHYSICFIELDBC_GENERAL_DERIVATIVE,
    PHYSICFIELDBC_ELECTROSTATIC_POTENTIAL,
    PHYSICFIELDBC_ELECTROSTATIC_SURFACE_CHARGE,
    PHYSICFIELDBC_MAGNETIC_VECTOR_POTENTIAL,
    PHYSICFIELDBC_MAGNETIC_SURFACE_CURRENT,
    PHYSICFIELDBC_HEAT_TEMPERATURE,
    PHYSICFIELDBC_HEAT_HEAT_FLUX,
    PHYSICFIELDBC_CURRENT_POTENTIAL,
    PHYSICFIELDBC_CURRENT_INWARD_CURRENT_FLOW,
    PHYSICFIELDBC_ELASTICITY_FIXED,
    PHYSICFIELDBC_ELASTICITY_FREE
};

inline bool isPhysicFieldVariableScalar(PhysicFieldVariable physicFieldVariable)
{
    switch (physicFieldVariable)
    {
    case PHYSICFIELDVARIABLE_GENERAL_VARIABLE:
    case PHYSICFIELDVARIABLE_GENERAL_CONSTANT:

    case PHYSICFIELDVARIABLE_ELECTROSTATIC_POTENTIAL:
    case PHYSICFIELDVARIABLE_ELECTROSTATIC_ENERGY_DENSITY:
    case PHYSICFIELDVARIABLE_ELECTROSTATIC_PERMITTIVITY:

    case PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL_REAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_VECTOR_POTENTIAL_IMAG:

    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_REAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_IMAG:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL_REAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_TOTAL_IMAG:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM_REAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_TRANSFORM_IMAG:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY_REAL:
    case PHYSICFIELDVARIABLE_MAGNETIC_CURRENT_DENSITY_INDUCED_VELOCITY_IMAG:
    case PHYSICFIELDVARIABLE_MAGNETIC_ENERGY_DENSITY:
    case PHYSICFIELDVARIABLE_MAGNETIC_POWER_LOSSES:
    case PHYSICFIELDVARIABLE_MAGNETIC_PERMEABILITY:
    case PHYSICFIELDVARIABLE_MAGNETIC_FLUX_DENSITY:
    case PHYSICFIELDVARIABLE_MAGNETIC_MAGNETICFIELD:
    case PHYSICFIELDVARIABLE_MAGNETIC_CONDUCTIVITY:

    case PHYSICFIELDVARIABLE_HEAT_TEMPERATURE:
    case PHYSICFIELDVARIABLE_HEAT_CONDUCTIVITY:

    case PHYSICFIELDVARIABLE_CURRENT_POTENTIAL:
    case PHYSICFIELDVARIABLE_CURRENT_LOSSES:

    case PHYSICFIELDVARIABLE_ELASTICITY_VON_MISES_STRESS:
        return true;
        break;
    default:
        return false;
        break;
    }
}

enum SceneMode
{
    SCENEMODE_OPERATE_ON_NODES,
    SCENEMODE_OPERATE_ON_EDGES,
    SCENEMODE_OPERATE_ON_LABELS,
    SCENEMODE_POSTPROCESSOR
};

enum SceneModePostprocessor
{
    POSTPROCESSOR_LOCALVALUE,
    POSTPROCESSOR_SURFACEINTEGRAL,
    POSTPROCESSOR_VOLUMEINTEGRAL
};

enum PaletteType
{
    PALETTE_JET,
    PALETTE_AUTUMN,
    PALETTE_COPPER,
    PALETTE_HOT,
    PALETTE_COOL,
    PALETTE_BW_ASC,
    PALETTE_BW_DESC
};

enum SceneViewPostprocessorShow
{
    SCENEVIEW_POSTPROCESSOR_SHOW_UNDEFINED,
    SCENEVIEW_POSTPROCESSOR_SHOW_NONE,
    SCENEVIEW_POSTPROCESSOR_SHOW_SCALARVIEW,
    SCENEVIEW_POSTPROCESSOR_SHOW_SCALARVIEW3D,
    SCENEVIEW_POSTPROCESSOR_SHOW_SCALARVIEW3DSOLID,
    SCENEVIEW_POSTPROCESSOR_SHOW_ORDER
};

// captions
QString physicFieldVariableString(PhysicFieldVariable physicFieldVariable);
QString physicFieldVariableUnits(PhysicFieldVariable physicFieldVariable);
QString physicFieldString(PhysicField physicField);
QString analysisTypeString(AnalysisType analysisType);
QString physicFieldBCString(PhysicFieldBC physicFieldBC);
QString physicFieldVariableCompString(PhysicFieldVariableComp physicFieldVariableComp);
QString problemTypeString(ProblemType problemType);
QString adaptivityTypeString(AdaptivityType adaptivityType);

// keys
void initLists();

QString physicFieldToStringKey(PhysicField physicField);
PhysicField physicFieldFromStringKey(const QString &physicField);

inline QString problemTypeToStringKey(ProblemType problemType) { return ((problemType == PROBLEMTYPE_PLANAR) ? "planar" : "axisymmetric"); }
inline ProblemType problemTypeFromStringKey(const QString &problemType) { if (problemType == "planar") return PROBLEMTYPE_PLANAR; else if (problemType == "axisymmetric") return PROBLEMTYPE_AXISYMMETRIC; else return PROBLEMTYPE_UNDEFINED; }

QString analysisTypeToStringKey(AnalysisType analysisType);
AnalysisType analysisTypeFromStringKey(const QString &analysisType);

QString physicFieldVariableToStringKey(PhysicFieldVariable physicFieldVariable);
PhysicFieldVariable physicFieldVariableFromStringKey(const QString &physicFieldVariable);

QString physicFieldVariableCompToStringKey(PhysicFieldVariableComp physicFieldVariableComp);
PhysicFieldVariableComp physicFieldVariableCompFromStringKey(const QString &physicFieldVariableComp);

QString physicFieldBCToStringKey(PhysicFieldBC physicFieldBC);
PhysicFieldBC physicFieldBCFromStringKey(const QString &physicFieldBC);

QString sceneViewPostprocessorShowToStringKey(SceneViewPostprocessorShow sceneViewPostprocessorShow);
SceneViewPostprocessorShow sceneViewPostprocessorShowFromStringKey(const QString &sceneViewPostprocessorShow);

QString adaptivityTypeToStringKey(AdaptivityType adaptivityType);
AdaptivityType adaptivityTypeFromStringKey(const QString &adaptivityType);

#endif // UTIL_H
