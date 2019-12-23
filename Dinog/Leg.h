#pragma once

#include <Vec3f.h>
#include <Quat.h>
#include <ServoEx.h>

class Leg
{
public:
    struct Config
    {
        static const int L1 = 28;
        static const int L2 = 55;
        static const int L3 = 80;

        int coxaPin;
        int femurPin;
        int tibiaPin;
        bool inverted;
        Vec3f offset;
        Quat rotation;

        int coxaTrim;
        int femurTrim;
        int tibiaTrim;
    };

    static Config& getConfig( int index );
    static void loadConfig();
    static void saveConfig();

    Leg();
    ~Leg();

    void init( const Config& config );

    void setPos( const Vec3f& value, bool force = false );
    const Vec3f& getPos() const;
    const Vec3f& getCenter();    

private:
    ServoEx m_coxa;
    ServoEx m_femur;
    ServoEx m_tibia;
    Vec3f m_position;
    const Config* m_config;
};
