#include "qtadvwidgets/TokenMimeData.h"

#include <qtadvwidgets/Token.h>

TokenMimeData::TokenMimeData(Token* token) : _token{token} {}

Token* TokenMimeData::token() const { return _token; }

QStringList TokenMimeData::formats() const { return {tokenMimeType()}; }

bool TokenMimeData::hasFormat(QString const& format) const {
  return format == tokenMimeType();
}

QString TokenMimeData::tokenMimeType() {
  static auto const str = QString{"application/x-tokenedit-token-ptr"};
  return str;
}

QVariant TokenMimeData::retrieveData(const QString& format,
                                     QVariant::Type preferredType) const {
  if (format == tokenMimeType()) {
    return QVariant::fromValue(_token);
  } else {
    return QMimeData::retrieveData(format, preferredType);
  }
}
