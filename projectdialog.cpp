#include "projectdialog.h"

ProjectDialog::ProjectDialog(ProjectInfo &projectInfo, bool newProject)
{
    this->m_newProject = newProject;
    this->m_projectInfo = &projectInfo;

    setMinimumSize(300, 200);
    setWindowTitle(tr("Project properties"));

    createControls();

    load();
}

ProjectDialog::~ProjectDialog()
{
    delete txtName;
    delete cmbProblemType;
    delete cmbPhysicField;
    delete dtmDate;
    delete txtNumberOfRefinements;
    delete txtPolynomialOrder;
    delete txtAdaptivitySteps;
    delete txtAdaptivityTolerance;
}

int ProjectDialog::showDialog()
{
    return exec();
}

void ProjectDialog::createControls()
{
    cmbProblemType = new QComboBox();
    if (this->m_newProject) cmbPhysicField = new QComboBox();
    txtName = new QLineEdit("");
    dtmDate = new QDateTimeEdit();
    dtmDate->setDisplayFormat("dd.MM.yyyy");
    dtmDate->setCalendarPopup(true);
    txtNumberOfRefinements = new QSpinBox(this);
    txtNumberOfRefinements->setMinimum(0);
    txtNumberOfRefinements->setMaximum(5);
    txtPolynomialOrder = new QSpinBox(this);
    txtPolynomialOrder->setMinimum(1);
    txtPolynomialOrder->setMaximum(10);
    txtAdaptivitySteps = new QSpinBox(this);
    txtAdaptivitySteps->setMinimum(0);
    txtAdaptivitySteps->setMaximum(100);
    txtAdaptivityTolerance = new SLineEdit("1", true, this);

    QGridLayout *layoutProject = new QGridLayout();
    layoutProject->addWidget(new QLabel(tr("Name:")), 0, 0);
    layoutProject->addWidget(txtName, 0, 1);
    layoutProject->addWidget(new QLabel(tr("Date:")), 1, 0);
    layoutProject->addWidget(dtmDate, 1, 1);
    layoutProject->addWidget(new QLabel(tr("Problem type:")), 2, 0);
    layoutProject->addWidget(cmbProblemType, 2, 1);
    layoutProject->addWidget(new QLabel(tr("Physic field:")), 3, 0);
    if (!this->m_newProject)
        layoutProject->addWidget(new QLabel(physicFieldString(m_projectInfo->physicField)), 3, 1);
    else
        layoutProject->addWidget(cmbPhysicField, 3, 1);
    layoutProject->addWidget(new QLabel(tr("Number of refinements:")), 4, 0);
    layoutProject->addWidget(txtNumberOfRefinements, 4, 1);
    layoutProject->addWidget(new QLabel(tr("Polynomial order:")), 5, 0);
    layoutProject->addWidget(txtPolynomialOrder, 5, 1);
    layoutProject->addWidget(new QLabel(tr("Adaptivity steps:")), 6, 0);
    layoutProject->addWidget(txtAdaptivitySteps, 6, 1);
    layoutProject->addWidget(new QLabel(tr("Adaptivity tolerance:")), 7, 0);
    layoutProject->addWidget(txtAdaptivityTolerance, 7, 1);

    fillComboBox();

    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(layoutProject);
    layout->addStretch();
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void ProjectDialog::fillComboBox()
{
    cmbProblemType->clear();
    cmbProblemType->addItem(tr("planar"), PROBLEMTYPE_PLANAR);
    cmbProblemType->addItem(tr("axisymmetric"), PROBLEMTYPE_AXISYMMETRIC);

    if (this->m_newProject)
    {
        cmbPhysicField->clear();
        cmbPhysicField->addItem(tr("electrostatic"), PHYSICFIELD_ELECTROSTATIC);
        cmbPhysicField->addItem(tr("magnetostatic"), PHYSICFIELD_MAGNETOSTATIC);
        // cmbPhysicField->addItem(tr("current field"), PHYSICFIELD_CURRENT);
        cmbPhysicField->addItem(tr("heat transfer"), PHYSICFIELD_HEAT_TRANSFER);
        // cmbPhysicField->addItem(tr("elasticity"), PHYSICFIELD_ELASTICITY);
    }
}

void ProjectDialog::load() {
    if (this->m_newProject) cmbPhysicField->setCurrentIndex(cmbPhysicField->findData(m_projectInfo->physicField));
    txtName->setText(m_projectInfo->name);
    cmbProblemType->setCurrentIndex(cmbProblemType->findData(m_projectInfo->problemType));
    dtmDate->setDate(m_projectInfo->date);
    txtNumberOfRefinements->setValue(m_projectInfo->numberOfRefinements);
    txtPolynomialOrder->setValue(m_projectInfo->polynomialOrder);
    txtAdaptivitySteps->setValue(m_projectInfo->adaptivitySteps);
    txtAdaptivityTolerance->setValue(m_projectInfo->adaptivityTolerance);
}

void ProjectDialog::save() {
    if (this->m_newProject) m_projectInfo->physicField = (PhysicField) cmbPhysicField->itemData(cmbPhysicField->currentIndex()).toInt();
    m_projectInfo->problemType = (ProblemType) cmbProblemType->itemData(cmbProblemType->currentIndex()).toInt();
    m_projectInfo->name = txtName->text();
    m_projectInfo->date = dtmDate->date();
    m_projectInfo->numberOfRefinements = txtNumberOfRefinements->value();
    m_projectInfo->polynomialOrder = txtPolynomialOrder->value();
    m_projectInfo->adaptivitySteps = txtAdaptivitySteps->value();
    m_projectInfo->adaptivityTolerance = txtAdaptivityTolerance->value();
}

void ProjectDialog::doAccept()
{
    save();
    accept();
}

void ProjectDialog::doReject()
{
    reject();
}
