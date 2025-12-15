#include "teacher.h"
#include "ui_teacher.h" // Assuming UI header is ui_teacher.h
#include <QStandardItem>
#include <QHeaderView> // Needed for QHeaderView::Stretch
#include "lms.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QFileDialog> // For QFileDialog::getOpenFileName
#include <QFileInfo>   // For QFileInfo::fileName()
#include <QDir>        // For QDir::currentPath()
#include <QLineEdit>   // For assignment name input
#include <QPushButton> // For the Browse button
#include <QComboBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QHeaderView>
#include <QApplication> // Needed for exec() loop in sub-dialog
#include <QDesktopServices> // Needed to get Downloads path (if available)
#include <QUrl> // Needed for QDesktopServices
#include <QDir> // For file paths
#include <QStandardPaths>


// Constructor implementation
teacher::teacher(const QStringList &teacherData, QWidget *parent)
    : QMainWindow(parent) // <<< QMainWindow base constructor call
    , ui(new Ui::teacher)
    , m_teacherData(teacherData) // Initialize the member variable
{
    ui->setupUi(this);
    ui->panel->setText("Welcome : "+m_teacherData[0]);
    // Call the function to populate the QTableView
    displayTeacherDetails();
}

teacher::~teacher()
{
    delete ui;
}

void teacher::displayTeacherDetails()
{
    // The CSV headers in order (to be used as row headers in the table):
    const QStringList headers = {
        "Name", "Enrollment", "Qualification", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree",
        "Enroll Year","Status", "Password"
    };

    // Check if we actually received data
    if (m_teacherData.isEmpty() || m_teacherData.size() != headers.size()) {
        return;
    }

    // 1. Create a model for the QTableView.
    QStandardItemModel *model = new QStandardItemModel(m_teacherData.size(), 2, this);

    // Set the headers for the columns (Field Name | Value)
    model->setHorizontalHeaderLabels({"Field", "Value"});

    // 2. Populate the model with the student data
    for (int i = 0; i < m_teacherData.size(); ++i) {
        // Set the Field Name in the first column
        QStandardItem *fieldItem = new QStandardItem(headers.at(i));
        model->setItem(i, 0, fieldItem);

        // Set the Value in the second column
        QStandardItem *valueItem = new QStandardItem(m_teacherData.at(i));
        model->setItem(i, 1, valueItem);
    }

    // 3. Set the model to the QTableView (assuming its object name is 'tableView' in student.ui)
    // Make sure your QMainWindow contains a QTableView named 'tableView'
    ui->tableView->setModel(model);

    // Optional: Make the table fit the content better
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void teacher::on_logout_clicked()
{
    lms *lmsWindow = new lms();//pointer to object of lms class
    lmsWindow->showMaximized();//open the lms.ui in it's maximum size
    this->close();//close admin when logout is clicked
}

void teacher::on_viewstudents_clicked()
{
    // The teacher's name is assumed to be the first element in m_teacherData
    const QString teacherName = m_teacherData.value(0); // Assuming index 0 is Name

    if (teacherName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Teacher name not found.");
        return;
    }

    // 1. Get the codes of courses assigned to this teacher
    QStringList assignedCourseCodes = getAssignedCourseCodes(teacherName);

    if (assignedCourseCodes.isEmpty()) {
        QMessageBox::information(this, "No Courses Assigned",
                                 "You have no courses currently assigned to you.");
        return;
    }

    // 2. Get the student data for those courses
    QStandardItemModel *studentModel = getStudentsInCourses(assignedCourseCodes);

    // 3. Display the data in the existing QTableView
    if (studentModel->rowCount() > 0) {

        // Ensure the old model (if it was a student model) is cleaned up by the QTableView.
        // The teacher details model (m_teacherDetailsModel) is intentionally kept alive
        // for restoration later.

        ui->tableView->setModel(studentModel);

        // Set the new model's parent to the QTableView. This ensures the student model
        // is automatically deleted when the next model is set (e.g., switching back to details).
        studentModel->setParent(ui->tableView);

        // Update the view settings for the new data
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->verticalHeader()->show(); // Show vertical headers for row numbering
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Change the panel text to reflect the current view
        ui->panel->setText("Viewing Students in Assigned Courses");

    } else {
        QMessageBox::information(this, "No Students Found",
                                 "No students are registered in your assigned courses.");
        delete studentModel; // Clean up the model if not used
    }
}


/**
 * @brief Reads courses.csv to find the codes of courses assigned to the teacher.
 * @param teacherName The name of the currently logged-in teacher.
 * @return A QStringList of course codes.
 */
QStringList teacher::getAssignedCourseCodes(const QString &teacherName)
{
    QStringList courseCodes;
    QFile file("courses.csv");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open courses.csv.");
        return courseCodes;
    }

    QTextStream in(&file);
    in.readLine(); // Skip header line: Title,Code,Credit Hours,Semester,Degree,Assigned Teacher

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(','); // Assuming comma-separated values

        // Check if the line has enough fields and the teacher matches
        // Code is at index 1 and 'Assigned Teacher' is at index 5 (0-indexed)
        if (fields.size() >= 6 && fields.at(5).trimmed() == teacherName.trimmed()) {
            courseCodes.append(fields.at(1).trimmed()); // Code is at index 1
        }
    }

    file.close();
    return courseCodes;
}

/**
 * @brief Reads registered_students.csv to find students in the given courses.
 * @param courseCodes The list of course codes to filter by.
 * @return A QStandardItemModel containing the filtered student data.
 */
QStandardItemModel *teacher::getStudentsInCourses(const QStringList &courseCodes)
{
    // Student data headers for the output table
    const QStringList outputHeaders = {
        "Course Title", "Code", "Student Name", "Attended Hours",
        "Total Hours Taught", "Attendance (%)", "Grade", "GPA"
    };

    // The model's parent is 'this' (the teacher window) initially
    QStandardItemModel *model = new QStandardItemModel(0, outputHeaders.size(), this);
    model->setHorizontalHeaderLabels(outputHeaders);

    QFile file("registered_students.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open registered_students.csv.");
        return model; // Return empty model on failure
    }

    QTextStream in(&file);
    in.readLine(); // Skip header line

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(','); // Assuming comma-separated values

        // Check if the course code (index 1) is in the assignedCourseCodes list
        if (fields.size() >= outputHeaders.size() && courseCodes.contains(fields.at(1).trimmed())) {

            QList<QStandardItem *> rowItems;
            // Add all fields as QStandardItems for the new row
            for (const QString &field : fields) {
                rowItems.append(new QStandardItem(field.trimmed()));
            }

            model->appendRow(rowItems);
        }
    }

    file.close();
    return model;
}

struct AttendanceRecord {
    QString courseCode;
    QString studentName;
    bool isPresent;
};

void teacher::on_attendence_clicked()
{
    QString teacherName = m_teacherData.value(0);
    QStringList assignedCourses = getAssignedCourseCodes(teacherName);

    if (assignedCourses.isEmpty()) {
        QMessageBox::information(this, "Attendance", "No courses assigned to you.");
        return;
    }

    // --- 1. Setup the Dialog ---
    QDialog dialog(this);
    dialog.setWindowTitle("Mark Attendance");
    dialog.resize(600, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // List to store references to widgets so we can read them later
    struct RowWidgets {
        QString courseCode;
        QString studentName;
        QRadioButton *rbPresent;
    };
    QList<RowWidgets> uiRows;

    // --- 2. Read Students and Populate Dialog ---
    // We reuse logic similar to getStudentsInCourses but customized for the dialog
    QFile file("registered_students.csv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // Skip header

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            // fields[1] = Code, fields[2] = Student Name
            if (fields.size() >= 3 && assignedCourses.contains(fields[1].trimmed())) {
                QString code = fields[1].trimmed();
                QString name = fields[2].trimmed();
                QString enrollment = getStudentEnrollment(name); // Get from students.csv

                // Create UI Row
                QFrame *lineFrame = new QFrame();
                lineFrame->setFrameShape(QFrame::StyledPanel);
                QHBoxLayout *rowLayout = new QHBoxLayout(lineFrame);

                QLabel *lblInfo = new QLabel(QString("<b>%1</b><br>Code: %2 | Enroll: %3")
                                                 .arg(name, code, enrollment));

                QRadioButton *rbPresent = new QRadioButton("Present");
                QRadioButton *rbAbsent = new QRadioButton("Absent");
                rbPresent->setChecked(true); // Default selected

                // Group buttons so they are exclusive per row
                QButtonGroup *group = new QButtonGroup(lineFrame);
                group->addButton(rbPresent);
                group->addButton(rbAbsent);

                rowLayout->addWidget(lblInfo);
                rowLayout->addWidget(rbPresent);
                rowLayout->addWidget(rbAbsent);

                scrollLayout->addWidget(lineFrame);

                // Store reference for saving
                uiRows.append({code, name, rbPresent});
            }
        }
        file.close();
    }

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // --- 3. Add Save Button ---
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // --- 4. Execute Dialog and Save if Accepted ---
    if (dialog.exec() == QDialog::Accepted) {
        QList<AttendanceRecord> recordsToSave;

        // Collect data from UI
        for (const auto &row : uiRows) {
            AttendanceRecord record;
            record.courseCode = row.courseCode;
            record.studentName = row.studentName;
            record.isPresent = row.rbPresent->isChecked();
            recordsToSave.append(record);
        }

        saveAttendanceToCSV(recordsToSave);
    }
}

/**
 * @brief Helper: Gets Enrollment from students.csv based on Name.
 */
QString teacher::getStudentEnrollment(const QString &studentName)
{
    QFile file("students.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "N/A";

    QTextStream in(&file);
    // Header: Name,Enrollment,...
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() >= 2 && fields[0].trimmed() == studentName.trimmed()) {
            return fields[1].trimmed();
        }
    }
    return "Not Found";
}

/**
 * @brief Helper: Gets Credit Hours from courses.csv based on Code.
 */
int teacher::getCourseCreditHours(const QString &courseCode)
{
    QFile file("courses.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream in(&file);
    // Header: Title,Code,Credit Hours,...
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        // Code is at index 1, Credit Hours at index 2
        if (fields.size() >= 3 && fields[1].trimmed() == courseCode.trimmed()) {
            return fields[2].toInt();
        }
    }
    return 0;
}

/**
 * @brief Helper: Updates registered_students.csv with new hours.
 */
void teacher::saveAttendanceToCSV(const QList<AttendanceRecord> &records)
{
    QStringList allLines;
    QFile fileRead("registered_students.csv");

    // 1. Read all data into memory
    if (fileRead.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileRead);
        while (!in.atEnd()) {
            allLines.append(in.readLine());
        }
        fileRead.close();
    } else {
        QMessageBox::critical(this, "Error", "Could not read registered students file.");
        return;
    }

    // 2. Process updates
    // Headers: Title[0], Code[1], Name[2], Attended[3], Total[4], %[5], Grade[6], GPA[7]
    bool dataUpdated = false;

    for (int i = 1; i < allLines.size(); ++i) { // Start at 1 to skip header
        QStringList fields = allLines[i].split(',');

        if (fields.size() < 6) continue;

        QString code = fields[1].trimmed();
        QString name = fields[2].trimmed();

        // Check if this line corresponds to a record we processed in the dialog
        for (const auto &rec : records) {
            if (rec.courseCode == code && rec.studentName == name) {

                int creditHours = getCourseCreditHours(code);
                double currentAttended = fields[3].toDouble();
                double currentTotal = fields[4].toDouble();

                // Logic: Always add to Total. Add to Attended only if Present.
                currentTotal += creditHours;
                if (rec.isPresent) {
                    currentAttended += creditHours;
                }

                // Update fields
                fields[3] = QString::number(currentAttended);
                fields[4] = QString::number(currentTotal);

                // Recalculate Percentage
                double percentage = 0.0;
                if (currentTotal > 0) {
                    percentage = (currentAttended / currentTotal) * 100.0;
                }
                fields[5] = QString::number(percentage, 'f', 2);

                allLines[i] = fields.join(',');
                dataUpdated = true;
                break; // Stop looking through records for this line
            }
        }
    }

    // 3. Write back to file
    if (dataUpdated) {
        QFile fileWrite("registered_students.csv");
        if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&fileWrite);
            for (const QString &line : allLines) {
                out << line << "\n";
            }
            fileWrite.close();
            QMessageBox::information(this, "Success", "Attendance saved successfully.");

            // Refresh table if it's currently showing students
            on_viewstudents_clicked();
        } else {
            QMessageBox::critical(this, "Error", "Could not write to file.");
        }
    }
}

void teacher::on_grades_clicked()
{
    QString teacherName = m_teacherData.value(0);
    QStringList assignedCourses = getAssignedCourseCodes(teacherName);

    if (assignedCourses.isEmpty()) {
        QMessageBox::information(this, "Grades", "No courses assigned to you.");
        return;
    }

    // --- 1. Setup the Dialog ---
    QDialog dialog(this);
    dialog.setWindowTitle("Assign Grades");
    dialog.resize(650, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // List to store widget references
    QList<GradeRowUI> uiRows;

    // --- 2. Read Students and Populate Dialog ---
    QFile file("registered_students.csv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // Skip header

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            // Check if course is assigned to this teacher
            if (fields.size() >= 3 && assignedCourses.contains(fields[1].trimmed())) {
                QString code = fields[1].trimmed();
                QString name = fields[2].trimmed();
                QString enrollment = getStudentEnrollment(name); // Helper from previous step

                // Get existing marks if available (Marks is now expected at index 8)
                QString currentMarks = "";
                if (fields.size() > 8) {
                    currentMarks = fields[8].trimmed();
                }

                // Create UI Row
                QFrame *lineFrame = new QFrame();
                lineFrame->setFrameShape(QFrame::StyledPanel);
                QHBoxLayout *rowLayout = new QHBoxLayout(lineFrame);

                QLabel *lblInfo = new QLabel(QString("<b>%1</b><br>Code: %2 | Enroll: %3")
                                                 .arg(name, code, enrollment));

                QLabel *lblMarks = new QLabel("Marks (0-100):");
                QLineEdit *txtMarks = new QLineEdit();
                txtMarks->setPlaceholderText("Enter Marks");
                txtMarks->setText(currentMarks); // Pre-fill if exists

                // restrict input to 0-100
                QDoubleValidator *validator = new QDoubleValidator(0.0, 100.0, 2, txtMarks);
                validator->setNotation(QDoubleValidator::StandardNotation);
                txtMarks->setValidator(validator);

                rowLayout->addWidget(lblInfo);
                rowLayout->addWidget(lblMarks);
                rowLayout->addWidget(txtMarks);

                scrollLayout->addWidget(lineFrame);

                // Store reference
                GradeRowUI row;
                row.courseCode = code;
                row.studentName = name;
                row.marksInput = txtMarks;
                uiRows.append(row);
            }
        }
        file.close();
    }

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // --- 3. Add Save Button ---
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // --- 4. Execute Dialog ---
    if (dialog.exec() == QDialog::Accepted) {
        saveGradesToCSV(uiRows);
    }
}

/**
 * @brief Helper: Calculates Grade and GPA based on the provided range.
 */
// Change "GradeResult" to "teacher::GradeResult"
teacher::GradeResult teacher::calculateGradeAndGPA(double marks)
{
    GradeResult res; // Inside the function, you don't need the prefix

    if (marks >= 85.00)      { res.grade = "A";  res.gpa = "4.00"; }
    else if (marks >= 80.00) { res.grade = "A-"; res.gpa = "3.67"; }
    else if (marks >= 75.00) { res.grade = "B+"; res.gpa = "3.33"; }
    else if (marks >= 71.00) { res.grade = "B";  res.gpa = "3.00"; }
    else if (marks >= 68.00) { res.grade = "B-"; res.gpa = "2.67"; }
    else if (marks >= 64.00) { res.grade = "C+"; res.gpa = "2.33"; }
    else if (marks >= 60.00) { res.grade = "C";  res.gpa = "2.00"; }
    else if (marks >= 57.00) { res.grade = "C-"; res.gpa = "1.67"; }
    else if (marks >= 53.00) { res.grade = "D+"; res.gpa = "1.33"; }
    else if (marks >= 50.00) { res.grade = "D";  res.gpa = "1.00"; }
    else                     { res.grade = "F";  res.gpa = "0.00"; }

    return res;
}

/**
 * @brief Helper: Saves Grades, GPA, and the new Marks column to CSV.
 */
void teacher::saveGradesToCSV(const QList<GradeRowUI> &rows)
{
    QStringList allLines;
    QFile fileRead("registered_students.csv");

    // 1. Read all data
    if (fileRead.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileRead);

        // Handle Header separately to add "Marks" column if missing
        if (!in.atEnd()) {
            QString header = in.readLine();
            QStringList headerFields = header.split(',');
            // If header doesn't have "Marks", append it
            if (headerFields.last().trimmed().compare("Marks", Qt::CaseInsensitive) != 0) {
                header += ",Marks";
            }
            allLines.append(header);
        }

        while (!in.atEnd()) {
            allLines.append(in.readLine());
        }
        fileRead.close();
    } else {
        QMessageBox::critical(this, "Error", "Could not read registered students file.");
        return;
    }

    // 2. Process updates
    bool dataUpdated = false;

    for (int i = 1; i < allLines.size(); ++i) { // Skip header
        QStringList fields = allLines[i].split(',');
        if (fields.size() < 3) continue;

        QString code = fields[1].trimmed();
        QString name = fields[2].trimmed();

        // Check if this line matches any row in our dialog
        for (const auto &row : rows) {
            if (row.courseCode == code && row.studentName == name) {

                QString marksStr = row.marksInput->text();
                double marks = marksStr.toDouble();

                // Calculate Grade/GPA
                GradeResult res = calculateGradeAndGPA(marks);

                // Update CSV indices:
                // 6: Grade, 7: GPA
                // 8: Marks (New Column)

                // Ensure list is big enough for Grade(6) and GPA(7)
                while(fields.size() <= 7) { fields.append(""); }

                fields[6] = res.grade;
                fields[7] = res.gpa;

                // Handle the Marks column
                if (fields.size() > 8) {
                    fields[8] = marksStr; // Update existing
                } else {
                    fields.append(marksStr); // Append new
                }

                allLines[i] = fields.join(',');
                dataUpdated = true;
                break;
            }
        }
    }

    // 3. Write back to file
    if (dataUpdated) {
        QFile fileWrite("registered_students.csv");
        if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&fileWrite);
            for (const QString &line : allLines) {
                out << line << "\n";
            }
            fileWrite.close();
            QMessageBox::information(this, "Success", "Grades and Marks saved successfully.");

            // Refresh main table
            on_viewstudents_clicked();
        } else {
            QMessageBox::critical(this, "Error", "Could not write to file.");
        }
    }
}

/**
 * @brief Slot to handle the 'Assign Assignment' button click.
 * Allows the teacher to select a course, name an assignment, and upload a file.
 */
void teacher::on_assignment_clicked()
{
    QString teacherName = m_teacherData.value(0);
    QStringList assignedCourseCodes = getAssignedCourseCodes(teacherName);

    if (assignedCourseCodes.isEmpty()) {
        QMessageBox::information(this, "Assignment", "No courses assigned to you.");
        return;
    }

    // --- 1. Setup the Dialog ---
    QDialog dialog(this);
    dialog.setWindowTitle("Assign New Assignment");
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // Course Selection (assuming you have a QComboBox for course selection if multiple)
    // For simplicity, if there is only one course, use it. If multiple, we need a selector.
    // Let's assume for now, we only handle the *first* course if multiple, or require a choice.
    // For a cleaner UI, we'll prompt the teacher to select one course.

    QComboBox *courseSelector = new QComboBox(&dialog);
    for (const QString &code : assignedCourseCodes) {
        QString title = getCourseTitle(code);
        courseSelector->addItem(code + " - " + title, code); // Store code as data
    }

    // Assignment Name Input
    QLineEdit *nameInput = new QLineEdit(&dialog);
    nameInput->setPlaceholderText("Enter Assignment Name (e.g., Q1 Test)");

    // File Selection Widgets
    QLineEdit *fileDisplay = new QLineEdit(&dialog);
    fileDisplay->setReadOnly(true);
    fileDisplay->setPlaceholderText("No file selected");

    QPushButton *browseButton = new QPushButton("Browse File", &dialog);
    QString *selectedFilePath = new QString(); // Store path temporarily

    // Connect browse button
    connect(browseButton, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        tr("Select Assignment File"),
                                                        QDir::homePath(),
                                                        tr("All Files (*)"));
        if (!filePath.isEmpty()) {
            *selectedFilePath = filePath;
            QFileInfo fileInfo(filePath);
            fileDisplay->setText(fileInfo.fileName());
        }
    });

    // Layout
    mainLayout->addWidget(new QLabel("Select Course:"));
    mainLayout->addWidget(courseSelector);
    mainLayout->addWidget(new QLabel("Assignment Name:"));
    mainLayout->addWidget(nameInput);
    mainLayout->addWidget(new QLabel("File Path:"));
    mainLayout->addWidget(fileDisplay);
    mainLayout->addWidget(browseButton);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // --- 2. Execute Dialog and Process Save ---
    if (dialog.exec() == QDialog::Accepted) {
        QString assignmentName = nameInput->text().trimmed();
        QString sourceFilePath = *selectedFilePath;
        QString courseCode = courseSelector->currentData().toString();
        QString courseTitle = getCourseTitle(courseCode);

        if (assignmentName.isEmpty() || sourceFilePath.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please provide a name and select a file.");
            return;
        }

        // Get the filename without the path
        QFileInfo fileInfo(sourceFilePath);
        QString fileName = fileInfo.fileName();

        // Define the destination path (e.g., in a new 'assignments' sub-directory)
        QString projectPath = QDir::currentPath();
        QString assignmentDir = projectPath + "/assignments";

        // Create the assignments directory if it doesn't exist
        QDir dir;
        if (!dir.exists(assignmentDir)) {
            dir.mkpath(assignmentDir);
        }

        // Construct the final destination path
        QString destinationFilePath = assignmentDir + "/" + fileName;

        // --- 3. Copy the File ---
        if (QFile::copy(sourceFilePath, destinationFilePath)) {
            // --- 4. Update the CSV Record ---
            updateAssignmentsCSV(courseCode, courseTitle, assignmentName, fileName);
            QMessageBox::information(this, "Success",
                                     QString("Assignment '%1' saved and file copied to project.")
                                         .arg(assignmentName));
        } else {
            QMessageBox::critical(this, "Error",
                                  QString("Failed to copy file from '%1' to '%2'.")
                                      .arg(sourceFilePath, destinationFilePath));
        }
    }

    delete selectedFilePath; // Clean up the heap allocated QString
}

/**
 * @brief Helper: Gets Course Title from courses.csv based on Code.
 */
QString teacher::getCourseTitle(const QString &courseCode)
{
    QFile file("courses.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "Title Not Found";

    QTextStream in(&file);
    in.readLine(); // Skip header: Title,Code,Credit Hours,...

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        // Title is at index 0, Code is at index 1
        if (fields.size() >= 2 && fields[1].trimmed() == courseCode.trimmed()) {
            return fields[0].trimmed();
        }
    }
    return "Title Not Found";
}

/**
 * @brief Helper: Updates or creates a row in assignment.csv.
 * Assignments are added in a single row for each course.
 * Format: Course Code, Course Title, Assignment Name 1|File Name 1, Assignment Name 2|File Name 2, ...
 */
void teacher::updateAssignmentsCSV(const QString &courseCode, const QString &courseTitle, const QString &assignmentName, const QString &fileName)
{
    QString assignmentCSV = "assignments.csv";
    QFile file(assignmentCSV);
    QStringList allLines;
    bool courseFound = false;

    // 1. Read all lines
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            allLines.append(in.readLine());
        }
        file.close();
    }

    // If the file is new or empty, add the header
    if (allLines.isEmpty()) {
        allLines.append("Course Code,Course Title,Assignments");
    }

    // New assignment entry to append
    QString newAssignmentEntry = assignmentName + "|" + fileName;

    // 2. Iterate and update the matching course row
    for (int i = 1; i < allLines.size(); ++i) { // Start at 1 to skip header
        QStringList fields = allLines[i].split(',');

        // Check for matching course code (index 0)
        if (!fields.isEmpty() && fields[0].trimmed() == courseCode.trimmed()) {

            // Append the new assignment to the end of the line
            allLines[i] += "," + newAssignmentEntry;
            courseFound = true;
            break;
        }
    }

    // 3. If course was not found, create a new row
    if (!courseFound) {
        QString newRow = courseCode + "," + courseTitle + "," + newAssignmentEntry;
        allLines.append(newRow);
    }

    // 4. Write all lines back to the file
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        for (const QString &line : allLines) {
            out << line << "\n";
        }
        file.close();
    } else {
        QMessageBox::critical(this, "File Write Error", "Could not write to assignments.csv.");
    }
}

// **REPLACE** the existing on_viewassignment_clicked() in teacher.cpp

void teacher::on_viewassignments_clicked()
{
    QString teacherName = m_teacherData.value(0);
    QStringList assignedCourseCodes = getAssignedCourseCodes(teacherName);

    if (assignedCourseCodes.isEmpty()) {
        QMessageBox::information(this, "View Assignments", "No courses assigned to you.");
        return;
    }

    // --- Stage 1: Course Selection Dialog ---
    QDialog courseDialog(this);
    courseDialog.setWindowTitle("Select Course to View Assignments");
    QVBoxLayout *layout = new QVBoxLayout(&courseDialog);

    QComboBox *courseSelector = new QComboBox(&courseDialog);
    for (const QString &code : assignedCourseCodes) {
        QString title = getCourseTitle(code);
        courseSelector->addItem(code + " - " + title, code);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addWidget(new QLabel("Select Course:"));
    layout->addWidget(courseSelector);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &courseDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &courseDialog, &QDialog::reject);

    if (courseDialog.exec() == QDialog::Accepted) {
        QString selectedCourseCode = courseSelector->currentData().toString();

        QStringList assignmentList = getAssignmentsForCourse(selectedCourseCode);

        if (assignmentList.isEmpty()) {
            QMessageBox::information(this, "No Assignments",
                                     QString("No assignments found for course code: %1.").arg(selectedCourseCode));
            return;
        }

        // --- Stage 2: Display Assignments in a dedicated Dialog ---
        // Use a pointer for explicit control over memory
        QDialog *assignmentViewer = new QDialog(this);
        assignmentViewer->setWindowTitle(QString("Assignments for %1").arg(selectedCourseCode));
        assignmentViewer->resize(700, 400);
        assignmentViewer->setAttribute(Qt::WA_DeleteOnClose); // Ensure it's deleted when closed

        QVBoxLayout *viewerLayout = new QVBoxLayout(assignmentViewer);
        QTableView *tableView = new QTableView(assignmentViewer);

        // --- CRITICAL STEP: Define Model with clear parentage ---
        QStandardItemModel *model = new QStandardItemModel(assignmentList.size(), 3, tableView);
        model->setHorizontalHeaderLabels({"Assignment Name", "File Name", "Download"});
        tableView->setModel(model); // Set the model immediately

        // --- Table View Configuration ---
        tableView->verticalHeader()->hide();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // --- Populate Model and Buttons ---
        for (int i = 0; i < assignmentList.size(); ++i) {
            QStringList parts = assignmentList.at(i).split('|');
            if (parts.size() < 2) continue;

            QString assignmentName = parts[0];
            QString fileName = parts[1];

            // 1. Set text items
            model->setItem(i, 0, new QStandardItem(assignmentName));
            model->setItem(i, 1, new QStandardItem(fileName));

            // 2. Create and connect the button
            QPushButton *downloadButton = new QPushButton("Download");

            // Set the filename property for retrieval in the slot
            downloadButton->setProperty("fileName", fileName);

            // Connect button click to a generic lambda that reads the property and calls the helper
            // We capture the button pointer to retrieve the property, and 'this' for the helper function.
            connect(downloadButton, &QPushButton::clicked, [this, downloadButton]() {
                QString fileToDownload = downloadButton->property("fileName").toString();
                if (!fileToDownload.isEmpty()) {
                    this->downloadAssignmentFile(fileToDownload);
                } else {
                    QMessageBox::critical(this, "Error", "Could not retrieve filename property.");
                }
            });

            // 3. Set the widget in the cell
            // This must be done *after* setting the model and configuring the view.
            tableView->setIndexWidget(model->index(i, 2), downloadButton);
        }

        // Final layout and show
        viewerLayout->addWidget(tableView);

        QPushButton *closeButton = new QPushButton("Close");
        viewerLayout->addWidget(closeButton);
        connect(closeButton, &QPushButton::clicked, assignmentViewer, &QDialog::accept);

        assignmentViewer->exec();
        // Since we set Qt::WA_DeleteOnClose, 'delete assignmentViewer;' is not strictly needed after exec()
        // but it's safe to keep 'assignmentViewer->exec(); delete assignmentViewer;' if you prefer.
        // For the sake of simplicity, we rely on Qt::WA_DeleteOnClose.
    }
}

// **ADD** the new download helper function to teacher.cpp

/**
 * @brief Handles the file copy from the project assignments folder to the system's Downloads folder.
 */
void teacher::downloadAssignmentFile(const QString &fileName)
{
    // 1. Determine Source Path (where the file is in the project)
    QString projectPath = QDir::currentPath();
    QString sourceFilePath = projectPath + "/assignments/" + fileName;

    if (!QFile::exists(sourceFilePath)) {
        QMessageBox::critical(this, "Download Error",
                              QString("Source file not found: %1").arg(sourceFilePath));
        return;
    }

    // 2. Determine Destination Path (Downloads folder)
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadsPath.isEmpty()) {
        QMessageBox::critical(this, "Download Error",
                              "Could not determine the system's Downloads folder path.");
        return;
    }

    QString destinationFilePath = downloadsPath + QDir::separator() + fileName;

    // 3. Perform Copy Operation
    if (QFile::exists(destinationFilePath)) {
        QFile::remove(destinationFilePath); // Remove existing file before copying
    }

    if (QFile::copy(sourceFilePath, destinationFilePath)) {
        QMessageBox::information(this, "Download Success",
                                 QString("Assignment saved to: %1").arg(QDir::toNativeSeparators(destinationFilePath)));
    } else {
        QMessageBox::critical(this, "Download Error",
                              "Failed to copy file to Downloads folder. Check permissions.");
    }
}
/**
 * @brief Reads assignments.csv and returns assignment entries for a given course.
 * @param courseCode The course code to search for.
 * @return QStringList where each item is "AssignmentName|FileName"
 */
QStringList teacher::getAssignmentsForCourse(const QString &courseCode)
{
    QStringList assignments;
    QFile file("assignments.csv");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Could not open assignments.csv.");
        return assignments;
    }

    QTextStream in(&file);

    // Skip header
    if (!in.atEnd()) {
        in.readLine();
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // fields[0] = Course Code
        if (fields.size() >= 3 && fields[0].trimmed() == courseCode.trimmed()) {

            // Assignments start from index 2 onward
            for (int i = 2; i < fields.size(); ++i) {
                QString assignmentEntry = fields[i].trimmed();
                if (!assignmentEntry.isEmpty()) {
                    assignments.append(assignmentEntry);
                }
            }
            break; // Course found, no need to continue
        }
    }

    file.close();
    return assignments;
}

