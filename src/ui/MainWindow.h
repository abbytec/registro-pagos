#pragma once
#include <QMainWindow>
class QLineEdit;
class QTableView;
class QSqlTableModel;
class QSqlQueryModel;
class QDoubleSpinBox;
class QDateEdit;
class QProgressBar;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    // Alumnos
    QLineEdit *docEdit;
    QLineEdit *nameEdit;
    QTableView *studentsTable;
    QSqlTableModel *studentsModel;
    // Pagos
    QLineEdit *payDocEdit;
    QDoubleSpinBox *amountSpin;
    QDateEdit *paidDate;
    QLineEdit *methodEdit;
    QLineEdit *notesEdit;
    QTableView *paymentsTable;
    QSqlQueryModel *paymentsModel;
    // Buscar
    QLineEdit *searchEdit;
    QTableView *searchTable;
    QSqlQueryModel *searchModel;
    // Licencia
    QLabel *limitLabel;
    QProgressBar *usageBar;
    QLineEdit *licenseKeyEdit;

    void refreshModels();
    void refreshLicenseWidgets();
};