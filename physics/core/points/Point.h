#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include "../../math/Vector.h"
#include "../../math/Integration.h"

#include "../Constants.h"

namespace Physics{

    using namespace Physics::Units;
    enum BodyType{
        STATIC,
        KINEMATIC,
        DYNAMIC
    };

    class Point {
        protected:
            BodyType type;

            PMath::Vector old_position;
            PMath::Vector old_velocity;
            PMath::Vector old_acceleration;

            PMath::Vector position;
            PMath::Vector velocity;
            PMath::Vector acceleration;

            PMath::Vector rotation;
            double mass = KG;

            double damping_coefficient = DEFAULT_DAMPING_COEFFICIENT;

            virtual void OnUpdate(){

            }

        public:

            Point(BodyType type = BodyType::KINEMATIC){
                this->type = type;;
            }

            virtual ~Point(){

            }

            void Update(double delta){

                old_position = position;
                old_velocity = velocity;
                old_acceleration = acceleration;

                std::vector<PMath::Vector> result = PMath::Verlet(position, velocity, old_acceleration, acceleration, delta);

                if (type == BodyType::DYNAMIC){
                    acceleration = result[2];
                    velocity = result[1];
                }
                if (type != BodyType::STATIC){
                    position = result[0];
                }

                // Apply damping effect to compensate for numerical errors
                velocity *= std::pow(damping_coefficient, delta);

                OnUpdate();
            }

            void Interpolate(double alpha){
                position = alpha*position + (1.0-alpha)*old_position;
                velocity = alpha*velocity + (1.0-alpha)*old_velocity;
                acceleration = alpha*acceleration + (1.0-alpha)*old_acceleration;
                OnUpdate();
            }

            /* Other Methods */
            void ApplyForce(PMath::Vector force){
                this->acceleration += force / mass;
            }

            /* Getters and Setters */
            PMath::Vector GetPosition() const{
                return position;
            }

            PMath::Vector GetScaledPosition() const{
                // Specifically for rendering, so as to scale positions correctly.
                return position * M;
            }

            void SetPosition(const PMath::Vector& position){
                this->position = position;
            }

            PMath::Vector GetVelocity() const{
                return velocity;
            }

            void SetVelocity(PMath::Vector& velocity){
                this->velocity = velocity;
            }

            PMath::Vector GetAcceleration() const{
                return acceleration;
            }

            void SetAcceleration(PMath::Vector& acceleration){
                this->acceleration = acceleration;
            }

            double GetMass() const{
                return mass;
            }

            void SetMass(double mass){
                if (mass != 0)
                    this->mass = mass * KG;
            }

            void SetInverseMass(double mass){
                if (mass == 0)
                    this->mass = INT_MAX;
                else
                    this->mass = (1.0 / mass) * KG;
            }


            void SetType(BodyType type){
                this->type = type;
            }
    };


}

#endif // POINT_H_INCLUDED
