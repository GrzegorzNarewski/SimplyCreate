#include "Waves.h"

namespace gll
{
    std::string takeName(std::string input)
    {
        std::string output = "";
        bool dotDetect = false;
        for (int i = input.size() - 1; i >= 0; i--)
        {
            if (!dotDetect)
            {
                if (input[i] == '.') dotDetect = true;
            }
            else if (input[i] != '/')
            {
                output.insert(output.begin(), input[i]);
            }
            else break;
        }
        if (output.size() < 0) std::cout<<"Stock::takeName: input incorrect"<<std::endl;
        return output;
    }

    std::vector<std::string> Sounds::devNames = std::vector<std::string>();
    ALCdevice *Sounds::device       = nullptr;
    ALCcontext *Sounds::context     = nullptr;
    ALboolean Sounds::g_bEAX        = false;
    std::string Sounds::defDecName  = "none";
    uint8_t Sounds::devCnt          = 0;
    Sounds::Sounds()
    {
        devCnt++;
        g_Buffers.clear();
        source3d.clear();
        source2d.clear();
        sourceLop.clear();

        if (device == nullptr)
        {
            if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
            {
                const ALCchar *devices;
                const ALCchar *defaultDeviceName;

                devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER); // Pass in NULL device handle to get list of devices
                // devices contains the device names, separated by NULL
                // and terminated by two consecutive NULLs.
                defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
                // defaultDeviceName contains the name of the default
                // device

                std::string tmpNames = devices;
                std::string tmpNam = "";
                for (uint16_t i = 0; i < tmpNames.size(); i++)
                {
                    if (tmpNames[i] == NULL)
                    {
                        devNames.push_back(tmpNam);
                    }
                    else tmpNam += tmpNames[i];
                }
                devNames.push_back(tmpNam);
                /*
                kn::show("Avilable sound devices:");
                for (uint16_t i = 0; i < devNames.size(); i++)
                {
                    kn::show(std::to_string(i) + ": ", false);
                    kn::show(devNames[i]);
                }
                */
                defDecName = defaultDeviceName;
                //kn::show( "Default sound device: ", false);
                //kn::show(defDecName);
                // Initialization
                device = alcOpenDevice(defaultDeviceName); // select the "preferred device"
                if (device)
                {
                    context = alcCreateContext(device, NULL);
                    alcMakeContextCurrent(context);
                }
                else
                {
                    device = nullptr;
                    std::cout<<"Could not find any of sound device!"<<std::endl;
                    return;
                }
                // Check for EAX 2.0 support
                g_bEAX = alIsExtensionPresent("EAX2.0");
                //if (g_bEAX) kn::show("EAX2.0");
                //else kn::show("No: EAX2.0");

                alDistanceModel(AL_LINEAR_DISTANCE);
            }
            else std::cout<<"Could not find any of sound device!"<<std::endl;
        }

        ALfloat listenerPos[] = { 1.0f, 0.0f, 0.0f };
        ALfloat listenerVel[] = { 0.0f, 0.0f, 0.0f };
        listenerOri = new ALfloat[6];
        listenerOri[0] = 1.0f;
        listenerOri[1] = 0.0f;
        listenerOri[2] = 0.0f;
        listenerOri[3] = 0.0f;
        listenerOri[4] = 0.0f;
        listenerOri[5] = 1.0f;
        // Position ...
        alGetError();
        alListenerfv(AL_POSITION, listenerPos);
        errors(alGetError(), "KN_ERROR: Sounds: listenerPos error: ");
        // Velocity ...
        alListenerfv(AL_VELOCITY, listenerVel);
        errors(alGetError(), "KN_ERROR: Sounds: listenerVel error: ");
        // Orientation ...
        alListenerfv(AL_ORIENTATION, listenerOri);
        errors(alGetError(), "KN_ERROR: Sounds: listenerOri error: ");
        // gain ...
        alListenerf(AL_GAIN, 1.0f);
        errors(alGetError(), "KN_ERROR: Sounds: AL_GAIN error: ");
    }

    void Sounds::update(ALfloat* camPos, ALfloat* fr_nt, ALfloat* up)
    {
        if (source3d.size() > 0)
        {
            alListenerfv(AL_POSITION, camPos);
            std::copy(fr_nt, fr_nt + 3, listenerOri);
            std::copy(up, up + 3, listenerOri + 3);
            alListenerfv(AL_ORIENTATION, listenerOri);

            for (auto const& [key, val] : source3d)
            {
                alGetSourcei(val[0], AL_SOURCE_STATE, &srcStat);
                if (srcStat != AL_PLAYING)
                {
                    alDeleteSources(1, val);
                    delete[] val;
                    source3d.erase(key);
                    if (source3d.size() == 0) break;
                }
            }
        }

        for (auto const& [key, val] : source2d)
        {
            alGetSourcei(val[0], AL_SOURCE_STATE, &srcStat);
            if (srcStat != AL_PLAYING)
            {
                alDeleteSources(1, val);
                delete[] val;
                source2d.erase(key);
                break;
            }
        }

        for (auto const& [key, val] : sourceLop)
        {
            alGetSourcei(val[0], AL_SOURCE_STATE, &srcStat);
            if (srcStat != AL_PLAYING)
            {
                alSourcePlay(val[0]);
            }
        }
    }

    void Sounds::addSnd(std::string& wave)
    {
        if (device != nullptr)
        {
            alGetError(); // clear error code
            if (g_Buffers[wave] != nullptr)
            {
                //kn::show("Sound buffer: " + wave + " alredy exist!");
                return;
            }
            g_Buffers[wave] = new ALuint[1];
            alGenBuffers(1, g_Buffers[wave]);
            errors(alGetError(), "KN_ERROR: addSnd: g_Buffers: ");
            addWave(wave, g_Buffers[wave][0]);
        }
        else std::cout<<"No sound device to play: " + wave<<std::endl;
    }

    void Sounds::playSnd(std::string& name, std::string& value, float volume, int8_t sndTyp, glm::vec3 position)
    {
        if (true)
        {
            if (g_Buffers[value] == nullptr)
            {
                std::cout<<"Could not find sound: " + value<<std::endl;
                g_Buffers.erase(value);
                return;
            }

            if (sndTyp == 1)
            {
                if (source3d[name] != nullptr)
                {
                    alSourceStop(source3d[name][0]);
                }
                else
                {
                    source3d[name] = new ALuint[1];
                    alGenSources(1, source3d[name]);
                    alSourcef(source3d[name][0], AL_GAIN, (100.0f) * volume);

                    alSourcef(source3d[name][0], AL_ROLLOFF_FACTOR, 1.0f);
                    alSourcef(source3d[name][0], AL_REFERENCE_DISTANCE, 3.0f);
                    alSourcef(source3d[name][0], AL_MAX_DISTANCE, 10.0f);
                }
                alSourcefv(source3d[name][0], AL_POSITION, &position[0]);
                alSourcei(source3d[name][0], AL_BUFFER, g_Buffers[value][0]);
                alSourcePlay(source3d[name][0]);
            }
            else if (sndTyp == 0)
            {
                if (source2d[name] != nullptr)
                {
                    alSourceStop(source2d[name][0]);
                }
                else
                {
                    source2d[name] = new ALuint[1];
                    alGenSources(1, source2d[name]);
                }
                alSourcef(source2d[name][0], AL_GAIN, (100.0f) * volume);
                alSourcei(source2d[name][0], AL_BUFFER, g_Buffers[value][0]);
                alSourcePlay(source2d[name][0]);
            }
            else if (sndTyp == 2)
            {
                if (sourceLop[name] != nullptr)
                {
                    alSourceStop(sourceLop[name][0]);
                }
                else
                {
                    sourceLop[name] = new ALuint[1];
                    alGenSources(1, sourceLop[name]);
                    alSourcef(sourceLop[name][0], AL_GAIN, (100.0f) * volume);
                }
                alSourcei(sourceLop[name][0], AL_BUFFER, g_Buffers[value][0]);
                alSourcePlay(sourceLop[name][0]);
            }
        }
    }

    void Sounds::setVolume(std::string& name, float& volume, int8_t sndTyp)
    {
         if (sndTyp == 0)
         {
             if (source2d[name] == nullptr)
             {
                 std::cout<<"No sound to change volume: " + name<<std::endl;
                 source2d.erase(name);
                 return;
             }
             alSourcef(source2d[name][0], AL_GAIN, (100.0f) * volume);
         }
        else if (sndTyp == 1)
        {
            if (source3d[name] == nullptr)
            {
                std::cout<<"No sound to change volume: " + name<<std::endl;
                source3d.erase(name);
                return;
            }
            alSourcef(source3d[name][0], AL_GAIN, (100.0f) * volume);
        }
        else if (sndTyp == 2)
        {
            if (sourceLop[name] == nullptr)
            {
                std::cout<<"No sound to change volume: " + name<<std::endl;
                sourceLop.erase(name);
                return;
            }
            alSourcef(sourceLop[name][0], AL_GAIN, (100.0f) * volume);
        }
    }

    void Sounds::stopSnd(std::string& name, int8_t sndTyp)
    {
        if (sndTyp == 1)
        {
            if (source3d[name] == nullptr)
            {
                std::cout<<"Could not find sound to stop: " + name<<std::endl;
                source3d.erase(name);
                return;
            }

            alSourceStop(source3d[name][0]);
            alDeleteSources(1, source3d[name]);
            delete[] source3d[name];
            source3d.erase(name);
            return;
        }
        else if (sndTyp == 0)
        {
            if (source2d[name] == nullptr)
            {
                std::cout<<"Could not find sound to stop: " + name<<std::endl;
                source2d.erase(name);
                return;
            }

            alSourceStop(source2d[name][0]);
            alDeleteSources(1, source2d[name]);
            delete[] source2d[name];
            source2d.erase(name);
            return;
        }
        else if (sndTyp == 2)
        {
            if (sourceLop[name] == nullptr)
            {
                std::cout<<"Could not find sound to stop: " + name<<std::endl;
                sourceLop.erase(name);
                return;
            }

            alSourceStop(sourceLop[name][0]);
            alDeleteSources(1, sourceLop[name]);
            delete[] sourceLop[name];
            sourceLop.erase(name);
            return;
        }
    }

    void Sounds::addWave(std::string path, ALuint& buff)
    {
        std::string newPath = "Resources/Sounds/" + path + ".snd";
        std::ifstream file(newPath, std::ios::binary);
        bool save = false;
        if (!file.good())
        {
            file.clear();
            newPath = "Resources/Sounds/wavs/" + path + ".wav";
            file.open(newPath, std::ios::binary);
            save = true;
            if (!file.good())
            {
                std::cout<<"Could not open: " + path<<std::endl;
                file.close();
                return;
            }
        }
        char		id[4];
        uint32_t	ChunkSize;
        char		head[4];
        char		fmt[4];
        uint32_t	fmtSize;
        uint16_t	audioFormat;
        uint16_t	channels;
        uint32_t	frequency;
        uint32_t	byteRate;
        uint16_t	blockAlign;
        uint16_t	bitPerSample;
        char		Subchunk2ID[4];
        uint32_t	dataSize;
        file.read((char *)id, 4);
        file.read((char *)&ChunkSize, 4);
        file.read((char *)head, 4);
        file.read((char *)fmt, 4);
        file.read((char *)&fmtSize, 4);
        file.read((char *)&audioFormat, 2);
        file.read((char *)&channels, 2);
        file.read((char *)&frequency, 4);
        file.read((char *)&byteRate, 4);
        file.read((char *)&blockAlign, 2);
        file.read((char *)&bitPerSample, 2);
        file.read((char *)Subchunk2ID, 4);
        file.read((char *)&dataSize, 4);
        void *data;
        data = new char[dataSize];
        file.read((char*)data, dataSize);
        file.close();

        ALenum error;
        alGetError();
        if (channels == 1)
        {
            if (bitPerSample == 8) alBufferData(buff, AL_FORMAT_MONO8, data, dataSize, frequency);
            else alBufferData(buff, AL_FORMAT_MONO16, data, dataSize, frequency);
        }
        else
        {
            if (bitPerSample == 8) alBufferData(buff, AL_FORMAT_STEREO8, data, dataSize, frequency);
            else alBufferData(buff, AL_FORMAT_STEREO16, data, dataSize, frequency);
        }
        if (alGetError() != AL_NO_ERROR)
        {
            std::cout<<"alBufferData buffer error"<<std::endl;
            alDeleteBuffers(1, &buff);
            delete[] data;
            return;
        }

        if (save)
        {
            newPath = "Resources/Sounds/" + path + ".snd";
            std::ofstream file2(newPath, std::ios::binary);
            if (file2.good())
            {
                file2.write((char *)id, 4);
                file2.write((char *)&ChunkSize, 4);
                file2.write((char *)head, 4);
                file2.write((char *)fmt, 4);
                file2.write((char *)&fmtSize, 4);
                file2.write((char *)&audioFormat, 2);
                file2.write((char *)&channels, 2);
                file2.write((char *)&frequency, 4);
                file2.write((char *)&byteRate, 4);
                file2.write((char *)&blockAlign, 2);
                file2.write((char *)&bitPerSample, 2);
                file2.write((char *)Subchunk2ID, 4);
                file2.write((char *)&dataSize, 4);

                file2.write((char*)data, dataSize);
            }
            file2.close();
            std::cout<<newPath + ": saved"<<std::endl;
        }
        delete[] data;
    }

    bool Sounds::errors(ALenum err, std::string txt)
    {
             if (0xA001 == err) { std::cout<<txt + "AL_INVALID_NAME"<<std::endl;        return false; }
        else if (0xA002 == err) { std::cout<<txt + "AL_INVALID_ENUM"<<std::endl;        return false; }
        else if (0xA003 == err) { std::cout<<txt + "AL_INVALID_VALUE"<<std::endl;       return false; }
        else if (0xA004 == err) { std::cout<<txt + "AL_INVALID_OPERATION"<<std::endl;   return false; }
        else if (0xA005 == err) { std::cout<<txt + "AL_OUT_OF_MEMORY"<<std::endl;       return false; }
        return true;
    }

    Sounds::~Sounds()
    {
        alGetError();

        for (auto const& [key, val] : source2d)
        {
            alDeleteSources(1, val);
            errors(alGetError(), "KN_ERROR: ~Sounds: source2d: " + key + ": ");
            delete[] val;
        }
        source2d.clear();

        for (auto const& [key, val] : source3d)
        {
            alDeleteSources(1, val);
            errors(alGetError(), "KN_ERROR: ~Sounds: source2d: " + key + ": ");
            delete[] val;
        }
        source3d.clear();

        for (auto const& [key, val] : sourceLop)
        {
            alDeleteSources(1, val);
            errors(alGetError(), "KN_ERROR: ~Sounds: source2d: " + key + ": ");
            delete[] val;
        }
        sourceLop.clear();

        for (auto const& [key, val] : g_Buffers)
        {
            alDeleteBuffers(1, val);
            errors(alGetError(), "KN_ERROR: ~Sounds: g_Buffers: " + key + ": ");
            delete[] val;
        }
        g_Buffers.clear();

        delete[] listenerOri;
        devCnt--;
        if (devCnt > 0 || device == nullptr) return;
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        device = nullptr;
        std::cout<<defDecName + ": close!"<<std::endl;
    }
}
