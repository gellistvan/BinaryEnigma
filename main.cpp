#include <iostream>

#include <array>

constexpr uint8_t  AlphabetLength = 3;
using Byte = uint8_t;
using Dial = std::array<Byte, AlphabetLength>;

template <uint8_t DIAL_COUNT, uint8_t DIAL_REPO_SIZE>
struct EnigmaConfig {
    using Config = std::array<Byte, DIAL_COUNT>;
    Config config;
    std::array<Dial, DIAL_REPO_SIZE> dials;
    std::array<Byte, DIAL_COUNT> chosen_dials;
};

template <uint8_t DIAL_COUNT, uint8_t DIAL_REPO_SIZE >
class Enigma
{
    using ConfigType = EnigmaConfig<DIAL_COUNT, DIAL_REPO_SIZE>;
    typename ConfigType::Config config;
    std::array<Dial, DIAL_COUNT> dials;
    std::array<Dial, DIAL_COUNT> decoding_dials;
    ConfigType starting_config;

public:
    Enigma(const ConfigType& p_config) : config(p_config.config), starting_config(p_config)
    {
        uint8_t index {0U};
        for (Byte dial_index : p_config.chosen_dials)
        {
            this->dials[index] = p_config.dials[dial_index];
            this->decoding_dials[index] = InvertDial(this->dials[index]);
            ++index;
        }
    }

    template<uint8_t INPUT_LENGTH>
    std::array<Byte, INPUT_LENGTH> Encode(const std::array<Byte, INPUT_LENGTH>& input)
    {
        Reset();
        std::array<Byte, INPUT_LENGTH> output;
        for (uint8_t index = 0U; index < INPUT_LENGTH; ++index)
        {
            output[index] = EncodeByte(input[index]);
        }
        return output;
    }

    template<uint8_t INPUT_LENGTH>
    std::array<Byte, INPUT_LENGTH> Decode(const std::array<Byte, INPUT_LENGTH>& input)
    {
        Reset();
        std::array<Byte, INPUT_LENGTH> output;
        for (uint8_t index = 0U; index < INPUT_LENGTH; ++index)
        {
            output[index] = DecodeByte(input[index]);
        }
        return output;
    }

private:
    Byte EncodeByte(Byte b)
    {
        Byte tmp {b};
        uint8_t index = 0U;
        for (const auto& dial : dials)
        {
            tmp = dial[(config[index] + tmp) % AlphabetLength];
            index++;
        }
        IncrementConfig();
        return tmp;
    }

    Byte DecodeByte(Byte b)
    {
        Byte temp {b};
        for (uint8_t index = DIAL_COUNT; index > 0U; --index)
        {
            temp = (AlphabetLength + decoding_dials[index - 1][temp] - config[index - 1]) % AlphabetLength;
        }

        IncrementConfig();
        return temp;
    }

    void IncrementConfig()
    {
        for(uint8_t index = DIAL_COUNT - 1; index < DIAL_COUNT; index--)
        {
            config[index] = (config[index]+1) % AlphabetLength;
            if (config[index])
            {
                break;
            }
        }
    }

    static Dial InvertDial(const Dial& dial)
    {
        Dial inverted_dial;
        for (uint8_t i = 0U; i < AlphabetLength; i++)
        {
            inverted_dial[dial[i]] = i;
        }
        return inverted_dial;
    }

    void Reset()
    {
        config = starting_config.config;
    }
};

template<uint8_t N>
void PrintArray(const std::array<Byte, N>& p_array, char* name) {
    std::cout << name << ": ";
    for(const auto& b : p_array)
    {
        std::cout << static_cast<int>(b) << " ";
    }
    std::cout <<std::endl;
}

int main() {
    constexpr uint8_t TestDialCount {3U};
    constexpr uint8_t TestDialRepoSize {5U};
    EnigmaConfig<TestDialCount, TestDialRepoSize> config;
    config.config = {2U, 1U, 2U};
    config.dials = {{
        {1U, 0U, 2U},
        {2U, 0U, 1U},
        {1U, 2U, 0U},
        {1U, 0U, 2U},
        {2U, 0U, 1U},
    }};
    config.chosen_dials = {0U, 1U, 2U};

    Enigma<TestDialCount, TestDialRepoSize> enigma {config};
    constexpr uint8_t TestInputLength {10U};
    std::array<Byte, TestInputLength> input = {0U, 1U, 1U, 0U, 2U, 2U, 0U, 0U, 2U, 1U};

    PrintArray<TestInputLength>(input, "input");

    auto output = enigma.Encode<TestInputLength>(input);
    PrintArray<TestInputLength>(output, "output");

    auto output2 = enigma.Decode<TestInputLength>(output);
    PrintArray<TestInputLength>(output2, "decoded output");

    return 0;
}
