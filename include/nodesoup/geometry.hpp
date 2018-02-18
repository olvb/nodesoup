#pragma once

#include <cmath>

namespace nodesoup {

struct Vector2D;

struct Point2D {
    double x;
    double y;

    explicit operator Vector2D() const;
    Point2D& operator+=(const Vector2D& vector);
    Point2D& operator-=(const Vector2D& vector);
};

struct Vector2D {
    double dx;
    double dy;

    double norm() const {
        return sqrt(dx * dx + dy * dy);
    }

    explicit operator Point2D() const;
    Vector2D& operator+=(const Vector2D& other);
    Vector2D& operator-=(const Vector2D& other);
    Vector2D& operator*=(double scalar);
    Vector2D& operator/=(double scalar);
};

Point2D operator+(const Point2D& point, const Vector2D& vector);
Point2D operator-(const Point2D& point, const Vector2D& vector);
Vector2D operator-(const Point2D& lhs, const Point2D& rhs);
Vector2D operator+(const Vector2D lhs, const Vector2D& rhs);
Vector2D operator-(const Vector2D& l, const Vector2D& rhs);
Vector2D operator*(const Vector2D& vector, double scalar);
Vector2D operator*(double scalar, const Vector2D& vector);
Vector2D operator/(const Vector2D& vector, double scalar);

}
