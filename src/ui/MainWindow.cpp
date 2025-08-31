#include "MainWindow.h"
#include "./data/repository/StudentRepository.h"
#include "./data/repository/PaymentRepository.h"
#include "./data/Database.h"
#include "LicenseManager.h"
#include "TableUtils.h"
#include <QtWidgets>
#include <QtSql>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          studentsModel(nullptr), paymentsModel(nullptr), searchModel(nullptr)
{
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // Alumnos
    auto *sTab = new QWidget;
    auto *sLayout = new QVBoxLayout(sTab);
    auto *sForm = new QHBoxLayout;
    docEdit = new QLineEdit;
    docEdit->setPlaceholderText("Documento");
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Nombre completo");
    auto *addStudentBtn = new QPushButton("Agregar");
    sForm->addWidget(docEdit);
    sForm->addWidget(nameEdit);
    sForm->addWidget(addStudentBtn);
    studentsTable = new QTableView;
    studentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sLayout->addLayout(sForm);
    sLayout->addWidget(studentsTable);
    tabs->addTab(sTab, "Alumnos");

    // Pagos
    auto *pTab = new QWidget;
    auto *pLayout = new QVBoxLayout(pTab);
    auto *pForm = new QHBoxLayout;
    payDocEdit = new QLineEdit;
    payDocEdit->setPlaceholderText("Documento del alumno");
    amountSpin = new QDoubleSpinBox;
    amountSpin->setMaximum(1e9);
    amountSpin->setDecimals(2);
    amountSpin->setPrefix("$ ");
    paidDate = new QDateEdit(QDate::currentDate());
    paidDate->setCalendarPopup(true);
    methodEdit = new QLineEdit;
    methodEdit->setPlaceholderText("Método (efectivo, transferencia...)");
    notesEdit = new QLineEdit;
    notesEdit->setPlaceholderText("Notas");
    auto *addPayBtn = new QPushButton("Registrar pago");
    pForm->addWidget(payDocEdit);
    pForm->addWidget(amountSpin);
    pForm->addWidget(paidDate);
    pForm->addWidget(methodEdit);
    pForm->addWidget(notesEdit);
    pForm->addWidget(addPayBtn);
    paymentsTable = new QTableView;
    paymentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    paymentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pLayout->addLayout(pForm);
    pLayout->addWidget(paymentsTable);
    tabs->addTab(pTab, "Pagos");

    // Buscar
    auto *fTab = new QWidget;
    auto *fLayout = new QVBoxLayout(fTab);
    auto *fForm = new QHBoxLayout;
    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Buscar por documento o nombre");
    auto *searchBtn = new QPushButton("Buscar");
    fForm->addWidget(searchEdit);
    fForm->addWidget(searchBtn);
    searchTable = new QTableView;
    searchTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    searchTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fLayout->addLayout(fForm);
    fLayout->addWidget(searchTable);
    tabs->addTab(fTab, "Buscar");

    // Licencia
    auto *lTab = new QWidget;
    auto *lLayout = new QVBoxLayout(lTab);
    auto *top = new QHBoxLayout;
    limitLabel = new QLabel;
    usageBar = new QProgressBar;
    usageBar->setTextVisible(true);
    top->addWidget(limitLabel);
    top->addWidget(usageBar);
    auto *lForm = new QHBoxLayout;
    licenseKeyEdit = new QLineEdit;
    licenseKeyEdit->setPlaceholderText("Pega tu clave de licencia (payload.signature)");
    auto *applyBtn = new QPushButton("Aplicar");
    lForm->addWidget(licenseKeyEdit);
    lForm->addWidget(applyBtn);
    lLayout->addLayout(top);
    lLayout->addLayout(lForm);
    tabs->addTab(lTab, "Licencia");

    // Modelos
    studentsModel = StudentRepository::model(this);
    studentsTable->setModel(studentsModel);
    TableUtils::applyInteractiveMinPerColumn(studentsTable, QVector<int>{48, 175, 232, 175}, 48);
    paymentsModel = PaymentRepository::all(this);
    paymentsTable->setModel(paymentsModel);
    TableUtils::applyInteractiveMinPerColumn(paymentsTable, QVector<int>{48, 175, 232, 72, 128, 128, 256}, 48);
    searchModel = new QSqlQueryModel(this);
    searchTable->setModel(searchModel);
    TableUtils::applyInteractiveMinPerColumn(searchTable, QVector<int>{48, 175, 232, 72, 128, 128, 256}, 48);

    // Conexiones
    connect(addStudentBtn, &QPushButton::clicked, this, [this]()
            {
        QString err;
        if (StudentRepository::add(docEdit->text(), nameEdit->text(), &err)) {
            docEdit->clear(); nameEdit->clear(); studentsModel->select();
        } else { QMessageBox::warning(this, "Error", "No se pudo agregar.\n" + err); } });
    connect(addPayBtn, &QPushButton::clicked, this, [this]()
            {
        int sid = StudentRepository::idByDoc(payDocEdit->text());
        if (sid < 0) { QMessageBox::warning(this, "Error", "Documento no encontrado."); return; }
        QString err;
        if (PaymentRepository::add(sid, amountSpin->value(), paidDate->date(), methodEdit->text(), notesEdit->text(), &err)) {
            methodEdit->clear(); notesEdit->clear();
            paymentsModel->setQuery(paymentsModel->query().lastQuery(), Database::db());
            refreshLicenseWidgets();
        } else { QMessageBox::warning(this, "Error", err); } });
    connect(searchBtn, &QPushButton::clicked, this, [this]()
            {
        auto* m = PaymentRepository::search(searchEdit->text(), this);
        searchTable->setModel(m);
        if (searchModel) searchModel->deleteLater();
        searchModel = m; });
    connect(applyBtn, &QPushButton::clicked, this, [this]()
            {
        QString err;
        if (LicenseManager::applyKey(licenseKeyEdit->text(), &err)) {
            refreshLicenseWidgets();
            QMessageBox::information(this, "Licencia", "Licencia aplicada.");
        } else { QMessageBox::warning(this, "Licencia", err); } });

    refreshLicenseWidgets();
}
void MainWindow::refreshModels()
{
    if (studentsModel)
        studentsModel->select();
    if (paymentsModel)
        paymentsModel->setQuery(paymentsModel->query().lastQuery(), Database::db());
}
void MainWindow::refreshLicenseWidgets()
{
    const int used = LicenseManager::usedRecords();
    const int limit = LicenseManager::allowedLimit();
    limitLabel->setText(QString("Uso: %1 / %2").arg(used).arg(limit));
    usageBar->setMaximum(limit);
    usageBar->setValue(used);
}