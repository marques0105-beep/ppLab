#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>
#include <QVariantMap>

class Passenger {
public:
    Passenger() = default;
    // Construtor que define a cor do passageiro 
    explicit Passenger(const QString& col) : color(col) {}

    // Atributo de cor do passageiro
    QString color;

    // Método para converter o passageiro em um mapa de variantes
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["color"] = color;
        return map;
    }
};

#endif // PASSENGER_H
