#include <iostream>
#include <string>
#include <cctype>
#include <cmath>

#include "Source/PulssarCalculations.h"

void noteToFrequencyInput()//converts a note, octave, and detune amount into Hz
{
    std::string note;

    int octave;
    //octaves are an int, so decimal values are not supported

    const int noteCount = sizeof(noteLetter) / sizeof(noteLetter[0]);
    int detuneAmount;
    bool validNote = false;

    //while loop to make sure the note is valid
    while (!validNote)
    {
        std::cout << "Note:" << std::endl;
        std::cin >> note;

        if (note.length() == 0)
        {
            continue;
        }

        note[0] = (char)std::toupper((unsigned char)note[0]);

        //the first character must be a note from A-G
        if (note[0] < 'A' || note[0] > 'G')
        {
            std::cout << "Invalid Note! Use A - G notes!" << std::endl;
            continue;
        }

        //if the second character is a #, keep it as a sharp
        //otherwise, everything after the first character is ignored
        if (note.length() >= 2 && note[1] == '#')
        {
            note = note.substr(0, 2);
        }
        else
        {
            note = note.substr(0, 1);
        }

        for (int i = 0; i < noteCount; i++)
        {
            if (note == noteLetter[i])
            {
                validNote = true;
                break;
            }
        }

        if (!validNote)
        {
            std::cout << "Invalid Note! Use A - G notes!" << std::endl;
        }
    }

    //keeps asking for an octave if the user enters something that is not a whole number
    while (true)
    {
        std::string octaveInput;

        std::cout << "Octave" << std::endl;
        std::cin >> octaveInput;

        try
        {
            size_t charactersRead = 0;
            int parsedOctave = std::stoi(octaveInput, &charactersRead);

            //makes sure the entire input was a valid integer
            if (charactersRead == octaveInput.length())
            {
                octave = parsedOctave;
                break;
            }
        }
        catch (...)
        {
            //invalid input is handled below
        }

        std::cout << "Invalid Octave! Please enter a whole number." << std::endl;
    }

    //keeps asking for a detune factor if the user enters something that is not a whole number
    while (true)
    {
        std::string detuneInput;

        std::cout << "Detune Factor" << std::endl;
        std::cin >> detuneInput;

        try
        {
            size_t charactersRead = 0;
            int parsedDetune = std::stoi(detuneInput, &charactersRead);

            //makes sure the entire input was a valid integer
            if (charactersRead == detuneInput.length())
            {
                detuneAmount = parsedDetune;
                break;
            }
        }
        catch (...)
        {
            //invalid input is handled below
        }

        std::cout << "Invalid Detune Factor! Please enter a whole number." << std::endl;
    }

    ///detune factor is an int, so decimal values are not supported
    //detune factor above 100 or below -100 is clamped to 100 or -100

    if (detuneAmount > 100)
    {
        detuneAmount = 100;
    }

    if (detuneAmount < -100)
    {
        detuneAmount = -100;
    }

    std::cout << std::endl;
    std::cout << "Note Selected: " << note << std::endl;
    std::cout << "Octave Selected: " << octave << std::endl;
    std::cout << "Detuning: " << detuneAmount << std::endl;
    std::cout << std::endl;

    int noteHz = noteToFrequency(note, octave, detuneAmount);

    std::cout << "Note: " << note << octave << " " << detuneAmount << std::endl;
    std::cout << "Hz NoteSpacing: " << noteHz << std::endl;
}

void frequencyToNoteInput()//converts a frequency into the closest note
{
    double frequency;

    //keeps asking for a frequency if the user enters something that is not a valid number
    while (true)
    {
        std::string frequencyInput;

        std::cout << "Enter frequency in Hz: " << std::endl;
        std::cin >> frequencyInput;

        try
        {
            size_t charactersRead = 0;
            double parsedFrequency = std::stod(frequencyInput, &charactersRead);

            //makes sure the entire input was a valid number
            if (charactersRead == frequencyInput.length())
            {
                frequency = parsedFrequency;
                break;
            }
        }
        catch (...)
        {
            //invalid input is handled below
        }

        std::cout << "Invalid frequency! Please enter a number." << std::endl;
    }

    //just in case the user puts zero or negative Hz, since log2 cannot calculate those values
    if (!std::isfinite(frequency) || frequency <= 0)
    {
        std::cout << "Invalid frequency! Frequency must be greater than 0 Hz." << std::endl;
        return;
    }

    std::string note = frequencyToNote(frequency);

    std::cout << "Frequency: " << frequency << " Hz corresponds to Note: "
        << note << std::endl;
}

void bpmToHzInput()// converts BPM into Hz, with an option for subdivisions
{
    double bpm = 0.0;
    double metricHz = 0.0;
    double subdivisionHz = 0.0;
    std::string subdivision;

    //keeps asking for BPM if the user enters something that is not a valid number
    while (true)
    {
        std::string bpmInput;

        std::cout << "Enter bpm to convert to Hz " << std::endl;
        std::cin >> bpmInput;

        try
        {
            size_t charactersRead = 0;
            double parsedBpm = std::stod(bpmInput, &charactersRead);

            //makes sure the entire input was a valid number
            if (charactersRead == bpmInput.length())
            {
                bpm = parsedBpm;
                break;
            }
        }
        catch (...)
        {
            //invalid input is handled below
        }

        std::cout << "Invalid BPM! Please enter a number." << std::endl;
    }

    //just in case the user puts zero or negative BPM
    if (!std::isfinite(bpm) || bpm <= 0)
    {
        std::cout << "Invalid BPM! BPM must be greater than 0." << std::endl;
        return;
    }

    metricHz = bpmToHz(bpm);

    //keeps asking for a subdivision until a valid fraction is entered
    while (true)
    {
        std::cout << "Enter subdivision: " << std::endl;
        std::cin >> subdivision;

        size_t slashPosition = subdivision.find('/');

        //make sure the user actually entered a fraction with a slash
        if (slashPosition == std::string::npos)
        {
            std::cout << "Invalid subdivision! Use a fraction such as 1/4 or 1/8." << std::endl;
            continue;
        }

        //make sure there is something before and after the slash
        if (slashPosition == 0 || slashPosition == subdivision.length() - 1)
        {
            std::cout << "Invalid subdivision! Use a fraction such as 1/4 or 1/8." << std::endl;
            continue;
        }

        //make sure there is only one slash
        if (subdivision.find('/', slashPosition + 1) != std::string::npos)
        {
            std::cout << "Invalid subdivision! Use a fraction such as 1/4 or 1/8." << std::endl;
            continue;
        }

        try
        {
            std::string numeratorInput = subdivision.substr(0, slashPosition);
            std::string denominatorInput = subdivision.substr(slashPosition + 1);

            size_t numeratorCharactersRead = 0;
            size_t denominatorCharactersRead = 0;

            double numerator =
                std::stod(numeratorInput, &numeratorCharactersRead);

            double denominator =
                std::stod(denominatorInput, &denominatorCharactersRead);

            //make sure the entire numerator and denominator are valid numbers
            if (numeratorCharactersRead != numeratorInput.length()
                || denominatorCharactersRead != denominatorInput.length())
            {
                std::cout << "Invalid subdivision! Use a fraction such as 1/4 or 1/8." << std::endl;
                continue;
            }

            //just in case the user enters a zero or negative subdivision
            if (!std::isfinite(numerator) || !std::isfinite(denominator)
                || numerator <= 0 || denominator <= 0)
            {
                std::cout << "Invalid subdivision! Both values must be greater than 0." << std::endl;
                continue;
            }

            double subdivisionValue = numerator / denominator;

            subdivisionHz = metricHz * (0.25 / subdivisionValue);

            break;
        }
        catch (...)
        {
            //just in case the user puts something that cannot be converted into a number
            std::cout << "Invalid subdivision! Use a fraction such as 1/4 or 1/8." << std::endl;
        }
    }

    std::cout << "Hz: " << subdivisionHz << std::endl;

    //might make another method that occupies parts of bpmToHz for metric modulation
    //with time signatures and BPM
    //this could eventually allow Pulssar to calculate relationships between different
    //rhythmic divisions rather than simply converting BPM directly to Hz
}

void hzToBpmInput()// converts Hz into BPM, with an option for subdivisions
{
    double hz;

    //keeps asking for Hz if the user enters something that is not a valid number
    while (true)
    {
        std::string hzInput;

        std::cout << "Enter frequency in Hz: " << std::endl;
        std::cin >> hzInput;

        try
        {
            size_t charactersRead = 0;
            double parsedHz = std::stod(hzInput, &charactersRead);

            //makes sure the entire input was a valid number
            if (charactersRead == hzInput.length())
            {
                hz = parsedHz;
                break;
            }
        }
        catch (...)
        {
            //invalid input is handled below
        }

        std::cout << "Invalid Hz! Please enter a number." << std::endl;
    }

    //just in case the user puts zero or negative Hz
    if (!std::isfinite(hz) || hz <= 0)
    {
        std::cout << "Invalid Hz! Frequency must be greater than 0 Hz." << std::endl;
        return;
    }

    double bpm = hzToBpm(hz);

    std::cout << "Frequency: " << hz << " Hz corresponds to BPM: " << bpm << std::endl;

    //might need a subdivision option here eventually
    //since an Hz value could represent a quarter note, eighth note, half note, etc.
}

int main()
{
    int choice;
    char continueCalculation;

    do
    {
        //keeps asking for a conversion until a valid option is selected
        do
        {
            std::cout << "Enter what you want to do:" << std::endl;
            std::cout << "1 - Note to Frequency" << std::endl;
            std::cout << "2 - Frequency to Note" << std::endl;
            std::cout << "3 - BPM to Hz" << std::endl;
            std::cout << "4 - Hz to BPM" << std::endl;
            std::cout << std::endl;

            std::string choiceInput;

            std::cin >> choiceInput;

            //just in case the user enters something that is not a whole number
            try
            {
                size_t charactersRead = 0;
                int parsedChoice = std::stoi(choiceInput, &charactersRead);

                if (charactersRead == choiceInput.length())
                {
                    choice = parsedChoice;
                }
                else
                {
                    choice = 0;
                }
            }
            catch (...)
            {
                choice = 0;
            }

            switch (choice)
            {
            case 1:
                noteToFrequencyInput();
                break;

            case 2:
                frequencyToNoteInput();
                break;

            case 3:
                bpmToHzInput();
                break;

            case 4:
                hzToBpmInput();
                break;

            default:
                std::cout << "Invalid option! Please choose 1-4."
                    << std::endl;
                std::cout << std::endl;
                break;
            }

        } while (choice < 1 || choice > 4);

        //after a calculation is completed, ask if the user wants to perform another calculation
        //if Y is entered, the program loops back to the conversion menu
        //if N is entered, the program exits
        while (true)
        {
            std::cout << std::endl;
            std::cout << "Would you like to do another calculation (Y / N)? ";
            std::cin >> continueCalculation;

            continueCalculation = (char)std::toupper((unsigned char)continueCalculation);

            if (continueCalculation == 'Y' || continueCalculation == 'N')
            {
                break;
            }

            //just in case the user enters something other than Y or N
            std::cout << "Invalid option! Please enter Y or N." << std::endl;
        }

        //allows both uppercase and lowercase Y/y to continue the program

    } while (continueCalculation == 'Y');

    //program exits after the user chooses not to perform another calculation
    std::cout << "Goodbye!" << std::endl;

    return 0;
}