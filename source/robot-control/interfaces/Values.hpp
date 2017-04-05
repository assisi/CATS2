#ifndef CATS2_VALUES_HPP
#define CATS2_VALUES_HPP

#include <QtCore/QList>

#include <functional>

typedef QList<qint16> Values;

typedef std::function<void(const Values&)> EventCallback;

#endif // CATS2_VALUES_HPP

