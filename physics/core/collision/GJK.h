#ifndef GJK_H_INCLUDED
#define GJK_H_INCLUDED

#include "Collider.h"
#include "../../math/Vector.h"
#include <array>
#include <cmath>

// Implementation based on this video: https://www.youtube.com/watch?v=MDusDn8oTSE

namespace Physics {
	// Helper functions
	using namespace PMath;
	namespace Utils {
		Vector GetSupport(const Collider* c1, const Collider* c2, Vector dir) {
			return c1->FindFurthestPoint(dir) - c2->FindFurthestPoint(-dir);
		}

		struct Simplex {
		private:
			std::array<Vector, 4> points;
			unsigned int size;

		public:
			Simplex() {
				points = { 0, 0, 0, 0 };
				size = 0;
			}

			Simplex& operator=(std::initializer_list<Vector> list) {
				for (auto v = list.begin(); v != list.end(); ++v) {
					points[std::distance(list.begin(), v)] = *v;
				}
				size = list.size();

				return *this;
			}

			void Push(Vector point) {
				points = { point, points[0], points[1], points[2] };
				size = std::min(size + 1, (unsigned int)4);
			}

			Vector& operator[](unsigned i) {
				return points[i];
			}

			unsigned Size() const {
				return size;
			}

			auto Begin() const {
				return points.begin();
			}

			auto End() const {
				return points.end();
			}
		};


		// Helper functions for each case of the GJK algorithm
		bool GJKLine(Simplex& simplex, Vector& direction) {
			Vector a = simplex[0];
			Vector b = simplex[1];

			Vector ab = b - a;
			Vector a0 = -a;

			if (is_same_dir(ab, a0)) {
				direction = cross(cross(ab, a0), ab);
			}
			else {
				simplex = { a };
				direction = a0;
			}

			return false;
		}

		bool GJKTriangle(Simplex& simplex, Vector& direction) {
			Vector a = simplex[0];
			Vector b = simplex[1];
			Vector c = simplex[2];

			Vector ab = b - a;
			Vector ac = c - a;
			Vector a0 = -a;
			Vector abc = cross(ab, ac);

			if (is_same_dir(cross(abc, ac), a0)) {
				if (is_same_dir(ac, a0)) {
					simplex = { a, c };
					direction = cross(cross(ac, a0), ac);
				}
				else {
					return GJKLine(simplex = { a,b }, direction);
				}
			}
			else {
				if (is_same_dir(cross(ab, abc), a0)) {
					return GJKLine(simplex = { a, b }, direction);
				}
				else {
					if (is_same_dir(abc, a0)) {
						direction = abc;
					}
					else {
						simplex = { a, c, b };
						direction = abc;
					}
				}
			}
			return false;
		}

		bool GJKTetrahedron(Simplex& simplex, Vector& direction) {
			Vector a = simplex[0];
			Vector b = simplex[1];
			Vector c = simplex[2];
			Vector d = simplex[3];

			Vector ab = b - a;
			Vector ac = c - a;
			Vector ad = d - a;
			Vector a0 = -a;

			Vector abc = cross(ab, ac);
			Vector acd = cross(ac, ad);
			Vector adb = cross(ad, ab);

			if (is_same_dir(abc, a0)) {
				return GJKTriangle(simplex = { a, b, c }, direction);
			}

			if (is_same_dir(acd, a0)) {
				return GJKTriangle(simplex = { a, c, d }, direction);
			}

			if (is_same_dir(adb, a0)) {
				return GJKTriangle(simplex = { a, d, b }, direction);
			}

			return true;
		}
	
		bool GetNextSimplex(Simplex& simplex, Vector& direction){
			switch (simplex.Size()) {
				case 2: return GJKLine(simplex, direction);
				case 3: return GJKTriangle(simplex, direction);
				case 4: return GJKTetrahedron(simplex, direction);
			}

			return false;
		}


		bool GJK(const Collider* A, const Collider* B) {
			Vector support = GetSupport(A, B, Physics::Units::RIGHT);

			Simplex simplex;
			simplex.Push(support);

			Vector direction = support;
			while (true) {
				support = GetSupport(A, B, direction);
				if (dot(support, direction) <= 0)
					return false;

				simplex.Push(support);
				if (GetNextSimplex(simplex, direction)) {
					return true;
				}
			}
		}
	}
}

#endif