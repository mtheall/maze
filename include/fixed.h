#pragma once
#ifndef FIXED_H
#define FIXED_H

#include <feosmath.h>

class f32 {
private:
  s32 v;
public:
  f32(s32 v);

  s32 getValue() const;
  v10 tov10() const;
  f32 const sqrt() const;

  const f32 operator+(const f32 &f) const;
  const f32 operator-(const f32 &f) const;
  const f32 operator*(const f32 &f) const;
  const f32 operator/(const f32 &f) const;
  const f32 operator-() const;
  f32& operator+=(const f32 &f);
  f32& operator-=(const f32 &f);
  f32& operator*=(const f32 &f);
  f32& operator/=(const f32 &f);

  bool operator< (const f32 &f) const;
  bool operator<=(const f32 &f) const;
  bool operator> (const f32 &f) const;
  bool operator>=(const f32 &f) const;
  bool operator==(const f32 &f) const;
  bool operator!=(const f32 &f) const;
};

inline f32::f32(s32 v) : v(v) {
}

inline s32 f32::getValue() const {
  return v;
}

inline v10 f32::tov10() const {
  return (v>>3);
}

inline const f32 f32::sqrt() const {
  return f32(sqrtf32(v));
}

inline const f32 f32::operator+(const f32 &f) const {
  return f32(v + f.v);
}

inline const f32 f32::operator-(const f32 &f) const {
  return f32(v - f.v);
}

inline const f32 f32::operator*(const f32 &f) const {
  return f32(mulf32(v, f.v));
}

inline const f32 f32::operator/(const f32 &f) const {
  return f32(divf32(v, f.v));
}

inline const f32 f32::operator-() const {
  return f32(-v);
}

inline f32& f32::operator+=(const f32 &f) {
  v += f.v;
  return *this;
}

inline f32& f32::operator-=(const f32 &f) {
  v -= f.v;
  return *this;
}

inline f32& f32::operator*=(const f32 &f) {
  v = mulf32(v, f.v);
  return *this;
}

inline f32& f32::operator/=(const f32 &f) {
  v = divf32(v, f.v);
  return *this;
}

bool f32::operator< (const f32 &f) const {
  return v < f.v;
}

bool f32::operator<=(const f32 &f) const {
  return v <= f.v;
}

bool f32::operator> (const f32 &f) const {
  return v > f.v;
}

bool f32::operator>=(const f32 &f) const {
  return v >= f.v;
}

bool f32::operator==(const f32 &f) const {
  return v == f.v;
}

bool f32::operator!=(const f32 &f) const {
  return v != f.v;
}

#endif

