#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include "../../math/Vector.h"
#include "../../math/Integration.h"

#include "../Constants.h"
#include <cmath>
#include <iostream>

#include "../Object.h"

namespace Physics{

    using namespace Physics::Units;

	// Abstraction for a particle / point mass
    class Point : public Object{
	protected:
        PMath::Vector rotation;
		
        float damping_coefficient = DEFAULT_DAMPING_COEFFICIENT;
		unsigned long long int id;

        virtual void OnUpdate(){

        }

    public:

        Point(BodyType type = BodyType::DYNAMIC, unsigned long long int id = 0){
            this->type = type;;
			this->id = id;
        }

        virtual ~Point(){

        }

        void Update(float delta) override{

            old_transform.position = transform.position;
            old_transform.velocity = transform.velocity;
            old_transform.acceleration = transform.acceleration;

			
            auto result = PMath::Verlet(transform.position, transform.velocity, old_transform.acceleration, transform.acceleration, delta);
            if (type == BodyType::DYNAMIC){
                transform.acceleration = result[2];
                transform.velocity = result[1];
            }
            if (type != BodyType::STATIC){
                transform.position = result[0];
            }

            // Apply damping effect to compensate for numerical errors
            transform.velocity *= (float) pow(damping_coefficient, delta);

            OnUpdate();
        }

        void OnFrameEnd(float alpha){
			PMath::Vector tmp_position = transform.position;
			PMath::Vector tmp_velocity = transform.velocity;
			PMath::Vector tmp_acceleration = transform.acceleration;

            transform.position = alpha* transform.position + (1.0f-alpha)*old_transform.position;
			transform.velocity = alpha* transform.velocity + (1.0f-alpha)*old_transform.velocity;
			transform.acceleration = alpha* transform.acceleration + (1.0f-alpha)*old_transform.acceleration;

			old_transform.position = tmp_position;
			old_transform.velocity = tmp_velocity;
			old_transform.acceleration = tmp_acceleration;

            OnUpdate();
        }

        /* Getters and Setters */
        PMath::Vector GetPosition() const{
            return transform.position;
        }

        PMath::Vector GetScaledPosition() const{
            // Specifically for rendering, so as to scale positions correctly.
            return transform.position / M;
        }


		unsigned long long int GetId() override{
			return id;
		}

		void SetId(unsigned long long int id) override{
			this->id = id;
		}

    };


}

#endif // POINT_H_INCLUDED
