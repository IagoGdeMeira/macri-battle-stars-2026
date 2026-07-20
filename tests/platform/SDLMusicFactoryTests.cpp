#include "platform/include/SDLMusicFactory/SDLMusicFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

class SDLMusicFactoryFixture
{
public:
    std::filesystem::path makeTempWavPath(const char* suffix) const
    {
        const auto ticks = SDL_GetTicks();
        return std::filesystem::temp_directory_path() / (std::string("mbs_music_factory_")
            + suffix + "_" + std::to_string(ticks) + ".wav");
    }

    void writeSilentWav(const std::filesystem::path& filePath) const
    {
        constexpr std::uint16_t channels = 1;
        constexpr std::uint32_t sampleRate = 22050;
        constexpr std::uint16_t bitsPerSample = 16;
        constexpr std::uint16_t bytesPerSample = bitsPerSample / 8;
        constexpr std::uint32_t sampleCount = 512;

        const std::uint32_t dataSize = sampleCount * channels * bytesPerSample;
        const std::uint32_t chunkSize = 36 + dataSize;
        const std::uint32_t byteRate = sampleRate * channels * bytesPerSample;
        const std::uint16_t blockAlign = channels * bytesPerSample;

        std::ofstream out(filePath, std::ios::binary);
        REQUIRE(out.good());

        out.write("RIFF", 4);
        out.write(reinterpret_cast<const char*>(&chunkSize), sizeof(chunkSize));
        out.write("WAVE", 4);

        out.write("fmt ", 4);
        const std::uint32_t fmtSize = 16;
        const std::uint16_t audioFormat = 1;
        out.write(reinterpret_cast<const char*>(&fmtSize), sizeof(fmtSize));
        out.write(reinterpret_cast<const char*>(&audioFormat), sizeof(audioFormat));
        out.write(reinterpret_cast<const char*>(&channels), sizeof(channels));
        out.write(reinterpret_cast<const char*>(&sampleRate), sizeof(sampleRate));
        out.write(reinterpret_cast<const char*>(&byteRate), sizeof(byteRate));
        out.write(reinterpret_cast<const char*>(&blockAlign), sizeof(blockAlign));
        out.write(reinterpret_cast<const char*>(&bitsPerSample), sizeof(bitsPerSample));

        out.write("data", 4);
        out.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));

        std::vector<std::int16_t> samples(sampleCount, 0);
        out.write(reinterpret_cast<const char*>(samples.data()), static_cast<std::streamsize>(dataSize));
    }

    void configureAudioDriverForCi() const
    {
        #if defined(__linux__)
                SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        #endif
    }

    bool initAudio() const
    {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) return false;
        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 512) < 0)
        {
            SDL_Quit();
            return false;
        }
        return true;
    }

    void shutdownAudio() const
    {
        Mix_CloseAudio();
        SDL_Quit();
    }
};

TEST_CASE_METHOD(SDLMusicFactoryFixture, "SDLMusicFactory returns nullptr for invalid path",
    "[integration][sdl_music_factory]"
) {
    this->configureAudioDriverForCi();
    if (!this->initAudio()) return;

    SDLMusicFactory factory;
    const auto music = factory.createMusic("this/path/does/not/exist.wav");

    REQUIRE(music == nullptr);

    this->shutdownAudio();
}

TEST_CASE_METHOD(SDLMusicFactoryFixture, "SDLMusicFactory loads a valid wav file",
    "[integration][sdl_music_factory]"
) {
    this->configureAudioDriverForCi();
    if (!this->initAudio()) return;

    const auto wavPath = this->makeTempWavPath("ok");
    this->writeSilentWav(wavPath);

    SDLMusicFactory factory;
    const auto music = factory.createMusic(wavPath.string());

    REQUIRE(music != nullptr);

    std::error_code ec;
    std::filesystem::remove(wavPath, ec);

    this->shutdownAudio();
}
