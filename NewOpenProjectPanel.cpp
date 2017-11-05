/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2009  Joseph Artsimovich <joseph_a@mail.ru>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NewOpenProjectPanel.h"
#include "RecentProjects.h"
#include "Utils.h"
#include "ColorSchemeManager.h"
#include <QPainter>
#include <QFileInfo>

NewOpenProjectPanel::NewOpenProjectPanel(QWidget* parent)
        : QWidget(parent) {
    setupUi(this);

    recentProjectsGroup->setLayout(new QVBoxLayout);
    newProjectLabel->setText(
            Utils::richTextForLink(newProjectLabel->text())
    );
    openProjectLabel->setText(
            Utils::richTextForLink(openProjectLabel->text())
    );

    RecentProjects rp;
    rp.read();
    if (!rp.validate()) {
        // Some project files weren't found.
        // Write the list without them.
        rp.write();
    }
    if (rp.isEmpty()) {
        recentProjectsGroup->setVisible(false);
    } else {
        rp.enumerate(
                [this](QString const& file_path) {
                    addRecentProject(file_path);
                }
        );
    }

    connect(
            newProjectLabel, SIGNAL(linkActivated(QString const &)),
            this, SIGNAL(newProject())
    );
    connect(
            openProjectLabel, SIGNAL(linkActivated(QString const &)),
            this, SIGNAL(openProject())
    );
}

void NewOpenProjectPanel::addRecentProject(QString const& file_path) {
    QFileInfo const file_info(file_path);
    QString base_name(file_info.baseName());
    if (base_name.isEmpty()) {
        base_name = QChar('_');
    }
    QLabel* label = new QLabel(recentProjectsGroup);
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setText(Utils::richTextForLink(base_name, file_path));
    label->setToolTip(file_path);

    int fontSize = recentProjectsGroup->font().pointSize();
    QFont widgetFont = label->font();
    widgetFont.setPointSize(fontSize);
    label->setFont(widgetFont);

    recentProjectsGroup->layout()->addWidget(label);

    connect(
            label, SIGNAL(linkActivated(QString const &)),
            this, SIGNAL(openRecentProject(QString const &))
    );
}

void NewOpenProjectPanel::paintEvent(QPaintEvent*) {
    // In fact Qt doesn't draw QWidget's background, unless
    // autoFillBackground property is set, so we can safely
    // draw our borders and shadows in the margins area.

    int left = 0, top = 0, right = 0, bottom = 0;
    layout()->getContentsMargins(&left, &top, &right, &bottom);

    QRect const widget_rect(rect());
    QRect const except_margins(
            widget_rect.adjusted(left, top, -right, -bottom)
    );

    int const border = 1;  // Solid line border width.

    QPainter painter(this);

    painter.setPen(QPen(ColorSchemeManager::instance()->getColorParam(
            "open_new_project_border_color",
            palette().windowText()), border));

    painter.drawRect(except_margins);
} // NewOpenProjectPanel::paintEvent

