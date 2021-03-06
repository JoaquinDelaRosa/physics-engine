#include "Vector.h"

namespace PMath{
    const Vector init(float w, float x, float y, float z){
        return Vector{w, x, y, z};
    }

    const Vector operator -(const Vector& v){
        return Vector{-v.vec[0], -v.vec[1], -v.vec[2], -v.vec[3]};
    }

    const Vector operator +(const Vector& v1, const Vector& v2){
        Vector result;
        _mm_store_ps(result.vec, _mm_add_ps(v1.vec4, v2.vec4));
        return result;
    }

    const Vector operator -(const Vector& v1, const Vector& v2){
        Vector result;
        _mm_store_ps(result.vec, _mm_sub_ps(v1.vec4, v2.vec4));
        return result;
    }

    const Vector operator *(float c, const Vector& v){
        Vector result;
        Vector scalar = Vector{c, c, c, c};
        _mm_store_ps(result.vec, _mm_mul_ps(v.vec4, scalar.vec4));
        return result;
    }

    const Vector operator *(const Vector& v, float c){
        Vector result;
        Vector scalar = Vector{c, c, c, c};
        _mm_store_ps(result.vec, _mm_mul_ps(v.vec4, scalar.vec4));
        return result;
    }

    const Vector operator /(const Vector& v, float c){
        Vector result;
        Vector scalar = Vector{c, c, c, c};
        _mm_store_ps(result.vec, _mm_div_ps(v.vec4, scalar.vec4));
        return result;
    }

    void operator +=(Vector& v1, const Vector& v2){
        v1 = PMath::operator+(v1, v2);
    }

    void operator -=(Vector& v1, const Vector& v2){
        v1 = PMath::operator-(v1, v2);
    }

    void operator *=(Vector& v, float c){
        v = PMath::operator*(v, c);
    }

    void operator /=(Vector& v, float c){
        v = PMath::operator/(v, c);
    }

    const float hsum(__m128 v) {
        // Horizontal sum based on https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
        __m128 shuf = _mm_movehdup_ps(v);        // broadcast elements 3,1 to 2,0
        __m128 sums = _mm_add_ps(v, shuf);
        shuf        = _mm_movehl_ps(shuf, sums); // high half -> low half
        sums        = _mm_add_ss(sums, shuf);
        return        _mm_cvtss_f32(sums);
    }

    const float dot(const Vector& v1, const Vector& v2){
        __m128 mult = _mm_mul_ps(v1.vec4, v2.vec4);
        return PMath::hsum(mult);
    }

    const float norm(const Vector& v){
        return sqrt(PMath::dot(v, v));
    }

    const Vector normalize(const Vector& v){
        float n = PMath::norm(v);
        return Vector{v.vec[0] / n, v.vec[1] / n, v.vec[2] / n, v.vec[3]/n};
    }

    const bool operator ==(const Vector& v1, const Vector& v2){
        return PMath::norm(PMath::operator-(v1, v2)) <= EQUALITY_THRESHOLD;
    }

    std::ostream& operator <<(std::ostream& os, const Vector& v){
        return os << "["<<v.vec[0]<<",\t"<<v.vec[1]<<",\t"<<v.vec[2]<<",\t"<<v.vec[3]<<"]";
    }
}
