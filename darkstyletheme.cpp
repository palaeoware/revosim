/**
 * @file
 * Dark Style Theme
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "darkstyletheme.h"

DarkStyleTheme::DarkStyleTheme():
    DarkStyleTheme(styleBase())
{

}

DarkStyleTheme::DarkStyleTheme(QStyle *style):
  QProxyStyle(style)
{

}

QStyle *DarkStyleTheme::styleBase(QStyle *style) const {
  static QStyle *base = !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
  return base;
}

QStyle *DarkStyleTheme::baseStyle() const
{
  return styleBase();
}

void DarkStyleTheme::polish(QPalette &palette)
{
  // modify palette to dark
  palette.setColor(QPalette::Window,QColor(53,53,53));
  palette.setColor(QPalette::WindowText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
  palette.setColor(QPalette::Base,QColor(42,42,42));
  palette.setColor(QPalette::AlternateBase,QColor(66,66,66));
  palette.setColor(QPalette::ToolTipBase,Qt::white);
  palette.setColor(QPalette::ToolTipText,QColor(53,53,53));
  palette.setColor(QPalette::Text,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
  palette.setColor(QPalette::Dark,QColor(35,35,35));
  palette.setColor(QPalette::Shadow,QColor(20,20,20));
  palette.setColor(QPalette::Button,QColor(53,53,53));
  palette.setColor(QPalette::ButtonText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
  palette.setColor(QPalette::BrightText,Qt::red);
  palette.setColor(QPalette::Link,QColor(0, 100, 0)); //Green
  palette.setColor(QPalette::Highlight,QColor(0, 100, 0)); //Green
  palette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
  palette.setColor(QPalette::HighlightedText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
}

void DarkStyleTheme::polish(QApplication *app)
{
  if (!app) return;

  //ARTS - Increase font size for better reading,
  // setPointSize was reduced from +2 because when applied this way in Qt5, the font is larger than intended for some reason
  QFont defaultFont = QApplication::font();
  defaultFont.setPointSize(defaultFont.pointSize()+1);
  app->setFont(defaultFont);

  // Load CSS Stylesheet from resources
  QFile qfDarkstyle(QStringLiteral(":/darkstyle/stylesheet_dark.qss"));
  if (qfDarkstyle.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    // Set stylesheet
    QString qsStylesheet = QString::fromLatin1(qfDarkstyle.readAll());
    app->setStyleSheet(qsStylesheet);
    qfDarkstyle.close();
  }
}
