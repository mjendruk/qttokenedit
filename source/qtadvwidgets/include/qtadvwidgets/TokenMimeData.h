#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QMimeData>

class Token;

class QTADVWIDGETS_API TokenMimeData : public QMimeData {
  Q_OBJECT
 public:
  TokenMimeData(Token* token);

  Token* token() const;

  QStringList formats() const override;
  bool hasFormat(QString const& format) const override;

  static QString tokenMimeType();

 protected:
  QVariant retrieveData(const QString& format,
                        QVariant::Type preferredType) const override;

 private:
  Token* _token;
};
