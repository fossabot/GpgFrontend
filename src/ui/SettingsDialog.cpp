/**
 * This file is part of GPGFrontend.
 *
 * GPGFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#include "ui/SettingsDialog.h"

SettingsDialog::SettingsDialog(GpgME::GpgContext *ctx, QWidget *parent)
        : QDialog(parent) {
    mCtx = ctx;
    tabWidget = new QTabWidget;
    generalTab = new GeneralTab(mCtx);
    appearanceTab = new AppearanceTab;
    mimeTab = new MimeTab;
    keyserverTab = new KeyserverTab;
    advancedTab = new AdvancedTab;
    gpgPathsTab = new GpgPathsTab;

    tabWidget->addTab(generalTab, tr("General"));
    tabWidget->addTab(appearanceTab, tr("Appearance"));
    tabWidget->addTab(mimeTab, tr("PGP/Mime"));
    tabWidget->addTab(keyserverTab, tr("Key Server"));
    // tabWidget->addTab(gpgPathsTab, tr("Gpg paths"));
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Settings"));

    // slots for handling the restartneeded member
    this->slotSetRestartNeeded(false);
    connect(generalTab, SIGNAL(signalRestartNeeded(bool)), this, SLOT(slotSetRestartNeeded(bool)));
    connect(appearanceTab, SIGNAL(signalRestartNeeded(bool)), this, SLOT(slotSetRestartNeeded(bool)));
    connect(mimeTab, SIGNAL(signalRestartNeeded(bool)), this, SLOT(slotSetRestartNeeded(bool)));
    connect(keyserverTab, SIGNAL(signalRestartNeeded(bool)), this, SLOT(slotSetRestartNeeded(bool)));
    connect(advancedTab, SIGNAL(signalRestartNeeded(bool)), this, SLOT(slotSetRestartNeeded(bool)));

    connect(this, SIGNAL(signalRestartNeeded(bool)), parent, SLOT(slotSetRestartNeeded(bool)));

    this->show();
}

bool SettingsDialog::getRestartNeeded() const {
    return this->restartNeeded;
}

void SettingsDialog::slotSetRestartNeeded(bool needed) {
    this->restartNeeded = needed;
}

void SettingsDialog::slotAccept() {
    generalTab->applySettings();
    mimeTab->applySettings();
    appearanceTab->applySettings();
    keyserverTab->applySettings();
    advancedTab->applySettings();
    gpgPathsTab->applySettings();
    if (getRestartNeeded()) {
        emit signalRestartNeeded(true);
    }
    close();
}

// http://www.informit.com/articles/article.aspx?p=1405555&seqNum=3
// http://developer.qt.nokia.com/wiki/How_to_create_a_multi_language_application
QHash<QString, QString> SettingsDialog::listLanguages() {
    QHash<QString, QString> languages;

    languages.insert("", tr("System Default"));

    QString appPath = qApp->applicationDirPath();
    QDir qmDir = QDir(appPath + "/ts/");
    QStringList fileNames =
            qmDir.entryList(QStringList("gpg4usb_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);

        // this works in qt 4.8
        QLocale qloc(locale);
#if QT_VERSION < 0x040800
        QString language =  QLocale::languageToString(qloc.language()) +" (" + locale + ")"; //+ " (" + QLocale::languageToString(qloc.language()) + ")";
#else
        QString language = qloc.nativeLanguageName() + " (" + locale +
                           ")"; //+ " (" + QLocale::languageToString(qloc.language()) + ")";
#endif
        languages.insert(locale, language);
    }
    return languages;
}


GeneralTab::GeneralTab(GpgME::GpgContext *ctx, QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {
    mCtx = ctx;

    /*****************************************
     * remember Password-Box
     *****************************************/
    auto *rememberPasswordBox = new QGroupBox(tr("Remember Password"));
    auto *rememberPasswordBoxLayout = new QHBoxLayout();
    rememberPasswordCheckBox = new QCheckBox(tr("Remember password until closing gpg4usb"), this);
    rememberPasswordBoxLayout->addWidget(rememberPasswordCheckBox);
    rememberPasswordBox->setLayout(rememberPasswordBoxLayout);

    /*****************************************
     * Save-Checked-Keys-Box
     *****************************************/
    auto *saveCheckedKeysBox = new QGroupBox(tr("Save Checked Keys"));
    auto *saveCheckedKeysBoxLayout = new QHBoxLayout();
    saveCheckedKeysCheckBox = new QCheckBox(tr("Save checked private keys on exit and restore them on next start."),
                                            this);
    saveCheckedKeysBoxLayout->addWidget(saveCheckedKeysCheckBox);
    saveCheckedKeysBox->setLayout(saveCheckedKeysBoxLayout);

    /*****************************************
     * Key-Impport-Confirmation Box
     *****************************************/
    auto *importConfirmationBox = new QGroupBox(tr("Confirm drag'n'drop key import"));
    auto *importConfirmationBoxLayout = new QHBoxLayout();
    importConfirmationCheckBox = new QCheckBox(tr("Import files dropped on the keylist without confirmation."), this);
    importConfirmationBoxLayout->addWidget(importConfirmationCheckBox);
    importConfirmationBox->setLayout(importConfirmationBoxLayout);

    /*****************************************
     * Language Select Box
     *****************************************/
    auto *langBox = new QGroupBox(tr("Language"));
    auto *langBoxLayout = new QVBoxLayout();
    langSelectBox = new QComboBox;
    lang = SettingsDialog::listLanguages();

            foreach(QString l, lang) {
            langSelectBox->addItem(l);
        }

    langBoxLayout->addWidget(langSelectBox);
    langBoxLayout->addWidget(
            new QLabel(tr("<b>NOTE: </b> Gpg4usb will restart automatically if you change the language!")));
    langBox->setLayout(langBoxLayout);
    connect(langSelectBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLanguageChanged()));

    /*****************************************
     * Own Key Select Box
     *****************************************/
    auto *ownKeyBox = new QGroupBox(tr("Own key"));
    auto *ownKeyBoxLayout = new QVBoxLayout();
    ownKeySelectBox = new QComboBox;

    ownKeyBox->setLayout(ownKeyBoxLayout);
    mKeyList = new KeyList(mCtx);

    // Fill the keyid hashmap
    keyIds.insert("", tr("<none>"));

    foreach (QString keyid, *mKeyList->getAllPrivateKeys()) {

            auto &key = mCtx->getKeyById(keyid);

            QString newKey = " (" + keyid + ")";
            if (!QString(key.email).isEmpty()) {
                newKey.prepend(" <" + key.email + ">");
            }
            if (!QString(key.name).isEmpty()) {
                newKey.prepend(" " + key.name);
            }
            keyIds.insert(key.id, newKey);
        }
        foreach(QString k, keyIds) {
            ownKeySelectBox->addItem(k);
        }
    connect(ownKeySelectBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOwnKeyIdChanged()));

    ownKeyBoxLayout->addWidget(new QLabel(tr("Encrypt all messages additionally to the chosen key:")));
    ownKeyBoxLayout->addWidget(ownKeySelectBox);

    /*****************************************
     * Mainlayout
     *****************************************/
    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(rememberPasswordBox);
    mainLayout->addWidget(saveCheckedKeysBox);
    mainLayout->addWidget(importConfirmationBox);
    mainLayout->addWidget(langBox);
    mainLayout->addWidget(ownKeyBox);

    setSettings();
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void GeneralTab::setSettings() {
    // Keysaving
    if (settings.value("keys/keySave").toBool()) {
        saveCheckedKeysCheckBox->setCheckState(Qt::Checked);
    }

    // Remember Password
    if (settings.value("general/rememberPassword").toBool()) {
        rememberPasswordCheckBox->setCheckState(Qt::Checked);
    }

    // Language setting
    QString langKey = settings.value("int/lang").toString();
    QString langValue = lang.value(langKey);
    if (langKey != "") {
        langSelectBox->setCurrentIndex(langSelectBox->findText(langValue));
    }

    // Get own key information from keydb/gpg.conf (if contained)
    QFile gpgConfFile(qApp->applicationDirPath() + "/keydb/gpg.conf");
    gpgConfFile.open(QFile::ReadOnly);
    while (!gpgConfFile.atEnd()) {
        QString line = gpgConfFile.readLine();
        if (line.startsWith("recipient")) {
            QStringList args;

            // get key id from gpg.conf
            args = line.split(" ");
            ownKeyId = args.at(1);
            // remove linebreak at end of id
            ownKeyId.remove("\n");
            ownKeyId.remove("\r");
        }
    }
    gpgConfFile.close();
    if (ownKeyId.isEmpty()) {
        ownKeySelectBox->setCurrentIndex(0);
    } else {
        ownKeySelectBox->setCurrentIndex(ownKeySelectBox->findText(ownKeyId, Qt::MatchContains));
    }

    if (settings.value("general/confirmImportKeys", Qt::Checked).toBool()) {
        importConfirmationCheckBox->setCheckState(Qt::Checked);
    }
}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void GeneralTab::applySettings() {
    settings.setValue("keys/keySave", saveCheckedKeysCheckBox->isChecked());
    // TODO: clear passwordCache instantly on unset rememberPassword
    settings.setValue("general/rememberPassword", rememberPasswordCheckBox->isChecked());
    settings.setValue("int/lang", lang.key(langSelectBox->currentText()));
    settings.setValue("general/confirmImportKeys", importConfirmationCheckBox->isChecked());
}

void GeneralTab::slotLanguageChanged() {
    emit signalRestartNeeded(true);
}

void GeneralTab::slotOwnKeyIdChanged() {
    // Set ownKeyId to currently selected

    QHashIterator<QString, QString> i(keyIds);
    while (i.hasNext()) {
        i.next();
        if (ownKeySelectBox->currentText() == i.value()) {
            ownKeyId = i.key();
        }
    }

    /*****************************************
     * Write keyid of own key to gpg.conf
     *****************************************/
    QFile gpgConfFile(qApp->applicationDirPath() + "/keydb/gpg.conf");
    gpgConfFile.open(QFile::ReadWrite);
    QFile gpgConfTempFile(qApp->applicationDirPath() + "/keydb/gpg.conf.swp");
    gpgConfTempFile.open(QFile::WriteOnly);

    // remove line with the hidden-encrypt-to
    while (!gpgConfFile.atEnd()) {
        QByteArray line = gpgConfFile.readLine();
        if (!line.startsWith("recipient")) {
            gpgConfTempFile.write(line);
        }
    }

    // add line with hidden-encrypt-to, if a key is chosen
    if (!ownKeyId.isEmpty()) {
        QByteArray string("recipient ");
        string.append(ownKeyId.toUtf8());
        string.append("\n");
        gpgConfTempFile.write(string);
    }

    gpgConfFile.close();
    gpgConfTempFile.close();

    // move the temporary gpg.conffile to the actual one
    gpgConfFile.remove();
    QFile::copy(gpgConfTempFile.fileName(), gpgConfFile.fileName());
    gpgConfTempFile.remove();
}

MimeTab::MimeTab(QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {
    /*****************************************
     * MIME-Parsing-Box
     *****************************************/
    auto *mimeQPBox = new QGroupBox(tr("Decode quoted printable"));
    auto *mimeQPBoxLayout = new QVBoxLayout();
    mimeQPCheckBox = new QCheckBox(tr("Try to recognize quoted printable."), this);
    mimeQPBoxLayout->addWidget(mimeQPCheckBox);
    mimeQPBox->setLayout(mimeQPBoxLayout);

    auto *mimeParseBox = new QGroupBox(tr("Parse PGP/MIME (Experimental)"));
    auto *mimeParseBoxLayout = new QVBoxLayout();
    mimeParseCheckBox = new QCheckBox(tr("Try to split attachments from PGP-MIME ecrypted messages."), this);
    mimeParseBoxLayout->addWidget(mimeParseCheckBox);
    mimeParseBox->setLayout(mimeParseBoxLayout);

    auto *mimeOpenAttachmentBox = new QGroupBox(tr("Open with external application (Experimental)"));
    auto *mimeOpenAttachmentBoxLayout = new QVBoxLayout();
    auto *mimeOpenAttachmentText = new QLabel(tr("Open attachments with default application for the filetype.<br> "
                                                 "There are at least two possible problems with this behaviour:"
                                                 "<ol><li>File needs to be saved unencrypted to attachments folder.<br> "
                                                 "Its your job to clean this folder.</li>"
                                                 "<li>The external application may have its own temp files.</li></ol>"));

    //mimeOpenAttachmentBox->setDisabled(true);
    mimeOpenAttachmentCheckBox = new QCheckBox(tr("Enable opening with external applications."), this);

    mimeOpenAttachmentBoxLayout->addWidget(mimeOpenAttachmentText);
    mimeOpenAttachmentBoxLayout->addWidget(mimeOpenAttachmentCheckBox);
    mimeOpenAttachmentBox->setLayout(mimeOpenAttachmentBoxLayout);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mimeParseBox);
    mainLayout->addWidget(mimeOpenAttachmentBox);
    mainLayout->addWidget(mimeQPBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
    setSettings();
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void MimeTab::setSettings() {

    // MIME-Parsing
    if (settings.value("mime/parsemime").toBool()) mimeParseCheckBox->setCheckState(Qt::Checked);

    // Qouted Printable
    if (settings.value("mime/parseQP", true).toBool()) mimeQPCheckBox->setCheckState(Qt::Checked);

    // Open Attachments with external app
    if (settings.value("mime/openAttachment").toBool()) mimeOpenAttachmentCheckBox->setCheckState(Qt::Checked);
}


/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void MimeTab::applySettings() {
    settings.setValue("mime/parsemime", mimeParseCheckBox->isChecked());
    settings.setValue("mime/parseQP", mimeQPCheckBox->isChecked());
    settings.setValue("mime/openAttachment", mimeOpenAttachmentCheckBox->isChecked());

}

AppearanceTab::AppearanceTab(QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {
    /*****************************************
      * Icon-Size-Box
      *****************************************/
    auto *iconSizeBox = new QGroupBox(tr("Iconsize"));
    iconSizeGroup = new QButtonGroup();
    iconSizeSmall = new QRadioButton(tr("small"));
    iconSizeMedium = new QRadioButton(tr("medium"));
    iconSizeLarge = new QRadioButton(tr("large"));

    iconSizeGroup->addButton(iconSizeSmall, 1);
    iconSizeGroup->addButton(iconSizeMedium, 2);
    iconSizeGroup->addButton(iconSizeLarge, 3);

    auto *iconSizeBoxLayout = new QHBoxLayout();
    iconSizeBoxLayout->addWidget(iconSizeSmall);
    iconSizeBoxLayout->addWidget(iconSizeMedium);
    iconSizeBoxLayout->addWidget(iconSizeLarge);

    iconSizeBox->setLayout(iconSizeBoxLayout);

    /*****************************************
      * Icon-Style-Box
      *****************************************/
    auto *iconStyleBox = new QGroupBox(tr("Iconstyle"));
    iconStyleGroup = new QButtonGroup();
    iconTextButton = new QRadioButton(tr("just text"));
    iconIconsButton = new QRadioButton(tr("just icons"));
    iconAllButton = new QRadioButton(tr("text and icons"));

    iconStyleGroup->addButton(iconTextButton, 1);
    iconStyleGroup->addButton(iconIconsButton, 2);
    iconStyleGroup->addButton(iconAllButton, 3);

    auto *iconStyleBoxLayout = new QHBoxLayout();
    iconStyleBoxLayout->addWidget(iconTextButton);
    iconStyleBoxLayout->addWidget(iconIconsButton);
    iconStyleBoxLayout->addWidget(iconAllButton);

    iconStyleBox->setLayout(iconStyleBoxLayout);

    /*****************************************
      * Window-Size-Box
      *****************************************/
    auto *windowSizeBox = new QGroupBox(tr("Windowstate"));
    auto *windowSizeBoxLayout = new QHBoxLayout();
    windowSizeCheckBox = new QCheckBox(tr("Save window size and position on exit."), this);
    windowSizeBoxLayout->addWidget(windowSizeCheckBox);
    windowSizeBox->setLayout(windowSizeBoxLayout);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconSizeBox);
    mainLayout->addWidget(iconStyleBox);
    mainLayout->addWidget(windowSizeBox);
    mainLayout->addStretch(1);
    setSettings();
    setLayout(mainLayout);
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void AppearanceTab::setSettings() {

    //Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(24, 24)).toSize();
    switch (iconSize.height()) {
        case 12:
            iconSizeSmall->setChecked(true);
            break;
        case 24:
            iconSizeMedium->setChecked(true);
            break;
        case 32:
            iconSizeLarge->setChecked(true);
            break;
    }
    // Iconstyle
    Qt::ToolButtonStyle iconStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle",
                                                                                    Qt::ToolButtonTextUnderIcon).toUInt());
    switch (iconStyle) {
        case Qt::ToolButtonTextOnly:
            iconTextButton->setChecked(true);
            break;
        case Qt::ToolButtonIconOnly:
            iconIconsButton->setChecked(true);
            break;
        case Qt::ToolButtonTextUnderIcon:
            iconAllButton->setChecked(true);
            break;
        default:
            break;
    }

    // Window Save and Position
    if (settings.value("window/windowSave").toBool()) windowSizeCheckBox->setCheckState(Qt::Checked);

}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void AppearanceTab::applySettings() {
    switch (iconSizeGroup->checkedId()) {
        case 1:
            settings.setValue("toolbar/iconsize", QSize(12, 12));
            break;
        case 2:
            settings.setValue("toolbar/iconsize", QSize(24, 24));
            break;
        case 3:
            settings.setValue("toolbar/iconsize", QSize(32, 32));
            break;
    }

    switch (iconStyleGroup->checkedId()) {
        case 1:
            settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextOnly);
            break;
        case 2:
            settings.setValue("toolbar/iconstyle", Qt::ToolButtonIconOnly);
            break;
        case 3:
            settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon);
            break;
    }

    settings.setValue("window/windowSave", windowSizeCheckBox->isChecked());
}

KeyserverTab::KeyserverTab(QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {

    auto keyServerList = settings.value("keyserver/keyServerList").toStringList();

    auto *mainLayout = new QVBoxLayout(this);

    auto *label = new QLabel(tr("Default Keyserver for import:"));
    comboBox = new QComboBox;
    comboBox->setEditable(false);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    for (const auto &keyServer : keyServerList) {
        comboBox->addItem(keyServer);
        qDebug() << "KeyserverTab Get ListItemText" << keyServer;
    }

    comboBox->setCurrentText(settings.value("keyserver/defaultKeyServer").toString());

    auto *addKeyServerBox = new QWidget(this);
    auto *addKeyServerLayout = new QHBoxLayout(addKeyServerBox);
    auto *http = new QLabel("URL: ");
    newKeyServerEdit = new QLineEdit(this);
    auto *newKeyServerButton = new QPushButton(tr("Add"), this);
    connect(newKeyServerButton, SIGNAL(clicked()), this, SLOT(addKeyServer()));
    addKeyServerLayout->addWidget(http);
    addKeyServerLayout->addWidget(newKeyServerEdit);
    addKeyServerLayout->addWidget(newKeyServerButton);

    mainLayout->addWidget(label);
    mainLayout->addWidget(comboBox);
    mainLayout->addWidget(addKeyServerBox);
    mainLayout->addStretch(1);

    // Read keylist from ini-file and fill it into combobox
    setSettings();
}


/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void KeyserverTab::setSettings() {
    auto *keyServerList = new QStringList();
    for (int i = 0; i < comboBox->count(); i++) {
        keyServerList->append(comboBox->itemText(i));
        qDebug() << "KeyserverTab ListItemText" << comboBox->itemText(i);
    }
    settings.setValue("keyserver/keyServerList", *keyServerList);
    settings.setValue("keyserver/defaultKeyServer", comboBox->currentText());
}

void KeyserverTab::addKeyServer() {
    if (newKeyServerEdit->text().startsWith("http://") || newKeyServerEdit->text().startsWith("https://")) {
        comboBox->addItem(newKeyServerEdit->text());
    } else {
        comboBox->addItem("http://" + newKeyServerEdit->text());
    }
    comboBox->setCurrentIndex(comboBox->count() - 1);
}

/***********************************
  * get the values of the buttons and
  * write them to settings-file
  *************************************/
void KeyserverTab::applySettings() {
    settings.setValue("keyserver/defaultKeyServer", comboBox->currentText());
}

AdvancedTab::AdvancedTab(QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {
    /*****************************************
     * Steganography Box
     *****************************************/
    auto *steganoBox = new QGroupBox(tr("Show Steganography Options [Advanced]"));
    auto *steganoBoxLayout = new QHBoxLayout();
    steganoCheckBox = new QCheckBox(tr("Show Steganographic Options."), this);
    steganoBoxLayout->addWidget(steganoCheckBox);
    steganoBox->setLayout(steganoBoxLayout);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(steganoBox);
    setSettings();
    mainLayout->addStretch(1);
    setLayout(mainLayout);

}

void AdvancedTab::setSettings() {
    if (settings.value("advanced/steganography").toBool()) {
        steganoCheckBox->setCheckState(Qt::Checked);
    }
}

void AdvancedTab::applySettings() {
    settings.setValue("advanced/steganography", steganoCheckBox->isChecked());
}

GpgPathsTab::GpgPathsTab(QWidget *parent)
        : QWidget(parent), appPath(qApp->applicationDirPath()),
          settings(appPath + "/conf/gpgfrontend.ini", QSettings::IniFormat) {
    setSettings();

    /*****************************************
     * Keydb Box
     *****************************************/
    auto *keydbBox = new QGroupBox(tr("Relative path to keydb"));
    auto *keydbBoxLayout = new QGridLayout();

    // Label containing the current keydbpath relative to default keydb path
    keydbLabel = new QLabel(accKeydbPath, this);

    auto *keydbButton = new QPushButton("Change keydb path", this);
    connect(keydbButton, SIGNAL(clicked()), this, SLOT(chooseKeydbDir()));
    auto *keydbDefaultButton = new QPushButton("Set keydb to default path", this);
    connect(keydbDefaultButton, SIGNAL(clicked()), this, SLOT(setKeydbPathToDefault()));

    keydbBox->setLayout(keydbBoxLayout);
    keydbBoxLayout->addWidget(new QLabel(tr("Current keydb path: ")), 1, 1);
    keydbBoxLayout->addWidget(keydbLabel, 1, 2);
    keydbBoxLayout->addWidget(keydbButton, 1, 3);
    keydbBoxLayout->addWidget(keydbDefaultButton, 2, 3);
    keydbBoxLayout->addWidget(
            new QLabel(tr("<b>NOTE: </b> Gpg4usb will restart automatically if you change the keydb path!")), 3, 1, 1,
            3);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(keydbBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QString GpgPathsTab::getRelativePath(const QString &dir1, const QString &dir2) {
    QDir dir(dir1);
    QString s;

    s = dir.relativeFilePath(dir2);
    qDebug() << "relative path: " << s;
    if (s.isEmpty()) {
        s = ".";
    }
    return s;
}

void GpgPathsTab::setKeydbPathToDefault() {
    accKeydbPath = ".";
    keydbLabel->setText(".");
}

QString GpgPathsTab::chooseKeydbDir() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose keydb directory"), accKeydbPath,
                                                    QFileDialog::ShowDirsOnly);

    accKeydbPath = getRelativePath(defKeydbPath, dir);
    keydbLabel->setText(accKeydbPath);
    return "";
}

void GpgPathsTab::setSettings() {
    defKeydbPath = qApp->applicationDirPath() + "/keydb";

    accKeydbPath = settings.value("gpgpaths/keydbpath").toString();
    if (accKeydbPath.isEmpty()) {
        accKeydbPath = ".";
    }
}

void GpgPathsTab::applySettings() {
    settings.setValue("gpgpaths/keydbpath", accKeydbPath);
}
