#include "Window.h"
#include <AL/al.h>
#include <AL/alc.h>

std::string takeName(std::string input);

namespace gll
{
    class Sounds
	{
	    static std::vector<std::string> devNames;
	    static ALCdevice                *device;
	    static ALCcontext               *context;
	    static ALboolean                g_bEAX;
	    static uint8_t                  devCnt;
	    std::map<std::string, ALuint*>  g_Buffers;
	    std::map<std::string, ALuint*>  source3d;
	    std::map<std::string, ALuint*>  source2d;
	    std::map<std::string, ALuint*>  sourceLop;
	    ALint                           srcStat;     // sources status test
	    ALfloat                         *listenerOri;

	    bool errors(ALenum, std::string);

    public:

        static std::string              defDecName;

	    Sounds();
	    ~Sounds();

        void update(ALfloat*, ALfloat*, ALfloat*);
        void addWave(std::string, ALuint&);
        void addSnd(std::string&);
        void playSnd(std::string&, std::string&, float = 1.0f, int8_t = 0, glm::vec3 = { 0.0f, 0.0f, 0.0f });
        void stopSnd(std::string&, int8_t);
        void setVolume(std::string&, float&, int8_t);

	};

}
