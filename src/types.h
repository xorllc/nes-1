#pragma once
#ifndef TYPES_H
#define TYPES_H

#define MAX_TITLE_LENGTH 128
#define TRAINER_SIZE 512

#define PPU_SCREEN_WIDTH 256
#define PPU_SCREEN_HEIGHT 240

#define APU_BUFFER_LENGTH 1024

struct Memory
{
    b32 created;
    u32 length;
    u8 *bytes;
};

enum MirrorType
{
    MIRROR_HORIZONTAL,
    MIRROR_VERTICAL,
    MIRROR_FOUR,
};

struct CartridgeHeader
{
    // Constant $4E $45 $53 $1A ("NES" followed by $1A)
    u8 nesStr[4];

    // Size of PRG ROM in 16 KB units. Number of 16kB ROM banks.
    u8 prgROMSize;

    // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM). Number of 8kB VROM banks.
    u8 chrROMSize;

    // Flags 6        
    // bit 0        1 for vertical mirroring, 0 for horizontal mirroring.
    // bit 1        1 for battery - backed RAM at $6000 - $7FFF.
    // bit 2        1 for a 512 - byte trainer at $7000 - $71FF.
    // bit 3        1 for a four - screen VRAM layout.
    // bit 4 - 7    Four lower bits of ROM Mapper Type.
    u8 flags6;

    // Flags 7
    // bit 0       1 for VS - System cartridges.
    // bit 1 - 3   Reserved, must be zeroes!
    // bit 4 - 7   Four higher bits of ROM Mapper Type.
    u8 flags7;

    // Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
    // Number of 8kB RAM banks. 
    // For compatibility with the previous versions of the.NES format, assume 1x8kB RAM page when this byte is zero.
    u8 prgRAMSize;

    // Flags 9
    // bit 0       1 for PAL cartridges, otherwise assume NTSC.
    // bit 1 - 7   Reserved, must be zeroes!
    u8 flags9;

    // Reserved, must be zeroes!
    u8 flags10;
    u8 reserved[5];
};

struct Cartridge
{
    MirrorType mirrorType;
    b32 hasBatteryPack;
    u8 mapper;
    u8 prgRAMSize;

    u8 title[MAX_TITLE_LENGTH];

    b32 hasTrainer;
    u8 trainer[TRAINER_SIZE];

    u32 prgBanks;
    u32 prgSizeInBytes;
    u8 *prg;

    u32 chrBanks;
    u32 chrSizeInBytes;
    u8 *chr;
};

enum CPUAddressingMode
{
    AM_NON = 0,     // None            

    AM_IMM = 1,     // Immediate                #$00

    AM_ABS = 2,     // Absolute                 $0000
    AM_ABX = 3,     //                          $0000, X
    AM_ABY = 4,     //                          $0000, Y

    AM_ZPA = 5,     // Zero-Page-Absolute       $00
    AM_ZPX = 6,     // Zero-Page-Indexed        $00, X
    AM_ZPY = 7,     // Zero-Page-Indexed        $00, Y

    AM_IND = 8,     // Indirect                 ($0000)
    AM_IZX = 9,     // Pre-Indexed-Indirect     ($00, X)
    AM_IZY = 10,    // Post-Indexed-Indirect    ($00), Y

    AM_IMP = 11,     // Implied                  
    AM_ACC = 12,     // Accumulator

    AM_REL = 13     // Relative                 $0000
};

enum CPURegister
{
    CPU_NR = 0,     // No register
    CPU_AR = 1,     // Register accumulator
    CPU_XR = 2,     // Register x
    CPU_YR = 3,     // register y
    CPU_ST = 4,     // Register status
    CPU_PC = 5,     // Register program counter
    CPU_SP = 6      // Register stack pointer
};

enum CPUInstructionSet
{
    CPU_ADC, //  Add Memory to Accumulator with Carry
    CPU_AND, //  "AND" Memory with Accumulator
    CPU_ASL, //  Shift Left One Bit(Memory or Accumulator)

    CPU_BCC, //  Branch on Carry Clear
    CPU_BCS, //  Branch on Carry Set
    CPU_BEQ, //  Branch on Result Zero
    CPU_BIT, //  Test Bits in Memory with Accumulator
    CPU_BMI, //  Branch on Result Minus
    CPU_BNE, //  Branch on Result not Zero
    CPU_BPL, //  Branch on Result Plus
    CPU_BRK, //  Force Break
    CPU_BVC, //  Branch on Overflow Clear
    CPU_BVS, //  Branch on Overflow Set

    CPU_CLC, //  Clear Carry Flag
    CPU_CLD, //  Clear Decimal Mode
    CPU_CLI, //  Clear interrupt Disable Bit
    CPU_CLV, //  Clear Overflow Flag
    CPU_CMP, //  Compare Memory and Accumulator
    CPU_CPX, //  Compare Memory and Index X
    CPU_CPY, //  Compare Memory and Index Y

    CPU_DEC, //  Decrement Memory by One
    CPU_DEX, //  Decrement Index X by One
    CPU_DEY, //  Decrement Index Y by One

    CPU_EOR, //  "Exclusive-Or" Memory with Accumulator

    CPU_INC, //  Increment Memory by One
    CPU_INX, //  Increment Index X by One
    CPU_INY, //  Increment Index Y by One

    CPU_JMP, //  Jump to New Location
    CPU_JSR, //  Jump to New Location Saving Return Address 

    CPU_LDA, //  Load Accumulator with Memory 
    CPU_LDX, //  Load Index X with Memory 
    CPU_LDY, //  Load Index Y with Memory 
    CPU_LSR, //  Shift Right One Bit(Memory or Accumulator) 

    CPU_NOP, //  No Operation 

    CPU_ORA, //  "OR" Memory with Accumulator 

    CPU_PHA, //  Push Accumulator on Stack 
    CPU_PHP, //  Push Processor Status on Stack 
    CPU_PLA, //  Pull Accumulator from Stack 
    CPU_PLP, //  Pull Processor Status from Stack 

    CPU_ROL, //  Rotate One Bit Left(Memory or Accumulator) 
    CPU_ROR, //  Rotate One Bit Right(Memory or Accumulator) 
    CPU_RTI, //  Return from Interrupt 
    CPU_RTS, //  Return from Subroutine 

    CPU_SBC, //  Subtract Memory from Accumulator with Borrow 
    CPU_SEC, //  Set Carry Flag 
    CPU_SED, //  Set Decimal Mode 
    CPU_SEI, //  Set Interrupt Disable Status 
    CPU_STA, //  Store Accumulator in Memory 
    CPU_STX, //  Store Index X in Memory 
    CPU_STY, //  Store Index Y in Memory 

    CPU_TAX, //  Transfer Accumulator to Index X 
    CPU_TAY, //  Transfer Accumulator to Index Y 
    CPU_TSX, //  Transfer Stack Pointer to Index X 
    CPU_TXA, //  Transfer Index X to Accumulator 
    CPU_TXS, //  Transfer Index X to Stack Pointer 
    CPU_TYA, //  Transfer Index Y to Accumulator

    CPU_SLO,
    CPU_ANC,
    CPU_RLA,
    CPU_SRE,
    CPU_ALR,
    CPU_RRA,
    CPU_ARR,
    CPU_SAX,
    CPU_XAA,
    CPU_AHX,
    CPU_TAS,
    CPU_SHY,
    CPU_SHX,
    CPU_LAX,
    CPU_LAS,
    CPU_DCP,
    CPU_AXS,
    CPU_ISC,

    CPU_KIL, // Crash

    CPU_FEX, //  Future Expansion
};

struct CPUInstruction
{
    u8 opcode;
    CPUInstructionSet instruction;
    CPUAddressingMode addressingMode;
    CPURegister cpuRegister;
    u8 bytesCount;
    u8 cyclesCount;
    b32 pageCycles;
};

enum CPUInterrupt
{
    CPU_INTERRUPT_NON = 0,
    CPU_INTERRUPT_NMI = 1,
    CPU_INTERRUPT_IRQ = 2,
    CPU_INTERRUPT_RES = 3
};

struct CPU
{
    u8 a;                       // accumulator register
    u8 x;                       // x register
    u8 y;                       // y register
    u8 sp;                      // stack pointer
    u8 p;                       // status register
    u16 pc;                     // program counter
    u64 cycles;                 // number of cycles
    u32 waitCycles;             // number of cycles to stall
    CPUInterrupt interrupt;     // interrupt type to perform
};

struct PPU
{
    u32 cycle;                  // PPU current scanline cycles count, 
                                // 341 total, 
                                // (0) idle, 
                                // (1-256) fetch background, 
                                // (257-320) fetch sprites, 
                                // (321-336) pre-fetch first two tiles of next scanline, 
                                // (337-340) dummy fetch
    s32 scanline;               // PPU current scanline, 
                                // 262 total, 
                                // (-1, 261) pre-render, 
                                // (0-239) visible, 
                                // (240) post, 
                                // (241-260) vblank
    u64 frameCount;             // PPU frame counter
    u64 totalCycles;            // PPU total cycles count

    u8 control;                 // PPU Control Register 1 (0x2000)
    u8 mask;                    // PPU Mask Register 2 (0x2001)
    u8 status;                  // PPU Status Register (0x2002)
    u8 oamAddress;              // OAM Read/Write Address (0x2003)
    u8 oamData;                 // OAM Data Read/Write (0x2004)
    u8 scroll;                  // Scroll Position (0x2005)
    u8 address;                 // PPU Read/Write address (0x2006)
    u8 data;                    // PPU Data Read/Write (0x2007)
    u8 oamDma;                  // OAM DMA high address (0x4014)

    // NMI flags
    b32 suppressNmi;
    /*b32 outputNmi;
    u8 delayNmi;*/

    // PPU registers
    u16 v;  // current vram address (15 bit)
    u16 t;  // temporary vram address (15 bit)
    u8 x;   // fine x scroll (3 bit)
    u8 w;   // write toggle (1 bit)

    // background temporary variables
    u8 nameTableByte;
    u8 attrTableByte;
    u8 lowTileByte;
    u8 highTileByte;
    u64 tileData;

    // sprite temporary variables
    u8 spriteCount;
};

struct APU
{
    // $4000 - $4007
    struct Pulse
    {
        b32 globalEnabled;
        
        b32 enabled;
        u8 channel;
        
        b32 lengthEnabled;
        u8 lengthValue;
        
        u16 timerPeriod;
        u16 timerValue;
        
        u8 dutyMode;
        u8 dutyValue;
        
        b32 sweepReload;
        b32 sweepEnabled;
        b32 sweepNegate;
        u8 sweepShift;
        u8 sweepPeriod;
        u8 sweepValue;
        
        b32 envelopeEnabled;
        b32 envelopeLoop;
        b32 envelopeStart;
        u8 envelopePeriod;
        u8 envelopeValue;
        u8 envelopeVolume;
        
        u8 constantVolume;

        s32 bufferIndex;
        s16 buffer[APU_BUFFER_LENGTH];
    } pulse1, pulse2;

    struct Triangle
    {
        b32 globalEnabled;

        b32 enabled;

        b32 linearEnabled;
        u8 linearPeriod;
        u8 linearValue;
        b32 linearReload;

        b32 lengthEnabled;
        u8 lengthValue;

        u16 timerPeriod;
        u16 timerValue;

        u8 tableIndex;

        s32 bufferIndex;
        s16 buffer[APU_BUFFER_LENGTH];
    } triangle;

    struct Noise
    {
        b32 globalEnabled;

        b32 enabled;

        b32 lengthEnabled;
        u8 lengthValue;

        b32 timerMode;
        u16 timerPeriod;
        u16 timerValue;

        u16 shiftRegister;

        b32 envelopeEnabled;
        b32 envelopeLoop;
        b32 envelopeStart;
        u8 envelopePeriod;
        u8 envelopeValue;
        u8 envelopeVolume;

        u8 constantVolume;

        s32 bufferIndex;
        s16 buffer[APU_BUFFER_LENGTH];
    } noise;

    struct DMC
    {
        b32 globalEnabled;

        b32 enabled;

        u16 sampleAddress;
        u16 sampleLength;
        u16 currentAddress;
        u16 currentLength;

        u8 timerPeriod;
        u8 timerValue;

        u8 shiftRegister;
        u8 bitCount;

        u8 value;

        b32 loop;
        b32 irq;

        s32 bufferIndex;
        s16 buffer[APU_BUFFER_LENGTH];
    } dmc;

    u64 cycles;
    u8 frameMode;
    u8 frameValue;
    s32 frameCounter;
    
    s32 sampleCounter;

    b32 inhibitIRQ;
    b32 dmcIRQ;

    s32 bufferIndex;
    s16 buffer[APU_BUFFER_LENGTH];
};

struct Controller
{
    u8 state;
    u8 index;
    u8 strobe;
};

struct GUI
{
    u32 width = 256;
    u32 height = 240;
    Color pixels[256 * 240];

    Color patterns[2][128 * 128];
    Color patternHover[8 * 8];
    Color sprites[64][8 * 16];
    Color sprites2[8][8 * 16];
    Color nametable[256 * 240];
    Color nametable2[32][30][64];
};

struct NES
{
    Memory cpuMemory;
    Memory ppuMemory;
    Memory oamMemory;
    Memory oamMemory2;

    CPU cpu;
    PPU ppu;
    APU apu;

    Cartridge cartridge;
    Controller controllers[2];

    GUI gui;

    void(*mapperInit)(NES *nes);
    u8(*mapperReadU8)(NES *nes, u16 address);
    void(*mapperWriteU8)(NES *nes, u16 address, u8 value);
    void(*mapperSave)(NES *nes, FILE *file);
    void(*mapperLoad)(NES *nes, FILE *file);
    void* mapperData;
};

struct CPUStep
{
    u32 cycles;
    CPUInstruction *instruction;
};

#endif // !TYPES_H

