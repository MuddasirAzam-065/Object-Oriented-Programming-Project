#include "student.h"
#include "qcombobox.h"
#include "ui_student.h"
#include <QStandardItem>
#include "lms.h"
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSet>
// New constructor implementation
// The base class constructor is now QMainWindow, which takes a QWidget* parent
student::student(const QStringList &studentData, QWidget *parent)
    : QMainWindow(parent) // <<< CHANGED base class call
    , ui(new Ui::student)
    , m_studentData(studentData) // Initialize the member variable
{
    // Line 11 is now correct, as 'this' is a student*, which inherits QMainWindow*
    ui->setupUi(this);
    ui->label->setText("Welcome : " +m_studentData[0]);
    // Call the function to populate the QTableView
    displayStudentDetails();
}

student::~student()
{
    delete ui;
}

void student::displayStudentDetails()
{
    // The CSV headers in order (to be used as row headers in the table):
    const QStringList headers = {
        "Name", "Enrollment", "Father Name", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree",
        "Enroll Year", "Password","Batch","Current Semester"
    };

    // Check if we actually received data
    if (m_studentData.isEmpty() || m_studentData.size() != headers.size()) {
        return;
    }

    // 1. Create a model for the QTableView.
    QStandardItemModel *model = new QStandardItemModel(m_studentData.size(), 2, this);

    // Set the headers for the columns (Field Name | Value)
    model->setHorizontalHeaderLabels({"Field", "Value"});

    // 2. Populate the model with the student data
    for (int i = 0; i < m_studentData.size(); ++i) {
        // Set the Field Name in the first column
        QStandardItem *fieldItem = new QStandardItem(headers.at(i));
        model->setItem(i, 0, fieldItem);

        // Set the Value in the second column
        QStandardItem *valueItem = new QStandardItem(m_studentData.at(i));
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
void student::on_logout_clicked()
{
    lms *lmsWindow = new lms();//pointer to object of lms class
    lmsWindow->showMaximized();//open the lms.ui in it's maximum size
    this->close();//close admin when logout is clicked
}

void student::on_offer_clicked()
{
    // --- 1. Get Current Semester and Student Degree ---
    const QStringList studentHeaders = {
        "Name", "Enrollment", "Father Name", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree", // <-- Student Degree is here
        "Enroll Year", "Password", "Batch", "Current Semester"
    };

    int currentSemesterIndex = studentHeaders.indexOf("Current Semester");
    int studentDegreeIndex = studentHeaders.indexOf("Degree"); // <-- New: Get Degree Index

    if (currentSemesterIndex == -1 || studentDegreeIndex == -1 ||
        m_studentData.size() <= currentSemesterIndex || m_studentData.size() <= studentDegreeIndex)
    {
        QMessageBox::critical(this, "Error", "Cannot find 'Current Semester' or 'Degree' data.");
        return;
    }

    // Get Current Semester
    bool ok;
    int currentSemester = m_studentData.at(currentSemesterIndex).toInt(&ok);

    if (!ok) {
        QMessageBox::critical(this, "Error", "Invalid 'Current Semester' value.");
        return;
    }

    // Get Student Degree
    const QString studentDegree = m_studentData.at(studentDegreeIndex).trimmed(); // <-- New: Get the student's degree
    if (studentDegree.isEmpty()) {
        QMessageBox::critical(this, "Error", "Student 'Degree' value is missing.");
        return;
    }

    // Target filtering values
    QString targetSemStr = QString::number(currentSemester);
    // Student Degree is now targetDegree for filtering
    const QString targetDegree = studentDegree;

    // --- 2. Load and Filter courses from courses.csv ---
    const QString filePath = "courses.csv";
    QList<QStringList> offeredCourses;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open courses file: " + filePath);
        return;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine().trimmed();
    const QStringList courseHeaders = headerLine.split(',');

    // New: Added Degree Index
    int titleIndex = courseHeaders.indexOf("Title");
    int codeIndex = courseHeaders.indexOf("Code");
    int creditHoursIndex = courseHeaders.indexOf("Credit Hours");
    int semesterIndex = courseHeaders.indexOf("Semester");
    int courseDegreeIndex = courseHeaders.indexOf("Degree"); // <-- New: Get the course's Degree index

    if (titleIndex == -1 || codeIndex == -1 || creditHoursIndex == -1 ||
        semesterIndex == -1 || courseDegreeIndex == -1) // <-- New: Check for Degree column
    {
        QMessageBox::critical(this, "CSV Format Error", "Missing required columns in courses.csv (Title, Code, Credit Hours, Semester, Degree).");
        return;
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // --- 3. FIX: Match CURRENT SEMESTER AND DEGREE ---
        if (fields.size() > semesterIndex && fields.size() > courseDegreeIndex) // Check bounds for both indices
        {
            QString courseSemester = fields.at(semesterIndex).trimmed();
            QString courseDegree = fields.at(courseDegreeIndex).trimmed(); // Get course's degree

            // Filter condition: Course Semester MUST match student's current semester
            // AND Course Degree MUST match student's degree.
            if (courseSemester == targetSemStr && courseDegree.compare(targetDegree, Qt::CaseInsensitive) == 0)
            {
                offeredCourses.append({
                    fields.at(titleIndex).trimmed(),
                    fields.at(codeIndex).trimmed(),
                    fields.at(creditHoursIndex).trimmed(),
                    courseSemester,
                    courseDegree // Including course degree in the list for completeness
                });
            }
        }
    }

    file.close();

    // --- 4. Display Results ---
    if (offeredCourses.isEmpty()) {
        QMessageBox::information(this, "No Courses Found",
                                 "No courses exist for Degree: " + targetDegree +
                                     " in Semester " + targetSemStr);
        ui->tableView->setModel(nullptr);
        return;
    }

    int rowCount = offeredCourses.size();
    int colCount = 5; // Updated to 5 to include the Degree column
    QStandardItemModel *model = new QStandardItemModel(rowCount, colCount, this);
    // Updated header labels
    model->setHorizontalHeaderLabels({"Title", "Code", "Credit Hours", "Semester", "Degree"});

    for (int i = 0; i < rowCount; ++i) {
        const QStringList &record = offeredCourses.at(i);
        for (int j = 0; j < colCount; ++j) {
            model->setItem(i, j, new QStandardItem(record.at(j)));
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void student::on_viewprofile_clicked()
{
    displayStudentDetails();
}

void student::on_reg_clicked()
{
    // --------------------------------------------------------
    // A. DETERMINE CURRENT SEMESTER, STUDENT NAME, AND DEGREE
    // --------------------------------------------------------

    const QStringList studentHeaders = {
        "Name", "Enrollment", "Father Name", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree",
        "Enroll Year", "Password", "Batch", "Current Semester"
    };

    int nameIndex = studentHeaders.indexOf("Name");
    int currentSemesterIndex = studentHeaders.indexOf("Current Semester");
    int studentDegreeIndex = studentHeaders.indexOf("Degree"); // <-- New: Get Degree Index

    if (nameIndex == -1 || currentSemesterIndex == -1 || studentDegreeIndex == -1 ||
        m_studentData.size() <= currentSemesterIndex || m_studentData.size() <= studentDegreeIndex)
    {
        QMessageBox::critical(this, "Error", "Cannot retrieve necessary student data (Name, Semester, or Degree).");
        return;
    }

    bool ok;
    int currentSemester = m_studentData.at(currentSemesterIndex).toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Error", "Invalid 'Current Semester' value.");
        return;
    }

    QString studentName = m_studentData.at(nameIndex).trimmed();
    QString studentDegree = m_studentData.at(studentDegreeIndex).trimmed(); // <-- New: Get Student Degree

    const int maxSemester = 8;
    if (currentSemester > maxSemester) {
        QMessageBox::information(this, "Enrollment Complete",
                                 "The student's semester is invalid.");
        return;
    }

    // --------------------------------------------------------
    // B. GET COURSES FOR CURRENT SEMESTER AND DEGREE
    // --------------------------------------------------------

    const QString coursesFilePath = "courses.csv";
    // Store both Title and Code for registration
    QList<QStringList> offeredCourses;

    QFile coursesFile(coursesFilePath);
    if (!coursesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open courses file: " + coursesFilePath);
        return;
    }

    QTextStream coursesIn(&coursesFile);
    QString headerLine = coursesIn.readLine().trimmed();
    QStringList courseHeaders = headerLine.split(',');

    int titleIndex = courseHeaders.indexOf("Title");
    int codeIndex = courseHeaders.indexOf("Code");       // <-- New: Get Code Index
    int semesterIndex = courseHeaders.indexOf("Semester");
    int courseDegreeIndex = courseHeaders.indexOf("Degree"); // <-- New: Get Course Degree Index

    if (titleIndex == -1 || codeIndex == -1 || semesterIndex == -1 || courseDegreeIndex == -1) {
        QMessageBox::critical(this, "CSV Format Error", "Missing required columns (Title, Code, Semester, or Degree) in courses.csv.");
        coursesFile.close();
        return;
    }

    QString curSemStr = QString::number(currentSemester);

    while (!coursesIn.atEnd()) {
        QString line = coursesIn.readLine();
        QStringList fields = line.split(',');

        if (fields.size() <= semesterIndex || fields.size() <= courseDegreeIndex) continue;

        QString courseSem = fields.at(semesterIndex).trimmed();
        QString courseDegree = fields.at(courseDegreeIndex).trimmed(); // Get course's degree

        // ✔ Filter: Match CURRENT SEMESTER AND STUDENT DEGREE
        if (courseSem == curSemStr && courseDegree.compare(studentDegree, Qt::CaseInsensitive) == 0) {
            offeredCourses.append({
                fields.at(titleIndex).trimmed(),
                fields.at(codeIndex).trimmed() // Save Course Code
            });
        }
    }
    coursesFile.close();

    if (offeredCourses.isEmpty()) {
        QMessageBox::information(this, "Registration Aborted",
                                 "No courses found for Degree '" + studentDegree +
                                     "' in Semester " + curSemStr + ".");
        return;
    }

    // --------------------------------------------------------
    // C. REGISTER STUDENT IN COURSES (Using NEW Structure)
    // --------------------------------------------------------

    const QString registeredFilePath = "registered_students.csv";
    QList<QStringList> registeredData;

    // Define the NEW required headers
    QStringList expectedHeaders = {
        "Course Title",
        "Code", // <-- New Header
        "Student Name",
        "Attended Hours", // Changed from Present Hours
        "Total Hours Taught",
        "Attendance(in percent)", // Changed from Attendance
        "Grade",
        "GPA"
    };

    QFile regFile(registeredFilePath);
    bool fileExists = regFile.exists();

    if (!regFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error",
                              "Could not open registration file for reading/writing: " + registeredFilePath);
        return;
    }

    // --- Handle Header/File Creation ---
    QTextStream regIn(&regFile);
    QString regHeader;

    // Check if file is new or empty
    if (!fileExists || regFile.size() == 0) {
        regHeader = expectedHeaders.join(',');
        regFile.write(regHeader.toUtf8() + "\n");
        regFile.flush();
        // Seek back to start of data area (after header)
        regFile.seek(regHeader.toUtf8().size() + 1);
    } else {
        // Read existing header
        regHeader = regIn.readLine().trimmed();
    }

    // Check if existing header matches expected format
    QStringList headerColumns = regHeader.split(',');
    if (headerColumns != expectedHeaders) {
        // Warning: This action will erase old data if the format is wrong!
        // In a real application, you might want a more sophisticated migration/warning.
        QMessageBox::warning(this, "File Format Warning", "Existing 'registered_students.csv' header format is incorrect. Overwriting file with new headers.");
        regFile.resize(0);
        QTextStream fixOut(&regFile);
        fixOut << expectedHeaders.join(',') << "\n";
        regFile.flush();
    }

    // Read existing data (if any, using the established format)
    while (!regIn.atEnd()) {
        QString line = regIn.readLine().trimmed();
        if (line.isEmpty()) continue;
        registeredData.append(line.split(','));
    }

    // Indices for checking existing data based on the *expected* structure
    int registeredTitleIndex = expectedHeaders.indexOf("Course Title");
    int registeredCodeIndex = expectedHeaders.indexOf("Code");
    int registeredNameIndex = expectedHeaders.indexOf("Student Name");

    int studentsRegisteredCount = 0;

    for (const QStringList &course : offeredCourses) {
        const QString courseTitle = course.at(0);
        const QString courseCode = course.at(1);

        bool alreadyRegistered = false;

        // Check if student is already registered for this specific Course Title AND Code
        for (const QStringList &row : registeredData) {
            // Need to check size against the NEW format (must have at least the first 3 columns)
            if (row.size() > registeredNameIndex &&
                row.at(registeredTitleIndex) == courseTitle &&
                row.at(registeredCodeIndex) == courseCode &&
                row.at(registeredNameIndex).trimmed().toLower() == studentName.toLower())
            {
                alreadyRegistered = true;
                break;
            }
        }

        if (alreadyRegistered) continue;

        // Create a new row with initial values matching the NEW expectedHeaders
        QStringList newRow = {
            courseTitle,
            courseCode,      // <-- Course Code
            studentName,
            "0",             // Attended Hours
            "0",             // Total Hours Taught
            "0%",            // Attendance(in percent)
            "-",             // Grade
            "0.0"            // GPA
        };

        registeredData.append(newRow);
        studentsRegisteredCount++;
    }

    // --- Write All Data Back to File ---
    regFile.resize(0);
    QTextStream regOut(&regFile);

    regOut << expectedHeaders.join(',') << "\n"; // Write Header

    for (const QStringList &row : registeredData) {
        regOut << row.join(',') << "\n";
    }

    regFile.close();

    // --------------------------------------------------------
    // D. FINAL MESSAGE
    // --------------------------------------------------------
    if (studentsRegisteredCount > 0) {
        QMessageBox::information(
            this, "Registration Success",
            QString("Student '%1' successfully registered in %2 courses (Degree: %3) for Semester %4.")
                .arg(studentName)
                .arg(studentsRegisteredCount)
                .arg(studentDegree)
                .arg(currentSemester));
    } else {
        QMessageBox::information(
            this, "Registration Status",
            "Student already registered for all available courses for the current semester and degree.");
    }
}

#include <QStandardItemModel>

void student::on_view_clicked()
{
    // -----------------------------------------------
    // A. Get Student Name and Define Expected Headers
    // -----------------------------------------------
    const QStringList studentHeaders = {
        "Name", "Enrollment", "Father Name", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree",
        "Enroll Year", "Password", "Batch", "Current Semester"
    };

    // Expected headers defined in on_reg_clicked()
    const QStringList expectedRegHeaders = {
        "Course Title",
        "Code",
        "Student Name", // <-- Student Name is at index 2 (0-based)
        "Attended Hours",
        "Total Hours Taught",
        "Attendance(in percent)",
        "Grade",
        "GPA"
    };

    int studentNameIndexInStudentData = studentHeaders.indexOf("Name");
    int studentNameIndexInRegFile = expectedRegHeaders.indexOf("Student Name"); // Index 2

    if (studentNameIndexInStudentData == -1 || m_studentData.size() <= studentNameIndexInStudentData) {
        QMessageBox::critical(this, "Error", "Cannot retrieve student name from student data.");
        return;
    }

    if (studentNameIndexInRegFile == -1) {
        QMessageBox::critical(this, "Error", "Internal error: 'Student Name' column not found in expected registration headers.");
        return;
    }

    QString studentName = m_studentData.at(studentNameIndexInStudentData).trimmed();

    // -----------------------------------------------
    // B. Open registered_students.csv
    // -----------------------------------------------
    const QString registeredFilePath = "registered_students.csv";
    QFile regFile(registeredFilePath);

    if (!regFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error",
                              "Could not open registration file: " + registeredFilePath);
        return;
    }

    QTextStream in(&regFile);
    QString headerLine = in.readLine().trimmed();  // Read header
    QStringList actualHeaders = headerLine.split(',');

    // Optional Check: Ensure the header format is what we expect
    if (actualHeaders.size() < expectedRegHeaders.size() ||
        !actualHeaders.contains("Code") || !actualHeaders.contains("Student Name"))
    {
        QMessageBox::critical(this, "CSV Format Error",
                              "The header format of registered_students.csv does not match the expected structure.");
        regFile.close();
        ui->tableView->setModel(nullptr);
        return;
    }


    // -----------------------------------------------
    // C. Create model for tableView
    // -----------------------------------------------
    QStandardItemModel *model = new QStandardItemModel(this);
    // Use the actual headers from the file, but filter which columns to display later if needed
    model->setHorizontalHeaderLabels(actualHeaders);

    // -----------------------------------------------
    // D. Read CSV & Add Rows for Current Student
    // -----------------------------------------------
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(',');

        // Ensure the row has enough columns to contain the Student Name field
        if (fields.size() <= studentNameIndexInRegFile) continue;

        // CRITICAL FIX: The index for Student Name is now studentNameIndexInRegFile (Index 2)
        QString fileStudentName = fields.at(studentNameIndexInRegFile).trimmed();

        if (fileStudentName.compare(studentName, Qt::CaseInsensitive) == 0) {
            QList<QStandardItem *> items;
            for (const QString &field : fields) {
                items.append(new QStandardItem(field));
            }
            model->appendRow(items);
        }
    }

    regFile.close();

    // -----------------------------------------------
    // E. Set model to tableView
    // -----------------------------------------------
    if (model->rowCount() == 0) {
        // Must delete model if no rows, or keep it empty. Keeping it empty is fine.
        QMessageBox::information(this, "Registered Courses",
                                 "No courses registered for student: " + studentName);
    }

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make it read-only for viewing
}

void student::on_withdraw_clicked()
{
    // -----------------------------------------------
    // A. Get Student Name and Define Indices
    // -----------------------------------------------
    const QStringList studentHeaders = {
        "Name", "Enrollment", "Father Name", "CNIC", "Contact",
        "Email", "Postal Address", "Permanent Address", "Degree",
        "Enroll Year", "Password", "Batch", "Current Semester"
    };

    // Based on the new registered_students.csv format:
    const int COURSE_CODE_INDEX = 1;
    const int STUDENT_NAME_INDEX = 2;

    int nameIndex = studentHeaders.indexOf("Name");
    if (nameIndex == -1 || m_studentData.size() <= nameIndex) {
        QMessageBox::critical(this, "Error", "Cannot retrieve student name.");
        return;
    }

    QString studentName = m_studentData.at(nameIndex).trimmed();

    // -----------------------------------------------
    // B. Create Withdraw Dialog (Ask for Course CODE)
    // -----------------------------------------------
    QDialog dialog(this);
    dialog.setWindowTitle("Withdraw Course");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // FIX: Ask for Code instead of Title
    QLabel *label = new QLabel("Enter Course CODE to Withdraw:", &dialog);
    QLineEdit *courseInput = new QLineEdit(&dialog);
    QPushButton *withdrawButton = new QPushButton("Withdraw", &dialog);

    layout->addWidget(label);
    layout->addWidget(courseInput);
    layout->addWidget(withdrawButton);

    dialog.setLayout(layout);

    // -----------------------------------------------
    // C. Connect Withdraw Button Logic
    // -----------------------------------------------
    connect(withdrawButton, &QPushButton::clicked, [&]() {

        QString courseCodeToWithdraw = courseInput->text().trimmed().toUpper(); // Use uppercase for code consistency
        if (courseCodeToWithdraw.isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Please enter a course code.");
            return;
        }

        // Read registered_students.csv
        const QString registeredFilePath = "registered_students.csv";
        QFile file(registeredFilePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(&dialog, "File Error", "Cannot open registration file.");
            return;
        }

        QTextStream in(&file);
        QString headerLine = in.readLine().trimmed();
        QStringList allLines;
        bool found = false;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            QStringList fields = line.split(',');
            // Ensure fields has enough elements for Code and Student Name
            if (fields.size() <= STUDENT_NAME_INDEX) {
                allLines.append(line); // Keep badly formatted lines, but skip check
                continue;
            }

            // FIX: Check against Course CODE (Index 1) and Student Name (Index 2)
            QString rowCourseCode = fields.at(COURSE_CODE_INDEX).trimmed();
            QString rowStudent = fields.at(STUDENT_NAME_INDEX).trimmed();

            // Check if this is the course/student to delete
            if (rowCourseCode.compare(courseCodeToWithdraw, Qt::CaseInsensitive) == 0 &&
                rowStudent.compare(studentName, Qt::CaseInsensitive) == 0) {

                found = true;
                continue; // skip this row → effectively deleting it
            }

            allLines.append(line); // keep all other rows
        }

        file.close();

        if (!found) {
            QMessageBox::information(&dialog, "Not Registered",
                                     "You are not registered in a course with code: " + courseCodeToWithdraw);
            return;
        }

        // Rewrite the CSV without the withdrawn course
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QMessageBox::critical(&dialog, "File Error", "Cannot write to registration file.");
            return;
        }

        QTextStream out(&file);
        out << headerLine << "\n";
        for (const QString &line : allLines) {
            out << line << "\n";
        }
        file.close();

        QMessageBox::information(&dialog, "Withdrawn",
                                 "Successfully withdrawn from course with code: " + courseCodeToWithdraw);

        dialog.accept(); // close dialog
    });

    dialog.exec();
}

QStringList student::getAssignmentsForCourse(const QString &courseCode)
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

void student::downloadAssignmentFile(const QString &fileName)
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
 * @brief Reads registered_students.csv to find all course codes for the student.
 * Uses the student's name (which should be fields[2]).
 */
QStringList student::getRegisteredCourseCodes(const QString &studentName)
{
    QStringList courseCodes;
    QFile file("registered_students.csv");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open registered_students.csv.");
        return courseCodes;
    }

    QTextStream in(&file);
    in.readLine(); // Skip header

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // Check if student name (index 2) matches the logged-in student
        // Course Code is at index 1
        if (fields.size() >= 3 && fields.at(2).trimmed() == studentName.trimmed()) {
            courseCodes.append(fields.at(1).trimmed());
        }
    }

    /// In student.cpp, inside student::getRegisteredCourseCodes

    // ... (code to read file and populate courseCodes QList) ...

    file.close();

    // --- FINAL GUARANTEED FIX START ---

    // 1. Create a QSet from the QList (removes duplicates)
    // This step must be done first.
    QSet<QString> uniqueCodes(courseCodes.begin(), courseCodes.end());

    // 2. Manually create the final QStringList by iterating over the QSet.
    QStringList resultList;

    // Iterate over every item in the QSet
    QSetIterator<QString> i(uniqueCodes);
    while (i.hasNext()) {
        resultList.append(i.next());
    }

    return resultList;
    // --- FINAL GUARANTEED FIX END ---
}

void student::on_viewassignments_clicked()
{
    // Assuming student name is stored at index 0 or similar in m_studentData
    // Adjust index if necessary based on your student login structure
    QString studentName = m_studentData.value(0);
    QStringList registeredCourseCodes = getRegisteredCourseCodes(studentName);

    if (registeredCourseCodes.isEmpty()) {
        QMessageBox::information(this, "View Assignments", "You are not registered in any courses.");
        return;
    }

    // --- Stage 1: Course Selection Dialog ---
    QDialog courseDialog(this);
    courseDialog.setWindowTitle("Select Course to View Assignments");
    QVBoxLayout *layout = new QVBoxLayout(&courseDialog);

    QComboBox *courseSelector = new QComboBox(&courseDialog);
    // You will need a helper function 'getCourseTitle' in student class too,
    // or copy it from teacher.cpp if not present.
    for (const QString &code : registeredCourseCodes) {
        QString title = getCourseTitle(code);
        courseSelector->addItem(code + " - " + title, code); // Store code as data
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
        QDialog *assignmentViewer = new QDialog(this);
        assignmentViewer->setWindowTitle(QString("Assignments for %1").arg(selectedCourseCode));
        assignmentViewer->resize(700, 400);
        assignmentViewer->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout *viewerLayout = new QVBoxLayout(assignmentViewer);
        QTableView *tableView = new QTableView(assignmentViewer);

        QStandardItemModel *model = new QStandardItemModel(assignmentList.size(), 3, tableView);
        model->setHorizontalHeaderLabels({"Assignment Name", "File Name", "Download"});
        tableView->setModel(model);

        tableView->verticalHeader()->hide();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // --- Populate Model and Buttons ---
        for (int i = 0; i < assignmentList.size(); ++i) {
            QStringList parts = assignmentList.at(i).split('|');
            if (parts.size() < 2) continue;

            QString assignmentName = parts[0];
            QString fileName = parts[1];

            model->setItem(i, 0, new QStandardItem(assignmentName));
            model->setItem(i, 1, new QStandardItem(fileName));

            QPushButton *downloadButton = new QPushButton("Download");

            downloadButton->setProperty("fileName", fileName);
            connect(downloadButton, &QPushButton::clicked, [this, downloadButton]() {
                QString fileToDownload = downloadButton->property("fileName").toString();
                if (!fileToDownload.isEmpty()) {
                    this->downloadAssignmentFile(fileToDownload);
                } else {
                    QMessageBox::critical(this, "Error", "Could not retrieve filename property.");
                }
            });

            tableView->setIndexWidget(model->index(i, 2), downloadButton);
        }

        viewerLayout->addWidget(tableView);

        QPushButton *closeButton = new QPushButton("Close");
        viewerLayout->addWidget(closeButton);
        connect(closeButton, &QPushButton::clicked, assignmentViewer, &QDialog::accept);

        assignmentViewer->exec();
    }
}

QString student::getCourseTitle(const QString &courseCode)
{
    QFile file("courses.csv");
    // Ensure you handle file open failure gracefully
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "Title Not Found";

    QTextStream in(&file);
    in.readLine(); // Skip header: Title,Code,Credit Hours,...

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        // Title is at index 0, Code is at index 1
        if (fields.size() >= 2 && fields[1].trimmed() == courseCode.trimmed()) {
            file.close();
            return fields[0].trimmed();
        }
    }
    file.close();
    return "Title Not Found";
}
