#ifndef music_h
#define music_h

class Music
{
public:
    virtual ~Music() = default;

    virtual void play(bool loop = false) = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void setVolume(int volume) = 0;
    virtual int getVolume() const = 0;
    virtual bool isPlaying() const = 0;
};

#endif // music_h
