#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>
#include <QVariantMap>

class Passenger {
public:
    Passenger() = default;
    explicit Passenger(const QString& col) : color(col) {}

    QString color;

    // For QML display
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["color"] = color;
        return map;
    }
};

#endif // PASSENGER_H
