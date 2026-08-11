#ifndef sound_h
#define sound_h

class Sound
{
public:
    virtual ~Sound() = default;

    virtual void play(bool loop = false) = 0;
    virtual void stop() = 0;
    virtual void setVolume(int volume) = 0;
    virtual int getVolume() const = 0;
    virtual bool isPlaying() const = 0;

protected:
    static int scaleVolume(int userVolume) { (void)userVolume; return 0; }
};

#endif // sound_h
