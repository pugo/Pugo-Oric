// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/assign.hpp>

#include "tape_tap.hpp"


TapeTap::TapeTap(MOS6522& a_Via, const std::string& a_Path) :
    m_Via(a_Via),
    m_Path(a_Path),
    m_Size(0),
    m_BodyStart(0),
    m_RunMotor(false),
    m_Delay(0),
    m_DuplicateBytes(0),
    m_TapePos(0),
    m_BitCount(0),
    m_CurrentBit(0),
    m_Parity(1),
    m_TapeCyclesCounter(2),
    m_TapePulse(0)
{
}


TapeTap::~TapeTap()
{
//    delete[] memblock;
}


void TapeTap::Reset()
{
    m_RunMotor = false;
    m_Delay = 0;
    m_DuplicateBytes = 0;
    m_TapePos = 0;
    m_BitCount = 0;
    m_CurrentBit = 0;
    m_TapeCyclesCounter = 2;
    m_TapePulse = 0;
    m_Parity = 1;
}


bool TapeTap::Init()
{
    Reset();
    std::cout << "Reading TAP file '" << m_Path << "':" << std::endl;

    std::ifstream file (m_Path, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        m_Size = file.tellg();
        m_Data = new uint8_t[m_Size];
        file.seekg (0, std::ios::beg);
        file.read (reinterpret_cast<char*>(m_Data), m_Size);
        file.close();

        std::cout << "the entire file content is in memory" << std::endl;

        if (!ReadHeader()) {
            std::cout << "Not a valid TAP file." << std::endl;
            return false;
        }
    }
    else {
        std::cout << "Unable to open file";
        return false;
    }

    return true;
}

bool TapeTap::ReadHeader()
{
    size_t i;

    for (i = 0; m_Data[m_TapePos + i] == 0x16; i++)
    {
        if (i >= m_Size) {
            return false;
        }
    }

    std::cout << "Tape: found " << i << " sync bytes (0x16)" << std::endl;
    if (i < 3) {
        std::cout << "Tape: too few sync bytes, failing." << std::endl;
        return false;
    }

    if (m_Data[m_TapePos + i] != 0x24) {
        std::cout << "Tape: missing end of sync bytes (0x24), failing." << std::endl;
        return false;
    }

    i++;

    if (i + 9 >= m_Size) {
        std::cout << "Tape: too short (no specs and addresses)." << std::endl;
        return false;
    }

    // Skip reserved bytes.
    i += 2;

    switch(m_Data[m_TapePos + i])
    {
        case 0x00:
            std::cout << "Tape: file is BASIC." << std::endl;
            break;
        case 0x80:
            std::cout << "Tape: file is machine code." << std::endl;
            break;
        default:
            std::cout << "Tape: file is unknown." << std::endl;
            break;
    }
    i++;

    switch(m_Data[m_TapePos + i])
    {
        case 0x80:
            std::cout << "Tape: RUN automatically as BASIC." << std::endl;
            break;
        case 0xc7:
            std::cout << "Tape: RUN automatically as machine code." << std::endl;
            break;
        default:
            std::cout << "Tape: Don't run automatically." << std::endl;
            break;
    }
    i++;

    uint16_t start_address;
    uint16_t end_address;

    end_address = m_Data[m_TapePos + i] << 8 | m_Data[m_TapePos + i + 1];
    i += 2;

    start_address = m_Data[m_TapePos + i] << 8 | m_Data[m_TapePos + i + 1];
    i += 2;

    std::cout << "Tape: start address: " << std::hex << (int)start_address << std::endl;
    std::cout << "Tape:   End address: " << std::hex << (int)end_address << std::endl;

    // Skip one reserved byte.
    i++;

    std::string name;
    while (m_Data[m_TapePos + i] != 0x00)
    {
        name += m_Data[m_TapePos + i];
        if (i >= m_Size) {
            return false;
        }
        i++;
    }
    std::cout << "Tape: file name = " << name << std::endl;

    // Store where body starts, to allow delay after header.
    m_BodyStart = i + 1;
    std::cout << "Tape: Body starts at: " << (int)m_BodyStart << std::endl;

    m_DuplicateBytes = 80;

    return true;
}


void TapeTap::PrintStat()
{
    std::cout << "Current Tape pos: " << m_TapePos << std::endl;
}


void TapeTap::SetMotor(bool a_MotorOn)
{
    if (a_MotorOn == m_RunMotor) {
        return;
    }

    m_RunMotor = a_MotorOn;

    if (!m_RunMotor)
    {
        if (m_BitCount > 0) {
            m_TapePos++;
            m_BitCount = 0;
        }
    }

    else {
        ReadHeader();
    }
}


short TapeTap::Exec(uint8_t a_Cycles)
{
    if (a_Cycles == 0) {
        return 0;
    }

    if (!m_RunMotor) {
        return 0;
    }

    if (m_TapeCyclesCounter > a_Cycles) {
        m_TapeCyclesCounter -= a_Cycles;

        if (m_Delay > 0) {
            m_Delay -= a_Cycles;
            if (m_Delay < 0) {
                m_Delay = 1;
            }
            std::cout << "Delay: " << m_Delay << std::endl;
        }
        return a_Cycles;
    }

    m_TapePulse ^= 0x01;
    m_Via.WriteCB1(m_TapePulse);

    if (m_Delay > 0) {
        m_Delay -= a_Cycles;

        if( m_Delay <= 0)
        {
            if (m_TapePulse) {
                m_Delay = 1;
            }
            else {
                m_Delay = 0;
            }
        }

        m_TapeCyclesCounter = Pulse_1;
        return a_Cycles;
    }

    if (m_TapePulse) {
        // Start of bit, pulse up.
        m_CurrentBit = CurrentBit();
//        m_TapeCyclesCounter = Pulse_1;
        m_TapeCyclesCounter = m_CurrentBit ? Pulse_1 : Pulse_0;
    }
    else {
        // Second part of bit, differently long down period.
        m_TapeCyclesCounter = m_CurrentBit ? Pulse_1 : Pulse_0;
    }

    return a_Cycles;
}


uint8_t TapeTap::CurrentBit()
{
    uint8_t current_byte = m_Data[m_TapePos];

    uint8_t result;
    switch (m_BitCount) {
        case 0:
//            std::cout << "****** [" << m_TapePos << "] Start byte (1)" << std::endl;
            result = 1;
            m_BitCount++;
            break;
        case 1:
//            std::cout << "****** [" << m_TapePos << "] Start bit (0)" << std::endl;
            // Start bit (always 0).
            result = 0;
            m_Parity = 1;
            m_BitCount++;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            result = (current_byte & (0x01 << m_BitCount-2)) ? 1 : 0;
            m_Parity ^= result;
//            std::cout << "****** [" << m_TapePos << "] Data bit " << m_BitCount - 2 << " : bit = " << (int)result << ", parity = " << (int)m_Parity << std::endl;
            m_BitCount++;
            break;
        case 10:
            // Parity.
//            std::cout << "******[" << m_TapePos << "] Parity bit: " << (int)m_Parity << std::endl;
            result = m_Parity;
            m_BitCount++;
            break;
        case 11:
        case 12:
        case 13:
            // 3 stop bits (always 1).
            result = 1;
//            std::cout << "******[" << m_TapePos << "] Stop bit (" << m_BitCount - 10 << ")" << std::endl;

            m_BitCount = (m_BitCount + 1) % 14;

            if (m_BitCount == 0) {
                if (m_DuplicateBytes > 0) {
                    m_DuplicateBytes--;
                }
                else {
                    m_TapePos++;
                }

//                std::cout << "****** ---------- > Tape pos = " << m_TapePos << ", body start: " << m_BodyStart << std::endl;
                if (m_TapePos == m_BodyStart) {
//                    std::cout << "****** ---------- > Tape pos == body start, adding delay!" << std::endl;
                    m_Delay = 1281;
                }
            }
            break;
    }

    return result;
}


