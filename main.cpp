#include <iostream>
#include <vector>
#include <cmath>

class Vector3 {
    public:
        double x, y, z;

        Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}

        //add two vectors
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        //subtract two vectors
        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        //multiply vector by scalar
        Vector3 operator*(double scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        //divide vector by scalar
        Vector3 operator/(double scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        //dot product of two vectors
        double dot(const Vector3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        //cross product of two vectors
        Vector3 cross(const Vector3& other) const {
            return Vector3(y * other.z - z * other.y,
                           z * other.x - x * other.z,
                           x * other.y - y * other.x);
        }
        
        //magnitude of vector
        double magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        //normalize vector
        Vector3 normalize() const {
            double mag = magnitude();
            if (mag < 1e-10){
                return Vector3(0.0, 0.0, 0.0);
            }
            return Vector3(x / mag, y / mag, z / mag);
        }        
};

// can be used for missile and/or target
class PointMass {
    public:
        Vector3 position;
        Vector3 velocity;
        Vector3 acceleration;

        PointMass(const Vector3& pos = Vector3(), const Vector3& vel = Vector3(), const Vector3& acc = Vector3())
            : position(pos), velocity(vel), acceleration(acc) {}

        //numerica integration
        void update(double dt) {
            position = position + velocity * dt;
            velocity = velocity + acceleration * dt;
        }
};

class Missile : public PointMass {
    public:
        double navigationGain;
        double maxAcceleration;
        double speed;

        Missile(const Vector3& pos, const Vector3& vel, double navGain = 3.0, double maxAcc = 100.0)
            :PointMass(pos, vel), navigationGain(navGain), maxAcceleration(maxAcc), speed(vel.magnitude()) {}

        void calculateGuidanceCommand(const PointMass& target) {
            Vector3 los = target.position - position;
            Vector3 closingVelocity = target.velocity - velocity;
            Vector3 losRate = los.cross(closingVelocity).cross(los) * (1.0/(los.magnitude()*los.magnitude()));
            Vector3 accCommand = losRate.cross(velocity.normalize()) * (navigationGain * closingVelocity.dot(los.normalize()));

            double accMagnitude = accCommand.magnitude();
            if (accMagnitude > maxAcceleration) {
                accCommand = accCommand * (maxAcceleration / accMagnitude);
            }
            acceleration = accCommand;
            velocity = velocity.normalize() * speed;
        }
};

class Target : public PointMass {
    public:
        Target(const Vector3& pos, const Vector3& vel)
            : PointMass(pos, vel) {}


        void maneuver(){
            double turnRate = 0.1;
            Vector3 direction = velocity.normalize();
            Vector3 perp = Vector3(-direction.y, direction.x, 0.0).normalize();
            acceleration = perp * (velocity.magnitude() * turnRate);
        }
};

class Simulation {
    private:
        Missile missile;
        Target target;
        double timeStep;
        double maxTime;
        double hitDistance;
    
    public:
        Simulation(const Missile& m, const Target& t, double dt = 0.01, double maxT = 100.0, double hitDist = 5.0)
            : missile(m), target(t), timeStep(dt), maxTime(maxT), hitDistance(hitDist) {}

        void run() {
            double time = 0.0;
            bool hit = false; //hacky but works

            std::cout << "Simulation started" << std::endl;

            while (time < maxTime && !hit) {
                Vector3 relPos = target.position - missile.position;
                double distance = relPos.magnitude();

                std::cout << time << ","
                          << missile.position.x << ","
                          << missile.position.y << ","
                          << missile.position.z << ","
                          << target.position.x << ","
                          << target.position.y << ","
                          << target.position.z << ","
                          << distance << std::endl;

                if (distance < hitDistance) {
                    hit = true;
                    std::cout << "Target hit at time: " << time << std::endl;
                    break;
                }

                missile.calculateGuidanceCommand(target);
                target.maneuver();
                missile.update(timeStep);
                target.update(timeStep);
                time += timeStep;
            }

            if (!hit) {
                std::cout << "Target escaped" << std::endl;
            }
        }
};

int main() {
    Vector3 missilePos(0.0, 0.0, 0.0);
    Vector3 missileVel(100.0, 0.0, 0.0);
    Missile missile(missilePos, missileVel, 3.0, 100.0);
    Vector3 targetPos(10000.0, 2000.0, 500.0);
    Vector3 targetVel(-20.0, 30.0, 0.0);
    Target target(targetPos, targetVel);

    Simulation sim(missile, target);
    sim.run();

    return 0;
}


