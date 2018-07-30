/**
 * @file
 * Dark Style Theme
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "darkstyletheme.h"

/**
 * @brief DarkStyleTheme::DarkStyleTheme
 */
DarkStyleTheme::DarkStyleTheme(): DarkStyleTheme(styleBase())
{

}

/**
 * @brief DarkStyleTheme::DarkStyleTheme
 * @param style
 */
DarkStyleTheme::DarkStyleTheme(QStyle *style): QProxyStyle(style)
{

}

/**
 * @brief DarkStyleTheme::styleBase
 * @param style
 * @return
 */
QStyle *DarkStyleTheme::styleBase(QStyle *style) const
{
    static QStyle *base = !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
    return base;
}

/**
 * @brief DarkStyleTheme::baseStyle
 * @return
 */
QStyle *DarkStyleTheme::baseStyle() const
{
    return styleBase();
}

/**
 * @brief DarkStyleTheme::polish
 * @param palette
 */
void DarkStyleTheme::polish(QPalette &palette)
{
    // modify palette to dark
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    palette.setColor(QPalette::Base, QColor(42, 42, 42));
    palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Dark, QColor(35, 35, 35));
    palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0, 100, 0)); //Green
    palette.setColor(QPalette::Highlight, QColor(0, 100, 0)); //Green
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
}

/**
 * @brief DarkStyleTheme::polish
 * @param app
 */
void DarkStyleTheme::polish(QApplication *app)
{
    if (!app) return;

    //ARTS - Increase font size for better reading,
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize() + 1);
    QApplication::setFont(defaultFont);

    // Load CSS Stylesheet from resources
    QFile darkstyleQSS(QStringLiteral(":/darkstyle/stylesheet_dark.qss"));
    if (darkstyleQSS.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Set stylesheet
        QString stylesheet_qss = QString::fromLatin1(darkstyleQSS.readAll());
        app->setStyleSheet(stylesheet_qss);
        darkstyleQSS.close();
    }
}
