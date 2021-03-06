#include "example.h"
#include "ui_example.h"

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the dialog and show it
    Example example;
    example.show();

    // Run the app
    return app.exec();
}

Example::Example (QWidget *parent) : QDialog(parent), ui(new Ui::Example) {
    // Create and configure the user interface
    ui->setupUi(this);
    ui->versionLineEdit->setText("0.1");
    ui->versionLineEdit->setPlaceholderText("0.1");
    ui->changelogTextEdit->setPlainText("Click the \"Check for updates\" button to download the change log");

    // Close the dialog when the close button is clicked
    connect (ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // Check for updates when the updates button is clicked
    connect (ui->updatesButton, SIGNAL(clicked()), this, SLOT(checkForUpdates()));

    // Initialize the updater
    updater = new QSimpleUpdater(this);

    // When the updater finishes checking for updates, show a message box
    // and show the change log of the latest version
    connect (updater, SIGNAL(checkingFinished()), this, SLOT(onCheckingFinished()));
}

Example::~Example() {
    delete ui;
}

void Example::checkForUpdates() {
    // Disable the check for updates button while the updater
    // is checking for updates
    ui->updatesButton->setEnabled(false);
    ui->updatesButton->setText("Checking for updates...");

    // If the user changed the text of the versionLineEdit, then change the
    // application version in the updater too
    if (!ui->versionLineEdit->text().isEmpty()) {
        updater->setApplicationVersion(ui->versionLineEdit->text());
    }

    // If the versionLineEdit is empty, then set the application version
    // to "0.1"
    else {
        updater->setApplicationVersion("0.1");
    }

    // Tell the updater where we should download the changelog, note that
    // the changelog can be any file you want,
    // such as an HTML page or (as in this example), a text file
    updater->setChangelogUrl("https://raw.githubusercontent.com/alex-97/"
                             "QSimpleUpdater/Files-for-example-project/changelog.txt");

    // Tell the updater where we can find the file that tells us the latest version
    // of the application
    updater->setReferenceUrl("https://raw.githubusercontent.com/alex-97/"
                             "QSimpleUpdater/Files-for-example-project/current_version.txt");

    // Tell the updater where to download the update, its recommended to use direct links
    updater->setDownloadUrl("https://codeload.github.com/alex-97/QSimpleUpdater/zip/master");

    // Finally, check for updates...
    updater->checkForUpdates();
}

void Example::onCheckingFinished() {
    // Enable the updatesButton and change its text to let the user know
    // that he/she can check for updates again
    ui->updatesButton->setEnabled(true);
    ui->updatesButton->setText("Check for updates");

    // There's a newer version of the application available, so we inform
    // the user that there's a newer version available and we replace the text
    // of the changelog text edit with the downloaded change log
    if (updater->newerVersionAvailable()) {
        ui->changelogTextEdit->setPlainText(updater->changeLog());

        // Create and configure a message box
        QMessageBox _messagebox;
        _messagebox.setIcon(QMessageBox::Information);
        _messagebox.setWindowTitle(tr("Update available"));
        _messagebox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        _messagebox.setText(tr("There's an update available!"));
        _messagebox.setInformativeText(tr("The latest version of the application is") + " " +
                                       updater->latestVersion() + ", " +
                                       tr("do you want to download it?"));

        // If the user clicks "yes" open the download dialog
        if (_messagebox.exec() == QMessageBox::Yes)
            updater->downloadLatestVersion();
    }

    // The installed version is equal or greater to the "official" latest version,
    // so we inform the user and clear the text of the change log text edit
    else {
        ui->changelogTextEdit->setPlainText("");
        ui->changelogTextEdit->setPlainText("The change log was not downloaded because you "
                                            "are running the latest version of the application...");

        QMessageBox::information(this, tr("No updates available"),
                                 tr("Congratulations! You are running the latest version of the application!"));
    }
}
