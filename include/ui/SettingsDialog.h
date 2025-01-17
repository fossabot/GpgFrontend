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

#ifndef __SETTINGSDIALOG_H__
#define __SETTINGSDIALOG_H__

#include "ui/widgets/KeyList.h"

class GeneralTab : public QWidget {
Q_OBJECT

public:
    explicit GeneralTab(GpgME::GpgContext *ctx, QWidget *parent = nullptr);

    void setSettings();

    void applySettings();

private:

    QString appPath;
    QSettings settings;

    QCheckBox *rememberPasswordCheckBox;
    QCheckBox *saveCheckedKeysCheckBox;
    QCheckBox *importConfirmationCheckBox;
    QComboBox *langSelectBox;
    QComboBox *ownKeySelectBox;
    QHash<QString, QString> lang;
    QHash<QString, QString> keyIds;
    QString ownKeyId;
    KeyList *mKeyList;
    GpgME::GpgContext *mCtx; /** The current gpg context */

private slots:

    void slotOwnKeyIdChanged();

    void slotLanguageChanged();

signals:

    void signalRestartNeeded(bool needed);

};

class MimeTab : public QWidget {
Q_OBJECT

public:
    explicit MimeTab(QWidget *parent = nullptr);

    void setSettings();

    void applySettings();

private:

    QString appPath;
    QSettings settings;

    QCheckBox *mimeParseCheckBox;
    QCheckBox *mimeQPCheckBox;
    QCheckBox *mimeOpenAttachmentCheckBox;

signals:

    void signalRestartNeeded(bool needed);

};

class AppearanceTab : public QWidget {
Q_OBJECT

public:
    //void setSettings();
    explicit AppearanceTab(QWidget *parent = nullptr);

    void setSettings();

    void applySettings();

private:

    QString appPath;
    QSettings settings;

    QButtonGroup *iconStyleGroup;
    QRadioButton *iconSizeSmall;
    QRadioButton *iconSizeMedium;
    QRadioButton *iconSizeLarge;
    QButtonGroup *iconSizeGroup;
    QRadioButton *iconTextButton;
    QRadioButton *iconIconsButton;
    QRadioButton *iconAllButton;
    QCheckBox *windowSizeCheckBox;

signals:

    void signalRestartNeeded(bool needed);

};

class KeyserverTab : public QWidget {
Q_OBJECT

public:
    explicit KeyserverTab(QWidget *parent = nullptr);

    void setSettings();

    void applySettings();

private:

    QString appPath;
    QSettings settings;

    QComboBox *comboBox;
    QLineEdit *newKeyServerEdit;

private slots:

    void addKeyServer();

signals:

    void signalRestartNeeded(bool needed);

};

class AdvancedTab : public QWidget {
Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);

    void setSettings();

    void applySettings();

private:

    QString appPath;
    QSettings settings;

    QCheckBox *steganoCheckBox;

signals:

    void signalRestartNeeded(bool needed);

};

class GpgPathsTab : public QWidget {
Q_OBJECT
public:
    explicit GpgPathsTab(QWidget *parent = nullptr);

    void applySettings();

private:
    static QString getRelativePath(const QString& dir1, const QString& dir2);

    QString appPath;
    QSettings settings;

    QString defKeydbPath; /** The default keydb path used by gpg4usb */
    QString accKeydbPath; /** The currently used keydb path */
    QLabel *keydbLabel;

    void setSettings();

private slots:

    QString chooseKeydbDir();

    void setKeydbPathToDefault();

};

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(GpgME::GpgContext *ctx, QWidget *parent = nullptr);

    GeneralTab *generalTab;
    MimeTab *mimeTab;
    AppearanceTab *appearanceTab;
    KeyserverTab *keyserverTab;
    AdvancedTab *advancedTab;
    GpgPathsTab *gpgPathsTab;

    static QHash<QString, QString> listLanguages();

public slots:

    void slotAccept();

signals:

    void signalRestartNeeded(bool needed);

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    GpgME::GpgContext *mCtx; /** The current gpg context */
    bool restartNeeded{};

    bool getRestartNeeded() const;

private slots:

    void slotSetRestartNeeded(bool needed);

};

#endif  // __SETTINGSDIALOG_H__
